#include <Arduboy2.h>
#include <EEPROM.h>

#include "images.h"


// ---------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------

void drawDice() {

    for (uint8_t i = 0; i < 5; i++) {

        bool isMarked = (hand.marked[i] == Marked::True || hand.marked[i] == Marked::True_NoHighlight);

        switch (hand.marked[i]) {

            case Marked::True:
                FX::drawBitmap(27, i * 13, Images::Dice_Reverse,((hand.dice[i] - 1) * 6) + (rollDice_Counter == 0 ? 0 : rollDice_Counter - 1), dbmNormal);
                break;

            case Marked::True_NoHighlight:
                FX::drawBitmap(27, i * 13, Images::Dice_Normal, ((hand.dice[i] - 1) * 11) + (rollDice_Counter == 0 ? 0 : rollDice_Counter - 1), dbmNormal);
                break;

            case Marked::False:
                FX::drawBitmap(27, i * 13, Images::Dice_Normal, ((hand.dice[i] - 1) * 11), dbmNormal);
                break;
                
        }

        if (i == cursor && rollDice_Counter == 0 && hand.dice[i] != 7 && arduboy.frameCount % 16 < 4) {
            arduboy.drawRect(27, i * 13, 12, 12, BLACK);
        }

    }

}


void drawNumber_Padded(uint8_t x, int8_t y, uint16_t number, uint8_t digits) {

    switch (digits) {
    
        case 1:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number);

            }
            break;

        case 2:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number % 10);

            }
            break;

        case 3:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number % 10);

            }
            break;

        case 4:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number % 10);

            }
            break;
    
    }

}

void drawNumber_Right(uint8_t x, int8_t y, uint16_t number) {

    switch (number) {
    
        case 0 ... 9:
            {
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number);

            }
            break;

        case 10 ... 99:
            {
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number % 10);

            }
            break;

        case 100 ... 999:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number % 10);

            }
            break;

        case 1000 ... 9999:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number % 10);

            }
            break;
    
    }

}

void drawNumber(uint8_t x, int8_t y, uint16_t number) {

    switch (number) {
    
        case 0 ... 9:
            {
                Sprites::drawSelfMasked(x, y + 6, Images::Numbers_WB, number);

            }
            break;

        case 10 ... 99:
            {
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number % 10);

            }
            break;

        case 100 ... 999:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y + 2, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 6, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 10, Images::Numbers_WB, number % 10);

            }
            break;

        case 1000 ... 9999:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers_WB, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers_WB, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers_WB, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers_WB, number % 10);

            }
            break;
    
    }

}

void drawSkull() {

    // FX::drawBitmap(76, 12, Images::Skull_Large, 0, dbmNormal);
    FX::drawBitmap(76, 12, Images::Skull_Large, skullData.getMouth(), dbmNormal);
    if (skullData.getEyes() != 255) FX::drawBitmap(99, 21, Images::Skull_Eyes, skullData.getEyes(), dbmWhite);

}

void drawLevelAndTarget(HandScore hand, uint8_t levelNumber, uint16_t target) {

    FX::drawBitmap(60, 0, Images::Background_02, 0, dbmNormal);

    drawNumber_Right(62, -2, levelNumber);
    drawNumber(62, 14, 25);
    drawNumber(62, 41, target);

}

void drawBonesMultTotal(HandScore handScore) {

    FX::drawBitmap(0, 0, Images::Background_00, 0, dbmNormal);


    if (handScore.totalBones > 0) drawNumber(44, 3, handScore.totalBones);
    if (handScore.totalMultiplier > 0) drawNumber(44, 24, handScore.totalMultiplier);
    if (handScore.score > 0) drawNumber(44, 46, handScore.score);

}


void drawFooterRoll() {

    if (hand.playHandHighlight > Constants::PlayHandHighlight_None) {

        FX::drawBitmap(0, 0, Images::Button_PlayHand, 0, dbmNormal);
        Sprites::drawSelfMasked(3, 6, Images::Numbers_BW, handsLeft);
        Sprites::drawSelfMasked(3, 16, Images::Numbers_BW, handsMax);

    }
    else {
    
        Sprites::drawSelfMasked(3, 6, Images::Numbers_WB, handsLeft);
        Sprites::drawSelfMasked(3, 16, Images::Numbers_WB, handsMax);

    }


    if (hand.rerollHighlight > Constants::RerollHighlight_None) {

        FX::drawBitmap(0, 39, Images::Button_Reroll, 0, dbmNormal);
        Sprites::drawSelfMasked(3, 45, Images::Numbers_BW, rerollsLeft);
        Sprites::drawSelfMasked(3, 55, Images::Numbers_BW, rerollsMax);

    }
    else {

        Sprites::drawSelfMasked(3, 45, Images::Numbers_WB, rerollsLeft);
        Sprites::drawSelfMasked(3, 55, Images::Numbers_WB, rerollsMax);

    }

    if (gameState == GameState::Game_Roll && arduboy.frameCount % 16 < 4) {

        switch (cursor) {

            case CURSOR_PLAY:
                if (hand.playHandHighlight == Constants::RerollHighlight_None) {
                    arduboy.drawRect(0, 0, 24, 25, BLACK);
                }
                break;

            case CURSOR_DECK:
                arduboy.drawRect(0, 26, 24, 12, BLACK);
                break;

            case CURSOR_REROLL:
                if (hand.rerollHighlight == Constants::RerollHighlight_None) {
                    arduboy.drawRect(0, 39, 24, 25, BLACK);
                }
                break;

        }

    }

}

void drawLevelIntro() {

    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);
    FX::drawBitmap(76, 12, Images::Skull_Large, 0, dbmNormal);
    FX::drawBitmap(0, 0 , Images::LevelEntry, 0, dbmMasked);
    drawNumber_Padded(43, 40, level, 2);
    drawNumber_Padded(32, 45, threshold, 4);
    drawNumber_Padded(21, 40, handsMax, 2);
    drawNumber_Padded(10, 43, rerollsMax, 2);

}