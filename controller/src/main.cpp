#include <Arduino.h>
#include <Wire.h>
#include "ControllerStorage.h"
#include "ControllerCommunication.h"
#include "ControllerInterface.h"

ControllerStorage storage;
ControllerCommunication communication(storage);
ControllerInterface interface(&storage);

enum class Mode {
    Standby,
    Playback,
    Recording,
} mode;

void setup() {
    Serial.begin(9600);
    Serial.println("Init");

    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
}

void loop() {
    interface.update();
    delay(15);
}

// Called to begin playback
void playbackSong(const u8 song) {
    // Inform serial monitor that we are beginning playback
    Serial.print("Beginning playback of song ");
    Serial.println(song);

    // Inform the CommunicationStorage of the song to play back
    storage.selectSong(song);

    // Message instruments to begin playback
    communication.startPlayback();
}

// Called to begin recording

/*
void playbackSong(const uint8_t song) {
    Serial.print("Beginning playback on song ");
    Serial.println(song);

    storage.selectSong(song);
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
*/

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