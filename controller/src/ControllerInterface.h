//
// Created by robertlucas on 27/11/23.
//

#ifndef CONTROLLER_CONTROLLERINTERFACE_H
#define CONTROLLER_CONTROLLERINTERFACE_H

#include <LiquidCrystal.h>
#include "../../shared/src/Constants.h"
#include "ControllerStorage.h"

class ControllerInterface {
private:
    ControllerStorage* storage;
    LiquidCrystal lcd;

    bool songSelected = false;
    uint8_t song = 1;
    bool recordingSelected = false;
    bool playbackSelected = false;

    bool prevLeft = false;
    bool prevRight = false;
    bool prevSelect = false;

    unsigned long time = 0;
public:
    explicit ControllerInterface(ControllerStorage* storage);

    bool isRecording() const { return recordingSelected; }

    bool isPlayback() const { return  playbackSelected; }

    uint8_t getSong() const { return song; }

    void onLeft();

    void onRight();

    void onSelect();

    void updateLCD();

    void updateButtons();

    void update();
};


#endif //CONTROLLER_CONTROLLERINTERFACE_H
