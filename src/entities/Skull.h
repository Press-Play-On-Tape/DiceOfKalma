#pragma once

#include <Arduino.h>

#include "../utils/Constants.h"
#include "../utils/Structs.h"

class Skull {
    
    public:
        SkullType skullType;
        boolean isMarked;
        uint8_t bones;
        uint8_t multiplier;

};
