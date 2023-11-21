//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONACTOR_H
#define COMMUNICATIONACTOR_H

#include <Wire.h>
#include "Communication.h"
#include "Actor.h"
#include "Util.h"

namespace CommunicationActor {
    // void(*beginRecording)(unsigned long) = nullptr; // Offset ms
    // void(*stopRecording)() = nullptr;
    // // Get the length of the currently stored song data
    // unsigned int(*getSongLength)() = nullptr;
    // // Get pointer to where data should be read from / written too
    // byte*(*getSong)() = nullptr;

    ActorInterface* actorBuffer = nullptr;

    Request request = Request::None;
    unsigned lastBufferLength = 0; // I miss Rust enums

    /// Interrupt called when the actor receives a request for bytes
    inline void requestEvent() {
        if (request == Request::None) {
            Serial.println("Received a request when the request type has not been specified");
        }

        switch (request) {
            case Request::BufferLength: {
                unsigned length = actorBuffer->getBufferLength();
                lastBufferLength = length;
                byte* b = nullptr;
                Util::toBytes(&length, b);
                for (unsigned i = 0; i < sizeof(unsigned); i++) {
                    Wire.write(b[i]);
                }
                break;
            }
            case Request::Buffer: {
                const unsigned length = lastBufferLength;
                const ArrAndOffset arr_data = actorBuffer->getBufferRead();
                for (unsigned i = 0; i < length; i++) {
                    unsigned index = (*arr_data.offset) + i;
                    if (index >= INSTRUMENT_BUFFER_SIZE) {
                        index -= INSTRUMENT_BUFFER_SIZE;
                    }
                    Wire.write(arr_data.arr[index]);
                }
                *arr_data.offset += length;
                if (*arr_data.offset > INSTRUMENT_BUFFER_SIZE) {
                    *arr_data.offset -= INSTRUMENT_BUFFER_SIZE;
                }
            }
            case Request::BufferEmpty: {
                unsigned length = actorBuffer->getBufferEmpty();
                byte* b = nullptr;
                Util::toBytes(&length, b);
                for (unsigned i = 0; i < sizeof(unsigned); i++) {
                    Wire.write(b[i]);
                }
                break;
            }
            default: {
                Serial.print("Request type '");
                Serial.print(static_cast<uint8_t>(request));
                Serial.println("' has not been implemented");
            }
        }

        request = Request::None;
    }

    /// Interrupt called when actor receives data from tbe controller
    inline void receiveEvent(int length) {
        if (Wire.available() <= 0) { return; }

        const Code code = static_cast<Code>(Wire.read());
        switch (code) {
            case Code::StartRecording: {
                actorBuffer->startRecording();
                break;
            }
            case Code::StopRecording: {
                actorBuffer->stopRecording();
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
                ArrAndOffset arr_data = actorBuffer->getBufferWrite();
                unsigned i = 0;
                while (Wire.available() > 0) {
                    unsigned index = (*arr_data.offset) + i;
                    if (index >= INSTRUMENT_BUFFER_SIZE) {
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
                actorBuffer->startPlayback();
                break;
            }
            case Code::StopPlayback: {
                actorBuffer->stopPlayback();
                break;
            }
            default: {
                Serial.print("Unrecognised code received: ");
                Serial.println(static_cast<uint8_t>(code));
            }
        }

        if (Wire.available() > 0) {
            Serial.print("Leftover data after code '");
            Serial.print(static_cast<uint8_t>(code));
            Serial.println("' was handled");
        }
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

    /// Initialises actor for communication - does not initialise serial
    inline void initialise(Instrument address, ActorInterface *_dataStore) {
        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(requestEvent);
        Wire.onReceive(receiveEvent);
        actorBuffer = _dataStore;
    }
}

#endif //COMMUNICATIONACTOR_H
