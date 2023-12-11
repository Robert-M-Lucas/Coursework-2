#include <Arduino.h>
#include <Wire.h>

#include "../../shared/src/ActorCommunication.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "../../shared/src/Util.h"

// Actor interface with controller
LEDActor actor = LEDActor(2, 4);

const int outputPiezoPin = 10;
const int inputPiezoPin = A0;

const int knockThreshold = 40;

const int toneFrequency = 300;
const int toneDuration  = 100;

unsigned long startTime = 0; // Timestamp of recording started

bool recording = false; // Used to keep track of whether we are currently recording a song
bool playback  = false; // Used to keep track of whether we are currently playing back a song
bool blocking  = false; // Whether to block the playback or recording
bool knockLastIteration = false;
unsigned long lastNoteEndTime; // Tracks the time that the last note should end, used to synchronise playback
unsigned long blockingEndTime = 0; // Timestamp when blocking ends

void setup() {
    // Setup pins
    pinMode(outputPiezoPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Initialize USB serial connection for debugging
    Serial.begin(9600);
    Serial.println(F("Init")); // F macro stores string in flash rather than RAM

    // Initialize I2C communication with the controller
    ActorCommunication::initialise(Instrument::Drum, &actor);
}

// Returns true when a knock is detected
bool knockDetected() {
    auto piezoValue = analogRead(inputPiezoPin); // Value read from the piezo sensor
    bool knock = piezoValue > knockThreshold; // Whether the sensor is currently being knocked
    bool newKnock = knock && !knockLastIteration; // Whether this is the first iteration where this knock is detected
    knockLastIteration = knock; // Update previous knock variable
    return newKnock;
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
                // Playback just started
                lastNoteEndTime = millis();
            }
            playback = true;

            if (actor.readDataAvailable(1)) {
                byte data;
                actor.readDataAndRemove(&data, 1);

                if (data == 0) {
                    // beat
                    knock = true;
                } else {
                    // interval
                    auto duration = static_cast<unsigned long>(data) * INSTRUMENT_POLL_INTERVAL;
                    lastNoteEndTime += duration;
                    blockUntil(lastNoteEndTime);
                }
            } else if (!actor.getPlayback()){
                playback = false;
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

    // Play tone when a knock occurs
    if (knock) {
        tone(outputPiezoPin, toneFrequency, toneDuration);
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }

    delay(5);
}
