#include <Arduboy2.h>
#include <EEPROM.h>

#include "images.h"


// ---------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------

void drawDice() {

    for (uint8_t i = 0; i < 5; i++) {

        bool isMarked = marked[i];

        if (isMarked) {
            FX::drawBitmap(27, i * 13, Images::Dice_Reverse, dice[i] - 1, dbmNormal);
        }
        else {
        Serial.println(dice[i] - 1);
            FX::drawBitmap(27, i * 13, Images::Dice_Normal, dice[i] - 1, dbmNormal);
        }


        if (i == cursor && arduboy.frameCount % 16 < 4) {
            arduboy.drawRect(27, i * 13, 12, 12, BLACK);
        }

    }

}




void drawNumber_Padded(uint8_t x, int8_t y, uint16_t number, uint8_t digits) {

    switch (digits) {
    
        case 1:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers, number);

            }
            break;

        case 2:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number % 10);

            }
            break;

        case 3:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number % 10);

            }
            break;

        case 4:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number % 10);

            }
            break;
    
    }

}

void drawNumber_Right(uint8_t x, int8_t y, uint16_t number) {

    switch (number) {
    
        case 0 ... 9:
            {
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number);

            }
            break;

        case 10 ... 99:
            {
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number % 10);

            }
            break;

        case 100 ... 999:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number % 10);

            }
            break;

        case 1000 ... 9999:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number % 10);

            }
            break;
    
    }

}

void drawNumber(uint8_t x, int8_t y, uint16_t number) {

    switch (number) {
    
        case 0 ... 9:
            {
                Sprites::drawSelfMasked(x, y + 6, Images::Numbers, number);

            }
            break;

        case 10 ... 99:
            {
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number % 10);

            }
            break;

        case 100 ... 999:
            {
                uint16_t thresh = threshold;
                Sprites::drawSelfMasked(x, y + 2, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 6, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 10, Images::Numbers, number % 10);

            }
            break;

        case 1000 ... 9999:
            {
                Sprites::drawSelfMasked(x, y, Images::Numbers, number / 1000);
                number = number - ((number / 1000) * 1000);
                Sprites::drawSelfMasked(x, y + 4, Images::Numbers, number / 100);
                number = number - ((number / 100) * 100);
                Sprites::drawSelfMasked(x, y + 8, Images::Numbers, number / 10);
                Sprites::drawSelfMasked(x, y + 12, Images::Numbers, number % 10);

            }
            break;
    
    }

}

void drawHeader(HandScore hand) {

    Sprites::drawOverwrite(0, 0, Images::Background, 0);
//   arduboy.setCursor(0, 0);
//   arduboy.print(F("LV"));
//   arduboy.print(level);
//   arduboy.setCursor(30, 0);
//   arduboy.print(runScore);
//   arduboy.print(F("/"));
//   arduboy.print(threshold);
    drawNumber_Right(62, -2, level);
    drawNumber(62, 14, 25);
    drawNumber(62, 41, threshold);


    if (hand.totalBones > 0) drawNumber(44, 3, hand.totalBones);
    if (hand.multiplier > 0) drawNumber(44, 24, hand.multiplier);
    if (hand.score > 0) drawNumber(44, 46, hand.score);


//   arduboy.setCursor(104, 0);
//   arduboy.print(F("D"));
//   arduboy.print(deckCount);
}


void drawFooterRoll() {

//   arduboy.setCursor(0, 25);
//   arduboy.print(lastHandName);

//   arduboy.setCursor(0, 34);
//   arduboy.print(lastChips);
//   arduboy.print(F("x"));
//   arduboy.print(lastMult);
//   arduboy.print(F("="));
//   arduboy.print(lastScore);

//   arduboy.setCursor(64, 34);
//   arduboy.print(F("RR:"));
//   arduboy.print(rerollsLeft);
//   arduboy.print(F(" H:"));
//   arduboy.print(handsMax - handsLeft + 1);
//   arduboy.print(F("/"));
//   arduboy.print(handsMax);


    Sprites::drawSelfMasked(3, 6, Images::Numbers, handsMax - handsLeft + 1);
    Sprites::drawSelfMasked(3, 16, Images::Numbers, handsMax);
    Sprites::drawSelfMasked(3, 50, Images::Numbers, rerollsLeft);

    if (state == GameState::Game_Roll && arduboy.frameCount % 16 < 4) {

        switch (cursor) {

            case CURSOR_PLAY:
                arduboy.drawRect(0, 0, 24, 25, BLACK);
                break;

            case CURSOR_DECK:
                arduboy.drawRect(0, 26, 24, 12, BLACK);
                break;

            case CURSOR_REROLL:
                arduboy.drawRect(0, 39, 24, 25, BLACK);
                break;

        }

    }

}

void drawLevelIntro() {

    FX::drawBitmap(0, 0 , Images::LevelEntry, 0, dbmNormal);
    // FX::drawOverwrite(0, 0, Images::LevelEntry, 0);
    drawNumber_Padded(43, 39, level, 2);
    drawNumber_Padded(32, 44, threshold, 4);
//   arduboy.setCursor(30, 16);
//   arduboy.print(F("LEVEL "));
//   arduboy.print(level);
//   arduboy.setCursor(14, 28);
//   arduboy.print(F("BEAT: "));
//   arduboy.print(threshold);
//   arduboy.setCursor(4, 40);
//   arduboy.print(F("HANDS:"));
//   arduboy.print(handsMax);
//   arduboy.print(F(" REROLLS:"));
//   arduboy.print(rerollsMax);
//   arduboy.setCursor(4, 52);
//   arduboy.print(F("DECK: "));
//   arduboy.print(deckCount);
//   arduboy.print(F("/"));
//   arduboy.print(MAX_DECK);
//   arduboy.print(F(" SKULLS"));
}