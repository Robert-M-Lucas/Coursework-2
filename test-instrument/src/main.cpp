#include <Arduino.h>
#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Actor.h"

DefaultActor actor;

char buffer[1024];
int i = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("hello");
    CommunicationActor::initialise(Instrument::TestInstrument, &actor);
}

void loop() {
    if (actor.getPlayback()) {
        for (int j = 0; j < 50; ++j) {
            if (actor.readDataAvailable(1)) {
                if (i > 990) {
                    Wire.end();
                    // finished buffer
                    buffer[i] = '\0';
                    Serial.println("printing data");
                    Serial.println(buffer);
                    i = 0;
                    return;
                } else {
                    byte data;
                    actor.readDataAndRemove(&data, 1);
                    buffer[i] = static_cast<char>(data);
                    i++;
                }
            } else {
                Serial.println("Ran out of data");
                Serial.println(actor.getBufferLength());
                break;
            }
        }

        if (i > 1000) {
            // finished buffer
            buffer[i++] = '\0';
            Serial.println(buffer);
        }
        delay(100);
    }
}