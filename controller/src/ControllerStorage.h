//
// Created by Benny on 20/11/2023.
//

#ifndef CONTROLLER_CONTROLLERSTORAGE_H
#define CONTROLLER_CONTROLLERSTORAGE_H

#include <Arduino.h>

#include "../../shared/src/Constants.h"
#include "../../shared/src/Communication.h"

class ControllerStorage {
public:
    ControllerStorage();

    // Set the current song to be recorded or played
    void selectSong(u8 song);

    // Returns a pointer to the internal storage buffer
    byte* getBuffer() { return buffer; }

    // Append data in the song buffer to the correct file on the SD card
    void storeBufferToDisk(u8 length, Instrument instrument) const;

    // Returns true if the song exists on disk
    bool hasSongOnDisk(u8 song) const;

    // Delete a song from the disk
    bool deleteSong(u8 song) const;

    // Reset the playback to start at the beginning of the file
    void resetPlayback();

    // Loads data from the SD card and stores it in the instrument file
    // Data is loaded from the current playback position
    // The loaded data is stored in the buffer, meaning that it is only valid until another
    // operation overwrites the buffer
    // Returns the number of bytes loaded, which is <= lengthRequested
    u16 loadSongData(Instrument instrument, u16 lengthRequested);

private:
    // Return the file path for the specified song and instrument
    String getFilePath(u8 song, Instrument instrument) const;

    // Buffer used to hold data to be transferred between the instruments and the SD card
    byte buffer[BUFFER_SIZE] = {};

    // Position of the playback pointer in bytes, for the given instrument
    u16 playbackPosition[MAX_INSTRUMENTS];

    u8 currentSong = 0;
};

#endif //CONTROLLER_CONTROLLERSTORAGE_H
