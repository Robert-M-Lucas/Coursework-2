#include <Arduino.h>
#include <Wire.h>

#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "../../shared/src/Util.h"

// Actor interface with controller
LEDActor actor = LEDActor(2, 4);

const int ledPin = 13;
const int piezoPin = A0;

const int knockThreshold = 10;

float ledValue = 0.0; // Led is bright if ledValue > 0
float ledDecay = 0.1; // Controls how quickly the LED switches off

unsigned long lastNoteTime = 0; // Timestamp of the last note played

bool recording = false; // Used to keep track of whether we are currently recording a song
bool playback  = false; // Used to keep track of whether we are currently playing back a song
bool blocking  = false; // Whether to block the playback or recording
unsigned long blockingEndTime = 0; // Timestamp when blocking ends

void setup() {
    // Setup pins
    pinMode(ledPin, OUTPUT);

    // Initialize USB serial connection for debugging
    Serial.begin(9600);
    Serial.println(F("Init")); // F macro stores string in flash rather than RAM

    // Initialize I2C communication with the controller
    CommunicationActor::initialise(Instrument::Drum, &actor);
}

// Returns true when a knock is detected
bool knockDetected() {
    auto piezoValue = analogRead(piezoPin); // Value read from the piezo sensor
    return piezoValue > knockThreshold;
}

void updateLed(bool knock) {
    if (knock) {
        ledValue = 1.0;
    } else {
        ledValue -= ledDecay;
    }

    if (ledValue > 0.0) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }
}

void updateBlocking() {
    if (millis() > blockingEndTime) {
        blocking = false;
    }
}

void blockUntil(unsigned long time) {
    blocking = true;
    blockingEndTime = time;
}

void loop() {
    bool knock = false;

    // Update blocking
    updateBlocking();

    if (!blocking) {
        if (playback || actor.getPlayback()) {
            // ----------------------------------------
            // Playback mode
            // ----------------------------------------

            if (!playback) {
                // Play last note
                // This will always play a note at the beginning of playback
                knock = true;

                // Update last note time
                lastNoteTime = millis();

                // Next note
                if (actor.readDataAvailable(2)) {
                    byte data[2];
                    actor.readDataAndRemove(data, 2);

                    auto noteTimestamp = Util::fromBytes<uint16_t>(data) + lastNoteTime;

                    // Block until note should play
                    blockUntil(noteTimestamp);
                }
            } else if (!actor.getPlayback()) {
                // Playback ended and all notes in the buffer have been played
                playback = false;
            } else {
                Serial.println(F("Playback ongoing but no data is available"));
            }
        } else if (recording || actor.getRecording()) {
            // ----------------------------------------
            // Recording mode
            // ----------------------------------------

            if (!recording) {
                // Recording is beginning
                Serial.println(F("Starting recording"));
                lastNoteTime = millis();
            }

            if (knockDetected()) {
                knock = true;

                // Record note
                auto currentTime = millis();
                auto timeTillNote = currentTime - lastNoteTime;
                byte data[2];
                Util::toBytes(timeTillNote, data);

                // Write note to buffer
                actor.writeData(data, 2);

                // Update lastNoteTime
                lastNoteTime = currentTime;
            }
        } else {
            // ----------------------------------------
            // Normal mode
            // ----------------------------------------

            knock = knockDetected();
        }
    }

    updateLed(knock);
}
