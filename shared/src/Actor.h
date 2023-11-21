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
    virtual uint8_t getBufferLength();
    /// Returns the buffer and the head pointer
    virtual ArrAndOffset getBufferRead();

    /// Empties the buffer
    virtual void clearBuffer();
    /// Returns the empty space in the buffer
    virtual uint8_t getBufferEmpty();
    /// Returns the buffer nad the tail pointer
    virtual ArrAndOffset getBufferWrite();
    /// Starts playback
    virtual void startPlayback();
    /// Stops playback
    virtual void stopPlayback();

    /// Writes data to the buffer
    virtual void writeData(byte* data, uint8_t length);
};

class DefaultActor : public ActorInterface {
private:
    byte buffer[BUFFER_SIZE] = {};
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

    uint8_t getBufferLength() override {
        if (bufferTail >= bufferHead) {
            return bufferTail - bufferHead;
        }
        else {
            return bufferTail + (BUFFER_SIZE - bufferHead);
        }
    }

    ArrAndOffset getBufferRead() override {
        return ArrAndOffset {buffer, &bufferHead };
    }

    void clearBuffer() override {
        bufferHead = 0;
        bufferTail = 0;
    }

    uint8_t getBufferEmpty() override {
        return BUFFER_SIZE - getBufferLength() - 1;
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

    void writeData(byte *data, uint8_t length) override {
        if (isPlayingBack) { return; }
        for (unsigned i = 0; i < length; i++) {
            unsigned index = i;
            if (index >= BUFFER_SIZE) { index -= BUFFER_SIZE; }

            buffer[index] = data[i];
        }
        if (bufferTail < bufferHead && bufferTail + length > bufferHead) {
            Serial.println("Overflowed buffer storage!");
        }
        bufferTail += length;
        if (bufferTail >= BUFFER_SIZE) {
            bufferTail -= BUFFER_SIZE;
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
