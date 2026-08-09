#include <ArduboyFX.h>  

void drawTitle() {

    FX::drawBitmap(0, 0, Images::Title_Options, 0, dbmNormal);
    FX::drawBitmap(76, 0, Images::Title_Skull, 0, dbmNormal);
    FX::drawBitmap(25, 0, Images::Title_Text, 0, dbmMasked);
    drawNumber(11, 46, bestLevel);

}


void updateTitle() {

    if (arduboy.justPressed(A_BUTTON)) {

        level = 1;
        hand.deckCount = 0;

        hand.addSkullToDeck(SkullType::Pair_Multiplier);
        hand.addSkullToDeck(SkullType::Big_Multiplier);
        hand.addSkullToDeck(SkullType::No_Reroll_Bonus);
        hand.addSkullToDeck(SkullType::Six_Bonus);
        hand.addSkullToDeck(SkullType::Six_Bonus);
        // hand.addSkullToDeck(SkullType::Extra_Hand);
        hand.deckCount = 5;

        startLevel();
        // gameState = GameState::Game_Upgrade_Choice_Init;

    }

}