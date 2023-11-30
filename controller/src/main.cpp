#include <Arduino.h>
#include <Wire.h>
#include "ControllerStorage.h"
#include "ControllerCommunication.h"
#include "ControllerInterface.h"
//#include "../../shared/src/Communication.h"

ControllerStorage storage;
ControllerCommunication communication(storage);
ControllerInterface interface(&storage);

enum class Mode {
    Standby,
    Playback,
    Recording,
} mode;

const char* instrument_to_name(Instrument instrument) {
    switch (instrument) {
        case (Instrument::Keyboard): {
            return "Keyboard";
        }
        default: {
            return "Unrecognised Instrument";
        }
    }
}

void bitmask_to_serial(uint8_t bitmask) {
    Serial.println("Connected instruments:");
    bool found = false;
    for (uint8_t i = 0; i < 8; i++) {
        if (bitmask & 1 << i) {
            Serial.println(instrument_to_name((Instrument) i));
            found = true;
        }
    }
    if (!found) {
        Serial.println("No instruments");
    }
}

void setup() {
    Serial.begin(9600);

    communication.updateConnected();
    storage.init();

    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
//    _bitmask_to_serial(communication.getConnected());
    bitmask_to_serial(communication.getConnected());
}

void loop() {
    interface.update();
    delay(15);
}

// Called to begin playback
//void playbackSong(const u8 song) {
//    // Inform serial monitor that we are beginning playback
//    Serial.print("Beginning playback of song ");
//    Serial.println(song);
//
//    // Inform the CommunicationStorage of the song to play back
//    storage.selectSong(song);
//
//    // Message instruments to begin playback
//    communication.startPlayback();
//}

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