//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONACTOR_H
#define COMMUNICATIONACTOR_H

#include <DataStoreActor.h>
#include <Wire.h>
#include "Communication.h"

namespace CommunicationActor {
    // void(*beginRecording)(unsigned long) = nullptr; // Offset ms
    // void(*stopRecording)() = nullptr;
    // // Get the length of the currently stored song data
    // unsigned int(*getSongLength)() = nullptr;
    // // Get pointer to where data should be read from / written too
    // byte*(*getSong)() = nullptr;

    DataStoreActorInterface* dataStore = nullptr;

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
                dataStore->stopRecording();
                break;
            default:
                Serial.print("Unrecognised code received: ");
                Serial.println(code);
        }

        if (Wire.available() != 0) { Serial.print("Leftover data after code '"); Serial.print(code); Serial.println("' was handled"); }
    }

    /*inline void initialise(Instrument address,
        void(*_beginRecording)(unsigned long), void(*_stopRecording)(),
        byte*(*_getSong)()) {

        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);

        beginRecording = _beginRecording;
        stopRecording = _stopRecording;
        getSong = _getSong;
    }*/

    inline void initialise(Instrument address, DataStoreActorInterface *_dataStore) {
        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);

        dataStore = _dataStore;
    }
};

#endif //COMMUNICATIONACTOR_H
