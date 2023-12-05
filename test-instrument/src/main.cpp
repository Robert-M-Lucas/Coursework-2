#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"

DefaultActor actor;

void setup() {
    Serial.begin(9600);
    Serial.println("hello");
    CommunicationActor::initialise(Instrument::TestInstrument, &actor);
}

void loop() {
    if (actor.getPlayback()) {
        for (int i = 0; i < 50; ++i) {
            if (actor.readDataAvailable(1)) {
                byte data;
                actor.readDataAndRemove(&data, 1);
                Serial.print(static_cast<char>(data));
            } else {
                Serial.println("Ran out of data");
                Serial.println(actor.getBufferLength());
                break;
            }
        }

        delay(1000);
    }
}