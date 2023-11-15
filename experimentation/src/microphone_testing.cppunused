#include <Arduino.h>

unsigned long last = 0;

void setup() {
    Serial.begin(9600);
    pinMode(A0, INPUT);

    last = millis();
}

unsigned long interval = 10;

void loop() {
    auto value = 0;

    for (int i = 0; i < 100; i++) {
        value = max(value, analogRead(A0));
    }

    if (value != 0) {
        Serial.println(value);
    }

    auto current = millis();
    if (current - last < interval) {
        delay(interval - (millis() - last));
    }
    last = current;
}