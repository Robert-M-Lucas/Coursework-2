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
    // Buffer used to hold data to be transferred between the instruments and the SD card
    byte buffer[BUFFER_SIZE] = {};

    char path_buf[10] = {};

    // Position of the playback pointer in bytes, for the given instrument
    u16 playbackPosition[MAX_INSTRUMENTS] = {};

    u8 currentSong = 0;

    static uint8_t write_num_to_buffer_pos(char* buffer_pos, uint8_t num);

    void buffer_folder(uint8_t song);

    void buffer_file(uint8_t song, uint8_t instrument);
public:
    static void init();

    // Set the current song to be recorded or played
    void selectSong(uint8_t song);

    // Returns a pointer to the internal storage buffer
    byte* getBuffer() { return buffer; }

    // Append data in the song buffer to the correct file on the SD card
    void storeBufferToDisk(Instrument instrument, uint8_t length);

    // Returns true if the song exists on disk
    bool hasSongOnDisk(uint8_t song);

    // Delete a song from the disk
    bool deleteSong(uint8_t song);

    // Reset the playback to start at the beginning of the file
    void resetPlayback();

    // Loads data from the SD card and stores it in the instrument file
    // Data is loaded from the current playback position
    // The loaded data is stored in the buffer, meaning that it is only valid until another
    // operation overwrites the buffer
    // Returns the number of bytes loaded, which is <= lengthRequested
    uint8_t loadSongData(Instrument instrument, uint8_t lengthRequested);

    // Return the file path for the specified song and instrument
    // static String getFilePath(u8 song, Instrument instrument) ;
};

#endif //CONTROLLER_CONTROLLERSTORAGE_H
