#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationController.h"
#include "ControllerStorage.h"
#include "ControllerInterface.h"

ControllerStorage storage;
ControllerInterface interface(&storage);

void setup() {
    Serial.begin(9600);
    Serial.println("Init");
    CommunicationController::initialise(&storage);

    Serial.println("Starting recording");
    CommunicationController::Internal::messageAll(Code::StartRecording);

    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
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
    interface.update();
    delay(15);
}

//void loop() {
//    unsigned len = CommunicationController::storeInstrumentBuffer(Instrument::Keyboard);
//
//    if (len > 0) {
//        Serial.println("Data:");
//        byte* buffer = CommunicationController::Internal::storage->getBuffer();
//        for (unsigned i = 0; i < len; i++) {
//            Serial.print((int) buffer[i]);
//            Serial.print("|");
//        }
//        Serial.println();
//    }
//    else {
//        Serial.println("Nothing to receive");
//    }
//}