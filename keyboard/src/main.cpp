#include <Arduino.h>

#include <Wire.h>
//#include <Tone.h>
#include "../../shared/src/ActorCommunication.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "constants.h"
#include <Adafruit_MCP3008.h>

LEDActor actor = LEDActor(2, 4);
Adafruit_MCP3008 inputAdcWhiteKeys;
Adafruit_MCP3008 inputAdcBlackKeys;

const int tonePin = 3;

constexpr unsigned highThreshold = 512;
constexpr byte emptyByte = 0;

bool playback = false;
bool recording = false;
byte whiteBitMask = 0;
byte blackBitMask = 0;
unsigned long lastNoteEndTime = 0; // Time that the last note ended, used to synchronise playback

unsigned long startTime = 0;

unsigned long iteration = 0; // Tracks the current iteration of loop

bool blocking = false; // Whether to block the playback or recording
unsigned long blockingEndTime = 0; // Timestamp when blocking ends

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    // Set
    pinMode(tonePin, OUTPUT);

    //Initiate USB serial communication to allow debugging via USB
    Serial.begin(SERIAL_BAUD_RATE);
    //Write startup message to USB serial, 'F' to store string in Flash over RAM
    Serial.println(F("Init"));

    //Initialise speakers
    //speaker0.begin(3);
    //speaker1.begin(5);
    //speaker2.begin(6);

    //Initialise the ADCs managing, white and Black piano keys respectively
    inputAdcWhiteKeys.begin(WHITE_ADC_PINS[3],WHITE_ADC_PINS[2],WHITE_ADC_PINS[1],WHITE_ADC_PINS[0]);
    inputAdcBlackKeys.begin(BLACK_ADC_PINS[3],BLACK_ADC_PINS[2],BLACK_ADC_PINS[1],BLACK_ADC_PINS[0]);

    //Initialise the Keyboard communication actor object
    ActorCommunication::initialise(Instrument::Keyboard, &actor);
}

bool readHigh(const unsigned reading) {
    //Converts analogue reading to boolean (0 or 1) for usage in Bitwise calculations
    return reading > highThreshold;
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

void getNotes(byte naturals, byte sharps, unsigned int* notesToPlay) {
    constexpr unsigned int notesSize = 3;
    unsigned int noteIndex = 0;

    bool done = false;
    while (noteIndex < notesSize && !done) {
        done = true;

        for (uint8_t i = 0; i < 8; i++) {
            if ((naturals & (1 << i)) != 0) {
                if (i == 0) { continue; }
                notesToPlay[noteIndex] = NATURALS[i];
                noteIndex++;
                done = false;
                break;
            }
        }

        if (done) {
            for (uint8_t i = 0; i < 7; i++) {
                if ((sharps & (1 << i)) != 0) {
                    if (i == 0) { continue; }
                    notesToPlay[noteIndex] = SHARPS[i];
                    noteIndex++;
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
    // So I just play the first one

    if (notes[0] != 0) {
        tone(tonePin, notes[0]);
    } else {
        noTone(tonePin);
    }

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
    Serial.println();
}

byte readKeys(Adafruit_MCP3008 *keys)
{
    //sets input mask to the empty byte '00000000'
    byte inputMask = emptyByte;

    for(uint8_t channel = 1; channel < 8; channel++)
    {
        //applies bitwise OR between the current channel reading and corresponding bit in the bitmask
        inputMask |= static_cast<byte>(readHigh(keys->readADC(channel)) << channel);
    }

    return inputMask;
}

void loop() {
    if (!blocking) {
        if (playback || actor.getPlayback()) {
            if (!playback) {
                // Playback just started
                Serial.println("Playback just started");

                // Reset lastNoteEndTime
                lastNoteEndTime = millis();
            }
            playback = true;

            if (actor.readDataAvailable(3)) {
                byte data[3] = {};
                actor.readDataAndRemove(data, 3);
                unsigned long duration_ms = data[2] * INSTRUMENT_POLL_INTERVAL;
                const byte whiteKeys = data[0];
                const byte blackKeys = data[1];

                // Get first three pressed notes
                unsigned int notes[3] = {0, 0, 0};
                getNotes(whiteKeys, blackKeys, notes);
                // Play notes
                playNotes(notes);

                lastNoteEndTime += duration_ms;
                blockUntil(lastNoteEndTime);
            } else if (!actor.getPlayback()) {
                playback = false;
                noTone(tonePin);
                return;
            } else {
                Serial.println(F("Playback ongoing but no data is available!"));
            }
        } else if (playback) {
            playback = false;
            unsigned int notes[3] = {0, 0, 0};
            playNotes(notes);
        } else if (recording || actor.getRecording()) {
            const unsigned long currentTime = millis();
            const unsigned long elapsedTime = currentTime - startTime;

            const byte newWhiteBitMask = readKeys(&inputAdcWhiteKeys);
            const byte newBlackBitMask = readKeys(&inputAdcBlackKeys);
            //Live playback
            unsigned int notes[3] = {0, 0, 0};
            getNotes(whiteBitMask, blackBitMask, notes);
            playNotes(notes);

            // If recording is starting
            if (!recording) {
                Serial.println(F("Starting recording"));
                startTime = currentTime;
                whiteBitMask = newWhiteBitMask;
                blackBitMask = newBlackBitMask;
            }
                // If not changed, note too long or recording is ending
            else if (elapsedTime > MAX_NOTE_DURATION_MS ||
                     whiteBitMask != newWhiteBitMask || blackBitMask != newBlackBitMask //||
                /*!actor.getRecording()*/) {
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

                auto duration = (uint8_t) constrain(elapsedTime / INSTRUMENT_POLL_INTERVAL, 0, 255);

                Serial.println();
                Serial.print(F("Duration (x60ms): "));
                Serial.println(duration);

                byte data[3] = {};
                data[0] = whiteBitMask; // White keys
                data[1] = blackBitMask; // Black keys
                data[2] = static_cast<byte>(duration); // Duration

                actor.writeData(data, 3);

                whiteBitMask = newWhiteBitMask;
                blackBitMask = newBlackBitMask;
                startTime = currentTime;
            }

            delay(INSTRUMENT_POLL_INTERVAL);

            recording = actor.getRecording();
        } else {
            // Normal mode

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

    updateBlocking();
}
