//
// Created by robertlucas on 20/11/23.
//

#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include "Arduino.h"
#include "Communication.h"
#include "Constants.h"

namespace Util {
    /// Convert a value to a byte array
    template<class T>
    void toBytes(T const &x, byte *output) {
        const auto bytes = reinterpret_cast<byte *>(x);
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            output[i] = bytes[i];
        }
    }

    /// Convert a byte array back into a value
    template <class T>
    T fromBytes(byte *input) {
        return *reinterpret_cast<T*>(input);
    }

    /// Converts an instrument to its name
    inline const char* instrument_to_name(const Instrument instrument) {
        // TODO: Change this to be stored on flash?
        switch (instrument) {
            case (Instrument::Keyboard): {
                return "Keyboard";
            }
            case (Instrument::TestInstrument): {
                return "Test Instrument";
            }
            default: {
                return "Unrecognised Instrument";
            }
        }
    }

    /// Takes an instruments connected bitmask as input and serial prints the connected instrument names
    inline void bitmask_to_serial(const byte* bitmask) {
        Serial.println(F("[INFO] Connected instruments:"));

        bool found = false;
        for (uint8_t i = 0; i < MAX_INSTRUMENTS; i++) {
            if (bitmask[i / 8] & 1 << (i % 8)) {
                Serial.print(F("\t"));
                Serial.println(instrument_to_name(static_cast<Instrument>(i)));
                found = true;
            }
        }
        if (!found) {
            Serial.println(F("No instruments"));
        }
    }
}

#endif //SHARED_UTIL_H
