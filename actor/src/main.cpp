#include <Arduino.h>
#include <Wire.h>

void requestEvent() {
    Wire.write("hello ");
}

void setup() {
    Wire.begin(8);
    Wire.onRequest(requestEvent);
}

void loop() {
    delay(100);
}