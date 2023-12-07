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
//    template <class T>
//    T readResponse();

    /// Reads the actors response to the buffer
    static unsigned readResponseToBuffer(byte* buffer);

    /// Transmit a `Code` alongside the buffer contents to an instrument
    static void sendBuffer(Instrument instrument, Code code, const byte *buffer, uint8_t length);

    /// Write instrument buffer to storage
    unsigned storeInstrumentBuffer(Instrument instrument);

    /// Write song data to an instrument
    bool writeInstrumentBuffer(Instrument instrument);

    /// Write all instrument buffers to storage
    void storeAllInstrumentBuffers();

    /// Write song data to all instruments
    bool writeAllInstrumentBuffers();

public:
    explicit ControllerCommunication(ControllerStorage &storage);

    static void init();

    /// Polls for connected devices and stores the result
    void updateConnected();

    /// Counts the connected devices from the stored bitmask
    uint8_t countConnected();

    byte* getConnectedBitmask() { return connected_devices_bitmask; }

    /// Called to begin recording
    void startRecording(uint8_t song);

    /// Called while recording
    void recordingLoop();

    /// Called to end recording
    void stopRecording();

    /// Called to begin playback
    void startPlayback(uint8_t song);

    /// Called during playback
    void playbackLoop();

    /// Called to end playback
    void stopPlayback();

    /// Clears every instrument's buffer
    void clearBuffers();
};

//template<class T>
//T ControllerCommunication::readResponse() {
//    byte bytes[sizeof(T)];
//
//    for (unsigned int i = 0; i < sizeof(T) && Wire.available(); ++i) {
//        bytes[i] = Wire.read();
//    }
//
//    return Util::fromBytes<T>(bytes);
//}

#endif //CONTROLLER_CONTROLLERCOMMUNICATION_H
