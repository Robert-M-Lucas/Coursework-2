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
        byte rc = twi_writeTo(addr, nullptr, 0, 1, 0);
        if (rc == 0) {
            connected_devices_bitmask[addr / 8] |= 1 << addr % 8;
        }
    }
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
    storeAllInstrumentBuffers();
}

void ControllerCommunication::startPlayback(uint8_t song) {
    storage.selectSong(song);
    clearBuffers();
    delay(TRANSMISSION_DELAY);
    writeAllInstrumentBuffers();
    delay(TRANSMISSION_DELAY);
    messageAll(Code::StartPlayback);
}

void ControllerCommunication::playbackLoop() {
    writeAllInstrumentBuffers();
}

void ControllerCommunication::stopPlayback() {
    messageAll(Code::StopPlayback);
}

void ControllerCommunication::clearBuffers() {
    messageAll(Code::ClearBuffer);
}

unsigned ControllerCommunication::storeInstrumentBuffer(Instrument instrument) {
    // Request buffer length
    message(instrument, Code::RequestBufferLength);

    delay(TRANSMISSION_DELAY);

    Wire.requestFrom(static_cast<uint8_t>(instrument), 1u);

    // Wait for response
    delay(TRANSMISSION_DELAY);

    if (Wire.available() <= 0) { Serial.println(F("[ERROR] [ControllerCommunication] Buffer length not transmitted!")); }

    // Read response
    const uint8_t length = Wire.read();
    if (Wire.available() > 0) { Serial.println(F("[ERROR] [ControllerCommunication] Too much buffer length data transferred")); }

    // Request buffer data
    message(instrument, Code::RequestBuffer);

    delay(TRANSMISSION_DELAY);

    Wire.requestFrom(static_cast<uint8_t>(instrument), length);

    // Receive buffer data
    readResponseToBuffer(storage.getBuffer());

    // Write to storage
    storage.storeBufferToDisk(length, instrument);

    return length;
}

bool ControllerCommunication::writeInstrumentBuffer(Instrument instrument) {
    // Request buffer length
    message(instrument, Code::RequestBufferNeeded);

    delay(TRANSMISSION_DELAY);

    Wire.requestFrom(static_cast<uint8_t>(instrument), 1u);

    // Wait for response
    delay(TRANSMISSION_DELAY);
    if (Wire.available() <= 0) { Serial.println(F("[ERROR] [ControllerCommunication] Buffer empty not transmitted!")); }

    // Read response
    auto length = static_cast<uint8_t>(Wire.read());
    if (Wire.available() > 0) { Serial.println(F("[ERROR] [ControllerCommunication] Too much buffer empty data transferred")); }

    Serial.print(F("[INFO] [ControllerCommunication] Actor buffer empty: "));
    Serial.println(length);

    delay(TRANSMISSION_DELAY);

    // Load song data for this instrument into the storage buffer
    length = storage.loadSongData(instrument, length);
    const byte* buffer = storage.getBuffer();

    for (uint8_t i = 0; i < length; i++) {
        Serial.print(static_cast<char>(buffer[i]));
    }
    Serial.println();

    Serial.println(length);

    // Send buffer to instrument
    sendBuffer(
            instrument,
            Code::BufferData,
            buffer,
            length
    );

    return true;
}

void ControllerCommunication::storeAllInstrumentBuffers() {
    for_all_instruments
        storeInstrumentBuffer(instrument);
    end_for_all_instruments
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