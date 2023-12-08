#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"
#include "constants.h"
#include <Adafruit_MCP3008.h>



LEDActor actor = LEDActor(2, 4);
Adafruit_MCP3008 inputAdcWhiteKeys;
Adafruit_MCP3008 inputAdcBlackKeys;

constexpr unsigned highThreshold = 512;
constexpr byte emptyByte = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(3, OUTPUT);

    //Initiate USB serial communication to allow debugging via USB
    Serial.begin(SERIAL_BAUD_RATE);
    //Write startup message to USB serial, 'F' to store string in Flash over RAM
    Serial.println(F("Init"));

    //Initialise the ADCs managing, white and Black piano keys respectively
    inputAdcWhiteKeys.begin(WHITE_ADC_PINS[3],WHITE_ADC_PINS[2],WHITE_ADC_PINS[1],WHITE_ADC_PINS[0]);
    inputAdcBlackKeys.begin(BLACK_ADC_PINS[3],BLACK_ADC_PINS[2],BLACK_ADC_PINS[1],BLACK_ADC_PINS[0]);

    //Initialise the Keyboard communication actor object
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

bool readHigh(const unsigned reading)
{
    //Converts analogue reading to boolean (0 or 1) for usage in Bitwise calculations
    return reading > highThreshold;
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

bool recording = false;
byte whiteBitMask = 0;
byte blackBitMask = 0;

unsigned long startTime = 0;

void loop() {
    // TODO: Move functionality to 'shared' where applicable
    if (actor.getPlayback()) {
        if (actor.readDataAvailable(3)) {
            byte data[3] = {};
            actor.readDataAndRemove(data, 3);
            unsigned long duration_ms = static_cast<unsigned int>(data[2]) * INSTRUMENT_POLL_INTERVAL;
            const byte whiteKeys = data[0];
            const byte blackKeys = data[1];
            if (whiteKeys != 0 || blackKeys != 0) {
                tone(3, 400, duration_ms);
                delay(duration_ms);
            }
            else {
                delay(duration_ms);
            }
        }
        else {
            Serial.println(F("Playback ongoing but no data is available!"));
        }
    }
    else if (recording || actor.getRecording()) {
        const unsigned long currentTime = millis();
        const unsigned long elapsedTime = currentTime - startTime;

        const byte newWhiteBitMask = readKeys(&inputAdcWhiteKeys);
        const byte newBlackBitMask = readKeys(&inputAdcBlackKeys);


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

        recording = actor.getRecording();
    }
}


