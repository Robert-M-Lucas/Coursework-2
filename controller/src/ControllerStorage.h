//
// Created by Benny on 20/11/2023.
//

#ifndef CONTROLLER_CONTROLLERSTORAGE_H
#define CONTROLLER_CONTROLLERSTORAGE_H

#include "Arduino.h"
#include "SD.h"
#include "SPI.h"

#include "../../shared/src/Constants.h"
#include "../../shared/src/Communication.h"

class ControllerStorage {
private:
    byte buffer[BUFFER_SIZE] = {};

    bool write = false;

    uint8_t song = 0;

    // Return the file path for the specified song and instrument
    String getFilePath(uint8_t song, Instrument instrument) const {
        auto instrumentIndex = static_cast<uint8_t>(instrument);
        return String(song) + "/" + String(instrumentIndex) + ".dat";
    }
public:
    byte* getBuffer() { return buffer; }

    // Append data in the song buffer to the correct file on the SD card
    void storeBufferToDisk(uint8_t length, Instrument instrument) const {
        // Get file path
        auto path = getFilePath(song, instrument);

        // Make sure that folder for song exists
        if (!hasSongOnDisk(song)) {
            SD.mkdir(String(song));
        }

        // Open file for writing
        auto file = SD.open(path.c_str(), FILE_WRITE);

        // Write data to file
        file.write(buffer, length);
    }

    // Returns true if the song exists on disk
    bool hasSongOnDisk(uint8_t song) const {
        // A song exists if there is a folder for it - a song folder will never be created
        // except when writing data to it
        return SD.exists(String(song));
    }

    void selectSong(const uint8_t song) { ControllerStorage::song = song; }

    void writeMode() { write = true; }
    void readMode() { write = false; }
};

#endif //CONTROLLER_CONTROLLERSTORAGE_H
