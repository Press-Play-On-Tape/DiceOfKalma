#include <ArduboyFX.h>  

void drawGameOver() {

    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);
    FX::drawBitmap(0, 0, Images::GameOver, 0, dbmNormal);
    drawNumber(40, 40, level);
    drawNumber(33, 40, bestLevel);
    drawNumber(26, 40, hand.deckCount);

}

void drawWin() {

    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);
    FX::drawBitmap(0, 0, Images::GameWin, 0, dbmMasked);
    drawNumber(36, 13, hand.deckCount);

}