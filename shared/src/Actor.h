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
    virtual void startRecording() = 0;
    /// Stops recording
    virtual void stopRecording() = 0;
    /// Returns the current length of the buffer
    virtual uint8_t getBufferLength() = 0;
    /// Returns the buffer and the head pointer
    virtual ArrAndOffset getBufferRead() = 0;

    /// Empties the buffer
    virtual void clearBuffer() = 0;
    /// Returns the empty space in the buffer
    virtual uint8_t getBufferSpaceRemaining() = 0;
    /// Returns the buffer nad the tail pointer
    virtual ArrAndOffset getBufferWrite() = 0;
    /// Starts playback
    virtual void startPlayback() = 0;
    /// Stops playback
    virtual void stopPlayback() = 0;

    /// Writes data to the buffer
    virtual void writeData(byte* data, uint8_t length) = 0;

    /// Checks if there is data to read from the buffer
    virtual bool readDataAvailable(uint8_t length) = 0;
    /// Reads data from the buffer
    virtual void readDataAndRemove(byte* destination, uint8_t length) = 0;

    /// Returns whether a recording is in progress
    virtual bool getRecording() = 0;
    /// Returns whether a playback is in progress
    virtual bool getPlayback() = 0;
};

class DefaultActor : public ActorInterface {
private:
    /// Circular data buffer
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

    uint8_t getBufferSpaceRemaining() override {
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

    void writeData(byte *data, const uint8_t length) override {
        if (isPlayingBack) { return; }

        // Write to circular buffer
        for (unsigned i = 0; i < length; i++) {
            unsigned index = bufferTail + i;
            if (index >= BUFFER_SIZE) { index -= BUFFER_SIZE; }

            buffer[index] = data[i];
        }
        if (bufferTail < bufferHead && bufferTail + length > bufferHead) {
            Serial.println(F("Overflowed buffer storage!"));
        }
        bufferTail += length;
        if (bufferTail >= BUFFER_SIZE) {
            bufferTail -= BUFFER_SIZE;
        }
    }

    bool readDataAvailable(const uint8_t length) override {
        return getBufferLength() >= length;
    }

    void readDataAndRemove(byte* destination, const uint8_t amount) override {
        // Read from circular buffer
        for (uint8_t i = 0; i < amount; i++) {
            unsigned index = bufferHead + i;
            if (index >= BUFFER_SIZE) {
                index -= BUFFER_SIZE;
            }

            destination[i] = buffer[index];
        }

        bufferHead += amount;
        if (bufferHead >= BUFFER_SIZE) {
            bufferHead -= BUFFER_SIZE;
        }
    }

    bool getRecording() override {
        return isRecording;
    }

    bool getPlayback() override {
        return isPlayingBack;
    }
};

class LEDActor final : public DefaultActor {
private:
    uint8_t recordingLED;
    uint8_t playbackLED;
public:
    LEDActor(const uint8_t recordingLED, const uint8_t playbackLED) :
        recordingLED(recordingLED), playbackLED(playbackLED) {
        pinMode(recordingLED, OUTPUT);
        pinMode(playbackLED, OUTPUT);
    }

    void startRecording() override {
        digitalWrite(recordingLED, HIGH);
        DefaultActor::startRecording();
    }

    void stopRecording() override {
        digitalWrite(recordingLED, LOW);
        DefaultActor::stopRecording();
    }

    void startPlayback() override {
        digitalWrite(playbackLED, HIGH);
        DefaultActor::startPlayback();
    }

    void stopPlayback() override {
        digitalWrite(playbackLED, LOW);
        DefaultActor::stopPlayback();
    }
};

#endif //DATASTOREACTOR_H
