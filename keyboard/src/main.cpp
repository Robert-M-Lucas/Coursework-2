#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include <Adafruit_MCP3008.h>


LEDActor actor = LEDActor(2, 3);
Adafruit_MCP3008 inputAdcWhiteKeys;
Adafruit_MCP3008 inputAdcBlackKeys;

constexpr unsigned highThreshold = 512;
constexpr byte fullByte = 255;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Serial.println("Init");

    //inputs used are not important, just placeholders for now
    inputAdcWhiteKeys.begin(13,11,12,10);
    inputAdcBlackKeys.begin(9,8,7,6);

    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

bool readHigh(const unsigned reading)
{
    //test if input is above highThreshold
    return reading > highThreshold;
}

byte readKeys(Adafruit_MCP3008 *keys)
{
    byte inputMask = fullByte;

    for(uint8_t channel = 0; channel < 8; channel++)
    {
        inputMask = inputMask & static_cast<byte>(readHigh(keys->readADC(channel)) << channel);
    }

    return inputMask;
}

bool recording = false;
byte whiteBitMask = 0;
byte blackBitMask = 0;

unsigned long startTime = 0;

bool playback = false;

void loop() {
    // TODO: Move functionality to 'shared' where applicable

    if (playback || actor.getPlayback()) {
        // If playback is starting
        if (!playback) {
            if (actor.readDataAvailable(3)) {
                byte data[3] = {};
                actor.readDataAndRemove(data, 3);
                unsigned long duration_ms = data[2] * INSTRUMENT_POLL_INTERVAL;
                delay(500); // TODO: Play sounds
            }
            // Playback ended and all notes in buffer have been played
            else if (!actor.getPlayback()) {
                playback = false;
            }
            else {
                Serial.println("Playback ongoing but no data is available!");
            }
        }
    }
    else if (recording || actor.getRecording()) {
        const unsigned long currentTime = millis();
        const unsigned long ellapsedTime = currentTime - startTime;

        const byte newWhiteBitMask = readKeys(&inputAdcWhiteKeys);
        const byte newBlackBitMask = readKeys(&inputAdcBlackKeys);

        // If recording is starting
        if (!recording) {
            startTime = currentTime;
            whiteBitMask = newWhiteBitMask;
            blackBitMask = newBlackBitMask;
        }
        // If not changed, note too long or recording is ending
        else if (ellapsedTime > MAX_NOTE_DURATION_MS ||
                whiteBitMask != newWhiteBitMask || blackBitMask != newBlackBitMask ||
                actor.getRecording() == false) {
                byte data[3] = {};
                data[0] = whiteBitMask; // White keys
                data[1] = blackBitMask; // Black keys
                data[2] = static_cast<byte>(ellapsedTime / INSTRUMENT_POLL_INTERVAL); // Duration

                actor.writeData(data, 3);

                whiteBitMask = newWhiteBitMask;
                blackBitMask = newBlackBitMask;
                startTime = currentTime;
        }

        delay(INSTRUMENT_POLL_INTERVAL);

        recording = actor.getRecording();
    }
}


