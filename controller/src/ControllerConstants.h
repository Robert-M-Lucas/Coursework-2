//
// Created by robertlucas on 27/11/23.
//

#ifndef CONTROLLER_CONTROLLERCONSTANTS_H
#define CONTROLLER_CONTROLLERCONSTANTS_H

#include <Arduino.h>
#include "../../shared/src/Constants.h"

uint8_t SONG_COUNT = 10;

pin LEFT_INPUT = A2;
pin RIGHT_INPUT = A0;
pin SELECT_INPUT = A1;

pin RECORDING_LED = 8;
pin PLAYBACK_LED = 9;

namespace LCD {
    pin RS = 2;
    pin ENABLE = 3;
    pin D0 = 4;
    pin D1 = 5;
    pin D2 = 6;
    pin D3 = 7;
}

#endif //CONTROLLER_CONTROLLERCONSTANTS_H
