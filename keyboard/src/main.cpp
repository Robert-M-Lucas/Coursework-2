#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include <Adafruit_MCP3008.h>


DebugActor actor = DebugActor();
Adafruit_MCP3008 inputAdcWhiteKeys;
Adafruit_MCP3008 inputAdcBlackKeys;

constexpr int highThreshold = 512;
constexpr byte fullByte = 255;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    //inputs used are not important, just placeholders for now
    inputAdcWhiteKeys.begin(13,11,12,10);
    inputAdcBlackKeys.begin(9,8,7,6);
    Serial.println("Init");
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

bool readHigh(const int reading)
{
    //test if input is above highThreshold
    return (reading > highThreshold);
}

uint16_t readKeys(Adafruit_MCP3008 *keys)
{
    uint16_t inputMask = fullByte;

    for(unsigned channel = 0; channel < 8; channel++)
    {
        inputMask = inputMask & static_cast<uint16_t>(readHigh(keys->readADC(channel)) << channel);
    }

    return inputMask;
}

// 2 Bytes
uint16_t fullBitMask = 0;
unsigned long startTime = 0;

void loop() {
    const unsigned long currentTime = millis();
    const unsigned long ellapsedTime = currentTime - startTime;

    uint16_t newFullBitMask = readKeys(&inputAdcWhiteKeys);
    newFullBitMask |= readKeys(&inputAdcBlackKeys) << 8;

    if (ellapsedTime > MAX_NOTE_DURATION_MS || fullBitMask != newFullBitMask) {
        // TODO: Implement this functionality in 'shared'
        byte data[3] = {};
        data[0] = static_cast<byte>(fullBitMask); // White keys
        data[1] = static_cast<byte>(fullBitMask >> 8); // Black keys
        data[2] = static_cast<byte>(ellapsedTime / 60); // Duration

        actor.writeData(data, 3);

        fullBitMask = newFullBitMask;
        startTime = currentTime;
    }

}


