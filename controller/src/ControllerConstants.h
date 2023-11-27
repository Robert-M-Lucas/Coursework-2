//
// Created by robertlucas on 27/11/23.
//

#ifndef CONTROLLER_CONTROLLERCONSTANTS_H
#define CONTROLLER_CONTROLLERCONSTANTS_H

#include <Arduino.h>
#include "../../shared/src/Constants.h"

const uint8_t SONG_COUNT = 10;

const pin LEFT_INPUT = A2;
const pin RIGHT_INPUT = A0;
const pin SELECT_INPUT = A1;

const pin RECORDING_LED = 8;
const pin PLAYBACK_LED = 9;

namespace LCD {
    const pin RS = 2;
    const pin ENABLE = 3;
    const pin D0 = 4;
    const pin D1 = 5;
    const pin D2 = 6;
    const pin D3 = 7;
}

#endif //CONTROLLER_CONTROLLERCONSTANTS_H
