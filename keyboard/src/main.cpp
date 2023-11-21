#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include <Adafruit_MCP3008.h>


DebugActor actor = DebugActor();
Adafruit_MCP3008 inputAdcWhiteKeys;

const int highThreshold = 512;
const byte fullByte = 255;
void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    inputAdcWhiteKeys.begin(13,11,12,10);
    Serial.println("Init");
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

bool readHigh(int reading)
{
    //test if input is above highThreshold
    return (reading > highThreshold);
}

void loop() {

    byte inputMask = fullByte;

    for(int channel = 0; channel < 8; channel++)
    {
        inputMask = inputMask & byte(readHigh(inputAdcWhiteKeys.readADC(channel)) * pow(2,channel));
    }
}


