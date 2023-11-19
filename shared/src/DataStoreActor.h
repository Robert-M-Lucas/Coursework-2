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
    virtual void startRecording();
    virtual void stopRecording();
    virtual unsigned getSongLength();
    virtual byte* getSongStorage();

    virtual void writeSongData(unsigned index, byte data);
};

class DataStoreActor final : public DataStoreActorInterface {
private:
    byte storage[INSTRUMENT_STORAGE_SIZE] = {};
    unsigned songLength = 0;
    bool isRecording = false;

public:
    void startRecording() override {
        isRecording = true;
    }

    void stopRecording() override {
        isRecording = false;
    }

    unsigned getSongLength() override {
        return songLength;
    }

    byte* getSongStorage() override {
        return storage;
    }

    void writeSongData(const unsigned index, const byte data) override {
        if (isRecording) { Serial.println("Data is being written during recording!"); }
        storage[index] = data;
    }
};

#endif //DATASTOREACTOR_H
