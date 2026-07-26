#pragma once

#include <Arduboy2.h>

// - Debug ------------------------------

#define _DEBUG

#define _DEBUG_PRINT    Serial.print
#define _DEBUG_PRINTLN  Serial.println
#define DEBUG_PRINT    void
#define DEBUG_PRINTLN  void


// ---------------------------------------


#define MAX_DECK 6
// cursor 0-4 = dice, CURSOR_REROLL = the REROLL button, CURSOR_PLAY = the PLAY button
#define CURSOR_PLAY   5
#define CURSOR_DECK 6
#define CURSOR_REROLL 7

namespace Constants {

    // constexpr uint16_t EEPROM_Start         = EEPROM_STORAGE_SPACE_START + 143;
    constexpr uint8_t ParticlesMax = 40;
}

// ---------------------------------------------------------------------
// SKULLS - the deckbuilding layer.
// Each skull is a passive that triggers off hand type / dice values /
// how the hand was played. Skulls are stackable: owning 2 copies of a
// mult skull doubles its bonus, etc.
// ---------------------------------------------------------------------

enum class SkullType : uint8_t {
    Pair_Multiplier = 0,        // +1 mult on PAIR / TWO PAIR
    Big_Multiplier,             // +1 mult on 3 KIND / STRAIGHT / FULL HOUSE
    Kind_3or4_Multiplier,       // +1 mult on 4 / 5 OF A KIND
    Flat_Bones,                 // +10 chips every hand
    Extra_Reroll,               // +1 reroll per hand
    Extra_Hand,                 // +1 hand per level
    Six_Bonus,                  // +3 chips per die showing a 6
    Ace_Bonus,                  // +3 chips per die showing a 1
    Even_Mulitplier,            // +1 mult if all 5 dice are even
    Odd_Mulitplier,             // +1 mult if all 5 dice are odd
    No_Reroll_Bonus,            // +15 chips if hand played with 0 rerolls used
    High_Roll_Save,             // HIGH ROLL hands score as if base 15 chips
    Threshold_Discount,         // threshold -5% (stacks, capped)
    Double_First,               // first hand played each level scores x2
    Skull_Count,
    None
};

enum class HandType : uint8_t {

    None,
    High_Roll,
    Pair,
    Two_Pair,
    Three_of_a_Kind,
    Straight,
    Full_House,
    Four_of_a_Kind,
    Five_of_a_Kind,

};

enum class GameState : uint8_t {

    Splash,
    Title,
    Game_Init,
    Game_Level_Intro,
    Game_Roll,
    Game_Hand_Result_Init,
    Game_Hand_Result_Base,
    Game_Hand_Result_Hand,
    Game_Hand_Result_Skulls_Played,
    Game_Hand_Result_Upgrades_Played,
    Game_Skull_Choice,
    Game_Deck_Full_Swap,
    Game_Deck_View,
    Game_Win,
    GameOver,

};

// --------------------------------------------------------------------

inline GameState &operator++(GameState &c ) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline GameState operator++(GameState &c, int ) {
    GameState result = c;
    ++c;
    return result;
}

inline GameState &operator--(GameState &c ) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline GameState operator--(GameState &c, int ) {
    GameState result = c;
    --c;
    return result;
}

