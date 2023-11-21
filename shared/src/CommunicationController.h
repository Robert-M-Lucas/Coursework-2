//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONCONTROLLER_H
#define COMMUNICATIONCONTROLLER_H

#include <Wire.h>

#include "Communication.h"
#include "ControllerStorage.h"
#include "Util.h"

namespace CommunicationController {
    namespace Internal {
        ControllerStorage storage;

        bool instrumentsConnected[MAX_INSTRUMENTS] = {
                false,
                true,
                false,
                false,
                false,
                false,
                false,
                false,
        };

        /// Transmit a `Code` to an instrument
        inline void message(Instrument instrument, Code code) {
            Wire.beginTransmission(static_cast<uint8_t>(instrument));
            Wire.write(static_cast<u8>(code));
            Wire.endTransmission();
        }

        /// Transmit a `Code` to all connected instruments
        inline void messageAll(Code code) {
            message(Instrument::Keyboard, code);
        }

        /// Read bytes from an instrument
        template <class T>
        inline T readResponse() {
            byte bytes[sizeof(T)];

            for (unsigned int i = 0; i < sizeof(T) && Wire.available(); ++i) {
                bytes[i] = Wire.read();
            }

            return Util::fromBytes<T>(bytes);
        }

        inline unsigned readResponseToBuffer(byte* buffer) {
            unsigned i = 0;
            while (Wire.available() > 0) {
                buffer[i] = static_cast<byte>(Wire.read());
                i++;
            }
            return i;
        }
    }

    /// Called during `setup()` - does not initialise serial
    inline void initialise() {
        Wire.begin();
    }

    /// Called to begin recording
    inline void startRecording() {
        Internal::messageAll(Code::StartRecording);
    }

    /// Called to end recording
    inline void stopRecording() {
        Internal::messageAll(Code::StopRecording);
    }

    /// Read song data from an instrument
    inline void readBuffer(Instrument instrument) {
        // Request buffer length
        Internal::message(instrument, Code::RequestBufferLength);

        // Wait for response
        delay(TRANSMISSION_DELAY);

        if (Wire.available() <= 0) { Serial.println("Buffer length not transmitted!"); }
        // Read response
        uint8_t length = Wire.read();
        if (Wire.available() > 0) { Serial.println("Too much buffer length data transferred"); }

        // Request buffer data
        Internal::message(instrument, Code::RequestBuffer);

        delay(TRANSMISSION_DELAY);

        Wire.requestFrom(static_cast<uint8_t>(instrument), length);

        // Receive buffer data
        Internal::readResponseToBuffer(Internal::storage.getBuffer());

        // Write to storage
        Internal::storage.writeBufferToSD(length, instrument);
    }
}

#endif //COMMUNICATIONCONTROLLER_H
