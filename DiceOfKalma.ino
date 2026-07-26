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
// ---------------------------------------------------------------------
// Game states
// ---------------------------------------------------------------------
// enum GameState : uint8_t {
//   STATE_TITLE,
//   STATE_LEVEL_INTRO,
//   STATE_ROLL,
//   STATE_HAND_RESULT,
//   STATE_SKULL_CHOICE,
//   STATE_DECK_FULL_SWAP,
//   STATE_DECK_VIEW,
//   STATE_GAMEOVER,
//   STATE_WIN
// };

GameState state = GameState::Splash;
GameState returnState =GameState::Splash; // where DECK_VIEW returns to
uint8_t stateTimer = 0;

// ---------------------------------------------------------------------
// Dice
// ---------------------------------------------------------------------
uint8_t dice[5];
bool    marked[5];
uint8_t cursor = 0;
uint8_t diceCursor = 0;
uint8_t selRow = 0;      // 0 = dice row, 1 = REROLL/PLAY button row
uint8_t buttonCursor = 0; // 0 = REROLL, 1 = PLAY (only used when selRow == 1)
bool    rerollUsedThisHand = false;
bool    firstHandOfLevel = true;

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


HandScore lastHandScore;
HandScore tempHandScore;

// last hand result, for the result screen


// cursor 0-4 = dice, CURSOR_REROLL = the REROLL button, CURSOR_PLAY = the PLAY button
#define CURSOR_PLAY   5
#define CURSOR_DECK 6
#define CURSOR_REROLL 7

// ---------------------------------------------------------------------
// SKULLS - the deckbuilding layer.
// Each skull is a passive that triggers off hand type / dice values /
// how the hand was played. Skulls are stackable: owning 2 copies of a
// mult skull doubles its bonus, etc.
// ---------------------------------------------------------------------
enum SkullType : uint8_t {
  SK_PAIR_MULT = 0,   // +1 mult on PAIR / TWO PAIR
  SK_BIG_MULT,        // +1 mult on 3 KIND / STRAIGHT / FULL HOUSE
  SK_KIND_MULT,       // +1 mult on 4 / 5 OF A KIND
  SK_FLAT_CHIPS,      // +10 chips every hand
  SK_EXTRA_REROLL,    // +1 reroll per hand
  SK_EXTRA_HAND,      // +1 hand per level
  SK_SIX_BONUS,       // +3 chips per die showing a 6
  SK_ACE_BONUS,       // +3 chips per die showing a 1
  SK_EVEN_MULT,       // +1 mult if all 5 dice are even
  SK_ODD_MULT,        // +1 mult if all 5 dice are odd
  SK_NO_REROLL_BONUS, // +15 chips if hand played with 0 rerolls used
  SK_HIGH_ROLL_SAVE,  // HIGH ROLL hands score as if base 15 chips
  SK_THRESH_DISCOUNT, // threshold -5% (stacks, capped)
  SK_DOUBLE_FIRST,    // first hand played each level scores x2
  SKULL_COUNT
};

#define MAX_DECK 6
uint8_t deck[MAX_DECK];
uint8_t deckCount = 0;

uint8_t skullChoiceA, skullChoiceB;
uint8_t skullCursor = 0;
uint8_t pendingSkull = 0;   // skull waiting for a deck slot when deck is full
uint8_t swapCursor = 0;
uint8_t deckViewCursor = 0;

uint8_t countSkull(uint8_t id) {
  uint8_t n = 0;
  for (uint8_t i = 0; i < deckCount; i++) if (deck[i] == id) n++;
  return n;
}

const __FlashStringHelper* skullName(uint8_t s) {
  switch (s) {
    case SK_PAIR_MULT:      return F("PAIR MULT +1");
    case SK_BIG_MULT:       return F("3K/STR/FH MULT+1");
    case SK_KIND_MULT:      return F("4-5 KIND MULT+1");
    case SK_FLAT_CHIPS:     return F("+10 CHIPS/HAND");
    case SK_EXTRA_REROLL:   return F("+1 REROLL/HAND");
    case SK_EXTRA_HAND:     return F("+1 HAND/LEVEL");
    case SK_SIX_BONUS:      return F("+3 CHIPS PER 6");
    case SK_ACE_BONUS:      return F("+3 CHIPS PER 1");
    case SK_EVEN_MULT:      return F("ALL EVEN MULT+1");
    case SK_ODD_MULT:       return F("ALL ODD MULT+1");
    case SK_NO_REROLL_BONUS:return F("NO REROLL:+15");
    case SK_HIGH_ROLL_SAVE: return F("HIGH ROLL SAVE");
    case SK_THRESH_DISCOUNT:return F("THRESHOLD -5%");
    case SK_DOUBLE_FIRST:   return F("1ST HAND x2");
  }
  return F("");
}

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------
void rollAllDice() {
  for (uint8_t i = 0; i < 5; i++) {
    dice[i] = random(1, 7);
    marked[i] = false;
  }
}

void rerollMarked() {
  for (uint8_t i = 0; i < 5; i++) {
    if (marked[i]) {
      dice[i] = random(1, 7);
      marked[i] = false;
    }
  }
}

uint8_t diceSum() {
  uint8_t s = 0;
  for (uint8_t i = 0; i < 5; i++) s += dice[i];
  return s;
}

// Scores the current dice hand. Fills lastChips/lastMult/lastHandName.
uint16_t evaluateHand() {

  uint8_t counts[7] = {0, 0, 0, 0, 0, 0, 0};
  for (uint8_t i = 0; i < 5; i++) counts[dice[i]]++;

  uint8_t maxCount = 0;
  for (uint8_t v = 1; v <= 6; v++) if (counts[v] > maxCount) maxCount = counts[v];

  uint8_t pairCount = 0, threeCount = 0;
  for (uint8_t v = 1; v <= 6; v++) {
    if (counts[v] == 2) pairCount++;
    if (counts[v] == 3) threeCount++;
  }

  bool straightLow  = counts[1] && counts[2] && counts[3] && counts[4] && counts[5];
  bool straightHigh = counts[2] && counts[3] && counts[4] && counts[5] && counts[6];
  bool isStraight = straightLow || straightHigh;

  bool allEven = true, allOdd = true;
  for (uint8_t i = 0; i < 5; i++) {
    if (dice[i] % 2 == 0) allOdd = false; else allEven = false;
  }

  uint8_t sixCount = counts[6];
  uint8_t aceCount = counts[1];

  uint16_t base;
  uint8_t mult;
  uint8_t handCategory; // 0=high,1=pair-ish,2=big,3=kind
  HandType handType = HandType::None; 

  if (maxCount == 5) {
    strcpy(lastHandScore.handName, "5 OF A KIND");
    base = 100; mult = 8; handCategory = 3;
    handType = HandType::Five_of_a_Kind;
  } else if (maxCount == 4) {
    strcpy(lastHandScore.handName, "4 OF A KIND");
    base = 60; mult = 5; handCategory = 3;
    handType = HandType::Four_of_a_Kind;
  } else if (maxCount == 3 && pairCount == 1) {
    strcpy(lastHandScore.handName, "FULL HOUSE");
    base = 35; mult = 4; handCategory = 2;
    handType = HandType::Full_House;
  } else if (isStraight) {
    strcpy(lastHandScore.handName, "STRAIGHT");
    base = 40; mult = 4; handCategory = 2;
    handType = HandType::Straight;
  } else if (threeCount == 1) {
    strcpy(lastHandScore.handName, "3 OF A KIND");
    base = 30; mult = 3; handCategory = 2;
    handType = HandType::Three_of_a_Kind;
  } else if (pairCount == 2) {
    strcpy(lastHandScore.handName, "TWO PAIR");
    base = 20; mult = 2; handCategory = 1;
    handType = HandType::Two_Pair;
  } else if (pairCount == 1) {
    strcpy(lastHandScore.handName, "PAIR");
    base = 10; mult = 2; handCategory = 1;
    handType = HandType::Pair;
  } else {
    strcpy(lastHandScore.handName, "HIGH ROLL");
    base = 0; mult = 1; handCategory = 0;
    handType = HandType::High_Roll;
  }

  if (handCategory == 0 && countSkull(SK_HIGH_ROLL_SAVE) > 0) {
    base = 15;
  }

  // skull-driven mult bonuses
  if (handCategory == 1) mult += countSkull(SK_PAIR_MULT);
  if (handCategory == 2) mult += countSkull(SK_BIG_MULT);
  if (handCategory == 3) mult += countSkull(SK_KIND_MULT);
  if (allEven) mult += countSkull(SK_EVEN_MULT);
  if (allOdd)  mult += countSkull(SK_ODD_MULT);

  // skull-driven chip bonuses
  uint16_t chips = base + diceSum();
  chips += countSkull(SK_FLAT_CHIPS) * 10;
  chips += countSkull(SK_SIX_BONUS) * 3 * sixCount;
  chips += countSkull(SK_ACE_BONUS) * 3 * aceCount;
  if (!rerollUsedThisHand) chips += countSkull(SK_NO_REROLL_BONUS) * 15;

  uint16_t score = chips * mult;
  if (firstHandOfLevel && countSkull(SK_DOUBLE_FIRST) > 0) score *= 2;

  lastHandScore.baseBones = diceSum();
  lastHandScore.totalBones = chips;
  lastHandScore.multiplier = mult;
  lastHandScore.score = score;
  lastHandScore.handType = handType;
  return score;
}

uint16_t computeThreshold() {
  uint32_t base = 150 + (uint32_t)(level - 1) * 60 + (uint32_t)(level - 1) * (level - 1) * 20;
  uint8_t discountPct = countSkull(SK_THRESH_DISCOUNT) * 5;
  if (discountPct > 40) discountPct = 40;
  base = base - (base * discountPct) / 100;
  return (uint16_t)base;
}

void startLevel() {
  threshold = computeThreshold();
  runScore = 0;
  handsMax = 3 + countSkull(SK_EXTRA_HAND);
  handsLeft = handsMax;
  rerollsMax = 3 + countSkull(SK_EXTRA_REROLL);
  rerollsLeft = rerollsMax;
  diceCursor = 0;
  selRow = 0;
  buttonCursor = 0;
  firstHandOfLevel = true;
  rerollUsedThisHand = false;
  rollAllDice();
  state = GameState::Game_Level_Intro;
  stateTimer = 0;
}

void newHand() {
  rerollsLeft = rerollsMax;
  cursor = 0;
  rerollUsedThisHand = false;
  rollAllDice();
}

void offerSkulls() {
  skullChoiceA = random(0, SKULL_COUNT);
  do {
    skullChoiceB = random(0, SKULL_COUNT);
  } while (skullChoiceB == skullChoiceA);
  skullCursor = 0;
  state = GameState::Game_Skull_Choice;
}

void addSkullToDeck(uint8_t id) {
  if (deckCount < MAX_DECK) {
    deck[deckCount++] = id;
    level++;
    if (level > 25) state = GameState::Game_Win; else startLevel();
  } else {
    pendingSkull = id;
    swapCursor = 0;
    state = GameState::Game_Deck_Full_Swap;
  }
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
    deckCount = 0;
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

  evaluateHand(); // live preview of current dice

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
      marked[cursor] = !marked[cursor];
    } else if (cursor == CURSOR_REROLL) {
      bool anyMarked = false;
      for (uint8_t i = 0; i < 5; i++) if (marked[i]) anyMarked = true;
      if (rerollsLeft > 0 && anyMarked) {
        rerollMarked();
        rerollsLeft--;
        rerollUsedThisHand = true;
      }
    } else if (cursor == CURSOR_PLAY) { // CURSOR_PLAY
      evaluateHand();
      runScore += lastHandScore.score;
      handsLeft--;
      firstHandOfLevel = false;
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
    uint8_t chosen = (skullCursor == 0) ? skullChoiceA : skullChoiceB;
    addSkullToDeck(chosen);
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
    deck[swapCursor] = pendingSkull;
    level++;
    if (level > 25) state = GameState::Game_Win; else startLevel();
  }
  if (arduboy.justPressed(B_BUTTON)) {
    level++;
    if (level > 25) state = GameState::Game_Win; else startLevel();
  }
}

void updateDeckView() {
  if (arduboy.justPressed(UP_BUTTON) && deckCount > 0) {
    deckViewCursor = (deckViewCursor == 0) ? deckCount - 1 : deckViewCursor - 1;
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

// ---------------------------------------------------------------------
// State draws
// ---------------------------------------------------------------------
void drawTitle() {

    // Sprites::drawOverwrite(0, 0, Images::Title, 0);
    FX::drawBitmap(0, 0 , Images::Title, 0, dbmNormal);
    drawNumber(11, 46, bestLevel);

}



void drawRoll() {
  drawHeader(lastHandScore);
  drawDice();
  drawFooterRoll();
}

void drawHandResult() {
  arduboy.setCursor(24, 14);
  arduboy.print(lastHandScore.handName);
  arduboy.setCursor(24, 26);
  arduboy.print(lastHandScore.baseBones);
  arduboy.print(F(" x "));
  arduboy.print(lastHandScore.multiplier);
  arduboy.setCursor(24, 36);
  arduboy.print(F("= "));
  arduboy.print(lastHandScore.score);
  arduboy.print(F(" PTS"));

  arduboy.setCursor(10, 50);
  arduboy.print(F("TOTAL: "));
  arduboy.print(runScore);
  arduboy.print(F("/"));
  arduboy.print(threshold);
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
    arduboy.print(skullName(deck[i]));
  }
  arduboy.setTextColor(WHITE);
}

void drawDeckView() {
  arduboy.setCursor(2, 0);
  arduboy.print(F("YOUR SKULLS ("));
  arduboy.print(deckCount);
  arduboy.print(F(")"));
  arduboy.drawLine(0, 9, 127, 9, WHITE);

  if (deckCount == 0) {
    arduboy.setCursor(10, 28);
    arduboy.print(F("NO SKULLS YET"));
  } else {
    for (uint8_t i = 0; i < deckCount; i++) {
      int16_t y = 11 + i * 8;
      if (i == deckViewCursor) arduboy.fillRect(0, y, 128, 8, WHITE);
      uint8_t col = (i == deckViewCursor) ? BLACK : WHITE;
      arduboy.setCursor(2, y);
      arduboy.setTextColor(col);
      arduboy.print(skullName(deck[i]));
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
  arduboy.print(deckCount);
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
  arduboy.print(deckCount);
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
            tempHandScore.baseBones = 0;
            tempHandScore.multiplier = 0;
            tempHandScore.score = 0;
            state = GameState::Game_Hand_Result_Base;
            [[fallthrough]]

        case GameState::Game_Hand_Result_Base:   
            renderHandResult_Base();           
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
    FX::display(CLEAR_BUFFER);

}

void renderHandResult_Base() {

    switch (renderHandResult_Counter) {
    
        case 0:
            for (uint8_t i = 0; i < 5; i++) marked[i] = false;
            renderHandResult_Counter++;
            [[fallthrough]]

        case 1 ... 5:
            {
                drawHeader(tempHandScore);
                drawDice();
                drawFooterRoll();
                arduboy.fillRect(60, 0, 16, 64, BLACK);
                renderHandResult_Counter++;
            }
            break;

        case 6:
            for (uint8_t i = 0; i < 5; i++) marked[i] = true;
            tempHandScore.totalBones++;
            tempHandScore.multiplier = 0;
            tempHandScore.score++;
            
            drawHeader(tempHandScore);
            drawDice();
            drawFooterRoll();
            Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
            Sprites::drawSelfMasked(59, 2, Images::Speech, 9);
            
            if (tempHandScore.totalBones == lastHandScore.baseBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:
            for (uint8_t i = 0; i < 5; i++) marked[i] = false;
            renderHandResult_Counter++;
            drawHeader(tempHandScore);
            drawDice();
            drawFooterRoll();
            // [[fallthrough]]
            break;

        case 11 ... 14:
            for (uint8_t i = 0; i < 5; i++) marked[i] = true;
            renderHandResult_Counter++;
            drawHeader(tempHandScore);
            drawDice();
            drawFooterRoll();
            // [[fallthrough]]
            break;

    }


}

