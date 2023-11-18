//
// Created by rober on 18/11/2023.
//

#ifndef COMMUNICATIONCONTROLLER_H
#define COMMUNICATIONCONTROLLER_H

#include <Wire.h>
#include "Communication.h"

namespace CommunicationController {
    inline void initialise() {
        Wire.begin();
    }
}

#endif //COMMUNICATIONCONTROLLER_H
