//
// Created by Benny on 20/11/2023.
//

#ifndef CONTROLLER_CONTROLLERSTORAGE_H
#define CONTROLLER_CONTROLLERSTORAGE_H

#include "Arduino.h"
#include "SD.h"
#include "SPI.h"

#include "ControllerConstants.h"

#include "../../shared/src/Constants.h"
#include "../../shared/src/Communication.h"

class ControllerStorage {
private:
    byte buffer[BUFFER_SIZE] = {};

    uint8_t currentSong = 0;

    // Return the file path for the specified song and instrument
    String getFilePath(uint8_t song, Instrument instrument) const;
public:
    ControllerStorage();

    // Returns a pointer to the internal storage buffer
    byte* getBuffer() { return buffer; }

    // Append data in the song buffer to the correct file on the SD card
    void storeBufferToDisk(uint8_t length, Instrument instrument) const;

    //
    bool fillBufferFromDisk() {

    }

    // Returns true if the song exists on disk
    static bool hasSongOnDisk(uint8_t song);

    void selectSong(const uint8_t song);
};

#endif //CONTROLLER_CONTROLLERSTORAGE_H
