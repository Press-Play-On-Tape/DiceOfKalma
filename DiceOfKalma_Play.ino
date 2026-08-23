#include <ArduboyFX.h>  

uint16_t computeThreshold() {

    // return 40;//SJH

    uint32_t base = 150 + (uint32_t)(level - 1) * 60 + (uint32_t)(level - 1) * (level - 1) * 20;
    uint8_t discountPct = hand.countSkull(SkullType::Threshold_Discount) * 5;
    if (discountPct > 20) discountPct = 20;
    base = base - (base * discountPct) / 100;
    return (uint16_t)base;

}

void startLevel() {

    threshold = computeThreshold();
    handsMax = 3 + hand.countSkull(SkullType::Extra_Hand);
    handsLeft = handsMax;
    rerollsMax = 3 + hand.countSkull(SkullType::Extra_Reroll);
    rerollsLeft = rerollsMax;
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
    takeTooLong =0;

}



void updateRoll() {

    takeTooLong++;

    if (takeTooLong % 512 == 0) { 
    
        takeTooLong_Type++;

        switch (takeTooLong_Type % 8) {

            case 0:
                addRollEyes(); 
                break;

            case 1:
                addLookLeft(false);
                break;

            case 2:
                addLookUp(false);
                break;

            case 3:
                addLookDown(false);
                break;

            case 4:
                addLookRight(false);
                break;

            case 5:
                addDeathStare(false);
                break;

            case 6:
                addPinPrick();
                break;       
                
            case 7:
                addWideEyes();
                break;                

        }

    }

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

        // // SJH .. Fix hands
        // for (uint8_t i = 0; i < 5; i++) {

        //     switch (i) {
        //         case 0:
        //         case 4:
        //             hand.dice[i] = 1;
        //             break;
        //         case 1:
        //         case 3:
        //             hand.dice[i] = 6;
        //             break;
        //         case 2:
        //             hand.dice[i] = 3;
        //             break;
        //     }
        // }

 // SJH .. Fix hands
        for (uint8_t i = 0; i < 5; i++) {

            hand.dice[i] = 3;
        }

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


void renderHandResult_DrawFrame(HandScore tempHandScore) {

    drawSkull();
    drawBonesMultTotal(tempHandScore);
    drawDice();
    drawFooterRoll();

};


void renderHandResult_DrawSpeechBubble(uint8_t imageIdx) {

    Sprites::drawOverwrite(60, 0, Images::Speech_Bubble, 0);
    FX::drawBitmap(59, 0, Images::Speech_Sml, imageIdx, dbmWhite);

};


void renderHandResult_Base() {

    switch (renderHandResult_Counter) {
    
        case 0:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_Timer = 0;
            addLookLeft(true);

            [[fallthrough]]

        case 1 ... 5:
            {
                renderHandResult_DrawFrame(tempHandScore);
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
            
            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_DrawSpeechBubble(9);

            if (renderHandResult_Timer > 48 && tempHandScore.totalBones == hand.lastHandScore.baseBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_DrawFrame(tempHandScore);

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
            addLookDown(true);            
            [[fallthrough]]

        case 1 ... 5:
            {
                renderHandResult_DrawFrame(tempHandScore);
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
// Serial.print(tempHandScore.totalBones - hand.lastHandScore.baseBones);
// Serial.print(" % ");
// Serial.print((hand.lastHandScore.handBones / hand.lastHandScore.handMultiplier));
// Serial.print(" ");
// Serial.println((tempHandScore.totalBones - hand.lastHandScore.baseBones) % (hand.lastHandScore.handBones / hand.lastHandScore.handMultiplier));
                    if ((tempHandScore.totalBones - hand.lastHandScore.baseBones) % (hand.lastHandScore.handBones / hand.lastHandScore.handMultiplier) == 0 &&
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
            
            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_DrawSpeechBubble(static_cast<uint8_t>(hand.lastHandScore.handType));
            
            if (renderHandResult_Timer > 48 && tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones) {
                renderHandResult_Counter++;
            }
            break;

        case 7 ... 10:

            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_DrawFrame(tempHandScore);

            if (renderHandResult_Counter == 10) {

                renderHandResult_Counter = 0;

                if (hand.lastHandScore.skullBones > 0 || hand.lastHandScore.skullMultiplier > 0) {

                    gameState = GameState::Game_Hand_Result_Skulls_Played;

                }
                else if (hand.lastHandScore.upgradeBones > 0 || hand.lastHandScore.upgradeMultiplier > 0) {

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
            addLookLeftThenRight(true);
            [[fallthrough]]

        case 1 ... 5:
            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_Counter++;

            if (renderHandResult_Counter == 6) {
                renderHandResult_Timer = 0;

                if (hand.lastHandScore.skullBones > 0) {
                    if (hand.lastHandScore.skullMultiplier > 0) {
                        tempHandScore.totalMultiplier++;
                    }
                }
                else {
                    renderHandResult_Counter = 7; // No skull bones -> multiplier-only phase
                }

            }

            break;

        case 6: // Player has skull bones and maybe multipliers
            renderHandResult_Timer++;

            if (hand.lastHandScore.skullBones >= 10 ||
                (hand.lastHandScore.skullBones < 10 && arduboy.isFrameCount(4))) {

                if (tempHandScore.totalBones < hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones) {

                    tempHandScore.totalBones++;

                    if (hand.lastHandScore.skullMultiplier > 0 && (tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones) % (hand.lastHandScore.skullMultiplier / hand.lastHandScore.skullMultiplier) == 0 &&
                        tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier) {
                        tempHandScore.totalMultiplier++;
                    }

                }

            }

            tempHandScore.score = tempHandScore.totalMultiplier > 0
                ? tempHandScore.totalBones * tempHandScore.totalMultiplier
                : tempHandScore.totalBones;

            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_DrawSpeechBubble(10);

            if (renderHandResult_Timer > 48 &&
                tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones) {
                renderHandResult_Counter = 8; // always skips case 7 from here
            }

            break;

        case 7: // Player only has skull multiplier (skullBones == 0)
            renderHandResult_Timer++;

            if (renderHandResult_Timer % 8 == 0 && tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier) {
                tempHandScore.totalMultiplier++;
                tempHandScore.score = tempHandScore.totalBones * tempHandScore.totalMultiplier;
            }

            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_DrawSpeechBubble(10);

            if (renderHandResult_Timer > 48 &&
                tempHandScore.totalMultiplier == hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier) {
                renderHandResult_Counter++;
            }

            break;

        case 8:
            hand.markAllCards(Marked::False);
            [[fallthrough]]

        case 9 ... 11:
            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_Counter++;

            if (renderHandResult_Counter == 10) {

                renderHandResult_Counter = 0;

                gameState = hand.lastHandScore.upgradeBones > 0
                    ? GameState::Game_Hand_Result_Upgrades_Played
                    : GameState::Game_Hand_Result_Countdown;

            }

            break;

    }

}

void renderHandResult_UpgradesPlayed() {

    switch (renderHandResult_Counter) {
    
        case 0:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            addLookLeftThenRight(true);
            [[fallthrough]]

        case 1 ... 5:
            {
                renderHandResult_DrawFrame(tempHandScore);
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
                    // Serial.print(tempHandScore.totalBones);
                    // Serial.print(" ");
                    // Serial.print(tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones - hand.lastHandScore.skullBones);
                    // Serial.print(" ");
                    // Serial.print((tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones - hand.lastHandScore.skullBones) % (hand.lastHandScore.upgradeBones / hand.lastHandScore.upgradeMultiplier));
                    if ((tempHandScore.totalBones - hand.lastHandScore.baseBones - hand.lastHandScore.handBones - hand.lastHandScore.skullBones) % (hand.lastHandScore.upgradeBones / hand.lastHandScore.upgradeMultiplier) == 0 &&
                        tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier + hand.lastHandScore.upgradeMultiplier) {
                    // Serial.print(" U ");
                    // Serial.print(tempHandScore.totalMultiplier + 1);

                        tempHandScore.totalMultiplier++;
                        
                    }
                    // Serial.println("  ");

                }

            }

            if (tempHandScore.totalMultiplier > 0) {
                tempHandScore.score = tempHandScore.totalBones * tempHandScore.totalMultiplier;
            }
            else {
                tempHandScore.score = tempHandScore.totalBones;
            }
            
            if (renderHandResult_Timer > 48 && (tempHandScore.totalBones == hand.lastHandScore.baseBones + hand.lastHandScore.handBones + hand.lastHandScore.skullBones + hand.lastHandScore.upgradeBones)) {

                if (tempHandScore.totalMultiplier < hand.lastHandScore.handMultiplier + hand.lastHandScore.skullMultiplier + hand.lastHandScore.upgradeMultiplier) {
                    tempHandScore.totalMultiplier++;
                    
                }

                renderHandResult_Counter++;

            }
            
            renderHandResult_DrawFrame(tempHandScore);
            renderHandResult_DrawSpeechBubble(11);

            break;

        case 7 ... 10:
            hand.markAllCards(Marked::False);
            renderHandResult_Counter++;
            renderHandResult_DrawFrame(tempHandScore);

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

