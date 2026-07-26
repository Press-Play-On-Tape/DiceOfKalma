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
#define EEPROM_ADDR   16
#define EEPROM_MAGIC  0xA5

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


const __FlashStringHelper* skullName(SkullType s) {
  switch (s) {
    case SkullType::Pair_Multiplier:      return F("PAIR MULT +1");
    case SkullType::Big_Multiplier:       return F("3K/STR/FH MULT+1");
    case SkullType::Kind_3or4_Multiplier:      return F("4-5 KIND MULT+1");
    case SkullType::Flat_Bones:     return F("+10 CHIPS/HAND");
    case SkullType::Extra_Reroll:   return F("+1 REROLL/HAND");
    case SkullType::Extra_Hand:     return F("+1 HAND/LEVEL");
    case SkullType::Six_Bonus:      return F("+3 CHIPS PER 6");
    case SkullType::Ace_Bonus:      return F("+3 CHIPS PER 1");
    case SkullType::Even_Mulitplier:      return F("ALL EVEN MULT+1");
    case SkullType::Odd_Mulitplier:       return F("ALL ODD MULT+1");
    case SkullType::No_Reroll_Bonus:return F("NO REROLL:+15");
    case SkullType::High_Roll_Save: return F("HIGH ROLL SAVE");
    case SkullType::Threshold_Discount:return F("THRESHOLD -5%");
    case SkullType::Double_First:   return F("1ST HAND x2");
  }
  return F("");
}



uint16_t computeThreshold() {

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

void saveHighScore() {
  if (level > bestLevel) {
    bestLevel = level;
    EEPROM.update(EEPROM_ADDR, EEPROM_MAGIC);
    EEPROM.update(EEPROM_ADDR + 1, bestLevel);
  }
}

void loadHighScore() {
  if (EEPROM.read(EEPROM_ADDR) == EEPROM_MAGIC) {
    bestLevel = EEPROM.read(EEPROM_ADDR + 1);
    if (bestLevel == 0) bestLevel = 1;
  } else {
    bestLevel = 1;
  }
}


// ---------------------------------------------------------------------
// State updates
// ---------------------------------------------------------------------
void updateTitle() {
  if (arduboy.justPressed(A_BUTTON)) {
    level = 1;
    hand.deckCount = 0;
    startLevel();
  }
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
  if (arduboy.justPressed(UP_BUTTON) || arduboy.justPressed(DOWN_BUTTON)) {
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
  arduboy.setCursor(14, 0);
  arduboy.print(F("CHOOSE A SKULL"));
  arduboy.drawLine(0, 10, 127, 10, WHITE);

  uint8_t boxY = 18, boxH = 30, boxW = 60;
  int16_t xA = 2, xB = 66;

  arduboy.drawRect(xA, boxY, boxW, boxH, WHITE);
  arduboy.drawRect(xB, boxY, boxW, boxH, WHITE);

  if (skullCursor == 0) arduboy.fillRect(xA, boxY - 4, boxW, 3, WHITE);
  else arduboy.fillRect(xB, boxY - 4, boxW, 3, WHITE);

  arduboy.setCursor(xA + 3, boxY + 10);
  arduboy.print(skullName(skullChoiceA));
  arduboy.setCursor(xB + 3, boxY + 10);
  arduboy.print(skullName(skullChoiceB));

  arduboy.setCursor(10, 54);
  arduboy.print(F("<- ->  A TO PICK"));
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

void renderHandResult_Base() {

    Serial.print("renderHandResult_Base ");
    Serial.println(renderHandResult_Counter);

    switch (renderHandResult_Counter) {
    
        case 0:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            launchParticles();
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;
            }
            break;

        case 6:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = true;

            if (arduboy.isFrameCount(2)) {
                tempHandScore.totalBones++;
                tempHandScore.totalMultiplier = 0;
                tempHandScore.score++;
            }
            
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();
            Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
            FX::drawBitmap(59, 0, Images::Speech, 9, dbmWhite);

            if (tempHandScore.totalBones == hand.lastHandScore.baseBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;
                state = GameState::Game_Hand_Result_Hand;
            }

            break;

        // case 11 ... 14:
        //     for (uint8_t i = 0; i < 5; i++) hand.marked[i] = true;
        //     renderHandResult_Counter++;
        //     drawSkull();
        //     drawBonesMultTotal(tempHandScore);
        //     drawDice();
        //     drawFooterRoll();

        //     if (renderHandResult_Counter == 14) {
        //         renderHandResult_Counter = 0;
        //         state = GameState::Game_Hand_Result_Hand;
        //     }
        //     break;

    }


}


void renderHandResult_Hand() {
    Serial.print("renderHandResult_Hand ");
    Serial.println(renderHandResult_Counter);

    switch (renderHandResult_Counter) {
    
        case 0:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

                if (renderHandResult_Counter == 6) {
                
                    if (hand.lastHandScore.handMultiplier > 0) {

                        tempHandScore.totalMultiplier++;
                    
                    }

                }
            }
            break;

        case 6:

            hand.markWiningHand(hand.lastHandScore);

            if (hand.lastHandScore.handBones > 10 || (hand.lastHandScore.handBones <= 10 && arduboy.isFrameCount(4))) {

                tempHandScore.totalBones++;
                if ((tempHandScore.totalBones - hand.lastHandScore.baseBones) % hand.lastHandScore.handMultiplier == 0 &&
                    tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier) {
                    tempHandScore.totalMultiplier++;
                    
                }

            }

            if (tempHandScore.totalMultiplier > 0) {
                tempHandScore.score = tempHandScore.totalBones * tempHandScore.totalMultiplier;
            }
            else {
                tempHandScore.score = tempHandScore.totalBones;
            }
            
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();
            Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
            FX::drawBitmap(59, 0, Images::Speech, static_cast<uint8_t>(hand.lastHandScore.handType), dbmWhite);
            
            if (tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;
                
                if (hand.lastHandScore.skullBones > 0 || hand.lastHandScore.skullMultiplier > 0) {
                    state = GameState::Game_Hand_Result_Skulls_Played;
                }
                else {
                    state = GameState::Game_Hand_Result_Upgrades_Played;
                }

            }

            break;

    }

}

void renderHandResult_SkullsPlayed() {

   Serial.print("renderHandResult_SkullsPlayed ");
    Serial.println(renderHandResult_Counter);

    switch (renderHandResult_Counter) {
    
        case 0:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

                if (renderHandResult_Counter == 6) {
                
                    if (hand.lastHandScore.skullMultiplier > 0) {

                        tempHandScore.totalMultiplier++;
                    
                    }

                }
            }
            break;

        case 6:

            // hand.markWiningHand(hand.lastHandScore);

            if (hand.lastHandScore.skullBones >= 10 || (hand.lastHandScore.skullBones < 10 && arduboy.isFrameCount(4))) {

                tempHandScore.totalBones++;
                if ((tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones) % hand.lastHandScore.skullMultiplier == 0 &&
                    tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier) {
                    tempHandScore.totalMultiplier++;
                    
                }

            }

            if (tempHandScore.totalMultiplier > 0) {
                tempHandScore.score = tempHandScore.totalBones * tempHandScore.totalMultiplier;
            }
            else {
                tempHandScore.score = tempHandScore.totalBones;
            }
            
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();
            Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
            FX::drawBitmap(59, 0, Images::Speech, 10, dbmWhite);
            
            if (tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:
            for (uint8_t i = 0; i < 5; i++) hand.marked[i] = false;
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;
                state = GameState::Game_Hand_Result_Upgrades_Played;
            }

            break;

    }
                


}

void renderHandResult_UpgradesPlayed() {

    Serial.print("renderHandResult_UpgradesPlayed ");
    Serial.println(renderHandResult_Counter);


                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                


}
