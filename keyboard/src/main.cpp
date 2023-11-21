#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Actor.h"
#include <Adafruit_MCP3008.h>

DebugActor actor = DebugActor();
Adafruit_MCP3008 inputAdc;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    inputAdc.begin(13,11,12,10);
    Serial.println("Init");
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

void loop() {

}
