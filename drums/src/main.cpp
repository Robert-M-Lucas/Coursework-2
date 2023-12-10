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

unsigned long startTime = 0; // Timestamp of recording started

bool recording = false; // Used to keep track of whether we are currently recording a song
bool playback  = false; // Used to keep track of whether we are currently playing back a song
bool blocking  = false; // Whether to block the playback or recording
bool knockLastIteration = false;
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
    bool knock = piezoValue > knockThreshold; // Whether the sensor is currently being knocked
    bool newKnock = knock && !knockLastIteration; // Whether this is the first iteration where this knock is detected
    knockLastIteration = knock; // Update previous knock variable
    return newKnock;
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
                if (actor.readDataAvailable(1)) {
                    byte data;
                    actor.readDataAndRemove(&data, 1);

                    if (data == 0) {
                        // beat
                        knock = true;
                    } else {
                        // interval
                        auto duration = static_cast<unsigned long>(data) * INSTRUMENT_POLL_INTERVAL;
                        blockUntil(millis() + duration);
                    }
                } else {
                    playback = false;
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

            knock = knockDetected();

            const unsigned long currentTime = millis();
            const unsigned long elapsedTime = currentTime - startTime;

            // If recording is starting
            if (!recording) {
                // Update log that recording has started
                Serial.println(F("Starting recording"));

                // Set recording start timestamp
                startTime = currentTime;
            }
            // If not changed, note too long or recording is ending
            else if (elapsedTime > MAX_NOTE_DURATION_MS || knock) {
                auto duration = (uint8_t) constrain(elapsedTime / INSTRUMENT_POLL_INTERVAL, 0, 255);
                duration = max(duration, 1);

                actor.writeData(&duration, 1);

                if (knock) {
                    uint8_t knockFlag = 0;
                    actor.writeData(&knockFlag, 1);
                }

                // Update start time
                startTime = currentTime;
            }

            recording = actor.getRecording();
        } else {
            // ----------------------------------------
            // Normal mode
            // ----------------------------------------

            knock = knockDetected();
        }
    }

    updateLed(knock);
    delay(5);
}
