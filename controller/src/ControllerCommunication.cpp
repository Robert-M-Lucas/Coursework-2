//
// Created by Benny on 28/11/2023.
//

#include "ControllerCommunication.h"
#include "../../shared/src/Constants.h"

extern "C" {
#include "utility/twi.h"
};

ControllerCommunication::ControllerCommunication(ControllerStorage &storage) :
        storage(storage) {}


void ControllerCommunication::init() {
    Serial.println(F("[INFO] [ControllerCommunication] Initialising"));
    Wire.begin();
}


void ControllerCommunication::updateConnected() {
    // Scan for connected devices
    for (uint8_t addr = 1; addr < MAX_INSTRUMENTS; addr++) {
        // Writes nothing to a device to see if the message is delivered
        byte rc = twi_writeTo(addr, nullptr, 0, 1, 0);
        if (rc == 0) {
            connected_devices_bitmask[addr / 8] |= 1 << (addr % 8);
        }
    }
}

uint8_t ControllerCommunication::countConnected() {
    uint8_t count = 0;
    for (uint8_t addr = 1; addr < MAX_INSTRUMENTS; addr++) {
        if ((connected_devices_bitmask[addr / 8] & (1 << (addr % 8))) != 0) {
            count++;
        }
    }
    return count;
}

void ControllerCommunication::startRecording(uint8_t song) {
    storage.selectSong(song);
    clearBuffers();
    delay(TRANSMISSION_DELAY);
    messageAll(Code::StartRecording);
}

void ControllerCommunication::recordingLoop() {
    storeAllInstrumentBuffers();
}

void ControllerCommunication::stopRecording() {
    messageAll(Code::StopRecording);
    delay(TRANSMISSION_DELAY);
    Serial.print(F("[INFO] [ControllerCommunication] Reading all buffers after recording... "));
    while (storeAllInstrumentBuffers()) {
        delay(TRANSMISSION_DELAY);
    }
    Serial.println(F("Done"));
}

void ControllerCommunication::startPlayback(uint8_t song) {
    storage.selectSong(song);
    clearBuffers();
    delay(TRANSMISSION_DELAY);
    Serial.print(F("[INFO] [ControllerCommunication] Writing all buffers before playback... "));
    while (writeAllInstrumentBuffers()) {
        delay(TRANSMISSION_DELAY);
    }
    Serial.println(F("Done"));
    delay(TRANSMISSION_DELAY);
    messageAll(Code::StartPlayback);
}

bool ControllerCommunication::playbackLoop() {
    return writeAllInstrumentBuffers();
}

void ControllerCommunication::stopPlayback() {
    messageAll(Code::StopPlayback);
    delay(TRANSMISSION_DELAY);
    clearBuffers();
}

void ControllerCommunication::clearBuffers() {
    messageAll(Code::ClearBuffer);
}

unsigned ControllerCommunication::storeInstrumentBuffer(Instrument instrument) {
    // Set next request to buffer length
    message(instrument, Code::RequestBufferLength);

    delay(TRANSMISSION_DELAY);

    // Request buffer length
    Wire.requestFrom(static_cast<uint8_t>(instrument), 1u);

    // Wait for response
    delay(TRANSMISSION_DELAY);

    if (Wire.available() <= 0) { Serial.println(F("[ERROR] [ControllerCommunication] Buffer length not transmitted!")); }

    // Read response
    const uint8_t length = Wire.read();
    if (Wire.available() > 0) { Serial.println(F("[ERROR] [ControllerCommunication] Too much buffer length data transferred")); }

    // Set next request to buffer data
    message(instrument, Code::RequestBuffer);

    delay(TRANSMISSION_DELAY);

    // Request buffer data
    Wire.requestFrom(static_cast<uint8_t>(instrument), length);

    // Receive buffer data
    readResponseToBuffer(storage.getBuffer());

    // Write to storage
    storage.storeBufferToDisk(instrument, length);

    return length;
}

bool ControllerCommunication::writeInstrumentBuffer(Instrument instrument) {
//    Serial.println("Writing to instrument");
    // Set next request buffer space remaining
    message(instrument, Code::RequestBufferSpaceRemaining);

    delay(TRANSMISSION_DELAY);

    // Request buffer space remaining
    Wire.requestFrom(static_cast<uint8_t>(instrument), 1u);

    // Wait for response
    delay(TRANSMISSION_DELAY);
    if (Wire.available() <= 0) { Serial.println(F("[ERROR] [ControllerCommunication] Buffer empty not transmitted!")); }

    // Read response
    auto length = static_cast<uint8_t>(Wire.read());
    if (Wire.available() > 0) { Serial.println(F("[ERROR] [ControllerCommunication] Too much buffer empty data transferred")); }

    Serial.println("Len");
    Serial.println(length);

    length = min(length, MAX_TRANSFER_SIZE - 2);

    Serial.println(length);

//    Serial.print("Length before: ");
//    Serial.println(length);

    // Load song data for this instrument into the storage buffer
    length = storage.loadSongData(instrument, length); // Length may be set to less than requested if not enough data is available
    Serial.println(length);
    const byte* buffer = storage.getBuffer();

//    Serial.print("Length after");
//    Serial.println(length);

//    if (length == 0) {
//        return false;
//    }

    delay(TRANSMISSION_DELAY);

    // Send buffer to instrument
    sendBuffer(
            instrument,
            Code::BufferData,
            buffer,
            length
    );

    return length > 0;
//    return true;
}

bool ControllerCommunication::storeAllInstrumentBuffers() {
    bool stored = false;
    for_all_instruments
        stored |= storeInstrumentBuffer(instrument) > 0;
    end_for_all_instruments
    return stored;
}

bool ControllerCommunication::writeAllInstrumentBuffers() {
    bool dataWritten = false;
    for_all_instruments
        dataWritten |= writeInstrumentBuffer(instrument);
    end_for_all_instruments
    return dataWritten;
}

void ControllerCommunication::message(Instrument instrument, Code code) {
    Wire.beginTransmission(static_cast<u8>(instrument));
    Wire.write(static_cast<u8>(code));
    Wire.endTransmission();
}

void ControllerCommunication::messageAll(const Code code) const {
    for_all_instruments
        message(instrument, code);
    end_for_all_instruments
}

unsigned ControllerCommunication::readResponseToBuffer(byte *buffer) {
    unsigned i = 0;
    while (Wire.available() > 0) {
        buffer[i] = static_cast<byte>(Wire.read());
        i++;
    }
    return i;
}

void ControllerCommunication::sendBuffer(
    const Instrument instrument,
    const Code code,
    const byte *buffer,
    const uint8_t length
) {
    Wire.beginTransmission(static_cast<uint8_t>(instrument));
    Wire.write(static_cast<uint8_t>(code));
    for (uint8_t i = 0; i < length; i++) Wire.write(buffer[i]);
    Wire.endTransmission();
}