//
// Created by rober on 19/11/2023.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef uint8_t pin;

/// Buffer size on the actors
constexpr unsigned int BUFFER_SIZE = 256;

/// Delay between consecutive transfers/requests to actors to prevent an interrupt during an interrupt
constexpr unsigned int TRANSMISSION_DELAY = 5;

constexpr unsigned int INSTRUMENT_POLL_INTERVAL = 30;

/// Debug value
constexpr unsigned int MAX_INSTRUMENTS = 8;

constexpr unsigned long MAX_NOTE_DURATION_MS = INSTRUMENT_POLL_INTERVAL * 256;

/// serial communications standardised baud rate
constexpr unsigned int SERIAL_BAUD_RATE = 9600;

#endif //CONSTANTS_H
