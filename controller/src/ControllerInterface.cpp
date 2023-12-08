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
    lcd.setCursor(0, 1);
    lcd.print(F("Instruments: "));
    lcd.setCursor(13, 1);
    lcd.print(communication->countConnected());

    delay(2000);

    updateLCD();
}

void ControllerInterface::onLeft() {
    if (!songSelected) { // Cycle through songs
        if (song == 1) { song = SONG_COUNT; }
        else { song--; }
        updateLCD();
    }
    else { // Start playback
        if (!recordingSelected && !playbackSelected && storage->hasSongOnDisk(song))  {
            playbackSelected = true;
            updateLCD();
        }
    }
}

void ControllerInterface::onRight() {
    if (!songSelected) { // Cycle through songs
        if (song == SONG_COUNT) { song = 1; }
        else { song++; }
        updateLCD();
    }
    else { // Start recording
        if (!recordingSelected && !playbackSelected)  {
            recordingSelected = true;
            updateLCD();
        }
    }
}

void ControllerInterface::onSelect() {
    if (!songSelected) { // Select song
        songSelected = true;
        updateLCD();
    }
    else {
        if (recordingSelected) { // Stop recording
            recordingSelected = false;
        }
        else if (playbackSelected) { // Stop playback
            playbackSelected = false;
        }
        else { // Go back to song selection
            songSelected = false;
        }
        updateLCD();
    }
}

void ControllerInterface::updateLCD() {
    lcd.clear();

    if (!songSelected) { // Show song details while selecting
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
    else { // Show selected song and options
        lcd.setCursor(0, 0);
        lcd.print(F("Song:"));
        lcd.print(song);

        if (playbackSelected) { // Show options to stop playback
            lcd.setCursor(8, 0);
            lcd.print(F("Has Data"));

            lcd.setCursor(0, 1);
            lcd.print(F("Playing [STOP]"));
        }
        else if (recordingSelected) { // Show options to stop recording
            lcd.setCursor(8, 0);
            lcd.print(F("Has Data"));

            lcd.setCursor(0, 1);
            lcd.print(F("Recording [STOP]"));
        }
        else {
            // Show whether a song has data
            if (storage->hasSongOnDisk(song)) {
                lcd.setCursor(8, 0);
                lcd.print(F("Has Data"));

                lcd.setCursor(0, 1);
                lcd.print(F("Play< [Bck] >Rec"));
            }
            else {
                lcd.setCursor(9, 0);
                lcd.print(F("No Data"));

                lcd.setCursor(0, 1);
                lcd.print(F("      [Bck] >Rec"));
            }
        }
    }
}

void ControllerInterface::updateButtons() {
    // Use !digitalRead as LOW is read when a button with a pull-up resistor is pressed
    bool left = !digitalRead(LEFT_INPUT);
    bool right = !digitalRead(RIGHT_INPUT);
    bool select = !digitalRead(SELECT_INPUT);

    if (left && right && select) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hold for 3s");
        lcd.setCursor(0, 1);
        lcd.print("To wipe drive");
        delay(1000);
        lcd.setCursor(0, 0);
        lcd.print("Hold for 2s");
        lcd.setCursor(0, 1);
        lcd.print("To wipe drive");
        delay(1000);
        lcd.setCursor(0, 0);
        lcd.print("Hold for 1s");
        lcd.setCursor(0, 1);
        lcd.print("To wipe drive");
        delay(1000);

        left = !digitalRead(LEFT_INPUT);
        right = !digitalRead(RIGHT_INPUT);
        select = !digitalRead(SELECT_INPUT);


        if (left && right && select) {
            storage->wipeDrive();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Done");
            delay(2000);
            updateLCD();
            return;
        }
        else {
            updateLCD();
            return;
        }
    }

    if (left && right) { return; }

    // On button down. De-bouncing handled by delay in main loop
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