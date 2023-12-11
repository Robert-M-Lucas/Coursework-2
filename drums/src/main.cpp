#include <Arduino.h>
#include <Wire.h>

#include "../../shared/src/ActorCommunication.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "../../shared/src/Util.h"

// Actor interface with controller
LEDActor actor = LEDActor(2, 4);

// Pins
const int outputPiezoPin = 10;
const int inputPiezoPin = A0;

// Minimum piezo sensor reading required to register a knock
// Lower values mean that it is easier to trigger a note but notes may be triggered
// accidentally and there is the possibility of a feedback loop where the sound output
// triggers the input piezo
const int knockThreshold = 40;

// Frequency and duration of the tone to paly
const int toneFrequency = 200;
const int toneDuration  = 100;

// Whether a knock was detected on the last iteration of loop(), in order to prevent a knock
// from being detected multiple times
bool knockLastIteration = false;

// Variables for recording and playback
bool recording = false; // Used to keep track of whether we are currently recording a song
bool playback  = false; // Used to keep track of whether we are currently playing back a song
bool blocking  = false; // Whether to block the playback or recording
unsigned long startTime = 0; // Timestamp of recording started
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

// This must be called each iteration of loop(), to stop blocking when blockingEndTime is reached
void updateBlocking() {
    if (millis() > blockingEndTime) { // Check if the current timestamp is after the blocking end time
        // Unset blocking flag
        blocking = false;
    }
}

// Replacement for delay, except it doesn't use inputs and allows you to block until a specific timestamp
// instead of for a specific duration (this is useful for synchronising instruments)
void blockUntil(unsigned long time) {
    // Set blocking flag
    blocking = true;

    // Update blocking end time
    blockingEndTime = time;
}

void loop() {
    // This variable will store whether a knock should be played
    bool knock = false;

    // Update blocking mechanism
    updateBlocking();

    if (!blocking) { // Only run main loop if not blocking
        if (playback || actor.getPlayback()) {
            // ----------------------------------------
            // Playback mode
            // ----------------------------------------

            // Check if playback has just began, i.e. the playback flag is false
            if (!playback) {
                // Initialise lastNoteEndTime
                lastNoteEndTime = millis();
            }
            // Set playback to true for the next iteration
            playback = true;

            if (actor.readDataAvailable(1)) { // Check if there is any data available to read
                // Read a single byte from the controller
                byte data;
                actor.readDataAndRemove(&data, 1);

                if (data == 0) { // As explained in the Data Format section of the writeup, a value of 0 represents a beat
                    knock = true;
                } else { // All other values represent intervals between notes
                    // Decode duration from data
                    auto duration = static_cast<unsigned long>(data) * INSTRUMENT_POLL_INTERVAL;

                    // Update lastNoteEndTime using duration
                    lastNoteEndTime += duration;

                    // Block until the next note should play
                    blockUntil(lastNoteEndTime);
                }
            } else if (!actor.getPlayback()){
                // Playback has ended
                playback = false;
            }
        } else if (recording || actor.getRecording()) {
            // ----------------------------------------
            // Recording mode
            // ----------------------------------------

            // Check for a knock on the sensor
            knock = knockDetected();

            // Calculate current time and elapsed time since the last beat
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
            else if (elapsedTime > MAX_NOTE_DURATION_MS || knock) { // Whether we need to write new data
                // Calculate duration of the interval since the last note
                auto duration = (uint8_t) constrain(elapsedTime / INSTRUMENT_POLL_INTERVAL, 0, 255);
                // Make sure that duration is not 0, as this would register as a beat
                duration = max(duration, 1);

                // Use actor interface to send interval to controller
                actor.writeData(&duration, 1);

                // Write knock flag if there was any
                if (knock) {
                    // As discussed in the write-up, a byte of 0 represents a knock
                    uint8_t knockFlag = 0;
                    // Send data to the controller
                    actor.writeData(&knockFlag, 1);
                }

                // Update start time
                startTime = currentTime;
            }

            // Update recording flag
            recording = actor.getRecording();
        } else {
            // ----------------------------------------
            // Normal mode
            // ----------------------------------------

            // Simply detect knock from the user
            knock = knockDetected();
        }
    }

    // Play tone when a knock occurs
    if (knock) {
        tone(outputPiezoPin, toneFrequency, toneDuration);

        // Flash on-board LED (for debug purposes)
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        // Flash on-board LED (for debug purposes)
        digitalWrite(LED_BUILTIN, LOW);
    }

    // Small delay for luck
    delay(5);
}
