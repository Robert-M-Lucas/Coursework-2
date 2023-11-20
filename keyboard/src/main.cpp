#include <Arduino.h>

// Wire Peripheral Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI peripheral device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include "../../shared/src/CommunicationActor.h"
#include "../../shared/src/Actor.h"

Actor actor = Actor();

void setup() {
    CommunicationActor::initialise(Instrument::Keyboard, &actor);
}

void loop() {

}
