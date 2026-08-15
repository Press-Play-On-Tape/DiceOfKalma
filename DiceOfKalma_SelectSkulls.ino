#include <ArduboyFX.h>  
#include "src/utils/Constants.h" 


// Offer three unique skulls ..

void offerSkulls() {

    skullChoiceA = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));

    do {
        skullChoiceB = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    } while (skullChoiceB == skullChoiceA);

    do {
        skullChoiceC = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    } while (skullChoiceC == skullChoiceA || skullChoiceC == skullChoiceB);

}



void skullChoice_Init() {

    if (handsLeft == 0) {
        saveHighScore();
        gameState = GameState::Game_Over;
    } 
    else {
        saveHighScore();
        offerSkulls();
        newHand();
        gameState = GameState::Game_Skull_Choice;
        skullCursor = 0;
    }
  
}


void skullChoice() {


    // Handle input ..
        
    if (arduboy.justPressed(RIGHT_BUTTON) && skullCursor > 0) {
        
        skullCursor = skullCursor - 1;

    }
    else if (arduboy.justPressed(LEFT_BUTTON) && skullCursor < 2) {
        
        skullCursor = skullCursor + 1;

    }
    else if (arduboy.justPressed(DOWN_BUTTON)) {
  
        switch (skullCursor) {

            case 0:
                skullInfoType = skullChoiceA;
                break;

            case 1:
                skullInfoType = skullChoiceB;
                break;

            case 2:
                skullInfoType = skullChoiceC;
                break;
                
        }

        gameState = GameState::Game_Skull_Info; 
        returnState = GameState::Game_Skull_Choice;

    }
    else if (arduboy.justPressed(A_BUTTON)) {

        SkullType chosen;
        
        switch (skullCursor) {

            case 0:
                chosen = skullChoiceA;
                break;

            case 1:
                chosen = skullChoiceB;
                break;

            case 2:
                chosen = skullChoiceC;
                break;
                
        }

        if (hand.deckCount < MAX_DECK) {

            hand.addSkullToDeck(chosen);
            gameState = GameState::Game_Upgrade_Choice_Init;
    
        } 
        else {

            pendingSkull = chosen;
            swapCursor = 0;
            gameState = GameState::Game_Deck_Full_Swap;

        }

    }
    else if (arduboy.justPressed(B_BUTTON)) {

        gameState = GameState::Game_Upgrade_Choice_Init;

    }


    // Render screen ..

    FX::drawBitmap(115, 0, Images::SkullSelect_Top, 0, dbmNormal);
    FX::drawBitmap(0, 0, Images::SkullSelect_Bot, 0, dbmNormal);

    uint24_t aIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(skullChoiceA));
    FX::drawBitmap(90, 0, aIcon, 0, dbmNormal);

    uint24_t bIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(skullChoiceB));
    FX::drawBitmap(90 - 24, 0, bIcon, 0, dbmNormal);

    uint24_t cIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(skullChoiceC));
    FX::drawBitmap(90 - 24 - 24, 0, cIcon, 0, dbmNormal);

    if (arduboy.frameCount % 24 < 12) {
        FX::drawBitmap(90 - (skullCursor * 24), 0, Images::Skull_Thumb_Cursor, 0, dbmWhite);
    }
    
}

void  drawSkullInfo() {

    FX::drawBitmap(0, 0, Images::SkullInfo, 0, dbmNormal);

    uint24_t img = FX::readIndexedUInt24(Images::Skulls, static_cast<uint8_t>(skullInfoType));
    FX::drawBitmap(65, 0, img, 0, dbmNormal);

    if (arduboy.justPressed(A_BUTTON)) {
        gameState = returnState;
    }

}
