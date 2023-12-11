//
// Created by Benny on 20/11/2023.
//

#ifndef CONTROLLER_CONTROLLERSTORAGE_H
#define CONTROLLER_CONTROLLERSTORAGE_H

#include <Arduino.h>

#include "../../shared/src/Constants.h"
#include "../../shared/src/Communication.h"

class ControllerStorage {
private:
    /// Stores whether ControllerStorage has initialised successfully
    bool initialised = false;

    /// Buffer used to hold data to be transferred between the instruments and the SD card
    byte buffer[BUFFER_SIZE] = {};

    /// Buffer used to construct file paths
    char path_buf[10] = {};

    /// Position of the playback pointer in bytes for all instruments
    u16 playbackPosition[MAX_INSTRUMENTS] = {};

    /// Current song being played
    u8 currentSong = 0;

    /// Writes a number as character into a buffer without a null terminator. Returns chars written
    static uint8_t write_num_to_buffer_pos(char* buffer_pos, uint8_t num);

    /// Writes a folder path to the path_buf
    void buffer_folder(uint8_t song);

    /// Writes a file path to the path_buf
    void buffer_file(uint8_t song, uint8_t instrument);
public:
    void init();

    /// Returns whether ControllerStorage correctly initialised
    bool is_initialised() const { return initialised; }

    /// Set the current song to be recorded or played
    void selectSong(uint8_t song);

    /// Returns a pointer to the internal storage buffer
    byte* getBuffer() { return buffer; }

    /// Append data in the song buffer to the correct file on the SD card
    void storeBufferToDisk(Instrument instrument, uint8_t length);

    /// Returns true if the song exists on disk
    bool hasSongOnDisk(uint8_t song);

    /// Delete a song from the disk. Returns success state
    bool deleteSong(uint8_t song);

    /// Reset the playback to start at the beginning of the file
    void resetPlayback();

    /// Loads data from the SD card and stores it in the instrument file
    /// Data is is_initialised from the current playback position
    /// The is_initialised data is stored in the buffer, meaning that it is only valid until another
    /// operation overwrites the buffer
    /// Returns the number of bytes is_initialised, which is <= lengthRequested
    uint8_t loadSongData(Instrument instrument, uint8_t lengthRequested);

    // Return the file path for the specified song and instrument
    // static String getFilePath(u8 song, Instrument instrument) ;

    /// Deletes all folders on the SD card. Debug function.
    void wipeDrive();
};

#endif //CONTROLLER_CONTROLLERSTORAGE_H
