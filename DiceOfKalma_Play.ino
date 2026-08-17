#include <ArduboyFX.h>  


uint16_t computeThreshold() {

    // return 400;//SJH

    uint32_t base = 150 + (uint32_t)(level - 1) * 60 + (uint32_t)(level - 1) * (level - 1) * 20;
    uint8_t discountPct = hand.countSkull(SkullType::Threshold_Discount) * 5;
    if (discountPct > 40) discountPct = 40;
    base = base - (base * discountPct) / 100;
    return (uint16_t)base;

}

void startLevel() {

    threshold = computeThreshold();
    // runScore = 0;
    handsMax = 3 + hand.countSkull(SkullType::Extra_Hand);
    handsLeft = handsMax;
    rerollsMax = 3 + hand.countSkull(SkullType::Extra_Reroll);
    rerollsLeft = rerollsMax;
    // hand.rollAll();
    hand.firstHandOfLevel = true;
    hand.rerollUsedThisHand = false;
    hand.resetDice();
    gameState = GameState::Game_Level_Intro;
    stateTimer = 0;

}

void newHand() {

    rerollsLeft = rerollsMax;
    cursor = 0;
    hand.rerollUsedThisHand = false;

}


void updateLevelIntro() {

    stateTimer++;

    if (stateTimer > 60 || arduboy.justPressed(A_BUTTON)) {
        hand.rollAll();
        gameState = GameState::Game_Roll_Dice;
        returnState = GameState::Game_Roll;
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

    if (arduboy.justPressed(B_BUTTON)) {

        gameState = GameState::Game_Hand_Info_Init;

    }

    if (arduboy.justPressed(A_BUTTON)) {

        if (cursor < 5) {

            if (hand.marked[cursor] == Marked::True) {
                hand.marked[cursor] = Marked::False;
            }
            else {
                hand.marked[cursor] = Marked::True;
            }

        } 
        else if (cursor == CURSOR_REROLL) {

            bool anyMarked = false;
            hand.rerollHighlight = Constants::RerollHighlight_Minimum;
            
            for (uint8_t i = 0; i < 5; i++) if (hand.marked[i] == Marked::True) anyMarked = true;

            if (rerollsLeft > 0 && anyMarked) {

                rerollsLeft--;
                hand.rerollUsedThisHand = 4;
                gameState = GameState::Game_Roll_Dice;
                returnState = GameState::Game_Roll;
                rollDice_Counter = 1;

            }

        } 
        else if (cursor == CURSOR_PLAY) { 

            hand.playHandHighlight = Constants::PlayHandlHighlight_Minimum;
            hand.evaluateHand();
            //   runScore += hand.lastHandScore.score;
            handsLeft--;
            hand.firstHandOfLevel = false;
            gameState = GameState::Game_Hand_Result_Init;
            stateTimer = 0;

        } 
        else if (cursor == CURSOR_DECK) {

            returnState = GameState::Game_Roll;
            deckViewCursor = 0;
            gameState = GameState::Game_Deck_View;

        }

    }

}



void updateWin() {
  if (arduboy.justPressed(A_BUTTON)) {
    gameState = GameState::Title;
  }
}


void renderRollDice() {

    if (rollDice_Counter == 6) {
// DEBUG_BREAK

        for (uint8_t i = 0; i < 5; i++) {
            
            if (hand.marked[i] == Marked::True || hand.marked[i] == Marked::True_NoHighlight) {
        
                hand.rerollDice(i);
                hand.marked[i] = Marked::True_NoHighlight;

            }

        }

        // SJH .. Fix hands
        // for (uint8_t i = 0; i < 5; i++) {
        //     hand.dice[i] = i + 1;
        // }
        //     hand.dice[4] = 6;

    }

    rollDice_Counter++;

    drawSkull();
    drawLevelAndTarget(hand.lastHandScore, level, threshold);
    drawBonesMultTotal(tempHandScore);
    drawDice();
    drawFooterRoll();

    if (rollDice_Counter == 11) {

        gameState = returnState;
        rollDice_Counter = 0;

        hand.markAllCards(Marked::False);
        hand.rerollHighlight = Constants::RerollHighlight_None;

    }

}

void renderHandResult_Base() {

    switch (renderHandResult_Counter) {
    
        case 0:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_Timer = 0;
            addLookLeft();

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

            renderHandResult_Timer++;
            hand.markAllCards(Marked::True);

            if (arduboy.isFrameCount(2) && tempHandScore.totalBones < hand.lastHandScore.baseBones) {
                tempHandScore.totalBones++;
                tempHandScore.totalMultiplier = 0;
                tempHandScore.score++;
            }
            
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();
            Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
            FX::drawBitmap(59, 0, Images::Speech_Sml, 9, dbmWhite);

            if (renderHandResult_Timer > 48 && tempHandScore.totalBones == hand.lastHandScore.baseBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;

                if (hand.lastHandScore.handBones > 0) {

                    gameState = GameState::Game_Hand_Result_Hand;

                }
                else if (hand.lastHandScore.skullBones > 0) {

                    gameState = GameState::Game_Hand_Result_Skulls_Played;

                }
                else if (hand.lastHandScore.upgradeBones > 0) {

                    gameState = GameState::Game_Hand_Result_Upgrades_Played;

                }
                else {

                    gameState = GameState::Game_Hand_Result_Countdown;
                
                };


            }

            break;

    }


}


void renderHandResult_Hand() {

    switch (renderHandResult_Counter) {
    
        case 0:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_Timer = 0;
            addLookDown();            
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

                if (renderHandResult_Counter == 6) {
                    
                    renderHandResult_Timer = 0;

                    if (hand.lastHandScore.handMultiplier > 0) {

                        tempHandScore.totalMultiplier++;
                    
                    }

                }
            }
            break;

        case 6:

            renderHandResult_Timer++;
            hand.markWiningHand(hand.lastHandScore);

            if (hand.lastHandScore.handBones > 10 || (hand.lastHandScore.handBones <= 10 && arduboy.isFrameCount(4))) {

                if (tempHandScore.totalBones < hand.lastHandScore.baseBones + hand.lastHandScore.handBones) {

                    tempHandScore.totalBones++;

                    if (tempHandScore.handBones % hand.lastHandScore.handMultiplier == 0 &&
                        tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier) {
                        tempHandScore.totalMultiplier++;
                        
                    }

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
            FX::drawBitmap(59, 0, Images::Speech_Sml, static_cast<uint8_t>(hand.lastHandScore.handType), dbmWhite);
            
            if (renderHandResult_Timer > 48 && tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {

                renderHandResult_Counter = 0;

                if (hand.lastHandScore.skullBones > 0) {

                    gameState = GameState::Game_Hand_Result_Skulls_Played;

                }
                else if (hand.lastHandScore.upgradeBones > 0) {

                    gameState = GameState::Game_Hand_Result_Upgrades_Played;

                }
                else {

                    gameState = GameState::Game_Hand_Result_Countdown;
                
                };

            }

            break;

    }

}

void renderHandResult_SkullsPlayed() {

    switch (renderHandResult_Counter) {
    
        case 0:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            addLookLeftThenRight();
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

                if (renderHandResult_Counter == 6) {
                
                    renderHandResult_Timer = 0;

                    if (hand.lastHandScore.skullMultiplier > 0) {

                        tempHandScore.totalMultiplier++;
                    
                    }

                }
            }
            break;

        case 6:

            renderHandResult_Timer++;

            if (hand.lastHandScore.skullBones >= 10 || (hand.lastHandScore.skullBones < 10 && arduboy.isFrameCount(4))) {

                if (tempHandScore.totalBones < hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones) {

                    tempHandScore.totalBones++;
                    if ((tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones) % hand.lastHandScore.skullMultiplier == 0 &&
                        tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier) {
                        tempHandScore.totalMultiplier++;
                        
                    }

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
            FX::drawBitmap(59, 0, Images::Speech_Sml, 10, dbmWhite);
            
            if (renderHandResult_Timer > 48 && tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;

                if (hand.lastHandScore.upgradeBones > 0) {

                    gameState = GameState::Game_Hand_Result_Upgrades_Played;

                }
                else {

                    gameState = GameState::Game_Hand_Result_Countdown;
                
                };

            }

            break;

    }

}

void renderHandResult_UpgradesPlayed() {

    switch (renderHandResult_Counter) {
    
        case 0:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            addLookLeftThenRight();
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

                if (renderHandResult_Counter == 6) {
                                
                    renderHandResult_Timer = 0;

                    if (hand.lastHandScore.upgradeMultiplier > 0) {

                        tempHandScore.totalMultiplier++;
                    
                    }

                }
            }
            break;

        case 6:

            renderHandResult_Timer++;

            if (hand.lastHandScore.upgradeBones >= 10 || (hand.lastHandScore.upgradeBones < 10 && arduboy.isFrameCount(4))) {

                if (tempHandScore.totalBones < hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones + hand.lastHandScore.upgradeBones) {

                    tempHandScore.totalBones++;
                    if ((tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones - hand.lastHandScore.skullBones) % hand.lastHandScore.skullMultiplier == 0 &&
                        tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier + hand.lastHandScore.upgradeMultiplier) {
                        tempHandScore.totalMultiplier++;
                        
                    }

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
            FX::drawBitmap(59, 0, Images::Speech_Sml, 11, dbmWhite);
            
            if (renderHandResult_Timer > 48 && (tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones + hand.lastHandScore.upgradeBones)) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {
                renderHandResult_Counter = 0;
                gameState = GameState::Game_Hand_Result_Countdown;
            }

            break;

    }


}

void renderHandResult_Countdown() {

    switch (renderHandResult_Counter) {
    
        case 0:
            countdownDiv = min(tempHandScore.score, threshold) / 50;
            if (countdownDiv == 0) countdownDiv = 1;
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            addWideEyes();

            if (tempHandScore.score < threshold && handsLeft == 0) {
// Serial.println("810");
                messageIdx = random(8, 10);
            }
            else {
// Serial.println("08");
                messageIdx = random(0, 8);
            }
            [[fallthrough]]

        case 1 ... 5:
            {
                drawSkull();
                drawLevelAndTarget(hand.lastHandScore, level, threshold);
                drawBonesMultTotal(tempHandScore);
                drawDice();
                drawFooterRoll();
                renderHandResult_Counter++;

            }
            break;

        case 6:

            // hand.markWiningHand(hand.lastHandScore);
            if (tempHandScore.score >= countdownDiv && threshold >= countdownDiv) {
                tempHandScore.score = tempHandScore.score - countdownDiv;
                threshold = threshold - countdownDiv;
            }
            else if (tempHandScore.score >= countdownDiv && threshold < countdownDiv) {
                tempHandScore.totalBones = 0;
                tempHandScore.totalMultiplier = 0;
                tempHandScore.score = tempHandScore.score - countdownDiv;
                threshold = 0;
                renderHandResult_Counter++;
            }
            else if (tempHandScore.score < countdownDiv && threshold >= countdownDiv) {
                tempHandScore.totalBones = 0;
                tempHandScore.totalMultiplier = 0;
                tempHandScore.score = 0;
                threshold = threshold - countdownDiv;
                renderHandResult_Counter++;
            }
            else if (tempHandScore.score < countdownDiv && threshold < countdownDiv) {
                tempHandScore.score = 0;
                tempHandScore.totalBones = 0;
                tempHandScore.totalMultiplier = 0;
                threshold = 0;
                renderHandResult_Counter++;
            }
            
            drawSkull();
            drawLevelAndTarget(hand.lastHandScore, level, threshold);
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();
             
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);

            if (renderHandResult_Counter < 10) {

                renderHandResult_Counter++;

                if (renderHandResult_Counter == 10) {
                    addLongTalk();
                }

            }

            drawSkull();
            drawLevelAndTarget(hand.lastHandScore, level, threshold);
            drawBonesMultTotal(tempHandScore);
            drawDice();
            drawFooterRoll();

            if (renderHandResult_Counter == 10) {

                hand.rerollHighlight = Constants::RerollHighlight_None;
                hand.playHandHighlight = Constants::PlayHandHighlight_None;

                if (threshold == 0) {

                    
                    FX::drawBitmap(42, 0, Images::Speech_Lrg, messageIdx, dbmNormal);

                    if (arduboy.isFrameCount(16)) launchParticles();

                    if (arduboy.justPressed(A_BUTTON)) {
                        renderHandResult_Counter = 0;
                        level++;

                        if (level > Constants::Level_Count) {
                            gameState = GameState::Game_Win; 
                        }
                        else {
                            gameState = GameState::Game_Skull_Choice_Init;
                        }

                    }

                }
                else {
                
                    renderHandResult_Counter++;

                    if (handsLeft == 0) {
                        renderHandResult_Counter++;
                    }

                }

            }

            break;

        case 11:

            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            FX::drawBitmap(42, 0, Images::Speech_Lrg, messageIdx, dbmNormal);
            drawFooterRoll();

            if (arduboy.justPressed(A_BUTTON)) {

                hand.markAllCards(Marked::True_NoHighlight);
                renderHandResult_Counter = 0;
                rollDice_Counter = 0;
                gameState = GameState::Game_Roll_Dice;
                returnState = GameState::Game_Roll;

            }

            break;

        case 12:

            drawSkull();
            drawBonesMultTotal(tempHandScore);
            drawDice();
            FX::drawBitmap(42, 0, Images::Speech_Lrg, messageIdx, dbmNormal);
            drawFooterRoll();

            if (arduboy.justPressed(A_BUTTON)) {

                // Serial.println("Game Over 1");
                gameState = GameState::Game_Over;

            }

            break;

    }

}

