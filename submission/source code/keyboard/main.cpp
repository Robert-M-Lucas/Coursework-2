#include <Arduino.h>

#include <Wire.h>
//#include <Tone.h>
#include "../../shared/src/ActorCommunication.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "constants.h"
#include <Adafruit_MCP3008.h>

// Actor interface with controller
LEDActor actor = LEDActor(2, 4);

// Interface with MCP3008 multiplexer (using Adafruit_MCP3008.h library)
Adafruit_MCP3008 inputAdcWhiteKeys; // white keys multiplexer
Adafruit_MCP3008 inputAdcBlackKeys; // black keys multiplexer

// ----------------------------------------
// Constants
// ----------------------------------------

constexpr unsigned highThreshold = 512; // Threshold for a key to register as pressed
constexpr byte emptyByte = 0; // Byte of all 0's

// ----------------------------------------
// Global variables
// ----------------------------------------

// Variables for recording and playback
bool playback = false; // Tracks whether the keyboard is currently in playback mode. This is also used to detect when we have just entered playback mode
bool recording = false; // Tracks whether the keyboard is currently in recording mode. This is also used to detect whether we have just entered recording mode
byte whiteBitMask = 0; // Bitmask for the white keys (naturals) - each bit corresponds to one tone
byte blackBitMask = 0; // Bitmask for the black keys (semitones) - each of the first 5 bits corresponds to one of the semitones
unsigned long lastNoteEndTime = 0; // Time that the last note ended, used to synchronise playback
unsigned long startTime = 0; // Time that recording began or the last note was played

// Variables for blocking
bool blocking = false; // Whether to block the playback or recording
unsigned long blockingEndTime = 0; // Timestamp when blocking ends

void setup() {
    // Setup pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(TONE_PIN, OUTPUT);

    //Initiate USB serial communication to allow debugging via USB
    Serial.begin(SERIAL_BAUD_RATE);

    //Write startup message to USB serial, 'F' to store string in Flash over RAM
    Serial.println(F("Init"));

    // Initialise the ADCs managing, white and Black piano keys respectively
    inputAdcWhiteKeys.begin(WHITE_ADC_PINS[3],WHITE_ADC_PINS[2],WHITE_ADC_PINS[1],WHITE_ADC_PINS[0]);
    inputAdcBlackKeys.begin(BLACK_ADC_PINS[3],BLACK_ADC_PINS[2],BLACK_ADC_PINS[1],BLACK_ADC_PINS[0]);

    // Initialise the Keyboard communication actor object
    ActorCommunication::initialise(Instrument::Keyboard, &actor);
}

// Returns true if a reading is High
bool readHigh(const unsigned reading) {
    // Converts analogue reading to boolean (0 or 1) for usage in Bitwise calculations
    return reading > highThreshold;
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

// Returns the first three notes that are held down
//
// Parameters:
//   naturals: bitmask of the white keys
//   flats: bitmask of the black keys (semitones)
//   notesToPlay: address of the first element of an array of 3 uints; this is where the result will be stored since we cannot return an array directly
//
// Note: we calculate 3 held keys even though only one is actually used at the moment. this is because eventually we
// would like to support playing three keys at once, but currently we are running into an obscure linker error when we
// try to add the library to do this
void getNotes(byte naturals, byte sharps, unsigned int* notesToPlay) {
    constexpr unsigned int notesSize = 3; // Number of notes to count
    unsigned int noteIndex = 0; // Index into the output array

    bool done = false; // Loop terminates if done == true

    while (noteIndex < notesSize && !done) {
        done = true; // Assume that we are done

        for (uint8_t i = 0; i < 8; i++) { // Iterate over each natural note in the octave
            if ((naturals & (1 << i)) != 0) { // Bit magic to calculate the bit in the bitmask corresponding to this note
                if (i == 0) { continue; } // The note is not held, so continue
                notesToPlay[noteIndex] = NATURALS[i]; // Since we are still here, the note is held, so add it to the notesToPlay array
                noteIndex++; // Increment noteIndex for the next note found
                done = false;
                break;
            }
        }

        if (done) { // Check if we are finished looking for the naturals, so
            for (uint8_t i = 0; i < 7; i++) { // Iterate over each bit in the bitmask
                if ((sharps & (1 << i)) != 0) { // Check if the corresponding bit in the sharps bitmask is set
                    if (i == 0) { continue; } // The key is not held, so continue
                    notesToPlay[noteIndex] = SHARPS[i]; // The key is held, so we store it in the output array
                    noteIndex++; // Update noteIndex for the next note found
                    done = false;
                    break;
                }
            }
        }
    }
}

void playNotes(unsigned int* notes) {
    // Initially, we planned to use the Tone.h library to control 3 Piezos, meaning that
    // we could play three tones at once (to allow for chords)
    //
    // Then this function would look like this:
    //   speaker0.play(notes[0]);
    //   speaker1.play(notes[1]);
    //   speaker2.play(notes[2]);
    //
    // However, this causes linker errors because two libraries are declaring handles to the same timers
    // So we just play the first one, if it exists

    if (notes[0] != 0) { // Note is held
        // Play the note!
        tone(TONE_PIN, notes[0]);
    } else { // No note is held
        // Cancel any tones
        noTone(TONE_PIN);
    }
}

// Reads the multiplexer and returns the values as a bitmask
byte readKeys(Adafruit_MCP3008 *keys) {
    // Sets input mask to the empty byte '00000000'
    byte inputMask = emptyByte;

    // Iterate over each channel
    for(uint8_t channel = 1; channel < 8; channel++) {
        // Applies bitwise OR between the current channel reading and corresponding bit in the bitmask
        inputMask |= static_cast<byte>(readHigh(keys->readADC(channel)) << channel);
    }

    return inputMask;
}

void loop() {
    if (!blocking) { // Only run main loop if not blocking
        if (playback || actor.getPlayback()) {
            // ----------------------------------------
            // Playback mode
            // ----------------------------------------

            // Check if playback has just began, i.e. the playback flag is false
            if (!playback) {
                // Update serial monitor that playback just began (for debugging purposes)
                Serial.println("Playback just started");

                // Reset lastNoteEndTime
                lastNoteEndTime = millis();
            }
            // Set playback to true for the next iteration
            playback = true;

            if (actor.readDataAvailable(3)) { // Check if there are three bytes of data available to read, i.e. there is a new note available
                // Read 3 bytes from the controller into the array data
                byte data[3] = {};
                actor.readDataAndRemove(data, 3);

                // Decode note duration from the data
                unsigned long duration_ms = data[2] * INSTRUMENT_POLL_INTERVAL;

                // Get white and black keys bitmasks
                const byte whiteKeys = data[0];
                const byte blackKeys = data[1];

                // Get the first three pressed notes
                unsigned int notes[3] = {0, 0, 0};
                getNotes(whiteKeys, blackKeys, notes);

                // Play notes
                playNotes(notes);

                // Update lastNoteEndTime for the next note
                lastNoteEndTime += duration_ms;

                // Block until the next note should play
                blockUntil(lastNoteEndTime);
            } else if (!actor.getPlayback()) {
                // Playback has finished, so we set the playback flag back to false
                playback = false;

                // Make sure to cancel any currently playing tones
                noTone(TONE_PIN);

                return;
            } else {
                // We are still in playback mode, but no data is available
                // This might mean that there is still data for other instruments, but not for this one
                // In this case we should just play no notes
                unsigned int notes[3] = {0, 0, 0};
                playNotes(notes);

                // Log that this has occurred
                Serial.println(F("Playback ongoing but no data is available!"));
            }
        } else if (playback) { // Controller has ended playback
            // Update playback flag to reflect that playback has ended
            playback = false;

            // Cancel any playing notes
            unsigned int notes[3] = {0, 0, 0};
            playNotes(notes);
        } else if (recording || actor.getRecording()) {
            // ----------------------------------------
            // Recording mode
            // ----------------------------------------

            // Calculate current time and elapsed time since the last note
            const unsigned long currentTime = millis();
            const unsigned long elapsedTime = currentTime - startTime;

            // Read keys from the piano
            const byte newWhiteBitMask = readKeys(&inputAdcWhiteKeys);
            const byte newBlackBitMask = readKeys(&inputAdcBlackKeys);

            // Play notes as they are recorded
            unsigned int notes[3] = {0, 0, 0};
            getNotes(whiteBitMask, blackBitMask, notes);
            playNotes(notes);

            // If recording is starting
            if (!recording) {
                // Update logthat recording has started
                Serial.println(F("Starting recording"));

                // Initialise startTime
                startTime = currentTime;

                // Initialise bitmasks
                whiteBitMask = newWhiteBitMask;
                blackBitMask = newBlackBitMask;
            }
            // If not changed, note too long or recording is ending
            else if (elapsedTime > MAX_NOTE_DURATION_MS ||
                     whiteBitMask != newWhiteBitMask || blackBitMask != newBlackBitMask) { // Whether we need to write more data
                // Log the notes that we are about to write
                Serial.println(F("Writing notes:"));
                for (uint8_t i = 0; i < 8; i++) {
                    if ((whiteBitMask & (1 << i)) != 0) Serial.print(1);
                    else Serial.print(0);
                }
                Serial.print("-");
                for (uint8_t i = 0; i < 8; i++) {
                    if ((blackBitMask & (1 << i)) != 0) Serial.print(1);
                    else Serial.print(0);
                }

                // Calculate duration of the interval since the last note
                auto duration = (uint8_t) constrain(elapsedTime / INSTRUMENT_POLL_INTERVAL, 0, 255);

                // Log the duration for debugging purposes
                Serial.println();
                Serial.print(F("Duration (x60ms): "));
                Serial.println(duration);

                // Encode the data to be sent to the controller
                byte data[3] = {};
                data[0] = whiteBitMask; // White keys
                data[1] = blackBitMask; // Black keys
                data[2] = static_cast<byte>(duration); // Duration

                // Send the data to the controller to be stored
                actor.writeData(data, 3);

                // Update whiteBitMask, blackBitMask and startTime
                whiteBitMask = newWhiteBitMask;
                blackBitMask = newBlackBitMask;
                startTime = currentTime;
            }

            // Delay until the next poll interval
            delay(INSTRUMENT_POLL_INTERVAL);

            // Update recording flag
            recording = actor.getRecording();
        } else {
            // ----------------------------------------
            // Normal mode
            // ----------------------------------------

            // Read keys
            whiteBitMask = readKeys(&inputAdcWhiteKeys);
            blackBitMask = readKeys(&inputAdcBlackKeys);

            // Get first three pressed notes
            unsigned int notes[3] = {0, 0, 0};
            getNotes(whiteBitMask, blackBitMask, notes);

            // Play notes
            playNotes(notes);
        }
    }

    // Update blocking mechanism
    updateBlocking();
}
