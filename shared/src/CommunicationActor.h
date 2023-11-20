//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONACTOR_H
#define COMMUNICATIONACTOR_H

#include <Wire.h>
#include "Communication.h"
#include "Actor.h"

namespace CommunicationActor {
    // void(*beginRecording)(unsigned long) = nullptr; // Offset ms
    // void(*stopRecording)() = nullptr;
    // // Get the length of the currently stored song data
    // unsigned int(*getSongLength)() = nullptr;
    // // Get pointer to where data should be read from / written too
    // byte*(*getSong)() = nullptr;

    ActorInterface* dataStore = nullptr;

    Request request = Request::None;
    unsigned lastBufferLength = 0; // I miss Rust enums

    inline void requestEvent() {
        if (request == Request::None) {
            Serial.println("Received a request when the request type has not been specified");
        }

        switch (request) {
            case Request::BufferLength : {
                unsigned length = dataStore->getBufferLength();
                lastBufferLength = length;
                const auto* b = static_cast<byte*>(static_cast<void*>(&length)); // Disgusting
                for (unsigned i = 0; i < sizeof(unsigned); i++) {
                    Wire.write(b[i]);
                }
                break;
            }
            case Request::Buffer: {
                const unsigned length = lastBufferLength;
                const ArrAndOffset arr_data = dataStore->getBufferRead();
                for (unsigned i = 0; i < length; i++) {
                    unsigned index = (*arr_data.offset) + i;
                    if (index > INSTRUMENT_BUFFER_SIZE) {
                        index -= INSTRUMENT_BUFFER_SIZE;
                    }
                    Wire.write(arr_data.arr[index]);
                }
                *arr_data.offset += length;
                if (*arr_data.offset > INSTRUMENT_BUFFER_SIZE) {
                    *arr_data.offset -= INSTRUMENT_BUFFER_SIZE;
                }
            }
            default: {
                Serial.print("Request type '");
                Serial.print(static_cast<uint8_t>(request));
                Serial.println("' has not been implemented");
            }
        }

        request = Request::None;
    }

    inline void receiveEvent(int length) {
        if (Wire.available() <= 0) { return; }

        const Code code = static_cast<Code>(Wire.read());
        switch (code) {
            case Code::StartRecording: {
                dataStore->startRecording();
                break;
            }
            case Code::StopRecording: {
                dataStore->stopRecording();
                break;
            }
            case Code::RequestBufferLength: {
                request = Request::BufferLength;
                break;
            }
            case Code::RequestBuffer: {
                request = Request::Buffer;
                break;
            }
            case Code::RequestBufferNeeded: {
                request = Request::BufferEmpty;
                break;
            }
            case Code::BufferData: {
                ArrAndOffset arr_data = dataStore->getBufferWrite();
                unsigned i = 0;
                while (Wire.available() > 0) {
                    unsigned index = (*arr_data.offset) + i;
                    if (index > INSTRUMENT_BUFFER_SIZE) {
                        index -= INSTRUMENT_BUFFER_SIZE;
                    }

                    arr_data.arr[index] = static_cast<byte>(Wire.read());

                    i++;
                }

                *arr_data.offset += i;
                if (*arr_data.offset > INSTRUMENT_BUFFER_SIZE) {
                    *arr_data.offset -= INSTRUMENT_BUFFER_SIZE;
                }
            }
            case Code::StartPlayback: {
                dataStore->startPlayback();
                break;
            }
            case Code::StopPlayback: {
                dataStore->stopPlayback();
                break;
            }
            default: {
                Serial.print("Unrecognised code received: ");
                Serial.println(static_cast<uint8_t>(code));
            }
        }

        if (Wire.available() >= 0) { Serial.print("Leftover data after code '"); Serial.print(static_cast<uint8_t>(code)); Serial.println("' was handled"); }
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

    inline void initialise(Instrument address, ActorInterface *_dataStore) {
        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);

        dataStore = _dataStore;
    }
}

#endif //COMMUNICATIONACTOR_H
