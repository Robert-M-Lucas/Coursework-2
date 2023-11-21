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

public:
    byte* getBuffer() { return buffer; }

    void writeBufferToSD(uint8_t length, Instrument instrument) {}

    byte* loadInstrumentDataIntoBuffer(Instrument instrument, uint8_t length) {
        return buffer;
    }
};

#endif //SHARED_CONTROLLERSTORAGE_H
