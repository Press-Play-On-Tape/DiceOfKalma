#pragma once

#include <Arduboy2.h>

// - Debug ------------------------------

#define _DEBUG

#define _DEBUG_HAND
#define _DEBUG_ADD_SKULLS
#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println
#define _DEBUG_PRINT    void
#define _DEBUG_PRINTLN  void
#define DEBUG_BREAK    asm volatile("break\n");

// ---------------------------------------


#define MAX_DECK 6
// cursor 0-4 = dice, CURSOR_REROLL = the REROLL button, CURSOR_PLAY = the PLAY button
#define CURSOR_PLAY   5
#define CURSOR_DECK 6
#define CURSOR_REROLL 7

namespace Constants {

    constexpr uint8_t ParticlesMax = 30;
    constexpr uint8_t EEPROM_Addr = 16;
    constexpr uint8_t EEPROM_Magic  = 0xA5;   
    constexpr uint8_t RerollHighlight_Minimum = 8;
    constexpr uint8_t RerollHighlight_None = 0;
    constexpr uint8_t PlayHandlHighlight_Minimum = 8;
    constexpr uint8_t PlayHandHighlight_None = 0;
    constexpr uint8_t UpgradeHand_Count = 8;
    constexpr uint8_t HandScore_StartingPos = 128 - 64;
    constexpr uint8_t HandScore_LineSpacing = 7;
    constexpr uint8_t Level_Count = 20;

}

// ---------------------------------------------------------------------
// SKULLS - the deckbuilding layer.
// Each skull is a passive that triggers off hand type / dice values /
// how the hand was played. Skulls are stackable: owning 2 copies of a
// mult skull doubles its bonus, etc.
// ---------------------------------------------------------------------

enum class SkullType : uint8_t {
    Pair_Multiplier = 0,            // +1 mult on PAIR / TWO PAIR
    Big_Multiplier = 1,             // +1 mult on 3 KIND / STRAIGHT / FULL HOUSE
    Kind_3or4_Multiplier = 2,       // +1 mult on 4 / 5 OF A KIND
    Flat_Bones = 3,                 // +10 chips every hand
    Extra_Reroll = 4,               // +1 reroll per hand
    Extra_Hand = 5,                 // +1 hand per level
    Six_Bonus = 6,                  // +3 chips per die showing a 6
    Ace_Bonus = 7,                  // +3 chips per die showing a 1
    Even_Mulitplier = 8,            // +1 mult if all 5 dice are even
    Odd_Mulitplier = 9,             // +1 mult if all 5 dice are odd
    No_Reroll_Bonus = 10,           // +15 chips if hand played with 0 rerolls used
    High_Roll_Save = 11,            // HIGH ROLL hands score as if base 15 chips
    Threshold_Discount = 12,        // threshold -5% (stacks, capped)
    Double_First = 13,              // first hand played each level scores x2
    Mountainous = 14,               // 3 multiplier for 0,1,2,1,0 or equivalent
    TwiceAsHigh = 15,               // 3 multiplier for 1,1,6,6,x
    Skull_16 = 16,
    Skull_17 = 17,
    Skull_18 = 18,
    Skull_19 = 19,
    Skull_Count = 20,
    None
};

enum class HandType : uint8_t {

    None,
    High_Roll,
    Pair,
    Two_Pair,
    Three_of_a_Kind,
    Full_House,
    Four_of_a_Kind,
    Straight,
    Five_of_a_Kind,

};

enum class Marked : uint8_t {

    False,
    True,
    True_NoHighlight,

};

enum class Inventory_BottomOptions : uint8_t {

    NoSkulls,
    NoScrolling,
    DownOnly,
    UpOnly,
    Both

};

enum class UpgradeHand_BottomOptions : uint8_t {

    DownOnly,
    UpOnly,
    Both

};

enum class HandScore_BottomOptions : uint8_t {

    DownOnly,
    UpOnly,
    Both,
    None

};

enum class Skull_EyeMode : uint8_t {

    DoNothing,
    LeftRight,
    UpDone,
    None

};


enum class GameState : uint8_t {

    Splash,
    Title,
    Game_Init,
    Game_Level_Intro,
    Game_Roll_Dice,
    Game_Roll,
    Game_Hand_Result_Init,
    Game_Hand_Result_Base,
    Game_Hand_Result_Hand,
    Game_Hand_Result_Skulls_Played,
    Game_Hand_Result_Upgrades_Played,
    Game_Hand_Result_Countdown,
    Game_Skull_Choice_Init,
    Game_Skull_Choice,
    Game_Upgrade_Choice_Init,
    Game_Upgrade_Choice,
    Game_Skull_Info,
    Game_Hand_Info_Init,
    Game_Hand_Info,
    Game_Deck_Full_Swap,
    Game_Deck_View,
    Game_Win,
    Game_Over,

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

