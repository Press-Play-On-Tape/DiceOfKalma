#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Utils.h"
#include "src/utils/Constants.h"

void setDeckViewTop() {

    switch (hand.deckCount) {
    
        case 0 ... 4:
            deckViewTop = 0;
            break;

        case 5:
            deckViewTop = deckViewCursor == 4 ? 1 : 0;
            break;

        case 6:

            switch (deckViewCursor) {

                case 0 ... 2:
                    deckViewTop = 0;
                    break;

                case 3:
                    deckViewTop = 1;
                    break;

                default:
                    deckViewTop = 2;
                    break;

            }

            break;

    }

}

void updateDeckView() {

    if (arduboy.justPressed(LEFT_BUTTON) && hand.deckCount > 0 && deckViewCursor < hand.deckCount - 1) {

        deckViewCursor++;
        setDeckViewTop();

    }

    if (arduboy.justPressed(RIGHT_BUTTON) && hand.deckCount > 0 && deckViewCursor > 0) {

        deckViewCursor--;
        setDeckViewTop();

    }

    if (arduboy.justPressed(DOWN_BUTTON)) {
    
        gameState = GameState::Game_Skull_Info; returnState = GameState::Game_Deck_View;

    }

    if (arduboy.justPressed(A_BUTTON)) {

        gameState = GameState::Game_Roll;

    }

}

void drawDeckView() {

    uint8_t x = 90;
    Inventory_BottomOptions botImage = Inventory_BottomOptions::Both;

    FX::drawBitmap(96, 0, Images::Inventory_Top, 0, dbmNormal);


    if (hand.deckCount == 0) {
        botImage = Inventory_BottomOptions::NoSkulls;
    }
    else if (hand.deckCount < 5) {
        botImage = Inventory_BottomOptions::NoScrolling;
    }
    else if (deckViewCursor > 0) { 

        if (deckViewCursor == hand.deckCount -1) {
            botImage = Inventory_BottomOptions::UpOnly;
        }
        else {
            botImage = Inventory_BottomOptions::Both;
        }
    }
    else { //Do not show up
        if (deckViewCursor == hand.deckCount -1) {
            botImage = Inventory_BottomOptions::NoScrolling;

        }
        else {
            botImage = Inventory_BottomOptions::DownOnly;
        }
    }
    

    FX::drawBitmap(0, 0, Images::Inventory_Bot, static_cast<uint8_t>(botImage), dbmNormal);

    drawNumber(119, 45, hand.deckCount);
//   arduboy.print(F(")"));
//   arduboy.drawLine(0, 9, 127, 9, WHITE);

  if (hand.deckCount != 0) {

    for (uint8_t i = deckViewTop; i < hand.deckCount; i++) {

        uint24_t aIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(hand.deck[i]));
        FX::drawBitmap(x, 0, aIcon, 0, dbmNormal);

      x= x - 24;
    }

  }


    if (arduboy.frameCount % 24 < 12) {
        FX::drawBitmap(90 - ((deckViewCursor - deckViewTop) * 24), 0, Images::Skull_Thumb_Cursor, 0, dbmWhite);
    }

}
