//
// Created by robertlucas on 17/11/23.
//

#ifndef SHARED_COMMUNICATION_H
#define SHARED_COMMUNICATION_H

#include <Arduino.h>
#include <Wire.h>

/// Unique instrument ID
enum class Instrument : uint8_t {
    Keyboard = 1,
    TestInstrument = 2,
    Drum = 3,
};

/// Code that is is always the first bit of a transfer representing the transfer type
enum class Code : uint8_t {
    StartRecording = 1,
    StopRecording = 2,
    RequestBufferLength = 3,
    RequestBuffer = 4,
    RequestBufferSpaceRemaining = 5,
    BufferData = 6,
    StartPlayback = 7,
    StopPlayback = 8,
    ClearBuffer = 9
};

/// Represents the data the controller is expecting when it next requests data from the actor
enum class Request : uint8_t {
    None = 0,
    BufferLength = 1,
    Buffer = 2,
    BufferSpaceRemaining = 3,
};

#endif //SHARED_COMMUNICATION_H
