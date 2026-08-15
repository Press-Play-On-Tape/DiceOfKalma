#include <ArduboyFX.h>  


void title() {

    if (arduboy.justPressed(A_BUTTON)) {

        level = 1;
        hand.deckCount = 0;

        hand.addSkullToDeck(SkullType::Pair_Multiplier);
        hand.addSkullToDeck(SkullType::Big_Multiplier);
        hand.addSkullToDeck(SkullType::Kind_3or4_Multiplier);
        hand.addSkullToDeck(SkullType::Flat_Bones);
        hand.addSkullToDeck(SkullType::Six_Bonus);
        hand.addSkullToDeck(SkullType::Extra_Hand);
        hand.deckCount = 6;

        startLevel();
        // gameState = GameState::Game_Upgrade_Choice_Init;

    }


    FX::drawBitmap(0, 0, Images::Fire, arduboy.getFrameCount(48) / 3, dbmNormal);


    FX::drawBitmap(1, 0, Images::Title_Options, 0, dbmMasked);
    FX::drawBitmap(76, 12, Images::Skull_Large, skullData.getMouth(), dbmNormal);
    if (skullData.getEyes() != 255) FX::drawBitmap(99, 21, Images::Skull_Eyes, skullData.getEyes(), dbmWhite);
    FX::drawBitmap(24, 0, Images::Title_Text, 0, dbmMasked);
    arduboy.fillRect(11, 51, 7, 5, BLACK);
    if (bestLevel > 10)    arduboy.fillRect(11, 55, 7, 5, BLACK);
    drawNumber(12, 46, bestLevel);


    // skullStack.push(1);

}