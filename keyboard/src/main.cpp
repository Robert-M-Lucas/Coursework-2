#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Actor.h"

DebugActor actor = DebugActor();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    Serial.println("Init");
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

void loop() {

}
