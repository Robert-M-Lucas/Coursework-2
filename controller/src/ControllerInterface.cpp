//
// Created by robertlucas on 27/11/23.
//

#include <Arduino.h>
#include "ControllerInterface.h"
#include "ControllerConstants.h"

ControllerInterface::ControllerInterface(ControllerStorage *storage, ControllerCommunication *communication):
storage(storage), communication(communication),
lcd(LCD::RS, LCD::ENABLE, LCD::D0, LCD::D1, LCD::D2, LCD::D3) {}


void ControllerInterface::init() {
    Serial.println(F("[INFO] [ControllerInterface] Initialising"));

    pinMode(LEFT_INPUT, INPUT_PULLUP);
    pinMode(RIGHT_INPUT, INPUT_PULLUP);
    pinMode(SELECT_INPUT, INPUT_PULLUP);

    lcd.begin(16, 2);

    if (!storage->loaded()) {
        lcd.setCursor(4, 0);
        lcd.print(F("SD ERROR"));
        return;
    }

    lcd.setCursor(0, 0);
    lcd.print(F("SD OK"));
    lcd.setCursor(1, 0);
    lcd.print(F("Instruments: "));
    lcd.setCursor(13, 0);
    lcd.print(communication->countConnected());

    delay(3000);

    updateLCD();
}

void ControllerInterface::onLeft() {
    if (!songSelected) {
        if (song == 1) { song = SONG_COUNT; }
        else { song--; }
        updateLCD();
    }
    else {
        if (!recordingSelected && !playbackSelected)  {
            playbackSelected = true;
            updateLCD();
        }
    }
}

void ControllerInterface::onRight() {
    if (!songSelected) {
        if (song == SONG_COUNT) { song = 1; }
        else { song++; }
        updateLCD();
    }
    else {
        if (!recordingSelected && !playbackSelected)  {
            recordingSelected = true;
            updateLCD();
        }
    }
}

void ControllerInterface::onSelect() {
    if (!songSelected) {
        songSelected = true;
        updateLCD();
    }
    else {
        if (recordingSelected) {
            recordingSelected = false;
        }
        else if (playbackSelected) {
            playbackSelected = false;
        }
        else {
            songSelected = false;
        }
        updateLCD();
    }
}

void ControllerInterface::updateLCD() {
    lcd.clear();

    if (!songSelected) {
        lcd.setCursor(0, 0);
        lcd.print(F("Song:"));
        lcd.print(song);
        if (storage->hasSongOnDisk(song)) {
            lcd.setCursor(8, 0);
            lcd.print(F("Has Data"));
        }
        else {
            lcd.setCursor(9, 0);
            lcd.print(F("No Data"));
        }
        lcd.setCursor(0, 1);
        lcd.print('<');
        lcd.setCursor(4, 1);
        lcd.print(F("[Select]"));
        lcd.setCursor(15, 1);
        lcd.print('>');
    }
    else {
        lcd.setCursor(0, 0);
        lcd.print(F("Song:"));
        lcd.print(song);

        if (playbackSelected) {
            lcd.setCursor(8, 0);
            lcd.print(F("Has Data"));

            lcd.setCursor(0, 1);
            lcd.print(F("Playing [STOP]"));
        }
        else if (recordingSelected) {
            lcd.setCursor(8, 0);
            lcd.print(F("Has Data"));

            lcd.setCursor(0, 1);
            lcd.print(F("Recording [STOP]"));
        }
        else {
            if (storage->hasSongOnDisk(song)) {
                lcd.setCursor(8, 0);
                lcd.print(F("Has Data"));
            }
            else {
                lcd.setCursor(9, 0);
                lcd.print(F("No Data"));
            }

            lcd.setCursor(0, 1);
            lcd.print(F("Play< [Bck] >Rec"));
        }
    }
}

void ControllerInterface::updateButtons() {
    // Use !digital read as LOW is read when a button with a pull-up resistor is read
    bool left = !digitalRead(LEFT_INPUT);
    bool right = !digitalRead(RIGHT_INPUT);
    bool select = !digitalRead(SELECT_INPUT);

    if (left && !prevLeft) { onLeft(); }
    if (right && !prevRight) { onRight(); }
    if (select && !prevSelect) { onSelect(); }

    prevLeft = left;
    prevRight = right;
    prevSelect = select;
}

void ControllerInterface::update() {
    updateButtons();
}