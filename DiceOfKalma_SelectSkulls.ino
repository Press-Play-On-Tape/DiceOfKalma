#include <ArduboyFX.h>  

void offerSkulls() {
    skullChoiceA = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    do {
        skullChoiceB = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    } while (skullChoiceB == skullChoiceA);
    skullCursor = 0;
    gameState = GameState::Game_Skull_Choice;
}



void updateHandResult_Init() {

    if (handsLeft == 0) {
        saveHighScore();
        gameState = GameState::GameOver;
    } 
    else {
        saveHighScore();
        // Serial.println("updateHandResult_Init");
        offerSkulls();
        newHand();
        gameState = GameState::Game_Skull_Choice;
        skullCursor = 0;
    }
  
}

void updateHandResult() {

  if (arduboy.justPressed(A_BUTTON)) {
    // if (runScore >= threshold) {
    //   Serial.println("updateHandResult");
    // } else 
    if (handsLeft == 0) {
      saveHighScore();
      gameState = GameState::GameOver;
    } else {
      newHand();
      gameState = GameState::Game_Roll;
    }
  }
}

void updateSkullChoice() {
  if (arduboy.justPressed(RIGHT_BUTTON) && skullCursor > 0) {
    skullCursor = skullCursor - 1;
  }
  else if (arduboy.justPressed(LEFT_BUTTON) && skullCursor < 3) {
    skullCursor = skullCursor + 1;
  }
  else if (arduboy.justPressed(DOWN_BUTTON)) {
    gameState = GameState::Game_Skull_Info;
  }
  if (arduboy.justPressed(A_BUTTON)) {
    SkullType chosen = (skullCursor == 0) ? skullChoiceA : skullChoiceB;
   

            if (hand.deckCount < MAX_DECK) {
                hand.addSkullToDeck(chosen);
                level++;
                if (level > 25) gameState = GameState::Game_Win; else startLevel();
            } 
            else {
                pendingSkull = chosen;
                swapCursor = 0;
                gameState = GameState::Game_Deck_Full_Swap;
            }

  }
}


void drawSkullChoice() {

    FX::drawBitmap(0, 0, Images::SkullSelect, 0, dbmNormal);

//   arduboy.setCursor(14, 0);
//   arduboy.print(F("CHOOSE A SKULL"));
//   arduboy.drawLine(0, 10, 127, 10, WHITE);

//   uint8_t boxY = 18, boxH = 30, boxW = 60;
//   int16_t xA = 2, xB = 66;

//   arduboy.drawRect(xA, boxY, boxW, boxH, WHITE);
//   arduboy.drawRect(xB, boxY, boxW, boxH, WHITE);

//   if (skullCursor == 0) arduboy.fillRect(xA, boxY - 4, boxW, 3, WHITE);
//   else arduboy.fillRect(xB, boxY - 4, boxW, 3, WHITE);

//   arduboy.setCursor(xA + 3, boxY + 10);
//   arduboy.print(skullName(skullChoiceA));
//   arduboy.setCursor(xB + 3, boxY + 10);
//   arduboy.print(skullName(skullChoiceB));

    uint24_t aIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(skullChoiceA));
    FX::drawBitmap(90, 0, aIcon, 0, dbmNormal);

    uint24_t bIcon = FX::readIndexedUInt24(Images::Skulls_Thumb, static_cast<uint8_t>(skullChoiceB));
    FX::drawBitmap(90 - 24, 0, bIcon, 0, dbmNormal);
    FX::drawBitmap(90 - 24 - 24, 0, bIcon, 0, dbmNormal);
    FX::drawBitmap(90 - 24 - 24 - 24, 0, bIcon, 0, dbmNormal);

    if (arduboy.frameCount % 16 < 8) {
        FX::drawBitmap(90 - (skullCursor * 24), 0, Images::Skull_Thumb_Cursor, 0, dbmWhite);
    }
    

//   arduboy.setCursor(10, 54);
//   arduboy.print(F("<- ->  A TO PICK"));
}

void  drawSkullInfo() {

    FX::drawBitmap(0, 0, Images::SkullInfo, 0, dbmNormal);

    uint24_t img = FX::readIndexedUInt24(Images::Skulls, static_cast<uint8_t>(skullChoiceA));
    FX::drawBitmap(65, 0, img, 0, dbmNormal);

  if (arduboy.justPressed(A_BUTTON)) {
    gameState = GameState::Game_Skull_Choice;

  }


}