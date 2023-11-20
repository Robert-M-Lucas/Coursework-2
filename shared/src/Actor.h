//
// Created by rober on 19/11/2023.
//

#ifndef DATASTOREACTOR_H
#define DATASTOREACTOR_H

#include <Arduino.h>

#include "Constants.h"

struct ArrAndOffset {
    byte* arr;
    unsigned* offset;
};

class ActorInterface {
public:
    virtual ~ActorInterface() = default;
    virtual void startRecording();
    virtual void stopRecording();
    virtual unsigned getBufferLength();
    virtual ArrAndOffset getBufferRead();

    virtual unsigned getBufferEmpty();
    virtual ArrAndOffset getBufferWrite();
    virtual void startPlayback();
    virtual void stopPlayback();

    virtual void writeSongData(unsigned index, byte data);
};

class Actor final : public ActorInterface {
private:
    byte buffer[INSTRUMENT_BUFFER_SIZE] = {};
    unsigned bufferHead = 0;
    unsigned bufferTail = 0;
    bool isRecording = false;
    bool isPlayingBack = false;

public:
    void startRecording() override {
        isRecording = true;
    }

    void stopRecording() override {
        isRecording = false;
    }

    unsigned getBufferLength() override {
        if (bufferTail > bufferHead) {
            return bufferTail - bufferHead;
        }
        else {
            return bufferTail + (INSTRUMENT_BUFFER_SIZE - bufferHead);
        }
    }

    ArrAndOffset getBufferRead() override {
        return ArrAndOffset {buffer, &bufferHead };
    }

    unsigned getBufferEmpty() override {
        return INSTRUMENT_BUFFER_SIZE - getBufferLength() - 1;
    }

    ArrAndOffset getBufferWrite() override {
        return ArrAndOffset {buffer, &bufferTail };
    }

    void startPlayback() override {
        isPlayingBack = true;
    }

    void stopPlayback() override {
        isPlayingBack = false;
    }

    void writeSongData(const unsigned index, const byte data) override {
        if (isRecording) { Serial.println("Data is being written during recording!"); }
        buffer[index] = data;
    }
};

#endif //DATASTOREACTOR_H
