//
// Created by Benny on 20/11/2023.
//

#ifndef SHARED_CONTROLLERSTORAGE_H
#define SHARED_CONTROLLERSTORAGE_H

#include "Arduino.h"
#include "Constants.h"
#include "Communication.h"

class ControllerStorage {
private:
    byte buffer[BUFFER_SIZE] = {};

    bool write = false;

    uint8_t song = 0;
public:
    byte* getBuffer() { return buffer; }

    // TODO:
    void writeBufferToSD(uint8_t length, Instrument instrument) {}

    unsigned instrumentDataAvailable(Instrument instrument);

    // TODO:
    byte* loadInstrumentDataIntoBuffer(Instrument instrument, uint8_t length) {
        return buffer;
    }

    void selectSong(const uint8_t song) { ControllerStorage::song = song; }

    void writeMode() { write = true; }
    void readMode() { write = false; }
};

#endif //SHARED_CONTROLLERSTORAGE_H
