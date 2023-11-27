//
// Created by robertlucas on 27/11/23.
//

#include <Arduino.h>
#include "ControllerInterface.h"
#include "ControllerConstants.h"

ControllerInterface::ControllerInterface(ControllerStorage *storage):
storage(storage), lcd(LCD::RS, LCD::ENABLE, LCD::D0, LCD::D1, LCD::D2, LCD::D3) {
    pinMode(LEFT_INPUT, INPUT);
    pinMode(RIGHT_INPUT, INPUT);
    pinMode(SELECT_INPUT, INPUT);

    lcd.begin(16, 2);
    updateLCD();
}

void ControllerInterface::onLeft() {
    if (!songSelected) {
        if (song == 1) { song = SONG_COUNT; }
        else { song--; }
    }
    updateLCD();
}

void ControllerInterface::onRight() {
    if (!songSelected) {
        if (song == SONG_COUNT) { song = 1; }
        else { song++; }
    }
    updateLCD();
}

void ControllerInterface::onSelect() {
    if (!songSelected) {
        songSelected = true;
    }
    updateLCD();
}

void ControllerInterface::updateLCD() {
    lcd.clear();
    if (!songSelected) {
        lcd.setCursor(0, 0);
        lcd.print("Song: ");
        lcd.print(song);
        lcd.setCursor(0, 1);
        lcd.print('<');
        lcd.setCursor(4, 1);
        lcd.print("[Select]");
        lcd.setCursor(15, 1);
        lcd.print('>');
    }
    else {
        lcd.setCursor(0, 0);
        lcd.print("S: ");
        lcd.print(song);

        if (true) {
            lcd.setCursor(8, 0);
            lcd.print("Has Data");
        }
        else {
            lcd.setCursor(9, 0);
            lcd.print("No Data");
        }

        lcd.setCursor(0, 1);
        lcd.print("Play< [Bck] >Rec");
    }
}

void ControllerInterface::updateButtons() {
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