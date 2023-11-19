//
// Created by rober on 19/11/2023.
//

#ifndef DATASTOREACTOR_H
#define DATASTOREACTOR_H

#include <Arduino.h>

#include "Constants.h"

class DataStoreActorInterface {
public:
    virtual ~DataStoreActorInterface() = default;
    virtual void stopRecording();
};

class DataStoreActor final : public DataStoreActorInterface {
private:
    byte storage[INSTRUMENT_STORAGE_SIZE] = {};
    bool isRecording = false;

public:
    void stopRecording() override {
        isRecording = false;
    }
};

#endif //DATASTOREACTOR_H
