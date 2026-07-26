#pragma once

#include <Arduboy2.h>

// - Debug ------------------------------

#define _DEBUG

#define _DEBUG_PRINT    Serial.print
#define _DEBUG_PRINTLN  Serial.println
#define DEBUG_PRINT    void
#define DEBUG_PRINTLN  void


// ---------------------------------------


namespace Constants {

    constexpr uint16_t EEPROM_Start         = EEPROM_STORAGE_SPACE_START + 143;

}


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

