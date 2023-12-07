//
// Created by robertlucas on 27/11/23.
//

#ifndef CONTROLLER_CONTROLLERCONSTANTS_H
#define CONTROLLER_CONTROLLERCONSTANTS_H

#include <Arduino.h>
#include "../../shared/src/Constants.h"

extern char serial_buffer[];

constexpr u8 SONG_COUNT = 10;

constexpr pin LEFT_INPUT = A2;
constexpr pin RIGHT_INPUT = A0;
constexpr pin SELECT_INPUT = A1;

constexpr pin RECORDING_LED = 8;
constexpr pin PLAYBACK_LED = 9;
constexpr pin CHIP_SELECT = 10;

namespace LCD {
    constexpr pin RS = 2;
    constexpr pin ENABLE = 3;
    constexpr pin D0 = 4;
    constexpr pin D1 = 5;
    constexpr pin D2 = 6;
    constexpr pin D3 = 7;
}

#endif //CONTROLLER_CONTROLLERCONSTANTS_H
