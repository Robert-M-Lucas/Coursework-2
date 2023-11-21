#include <Arduino.h>

#include "../../shared/src/CommunicationController.h"

ControllerStorage storage;

void setup() {
    Serial.begin(9600);
    CommunicationController::initialise(&storage);
}

void playbackSong(const uint8_t song) {
    storage.selectSong(song);
    storage.readMode();
    CommunicationController::clearBuffers();
    CommunicationController::writeAllInstrumentBuffers();
    CommunicationController::startPlayback();

    // TODO:
    while (true) {
        CommunicationController::writeAllInstrumentBuffers();
    }
}

void recordSong(const uint8_t song) {
    storage.selectSong(song);
    storage.writeMode();
    CommunicationController::clearBuffers();
    CommunicationController::startRecording();

    // TODO:
    while (true) {
        CommunicationController::storeAllInstrumentBuffers();
    }

    CommunicationController::storeAllInstrumentBuffers();
}

void loop() {

}