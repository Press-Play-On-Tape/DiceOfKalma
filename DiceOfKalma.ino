/*
  DICE OF KALMA - Arduboy
  ------------------------
  A cut-down homage to Pepperbox Studios' "Dice of Kalma" (a Balatro-style
  dice poker roguelike), scaled down to fit the Arduboy.

  CORE LOOP
  - Each level has a score THRESHOLD you must reach within a limited
    number of hands.
  - Each hand: 5 dice are rolled. Mark dice with A, then press B to
    reroll marked dice (costs 1 of your rerolls). Press UP to lock in
    the current dice as your played hand.
  - Hand type (pair, straight, full house, etc.) determines base chips
    and a multiplier, Balatro-style: score = (chips + dice sum) * mult.
  - Clear the threshold before hands run out to advance. After clearing
    a level you pick 1 of 2 SKULLS to add to your deck. Skulls are
    passive abilities that trigger off hand types, dice values, or how
    you played the hand -- your deck persists (and grows) for the run.
  - Run out of hands before hitting the threshold -> game over.

  CONTROLS
    LEFT / RIGHT : move cursor between dice
    A            : mark / unmark highlighted die for reroll
    B            : reroll all marked dice (uses 1 reroll)
    UP           : play the current hand (score it)
    DOWN         : view your current skull deck (while rolling)
    (menus: LEFT/RIGHT or UP/DOWN to navigate, A to confirm, B to back)

  Requires the Arduboy2 library.
*/

#include <ArduboyFX.h>  
#include "fxdata/fxdata.h"
#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Constants.h"
#include "src/utils/Structs.h"
#include "src/entities/Hand.h"
#include "src/entities/Particle.h"
#include <EEPROM.h>

#include "images.h"

Arduboy2Ext arduboy;

// ---------------------------------------------------------------------
// EEPROM (Arduboy2 system area occupies the first 16 bytes, so we start
// safely after that)
// ---------------------------------------------------------------------


uint8_t renderHandResult_Counter = 0;


GameState state = GameState::Splash;
GameState returnState =GameState::Splash; // where DECK_VIEW returns to

uint8_t stateTimer = 0;
uint8_t cursor = 0;


// ---------------------------------------------------------------------
// Run state
// ---------------------------------------------------------------------
uint8_t  level = 1;
uint16_t threshold = 0;
uint16_t runScore = 0;      // score accumulated this level
uint8_t  handsLeft = 0;
uint8_t  handsMax  = 0;
uint8_t  rerollsLeft = 0;
uint8_t  rerollsMax  = 0;
uint8_t  bestLevel = 1;

Hand hand;

// last hand result, for the result screen

HandScore tempHandScore;
Particle particles[Constants::ParticlesMax];

SkullType skullChoiceA, skullChoiceB;
SkullType pendingSkull = SkullType::None;   // skull waiting for a deck slot when deck is full
uint8_t skullCursor = 0;
uint8_t swapCursor = 0;
uint8_t deckViewCursor = 0;



/*

uint16_t computeThreshold() {

    return 450;//SJH

    uint32_t base = 150 + (uint32_t)(level - 1) * 60 + (uint32_t)(level - 1) * (level - 1) * 20;
    uint8_t discountPct = hand.countSkull(SkullType::Threshold_Discount) * 5;
    if (discountPct > 40) discountPct = 40;
    base = base - (base * discountPct) / 100;
    return (uint16_t)base;

}

void startLevel() {
    threshold = computeThreshold();
    runScore = 0;
    handsMax = 3 + hand.countSkull(SkullType::Extra_Hand);
    handsLeft = handsMax;
    rerollsMax = 3 + hand.countSkull(SkullType::Extra_Reroll);
    rerollsLeft = rerollsMax;
    hand.firstHandOfLevel = true;
    hand.rerollUsedThisHand = false;
    hand.rollAllDice();
    state = GameState::Game_Level_Intro;
    stateTimer = 0;
}

void newHand() {
    rerollsLeft = rerollsMax;
    cursor = 0;
    hand.rerollUsedThisHand = false;
    hand.rollAllDice();
}

void offerSkulls() {
    skullChoiceA = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    do {
        skullChoiceB = static_cast<SkullType>(random(0, static_cast<uint8_t>(SkullType::Skull_Count)));
    } while (skullChoiceB == skullChoiceA);
    skullCursor = 0;
    state = GameState::Game_Skull_Choice;
}



void updateLevelIntro() {
  stateTimer++;
  if (stateTimer > 60 || arduboy.justPressed(A_BUTTON)) {
    state = GameState::Game_Roll;
  }
}



void updateRoll() {

  hand.evaluateHand(); // live preview of current dice

  if (arduboy.justPressed(UP_BUTTON)) {
    cursor = (cursor == 0) ? CURSOR_REROLL : cursor - 1;
  }

  if (arduboy.justPressed(LEFT_BUTTON)) {
  
    if (cursor == 0 || cursor == 1) cursor = CURSOR_PLAY;
    if (cursor == 2) cursor = CURSOR_DECK;
    if (cursor == 3 || cursor == 4) cursor = CURSOR_REROLL;
  }

  if (arduboy.justPressed(RIGHT_BUTTON)) {
    if (cursor == CURSOR_PLAY) cursor = 0;
    if (cursor == CURSOR_DECK) cursor = 2;
    if (cursor == CURSOR_REROLL) cursor = 4;
  }

  if (arduboy.justPressed(DOWN_BUTTON)) {
    cursor = (cursor == CURSOR_REROLL) ? 0 : cursor + 1;
  }

  if (arduboy.justPressed(A_BUTTON)) {
    if (cursor < 5) {
      hand.marked[cursor] = !hand.marked[cursor];
    } else if (cursor == CURSOR_REROLL) {
      bool anyMarked = false;
      for (uint8_t i = 0; i < 5; i++) if (hand.marked[i]) anyMarked = true;
      if (rerollsLeft > 0 && anyMarked) {
        hand.rerollMarked();
        rerollsLeft--;
        hand.rerollUsedThisHand = true;
      }
    } else if (cursor == CURSOR_PLAY) { // CURSOR_PLAY
      hand.evaluateHand();
      runScore += hand.lastHandScore.score;
      handsLeft--;
      hand.firstHandOfLevel = false;
      state = GameState::Game_Hand_Result_Init;
      stateTimer = 0;
    } else if (cursor == CURSOR_DECK) {
        returnState = GameState::Game_Roll;
        deckViewCursor = 0;
        state = GameState::Game_Deck_View;
    }
  }
}

void updateHandResult() {
  stateTimer++;
  if (stateTimer > 70 || arduboy.justPressed(A_BUTTON)) {
    if (runScore >= threshold) {
      saveHighScore();
      Serial.println("OfferSkulls");
      offerSkulls();
    } else if (handsLeft == 0) {
      saveHighScore();
      state = GameState::GameOver;
    } else {
      newHand();
      state = GameState::Game_Roll;
    }
  }
}

void updateSkullChoice() {
  if (arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(RIGHT_BUTTON)) {
    skullCursor = 1 - skullCursor;
  }
  if (arduboy.justPressed(A_BUTTON)) {
    SkullType chosen = (skullCursor == 0) ? skullChoiceA : skullChoiceB;
   

            if (hand.deckCount < MAX_DECK) {
                hand.addSkullToDeck(chosen);
                level++;
                if (level > 25) state = GameState::Game_Win; else startLevel();
            } 
            else {
                pendingSkull = chosen;
                swapCursor = 0;
                state = GameState::Game_Deck_Full_Swap;
            }

  }
}

void updateDeckFullSwap() {
  if (arduboy.justPressed(UP_BUTTON)) {
    swapCursor = (swapCursor == 0) ? MAX_DECK - 1 : swapCursor - 1;
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    swapCursor = (swapCursor == MAX_DECK - 1) ? 0 : swapCursor + 1;
  }
  if (arduboy.justPressed(A_BUTTON)) {
    hand.deck[swapCursor] = pendingSkull;
    level++;
    if (level > 25) state = GameState::Game_Win; else startLevel();
  }
  if (arduboy.justPressed(B_BUTTON)) {
    level++;
    if (level > 25) state = GameState::Game_Win; else startLevel();
  }
}

void updateDeckView() {
  if (arduboy.justPressed(UP_BUTTON) && hand.deckCount > 0) {
    deckViewCursor = (deckViewCursor == 0) ? hand.deckCount - 1 : deckViewCursor - 1;
  }
  if (arduboy.justPressed(DOWN_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    if (returnState == GameState::Game_Roll) { state = GameState::Game_Roll; return; }
  }
  if (arduboy.justPressed(A_BUTTON)) {
    state = returnState;
  }
}

void updateGameOver() {
  if (arduboy.justPressed(A_BUTTON)) {
    state = GameState::Title;
  }
}

void updateWin() {
  if (arduboy.justPressed(A_BUTTON)) {
    state = GameState::Title;
  }
}




void drawRoll() {
    drawSkull();
    drawLevelAndTarget(hand.lastHandScore, level, threshold);
    drawBonesMultTotal(hand.lastHandScore);
    drawDice();
    drawFooterRoll();
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
Serial.print((uint8_t)skullChoiceA);
Serial.print(" ");
Serial.println((uint8_t)skullChoiceB);
    uint24_t aIcon = FX::readIndexedUInt24(Images::Skulls, static_cast<uint8_t>(skullChoiceA));
    FX::drawBitmap(64, 0, aIcon, 0, dbmNormal);

    uint24_t bIcon = FX::readIndexedUInt24(Images::Skulls, static_cast<uint8_t>(skullChoiceB));
    FX::drawBitmap(16, 0, bIcon, 0, dbmNormal);

    if (skullCursor == 0 && arduboy.frameCount % 16 < 8) {
        arduboy.drawRect(64, 0, 48, 64);
    }
    if (skullCursor == 1 && arduboy.frameCount % 16 < 8) {
        arduboy.drawRect(16, 0, 48, 64);
    }

    

//   arduboy.setCursor(10, 54);
//   arduboy.print(F("<- ->  A TO PICK"));
}

void drawDeckFullSwap() {
  arduboy.setCursor(2, 0);
  arduboy.print(F("DECK FULL - SWAP?"));
  arduboy.drawLine(0, 9, 127, 9, WHITE);

  for (uint8_t i = 0; i < MAX_DECK; i++) {
    int16_t y = 11 + i * 8;
    if (i == swapCursor) arduboy.fillRect(0, y, 128, 8, WHITE);
    uint8_t col = (i == swapCursor) ? BLACK : WHITE;
    arduboy.setCursor(2, y);
    arduboy.setTextColor(col);
    arduboy.print(skullName(hand.deck[i]));
  }
  arduboy.setTextColor(WHITE);
}

void drawDeckView() {
  arduboy.setCursor(2, 0);
  arduboy.print(F("YOUR SKULLS ("));
  arduboy.print(hand.deckCount);
  arduboy.print(F(")"));
  arduboy.drawLine(0, 9, 127, 9, WHITE);

  if (hand.deckCount == 0) {
    arduboy.setCursor(10, 28);
    arduboy.print(F("NO SKULLS YET"));
  } else {
    for (uint8_t i = 0; i < hand.deckCount; i++) {
      int16_t y = 11 + i * 8;
      if (i == deckViewCursor) arduboy.fillRect(0, y, 128, 8, WHITE);
      uint8_t col = (i == deckViewCursor) ? BLACK : WHITE;
      arduboy.setCursor(2, y);
      arduboy.setTextColor(col);
      arduboy.print(skullName(hand.deck[i]));
    }
    arduboy.setTextColor(WHITE);
  }

  arduboy.setCursor(2, 56);
  arduboy.print(F("v/B-BACK  ^-SCROLL"));
}

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
*/
// ---------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------
void setup() {
    
    arduboy.boot();
    arduboy.setFrameRate(30);
    arduboy.initRandomSeed();
    loadHighScore();

      FX::display(CLEAR_BUFFER);

  FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);

}

void loop() {

    if (!arduboy.nextFrame()) return;
    arduboy.pollButtons();
    arduboy.clear();

    switch (state) {

        case GameState::Splash:         
            splashScreen();         
            break;

        case GameState::Title:         
            updateTitle();         
            drawTitle();         
            break;

        case GameState::Game_Level_Intro:   
            updateLevelIntro();    
            drawLevelIntro();    
            break;

        case GameState::Game_Roll:          
            updateRoll();          
            drawRoll();          
            break;

        case GameState::Game_Hand_Result_Init:   
            renderHandResult_Counter = 0;
            tempHandScore.reset();
            state = GameState::Game_Hand_Result_Base;
            [[fallthrough]]

        case GameState::Game_Hand_Result_Base:   
            renderHandResult_Base();           
            break;

        case GameState::Game_Hand_Result_Hand:   
            renderHandResult_Hand();           
            break;

        case GameState::Game_Hand_Result_Skulls_Played:   
            renderHandResult_SkullsPlayed();           
            break;

        case GameState::Game_Hand_Result_Upgrades_Played:   
            renderHandResult_UpgradesPlayed();           
            break;

        case GameState::Game_Hand_Result_Countdown:
            renderHandResult_Countdown();
            break;

        case GameState::Game_Skull_Choice:  
            updateSkullChoice();   
            drawSkullChoice();   
            break;

        case GameState::Game_Deck_Full_Swap:
            updateDeckFullSwap();  
            drawDeckFullSwap();  
            break;

        case GameState::Game_Deck_View:     
            updateDeckView();      
            drawDeckView();      
            break;

        case GameState::GameOver:      
            updateGameOver();      
            drawGameOver();      
            break;

        case GameState::Game_Win:           
            updateWin();           
            drawWin();           
            break;
    }

    // arduboy.display();

    updateAndRenderParticles();
    FX::display(CLEAR_BUFFER);

}
