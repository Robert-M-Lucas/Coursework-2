//
// Created by robertlucas on 27/11/23.
//

#ifndef CONTROLLER_CONTROLLERINTERFACE_H
#define CONTROLLER_CONTROLLERINTERFACE_H

#include <LiquidCrystal.h>
#include "../../shared/src/Constants.h"
#include "ControllerStorage.h"
#include "ControllerCommunication.h"

class ControllerInterface {
private:
    ControllerStorage* storage;
    ControllerCommunication* communication;
    LiquidCrystal lcd;

    bool songSelected = false;
    uint8_t song = 1;
    bool recordingSelected = false;
    bool playbackSelected = false;

    bool prevLeft = false;
    bool prevRight = false;
    bool prevSelect = false;

    unsigned long time = 0;

    /// Called when left button is pressed
    void onLeft();

    /// Called when right button is pressed
    void onRight();

    /// Called when select button is pressed
    void onSelect();

    /// Called to refresh LCD contents
    void updateLCD();

    /// Called to update the button states and call corresponding function if a button is pressed
    void updateButtons();
public:

    explicit ControllerInterface(ControllerStorage* storage, ControllerCommunication* communication);

    void init();

    bool isRecording() const { return recordingSelected; }

    bool isPlayback() const { return  playbackSelected; }

    uint8_t getSong() const { return song; }

    /// Called to update interface state
    void update();
};


#endif //CONTROLLER_CONTROLLERINTERFACE_H
