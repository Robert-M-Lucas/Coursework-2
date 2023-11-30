//
// Created by Benny on 27/11/2023.
//
#include "ControllerStorage.h"

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include "ControllerConstants.h"

ControllerStorage::ControllerStorage() {
}

void ControllerStorage::init() {
    Serial.println("Initialising SD");
    // Initialise SD library
    bool ok = SD.begin(CHIP_SELECT);
    if (ok) {
        Serial.println("[ControllerStorage] SD card initialised successfully");
    } else {
        Serial.println("[ControllerStorage] Failed to initialise SD card!");
    }

    // Reset playback to begin with
    resetPlayback();
}

String ControllerStorage::getFilePath(const u8 song, const Instrument instrument) const {
    auto instrumentIndex = static_cast<u8>(instrument);
    return String(song) + "/" + String(instrumentIndex) + ".dat";
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
    return SD.rmdir(String(song));
}

bool ControllerStorage::hasSongOnDisk(const u8 song) const {
    // A song exists if there is a folder for it - a song folder will never be created
    // except when writing data to it
    return SD.exists(String(song));
}

u16 ControllerStorage::loadSongData(const Instrument instrument, const u16 lengthRequested) {
    // Open file for reading
    auto path = getFilePath(currentSong, instrument);
    auto file = SD.open(path, FILE_READ);

    // Move to the position in the file corresponding to the current playback position
    auto instrumentIndex = static_cast<u8>(instrument);
    file.seek(playbackPosition[instrumentIndex]);

    // Query file for the number of remaining bytes to read
    // The actual number of bytes to read is the minimum of this and the length requested
    auto length = static_cast<u16>(file.available());
    length = min(length, lengthRequested);

    // Read song data into buffer, using buffered read for efficiency
    file.read(
            reinterpret_cast<void*>(buffer),
            length
    );

    // Update playback position so that the next call to loadSongData starts at the next segment
    playbackPosition[instrumentIndex] += length;

    // Make sure to close the file now we're finished
    file.close();

    return length;
}

void ControllerStorage::resetPlayback() {
    for (u16 &i : playbackPosition) {
        i = 0;
    }
}

