//
// Created by Benny on 28/11/2023.
//

#include "ControllerCommunication.h"
#include "../../shared/src/Constants.h"

extern "C" {
#include "utility/twi.h"
};

ControllerCommunication::ControllerCommunication(ControllerStorage &storage) :
        storage(storage) {
    Wire.begin();
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

    Wire.requestFrom(static_cast<u8>(instrument), (uint8_t) 1);

    // Wait for response
    delay(TRANSMISSION_DELAY);

    if (Wire.available() <= 0) { Serial.println("Buffer length not transmitted!"); }

    // Read response
    const u16 length = Wire.read();
    if (Wire.available() > 0) { Serial.println("Too much buffer length data transferred"); }

    // Request buffer data
    message(instrument, Code::RequestBuffer);

    delay(TRANSMISSION_DELAY);

    Wire.requestFrom(static_cast<u8>(instrument), length);

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

    Wire.requestFrom(static_cast<u8>(instrument), (uint8_t) 1);

    // Wait for response
    delay(TRANSMISSION_DELAY);
    if (Wire.available() <= 0) { Serial.println("Buffer length not transmitted!"); }

    // Read response
    u16 length = static_cast<u16>(Wire.read());
    if (Wire.available() > 0) { Serial.println("Too much buffer length data transferred"); }

    Serial.println(length);

    delay(TRANSMISSION_DELAY);

    // Load song data for this instrument into the storage buffer
    length = storage.loadSongData(instrument, length);
    const byte* buffer = storage.getBuffer();

    for (u16 i = 0; i < length; i++) {
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
    storeInstrumentBuffer(Instrument::TestInstrument);
}

bool ControllerCommunication::writeAllInstrumentBuffers() {
    bool dataWritten = false;
    // TODO:
    dataWritten |= writeInstrumentBuffer(Instrument::TestInstrument);
    return dataWritten;
}

void ControllerCommunication::message(Instrument instrument, Code code) {
    Wire.beginTransmission(static_cast<u8>(instrument));
    Wire.write(static_cast<u8>(code));
    Wire.endTransmission();
}

void ControllerCommunication::messageAll(const Code code) {
    message(Instrument::TestInstrument, code);
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
    const u8 *buffer,
    const u16 length
) {
    Wire.beginTransmission(static_cast<u8>(instrument));
    Wire.write(static_cast<u8>(code));
    for (u16 i = 0; i < length; i++) Wire.write(buffer[i]);
    Wire.endTransmission();
}

void ControllerCommunication::updateConnected() {
    // Scan for connected devices
    for (uint8_t addr = 1; addr < MAX_INSTRUMENTS; addr++) {
        byte rc = twi_writeTo(addr, nullptr, 0, 1, 0);
        if (rc == 0) {
            connected_devices_bitmask |= 1 << addr;
        }
    }
}
