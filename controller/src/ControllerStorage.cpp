//
// Created by Benny on 27/11/2023.
//
#include "ControllerStorage.h"
#include "ControllerConstants.h"

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include "ControllerConstants.h"

void ControllerStorage::init() {
    Serial.println(F("[INFO] [ControllerStorage] Initialising"));
    Serial.println(F("[INFO] [ControllerStorage] Initialising SD"));

    // Initialise SD library
    const bool ok = SD.begin(CHIP_SELECT);
    if (ok) {
        Serial.println(F("[INFO] [ControllerStorage] SD card initialised successfully"));
    } else {
        Serial.println(F("[ERROR] [ControllerStorage] Failed to initialise SD card!"));
    }

    // Reset playback to begin with
    resetPlayback();
}

String ControllerStorage::getFilePath(const u8 song, const Instrument instrument) {
    const auto instrumentIndex = static_cast<u8>(instrument);
    return String(song) + "/" + String(instrumentIndex) + ".DAT";
}

void ControllerStorage::selectSong(const u8 song) {
    currentSong = song;
}

void ControllerStorage::storeBufferToDisk(const u8 length, const Instrument instrument) const {
    // Get file path
    auto path = getFilePath(currentSong, instrument);

    // Make sure that folder for song exists
    if (!hasSongOnDisk(currentSong)) {
        SD.mkdir(String(currentSong));
    }

    // Open file for writing
    auto file = SD.open(path, FILE_WRITE);

    // Write data to file
    file.write(buffer, length);

    file.close();
}

bool ControllerStorage::deleteSong(const u8 song) const {
    return SD.rmdir("1");
}

bool ControllerStorage::hasSongOnDisk(const u8 song) const {
    // A song exists if there is a folder for it - a song folder will never be created
    // except when writing data to it
    return SD.exists("1");
}

u16 ControllerStorage::loadSongData(const Instrument instrument, const u16 lengthRequested) {
    // Open file for reading
    // TODO:
//    auto path = getFilePath(currentSong, instrument);
//    Serial.print("Path: ");
//    Serial.println(path);
    Serial.println(F("1"));
    auto file = SD.open("1/2.DAT", FILE_READ);
    Serial.println(F("2"));
    // Move to the position in the file corresponding to the current playback position
    auto instrumentIndex = static_cast<u8>(instrument);
//    file.seek(playbackPosition[instrumentIndex]);
    Serial.println(F("3"));
    // Query file for the number of remaining bytes to read
    // The actual number of bytes to read is the minimum of this and the length requested
    auto length = static_cast<u16>(file.available());
    Serial.println(length);
    length = min(length, lengthRequested);
    Serial.println(length);
    // Read song data into buffer, using buffered read for efficiency
    file.read(
            reinterpret_cast<void*>(buffer),
            length
    );
    Serial.println(F("5"));
    // Update playback position so that the next call to loadSongData starts at the next segment
    playbackPosition[instrumentIndex] += length;
    Serial.println(F("6"));
    // Make sure to close the file now we're finished
    file.close();
    Serial.println(F("7"));
    return length;
}

void ControllerStorage::resetPlayback() {
    for (u16 &i : playbackPosition) {
        i = 0;
    }
}

