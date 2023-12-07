//
// Created by Benny on 28/11/2023.
//

#ifndef CONTROLLER_CONTROLLERCOMMUNICATION_H
#define CONTROLLER_CONTROLLERCOMMUNICATION_H

#include <Arduino.h>
#include <Wire.h>

#include "../../shared/src/Communication.h"
#include "../../shared/src/Util.h"
#include "../../shared/src/Constants.h"
#include "ControllerStorage.h"

#define for_all_instruments \
    for (uint8_t i = 0; i < MAX_INSTRUMENTS; i++) { \
        if (connected_devices_bitmask[i / 8] & 1 << (i % 8)) { \
            Instrument instrument = static_cast<Instrument>(i);
#define end_for_all_instruments \
        }\
    }

class ControllerCommunication {
private:
    ControllerStorage &storage;
    byte connected_devices_bitmask[MAX_INSTRUMENT_BITMASK_BYTES] = {}; // Ceiling division by 8

    /// Transmit a `Code` to an instrument
    static void message(Instrument instrument, Code code);

    /// Transmit a `Code` to all connected instruments
    void messageAll(Code code) const;

    /// Read bytes from an instrument
    template <class T>
    T readResponse();

    static unsigned readResponseToBuffer(byte* buffer);

    /// Transmit a `Code` alongside the buffer contents to an instrument
    static void sendBuffer(Instrument instrument, Code code, const byte *buffer, uint8_t length);

    /// Write song data to an instrument
    bool writeInstrumentBuffer(Instrument instrument);

    void storeAllInstrumentBuffers();

    bool writeAllInstrumentBuffers();

public:
    explicit ControllerCommunication(ControllerStorage &storage);

    static void init();

    void updateConnected();

    uint8_t countConnected();

    byte* getConnected() { return connected_devices_bitmask; }

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
