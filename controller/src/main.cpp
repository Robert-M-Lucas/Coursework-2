#include <Arduino.h>
#include <Wire.h>

void setup() {
    Wire.begin();
    Serial.begin(9600);
}

void loop() {
    // write

    Wire.beginTransmission(8);
    Wire.write("hello Rob");
    Wire.endTransmission();

    delay(500);

    // read

    Wire.requestFrom(8, 6);

    while (Wire.available()) {
        char c = static_cast<char>(Wire.read());
        Serial.print(c);
    }

    delay(500);
}