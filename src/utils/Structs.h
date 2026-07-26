#pragma once

#include "Constants.h"

// struct GamePlayVars {

//     uint16_t bombCounter;
//     uint8_t waveCounter;
//     bool waveCleared;

//     void clone(GamePlayVars gpv) {
//         this->bombCounter = gpv.bombCounter;
//         this->waveCounter = gpv.waveCounter;
//         this->waveCleared = gpv.waveCleared;
//     }
    
// };

struct HandScore {
    uint16_t baseBones = 0;
    uint16_t totalBones = 0;
    uint8_t  multiplier = 0;
    uint16_t score = 0;
    HandType  handType = HandType::None;
    char     handName[12];
};
