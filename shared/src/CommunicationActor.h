//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONACTOR_H
#define COMMUNICATIONACTOR_H

#include <Wire.h>
#include "Communication.h"

namespace CommunicationActor {
    void(*beginRecording)(unsigned long) = nullptr; // Offset ms
    void(*stopRecording)() = nullptr;
    unsigned int(*getSongLength)() = nullptr;
    byte*(*getSong)() = nullptr;

    // Gets the address where the song data should be written into
    bool*(*getSongStorage)() = nullptr;

    Request request = Request::None;

    inline void requestEvent() {
        if (request == Request::None) {
            Serial.println("Received a request when the request type has not been specified");
        }

        switch (request) {
            default:
                Serial.print("Request type '");
                Serial.print(static_cast<uint8_t>(request));
                Serial.println("' has not been implemented");
        }

        request = Request::None;
    }

    inline void receiveEvent(int length) {
        if (Wire.available() == 0) { return; }

        const uint8_t code = Wire.read();
        switch (code) {
            case static_cast<int>(Code::StopRecording):
                stopRecording();
                break;
            default:
                Serial.print("Unrecognised code received: ");
                Serial.println(code);
        }
    }

    inline void initialise(Instrument address) {
        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);
    }
};

#endif //COMMUNICATIONACTOR_H
