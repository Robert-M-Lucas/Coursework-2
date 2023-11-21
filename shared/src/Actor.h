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
    /// Starts recording
    virtual void startRecording();
    /// Stops recording
    virtual void stopRecording();
    /// Returns the current length of the buffer
    virtual unsigned getBufferLength();
    /// Returns the buffer and the head pointer
    virtual ArrAndOffset getBufferRead();

    /// Empties the buffer
    virtual void clearBuffer();
    /// Returns the empty space in the buffer
    virtual unsigned getBufferEmpty();
    /// Returns the buffer nad the tail pointer
    virtual ArrAndOffset getBufferWrite();
    /// Starts playback
    virtual void startPlayback();
    /// Stops playback
    virtual void stopPlayback();

    /// Writes data to the buffer
    virtual void writeData(byte* data, unsigned length);
};

class DefaultActor : public ActorInterface {
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
        if (bufferTail >= bufferHead) {
            return bufferTail - bufferHead;
        }
        else {
            return bufferTail + (INSTRUMENT_BUFFER_SIZE - bufferHead);
        }
    }

    ArrAndOffset getBufferRead() override {
        return ArrAndOffset {buffer, &bufferHead };
    }

    void clearBuffer() override {
        bufferHead = 0;
        bufferTail = 0;
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

    void writeData(byte *data, unsigned int length) override {
        if (isPlayingBack) { return; }
        for (unsigned i = 0; i < length; i++) {
            unsigned index = i;
            if (index >= INSTRUMENT_BUFFER_SIZE) { index -= INSTRUMENT_BUFFER_SIZE; }

            buffer[index] = data[i];
        }
        bufferTail += length;
        if (bufferTail >= INSTRUMENT_BUFFER_SIZE) {
            bufferTail -= INSTRUMENT_BUFFER_SIZE;
        }
    }
};

class DebugActor: public DefaultActor {
    void startRecording() override {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("Starting recording");
        DefaultActor::startRecording();
    }

    void stopRecording() override {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("Stopping recording");
        DefaultActor::stopRecording();
    }
};

#endif //DATASTOREACTOR_H
