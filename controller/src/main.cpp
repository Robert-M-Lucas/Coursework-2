#include <Arduino.h>
#include <Wire.h>
#include "ControllerStorage.h"
#include "ControllerCommunication.h"
#include "ControllerInterface.h"
#include "ControllerConstants.h"
#include "../../shared/src/Util.h"

ControllerStorage storage;
ControllerCommunication communication(storage);
ControllerInterface interface(&storage, &communication);

void setup() {
    Serial.begin(9600);
    Serial.println(F("[INFO] [Setup] Init"));

    // Ensure that the Controller is last to initialise so that when it polls for other instruments it finds them
    delay(1000);

    communication.init();
    communication.updateConnected();

    Util::bitmask_to_serial(communication.getConnectedBitmask());

    storage.init();
    interface.init();

    pinMode(RECORDING_LED, OUTPUT);
    pinMode(PLAYBACK_LED, OUTPUT);
}

bool prevRecording;
bool prevPlaying;

void loop() {
    // Stop in case of SD not initialising
    if (!storage.is_initialised()) { return; }

    interface.update();

    if (interface.isRecording()) { // ? Recording
        if (!prevRecording) { // ? Starting recording
            Serial.println(F("[INFO] [Main Loop] Starting recording"));
            // Required to prevent instruments only appending to previous files
            storage.deleteSong(interface.getSong());
            communication.startRecording(interface.getSong());
            digitalWrite(PLAYBACK_LED, LOW);
        }

        // ? Recording loop

        // Flash recording LED
        if ((millis() / 1000) % 2 == 0) {
            digitalWrite(RECORDING_LED, HIGH);
        }
        else {
            digitalWrite(RECORDING_LED, LOW);
        }

        communication.recordingLoop();
    }
    else if (prevRecording) { // ? Stopped recording
        Serial.println(F("[INFO] [Main Loop] Stopped recording"));
        communication.stopRecording();
        digitalWrite(RECORDING_LED, LOW);
    }
    else if (interface.isPlayback()) { // ? Playing back
        if (!prevPlaying) { // ? Started playing back
            Serial.println(F("[INFO] [Main Loop] Starting playback"));
            // Reset saved positions in files to 0 for all instruments
            storage.resetPlayback();
            communication.startPlayback(interface.getSong());
            digitalWrite(RECORDING_LED, LOW);
            digitalWrite(PLAYBACK_LED, HIGH);
        }

        // ? Playback loop
        communication.playbackLoop();
    }
    else if (prevPlaying) { // ? Stopped playing back
        Serial.println(F("[INFO] [Main Loop] Stopping playback"));
        digitalWrite(PLAYBACK_LED, LOW);
        communication.stopPlayback();
    }

    prevRecording = interface.isRecording();
    prevPlaying = interface.isPlayback();

    delay(TRANSMISSION_DELAY);
}