//
// Created by robertlucas on 17/11/23.
//

#ifndef SHARED_COMMUNICATION_H
#define SHARED_COMMUNICATION_H

#include <Wire.h>

enum class Instrument : uint8_t {
    Keyboard = 1
};

namespace Communication {
    void initialise_controller();

    void initialise_instrument(Instrument address);
};


#endif //SHARED_COMMUNICATION_H
