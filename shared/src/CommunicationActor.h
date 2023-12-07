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
    namespace Internal {
        // void(*beginRecording)(unsigned long) = nullptr; // Offset ms
        // void(*stopRecording)() = nullptr;
        // // Get the length of the currently stored song data
        // unsigned int(*getSongLength)() = nullptr;
        // // Get pointer to where data should be read from / written too
        // byte*(*getSong)() = nullptr;

        ActorInterface* actorBuffer = nullptr;

        Request request = Request::None;
        uint8_t lastBufferLength = 0; // I miss Rust enums

        /// Interrupt called when the actor receives a request for bytes
        inline void requestEvent() {
            //Serial.println("request event happened");
            if (request == Request::None) {
                Serial.println(F("Received a request when the request type has not been specified"));
            }
            //Serial.println("request: " + String(static_cast<uint8_t>(request)));

            switch (request) {
                case Request::BufferLength: {
                    const uint8_t length = actorBuffer->getBufferLength();
                    lastBufferLength = length;
                    //Serial.println("length: " + String(length));
                    Wire.write(length);
                    break;
                }
                case Request::Buffer: {
                    const uint8_t length = lastBufferLength;
                    const ArrAndOffset arr_data = actorBuffer->getBufferRead();
                    for (uint8_t i = 0; i < length; i++) {
                        unsigned index = *arr_data.offset + i;
                        if (index >= BUFFER_SIZE) {
                            index -= BUFFER_SIZE;
                        }
                        Wire.write(arr_data.arr[index]);
                    }
                    *arr_data.offset += length;
                    if (*arr_data.offset >= BUFFER_SIZE) {
                        *arr_data.offset -= BUFFER_SIZE;
                    }
                }
                case Request::BufferEmpty: {
                    const uint8_t length = actorBuffer->getBufferEmpty();
                    Wire.write(length);
                    //Serial.println("buffer empty request, empty space: " + String(length));
                    break;
                }
                case Request::None: {
                    Serial.println(F("Request type not set when request was received"));
                }
                default: {
                    Serial.print(F("Request type '"));
                    Serial.print(static_cast<uint8_t>(request));
                    Serial.println(F("' has not been implemented"));
                }
            }

            request = Request::None;
        }

        /// Interrupt called when actor receives data from tbe controller
        inline void receiveEvent(int length) {
            //Serial.println("received event");

            if (Wire.available() <= 0) { return; }

            const Code code = static_cast<Code>(Wire.read());
            //Serial.println("code: " + String(static_cast<uint8_t>(code)));
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
                    Serial.println(F("receiving buffer data"));
                    const ArrAndOffset arr_data = actorBuffer->getBufferWrite();
                    unsigned i = 0;
                    while (Wire.available() > 0) {
                        unsigned index = *arr_data.offset + i;
                        if (index >= BUFFER_SIZE) {
                            index -= BUFFER_SIZE;
                        }

                        arr_data.arr[index] = static_cast<byte>(Wire.read());

                        Serial.print(F("byte: "));
                        Serial.println(static_cast<char>(arr_data.arr[index]));

                        i++;
                    }

                    *arr_data.offset += i;
                    if (*arr_data.offset >= BUFFER_SIZE) {
                        *arr_data.offset -= BUFFER_SIZE;
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
                case Code::ClearBuffer: {
                    actorBuffer->clearBuffer();
                    break;
                }
                default: {
                    Serial.print(F("Unrecognised code received: "));
                    Serial.println(static_cast<uint8_t>(code));
                }
            }

            if (Wire.available() > 0) {
                Serial.print(F("Leftover data after code '"));
                Serial.print(static_cast<uint8_t>(code));
                Serial.println(F("' was handled"));
            }
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
    inline void initialise(Instrument address, ActorInterface *actorBuffer) {
        Wire.begin(static_cast<uint8_t>(address));
        Wire.onRequest(Internal::requestEvent);
        Wire.onReceive(Internal::receiveEvent);
        Internal::actorBuffer = actorBuffer;
    }
}

#endif //COMMUNICATIONACTOR_H
