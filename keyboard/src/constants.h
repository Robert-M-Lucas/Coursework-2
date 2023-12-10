//
// Created by lachl on 27/11/2023.
//

#ifndef KEYBOARD_CONSTANTS_H
#define KEYBOARD_CONSTANTS_H

constexpr unsigned int WHITE_ADC_PINS[4] = {10,12,11,13};
constexpr unsigned int BLACK_ADC_PINS[4] = {4,8,7,9};

constexpr unsigned int SPEAKER_PINS[3] = {3,5,6};

constexpr unsigned int OCTAVE[8] = {261,293,329,349,392,440,494};
constexpr unsigned int SHARPS[5] = {277,311,349,415,466};

constexpr unsigned int KEY_TIME = 100;
#endif //KEYBOARD_CONSTANTS_H
