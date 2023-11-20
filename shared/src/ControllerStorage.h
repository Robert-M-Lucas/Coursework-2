//
// Created by Benny on 20/11/2023.
//

#ifndef SHARED_CONTROLLERSTORAGE_H
#define SHARED_CONTROLLERSTORAGE_H

#include "Arduino.h"
#include "Constants.h"

class ControllerStorage {
public:


private:
    byte storage[INSTRUMENT_BUFFER_SIZE] = {};
};

#endif //SHARED_CONTROLLERSTORAGE_H
