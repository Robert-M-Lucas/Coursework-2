#include <Arduino.h>
#include <Wire.h>
#include "ControllerStorage.h"
#include "ControllerCommunication.h"
#include "ControllerInterface.h"
#include "ControllerConstants.h"
#include "../../shared/src/Util.h"

char serial_buffer[30] = {};

ControllerStorage storage;
ControllerCommunication communication(storage);
ControllerInterface interface(&storage);

void setup() {
    Serial.begin(9600);
    Serial.println(F("[INFO] [Setup] Init"));

    communication.init();

    communication.updateConnected();
    Util::bitmask_to_serial(communication.getConnected());

    storage.init();
    interface.init();

    pinMode(RECORDING_LED, OUTPUT);
    pinMode(PLAYBACK_LED, OUTPUT);
}

bool prevRecording;
bool prevPlaying;

void loop() {
    interface.update();

    if (interface.isRecording()) {
        if (!prevRecording) {
            Serial.println(F("[INFO] [Main Loop] Starting recording"));
            storage.deleteSong(interface.getSong());
            communication.startRecording(interface.getSong());
            digitalWrite(RECORDING_LED, HIGH);
        }

        if ((millis() / 1000) % 2 == 0) {
            digitalWrite(RECORDING_LED, HIGH);
        }
        else {
            digitalWrite(RECORDING_LED, LOW);
        }

        communication.recordingLoop();
    }
    else if (prevRecording) {
        Serial.println(F("[INFO] [Main Loop] Starting recording"));
        communication.stopRecording();
        digitalWrite(RECORDING_LED, LOW);
    }
    else if (interface.isPlayback()) {
        if (!prevPlaying) {
            Serial.println(F("[INFO] [Main Loop] Starting playback"));
            storage.resetPlayback();
            communication.startPlayback(interface.getSong());
            digitalWrite(PLAYBACK_LED, HIGH);
        }

        communication.playbackLoop();
    }
    else if (prevPlaying) {
        Serial.println(F("[INFO] [Main Loop] Stopping playback"));
        digitalWrite(PLAYBACK_LED, LOW);
        communication.stopPlayback();
    }

    prevRecording = interface.isRecording();
    prevPlaying = interface.isPlayback();

    delay(TRANSMISSION_DELAY);
}