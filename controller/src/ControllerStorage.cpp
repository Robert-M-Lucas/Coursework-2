//
// Created by Benny on 27/11/2023.
//
#include "ControllerStorage.h"
#include "ControllerConstants.h"

#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h>

#include "ControllerConstants.h"

SdFat SD;

uint8_t ControllerStorage::write_num_to_buffer_pos(char* buffer_pos, uint8_t num) {
    uint8_t i = 0;
    while (true) {
        // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
        switch (num % 10) {
            case 0:
                buffer_pos[i] = '0';
                break;
            case 1:
                buffer_pos[i] = '1';
                break;
            case 2:
                buffer_pos[i] = '2';
                break;
            case 3:
                buffer_pos[i] = '3';
                break;
            case 4:
                buffer_pos[i] = '4';
                break;
            case 5:
                buffer_pos[i] = '5';
                break;
            case 6:
                buffer_pos[i] = '6';
                break;
            case 7:
                buffer_pos[i] = '7';
                break;
            case 8:
                buffer_pos[i] = '8';
                break;
            case 9:
                buffer_pos[i] = '9';
                break;
        }
        i++;
        num /= 10;
        if (num == 0) { break; }
    }
    return i;
}

void ControllerStorage::buffer_folder(const uint8_t song) {
    const uint8_t pos = write_num_to_buffer_pos(path_buf, song);
    path_buf[pos] = '\0'; // Null terminator
}

void ControllerStorage::buffer_file(const uint8_t song, const uint8_t instrument) {
    uint8_t pos = write_num_to_buffer_pos(path_buf, song);
    path_buf[pos] = '/';
    pos++;
    pos += write_num_to_buffer_pos(path_buf + pos, instrument);
    path_buf[pos] = '.';
    path_buf[pos+1] = 'D';
    path_buf[pos+2] = 'A';
    path_buf[pos+3] = 'T';
    path_buf[pos+4] = '\0'; // Null terminator
}

void ControllerStorage::init() {
    Serial.println(F("[INFO] [ControllerStorage] Initialising"));
    Serial.println(F("[INFO] [ControllerStorage] Initialising SD"));

    // Initialise SD library
    const bool ok = SD.begin(CHIP_SELECT);
    if (ok) {
        load = true;
        Serial.println(F("[INFO] [ControllerStorage] SD card initialised successfully"));
    } else {
        load = false;
        Serial.println(F("[ERROR] [ControllerStorage] Failed to initialise SD card!"));
    }
}

void ControllerStorage::selectSong(const uint8_t song) {
    currentSong = song;
}

void ControllerStorage::storeBufferToDisk(Instrument instrument, uint8_t length) {
    // Make sure that folder for song exists
    if (!hasSongOnDisk(currentSong)) {
        buffer_folder(currentSong); // Theoretically redundant
        SD.mkdir(path_buf);
    }

    // Open file for writing
    buffer_file(currentSong, static_cast<uint8_t>(instrument));
    auto file = SD.open(path_buf, FILE_WRITE);

    // Write data to file
    file.write(buffer, length);
    // Close file
    file.close();
}

bool ControllerStorage::deleteSong(const uint8_t song) {
    for (uint8_t i = 0; i < MAX_INSTRUMENTS; i++) {
        buffer_file(song, i);
        SD.remove(path_buf);
    }
    buffer_folder(song);
    return SD.rmdir(path_buf);
}

bool ControllerStorage::hasSongOnDisk(const uint8_t song) {
    // A song exists if there is a folder for it - a song folder will never be created
    // except when writing data to it
    buffer_folder(song);
    return SD.exists(path_buf);
}

uint8_t ControllerStorage::loadSongData(const Instrument instrument, const uint8_t lengthRequested) {
    // Open file for reading
    buffer_file(currentSong, static_cast<uint8_t>(instrument));
    auto file = SD.open(path_buf, FILE_READ);

    Serial.println(path_buf);

    // Move to the position in the file corresponding to the current playback position
    const auto instrumentIndex = static_cast<uint8_t>(instrument);
    file.seek(playbackPosition[instrumentIndex]);
    Serial.println(playbackPosition[instrumentIndex]);

    // Query file for the number of remaining bytes to read
    // The actual number of bytes to read is the minimum of this and the length requested
    auto length = static_cast<u16>(file.available());

    // Catch file not existing
    if (length == 0) { return 0; }

    length = min(length, static_cast<u16>(lengthRequested));

    // Read song data into buffer, using buffered read for efficiency
    file.read(
            buffer,
            length
    );

    // Update playback position so that the next call to loadSongData starts at the next segment
    playbackPosition[instrumentIndex] += length;

    // Make sure to close the file now we're finished
    file.close();

    return static_cast<uint8_t>(length);
}

void ControllerStorage::resetPlayback() {
    for (u16 &i : playbackPosition) {
        i = 0;
    }
}

void ControllerStorage::wipeDrive() {
    for (uint8_t i = 1; i <= SONG_COUNT; i++) {
        deleteSong(i);
    }
}

