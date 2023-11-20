//
// Created by robertlucas on 17/11/23.
//

#ifndef SHARED_COMMUNICATION_H
#define SHARED_COMMUNICATION_H

#include <Arduino.h>
#include <Wire.h>

enum class Instrument : uint8_t {
    Keyboard = 1
};

enum class Code : uint8_t {
    StartRecording = 1,
    StopRecording = 2,
    RequestBufferLength = 3,
    RequestBuffer = 4,
    RequestBufferNeeded = 5,
    BufferData = 6,
    StartPlayback = 7,
    StopPlayback = 8,
};

enum class Request : uint8_t {
    None = 0,
    BufferLength = 1,
    Buffer = 2,
    BufferEmpty = 3,
};

#endif //SHARED_COMMUNICATION_H
