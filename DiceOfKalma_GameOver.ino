#include <ArduboyFX.h>  

void drawGameOver() {
    arduboy.setCursor(30, 8);
    arduboy.print(F("GAME OVER"));
    arduboy.setCursor(14, 20);
    arduboy.print(F("REACHED LEVEL "));
    arduboy.print(level);
    arduboy.setCursor(14, 30);
    arduboy.print(F("BEST LEVEL: "));
    arduboy.print(bestLevel);
    arduboy.setCursor(4, 40);
    arduboy.print(F("SKULLS COLLECTED: "));
    arduboy.print(hand.deckCount);
    arduboy.setCursor(18, 54);
    arduboy.print(F("PRESS A TO RETRY"));
}

void drawWin() {
    arduboy.setCursor(20, 12);
    arduboy.print(F("YOU ESCAPED"));
    arduboy.setCursor(10, 22);
    arduboy.print(F("THE UNDERWORLD!"));
    arduboy.setCursor(6, 34);
    arduboy.print(F("FINAL DECK: "));
    arduboy.print(hand.deckCount);
    arduboy.print(F(" SKULLS"));
    arduboy.setCursor(30, 44);
    arduboy.print(F("KALMA BOWS."));
    arduboy.setCursor(18, 56);
    arduboy.print(F("PRESS A TO PLAY"));
}