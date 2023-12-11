//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONACTOR_H
#define COMMUNICATIONACTOR_H

#include <Wire.h>
#include "Communication.h"
#include "Actor.h"
#include "Util.h"

namespace ActorCommunication {
    namespace Internal {
        ActorInterface* actorInterface = nullptr;

        /// Stores transmitted request type
        Request request = Request::None;
        /// Stores length of buffer when length was last transmitted
        uint8_t lastBufferLength = 0; // I miss Rust enums

        /// Interrupt called when the actor receives a request for bytes
        inline void requestEvent() {
            if (request == Request::None) {
                Serial.println(F("Received a request when the request type has not been specified"));
            }

            switch (request) {
                case Request::BufferLength: {
                    uint8_t length = actorInterface->getBufferLength();
                    length = min(length, MAX_TRANSFER_SIZE - 2);
                    lastBufferLength = length;
                    Wire.write(length);
                    break;
                }
                case Request::Buffer: {
                    const uint8_t length = lastBufferLength;
                    const ArrAndOffset arr_data = actorInterface->getBufferRead();
                    // TODO: Use actorInterface functions
                    // Read and remove data from circular buffer
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
                case Request::BufferSpaceRemaining: {
                    const uint8_t length = actorInterface->getBufferSpaceRemaining();
                    Wire.write(length);
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
            if (Wire.available() <= 0) { return; } // A receive event with no data occurs during I2C polling

            const Code code = static_cast<Code>(Wire.read());

            switch (code) {
                case Code::StartRecording: {
                    actorInterface->startRecording();
                    break;
                }
                case Code::StopRecording: {
                    actorInterface->stopRecording();
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
                case Code::RequestBufferSpaceRemaining: {
                    request = Request::BufferSpaceRemaining;
                    break;
                }
                case Code::BufferData: {
                    // TODO: Use actorInterface functions
                    const ArrAndOffset arr_data = actorInterface->getBufferWrite();
                    unsigned i = 0;
                    while (Wire.available() > 0) {
                        unsigned index = *arr_data.offset + i;
                        if (index >= BUFFER_SIZE) {
                            index -= BUFFER_SIZE;
                        }

                        arr_data.arr[index] = static_cast<byte>(Wire.read());

                        i++;
                    }

                    *arr_data.offset += i;
                    if (*arr_data.offset >= BUFFER_SIZE) {
                        *arr_data.offset -= BUFFER_SIZE;
                    }
                }
                case Code::StartPlayback: {
                    actorInterface->startPlayback();
                    break;
                }
                case Code::StopPlayback: {
                    actorInterface->stopPlayback();
                    break;
                }
                case Code::ClearBuffer: {
                    Serial.println("Cleared buffer");
                    actorInterface->clearBuffer();
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

    /// Initialises actor for communication - does not initialise serial
    inline void initialise(Instrument address, ActorInterface *actorBuffer) {
        Wire.begin(static_cast<uint8_t>(address));

        // Setup interrupts
        Wire.onRequest(Internal::requestEvent);
        Wire.onReceive(Internal::receiveEvent);

        Internal::actorInterface = actorBuffer;
    }
}

#endif //COMMUNICATIONACTOR_H
