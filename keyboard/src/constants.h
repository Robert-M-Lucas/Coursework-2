//
// Created by lachl on 27/11/2023.
//

#ifndef KEYBOARD_CONSTANTS_H
#define KEYBOARD_CONSTANTS_H

#include "tones.h"

constexpr unsigned int WHITE_ADC_PINS[4] = {10,12,11,13};
constexpr unsigned int BLACK_ADC_PINS[4] = {4,8,7,9};

constexpr unsigned int SPEAKER_PINS[3] = {3,5,6};

constexpr unsigned int NATURALS[8] = {NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4};
constexpr unsigned int SHARPS[7] = {NOTE_CS3,NOTE_DS3,NOTE_FS3,NOTE_GS3,NOTE_AS3, NOTE_CS3, NOTE_C3}; // Last two used due to dead pins

constexpr unsigned int KEY_TIME = 100;
#endif //KEYBOARD_CONSTANTS_H
