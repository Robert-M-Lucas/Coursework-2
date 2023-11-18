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
    StopRecording = 1
};

enum class Request : uint8_t {
    None = 0,
    SongLength = 1
};

#endif //SHARED_COMMUNICATION_H
