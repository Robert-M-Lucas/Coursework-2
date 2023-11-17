#include <Arduino.h>

// Wire Peripheral Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI peripheral device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void requestEvent();
void receiveEvent(int length);

void setup() {
    Serial.begin(9600);
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent); // register event
    Serial.println("Init");
}

void loop() {
    delay(100);
}

void receiveEvent(int length) {
    while(0 < Wire.available()) // loop through all but the last
    {
        char c = static_cast<char>(Wire.read()); // receive byte as a character
        Serial.print(c);         // print the character
    }

    Serial.println();
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
    Wire.write("hello "); // respond with message of 6 bytes
    // as expected by master
}