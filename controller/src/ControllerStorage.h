//
// Created by Benny on 20/11/2023.
//

#ifndef CONTROLLER_CONTROLLERSTORAGE_H
#define CONTROLLER_CONTROLLERSTORAGE_H

#include "Arduino.h"
#include "../../shared/src/Constants.h"
#include "../../shared/src/Communication.h"

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

#endif //CONTROLLER_CONTROLLERSTORAGE_H
