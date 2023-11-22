#include <Arduino.h>

#include "../../shared/src/CommunicationController.h"

ControllerStorage storage;

void setup() {
    Serial.begin(9600);
    CommunicationController::initialise(&storage);
}

void playbackSong(const uint8_t song) {
    Serial.print("Beginning playback on song ");
    Serial.println(song);

    storage.selectSong(song);
    storage.readMode();
    CommunicationController::clearBuffers();
    CommunicationController::writeAllInstrumentBuffers();
    CommunicationController::startPlayback();

    while (true) {
        // If no more data available
        if (!CommunicationController::writeAllInstrumentBuffers()) {
            break;
        }
    }

    CommunicationController::stopPlayback();
}

void recordSong(const uint8_t song) {
    Serial.print("Recording to song");
    Serial.println(song);

    storage.selectSong(song);
    storage.writeMode();
    CommunicationController::clearBuffers();
    CommunicationController::startRecording();

    // TODO:
    while (true) {
        CommunicationController::storeAllInstrumentBuffers();
    }

    CommunicationController::stopRecording();

    delay(INSTRUMENT_POLL_INTERVAL * 4); // Allow instruments to save last data

    CommunicationController::storeAllInstrumentBuffers();

    CommunicationController::clearBuffers();
}

void loop() {

}