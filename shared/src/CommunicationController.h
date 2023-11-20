//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONCONTROLLER_H
#define COMMUNICATIONCONTROLLER_H

#include <Wire.h>

#include "Communication.h"
#include "ControllerStorage.h"

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
        inline void message(u8 instrument, Code code) {
            Wire.beginTransmission(instrument);
            Wire.write(static_cast<u8>(Code::StartRecording));
            Wire.endTransmission();
        }

        /// Transmit a `Code` to all connected instruments
        inline void messageAll(Code code) {
            for (u8 instrument = 0; instrument < MAX_INSTRUMENTS; ++instrument) {
                if (instrumentsConnected[instrument]) {
                    message(instrument, code);
                }
            }
        }





    }

    /// Called during `setup()`
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
    inline void readSongData(int instrument) {
        // Request song length
        Internal::message(instrument, Code::RequestSongLength);

        //

    }
}

#endif //COMMUNICATIONCONTROLLER_H
