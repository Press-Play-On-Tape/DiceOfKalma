#include <ArduboyFX.h>  

void updateGameOver() {

    if (arduboy.justPressed(A_BUTTON)) {
        gameState = GameState::Title;
    }

}

void drawGameOver() {

    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);
    FX::drawBitmap(76, 12, Images::Skull_Large, skullData.getMouth(), dbmNormal);
    FX::drawBitmap(0, 0, Images::GameOver, 0, dbmMasked);
    arduboy.fillRect(27, 41 + (level < 10 ? 4 : 0), 7, 5 + (level >= 10 ? 4 : 0), BLACK);
    drawNumber_Right(28, 34, level);
    arduboy.fillRect(20, 41 + (bestLevel < 10 ? 4 : 0), 7, 5 + (bestLevel >= 10 ? 4 : 0), BLACK);
    drawNumber_Right(21, 34, bestLevel);
    arduboy.fillRect(13, 41 + (hand.deckCount < 10 ? 4 : 0), 7, 5 + (hand.deckCount >= 10 ? 4 : 0), BLACK);
    drawNumber_Right(14, 34, hand.deckCount);

}

void drawWin() {

    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);
    FX::drawBitmap(0, 0, Images::GameWin, 0, dbmMasked);
    drawNumber(36, 13, hand.deckCount);

}