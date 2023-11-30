//
// Created by Benny on 28/11/2023.
//

#ifndef CONTROLLER_CONTROLLERCOMMUNICATION_H
#define CONTROLLER_CONTROLLERCOMMUNICATION_H

#include <Arduino.h>
#include <Wire.h>

#include "../../shared/src/Communication.h"
#include "../../shared/src/Util.h"
#include "ControllerStorage.h"


class ControllerCommunication {
private:
    ControllerStorage &storage;
    uint8_t connected_devices_bitmask = 0;

    /// Transmit a `Code` to an instrument
    void message(Instrument instrument, Code code);

    /// Read bytes from an instrument
    template <class T>
    T readResponse();

    /// Transmit a `Code` to all connected instruments
    void messageAll(Code code);

    unsigned readResponseToBuffer(byte* buffer);

    /// Transmit a `Code` alongside the buffer contents to an instrument
    void sendBuffer(Instrument instrument, Code code, const u8 *buffer, u16 length);

public:
    explicit ControllerCommunication(ControllerStorage &storage);

    void updateConnected();

    uint8_t getConnected() { return connected_devices_bitmask; }

    /// Called to begin isRecording
    void startRecording(uint8_t song);

    void recordingLoop();

    /// Called to end isRecording
    void stopRecording();

    /// Called to begin isPlayback
    void startPlayback(uint8_t song);

    void playbackLoop();

    /// Called to end isPlayback
    void stopPlayback();

    /// Empties the buffers of every instrument
    void clearBuffers();

    /// Read song data from an instrument and store it
    unsigned storeInstrumentBuffer(Instrument instrument);

    // TODO: Handle no more data
    /// Write song data to an instrument
    bool writeInstrumentBuffer(Instrument instrument);

    void storeAllInstrumentBuffers();

    bool writeAllInstrumentBuffers();
};

template<class T>
T ControllerCommunication::readResponse() {
    byte bytes[sizeof(T)];

    for (unsigned int i = 0; i < sizeof(T) && Wire.available(); ++i) {
        bytes[i] = Wire.read();
    }

    return Util::fromBytes<T>(bytes);
}

#endif //CONTROLLER_CONTROLLERCOMMUNICATION_H
