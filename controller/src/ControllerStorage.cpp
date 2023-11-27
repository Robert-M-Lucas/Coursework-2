//
// Created by Benny on 27/11/2023.
//
#include "ControllerStorage.h"

ControllerStorage::ControllerStorage() {
    // Init
    SD.begin(CHIP_SELECT);
}

String ControllerStorage::getFilePath(uint8_t song, Instrument instrument) const {
    auto instrumentIndex = static_cast<uint8_t>(instrument);
    return String(song) + "/" + String(instrumentIndex) + ".dat";
}

void ControllerStorage::storeBufferToDisk(uint8_t length, Instrument instrument) const {
    // Get file path
    auto path = getFilePath(currentSong, instrument);

    // Make sure that folder for song exists
    if (!hasSongOnDisk(currentSong)) {
        SD.mkdir(String(currentSong));
    }

    // Open file for writing
    auto file = SD.open(path.c_str(), FILE_WRITE);

    // Write data to file
    file.write(buffer, length);
}

bool ControllerStorage::hasSongOnDisk(uint8_t song) {
    // A song exists if there is a folder for it - a song folder will never be created
    // except when writing data to it
    return SD.exists(String(song));
}

void ControllerStorage::selectSong(const uint8_t song) {
    currentSong = song;
}
