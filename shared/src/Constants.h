//
// Created by rober on 19/11/2023.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

/// Buffer size on the actors
constexpr unsigned int BUFFER_SIZE = 1024;

/// Delay between consecutive transfers/requests to actors to prevent an interrupt during an interrupt
constexpr unsigned int TRANSMISSION_DELAY     = 5;

/// Debug value
constexpr unsigned int MAX_INSTRUMENTS        = 8;

#endif //CONSTANTS_H
