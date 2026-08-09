#include <ArduboyFX.h>  
#include "src/utils/Constants.h" 

int16_t handScore_xMax = 0;
int16_t handScore_x = 0;

void handInfo_Init() {

    handScore_x = Constants::HandScore_StartingPos;
    gameState = GameState::Game_Hand_Info;

}

void handInfo() {


    UpgradeHand_BottomOptions botImage = UpgradeHand_BottomOptions::Both;


    // Handle input ..

    if (arduboy.justPressed(A_BUTTON)) {
        gameState = GameState::Game_Roll;
    }

    if (arduboy.pressed(LEFT_BUTTON)) {
        if (handScore_x < handScore_xMax) {
            handScore_x = handScore_x + 2;
        }
    
    }

    if (arduboy.pressed(RIGHT_BUTTON)) {
        if (handScore_x > Constants::HandScore_StartingPos) {
            handScore_x = handScore_x - 2;
        }
    
    }

    int16_t x = handScore_x;

    
    // Render screen ..
    

    FX::drawBitmap(x, 4, Images::Bones_Heading, 0, dbmNormal);
    drawNumber_Right(x, 45, hand.lastHandScore.baseBones);
    x = x - Constants::HandScore_LineSpacing;


    // Hand Bones ..

    if (hand.lastHandScore.handType != HandType::None) {

        FX::drawBitmap(x, 4, Images::Hand_Names, static_cast<uint8_t>(hand.lastHandScore.handType), dbmNormal);
        drawNumber_Right(x, 45, hand.lastHandScore.handBones);
        x = x - Constants::HandScore_LineSpacing;
    
    }


    // Skull Bones ..

    for (uint8_t i = 0; i < hand.deckCount; i++) {

        if (hand.deck[i].bones > 0) {
            FX::drawBitmap(x, 4, Images::Skull_Names, static_cast<uint8_t>(hand.deck[i].skullType), dbmNormal);
            drawNumber_Right(x, 45, hand.deck[i].bones);
            x = x - Constants::HandScore_LineSpacing;
        }

    }


    // Upgrade Bones ..

    if (hand.lastHandScore.upgradeBones > 0) {
        FX::drawBitmap(x, 4, Images::Upgrade, 0, dbmNormal);
        drawNumber_Right(x, 45, hand.lastHandScore.upgradeBones);
        x = x - Constants::HandScore_LineSpacing;
    }



    // Total Bones ..

    x = x - Constants::HandScore_LineSpacing + 4;
    drawNumber_Right(x, 45, hand.lastHandScore.totalBones);
    arduboy.drawFastVLine(x + 7, 50, 11, WHITE);
    x = x - Constants::HandScore_LineSpacing - 5;
    FX::drawBitmap(x, 4, Images::Multiplier, 0, dbmNormal);
    x = x - Constants::HandScore_LineSpacing - 2;


    // Hand Bones ..

    if (hand.lastHandScore.handType != HandType::None) {

        FX::drawBitmap(x, 4, Images::Hand_Names, static_cast<uint8_t>(hand.lastHandScore.handType), dbmNormal);
        drawNumber_Right(x, 45, hand.lastHandScore.handMultiplier);
        x = x - Constants::HandScore_LineSpacing;
    
    }


    // Skull Bones ..

    for (uint8_t i = 0; i < hand.deckCount; i++) {

        if (hand.deck[i].multiplier > 0) {
            FX::drawBitmap(x, 4, Images::Skull_Names, static_cast<uint8_t>(hand.deck[i].skullType), dbmNormal);
            drawNumber_Right(x, 45, hand.deck[i].multiplier);
            x = x - Constants::HandScore_LineSpacing;
        }

    }


    // Upgrade Bones ..

    if (hand.lastHandScore.upgradeMultiplier > 0) {
        FX::drawBitmap(x, 4, Images::Upgrade, 0, dbmNormal);
        drawNumber_Right(x, 45, hand.lastHandScore.upgradeMultiplier);
        x = x - Constants::HandScore_LineSpacing;

    }


    // Total Bones ..

    x = x - Constants::HandScore_LineSpacing + 4;
    drawNumber_Right(x, 45, hand.lastHandScore.totalMultiplier);
    arduboy.drawFastVLine(x + 7, 50, 11, WHITE);
    x = x - Constants::HandScore_LineSpacing;


    // Total ..

    x = x - Constants::HandScore_LineSpacing;
    FX::drawBitmap(x, 4, Images::Total, 0, dbmNormal);
    drawNumber_Right(x, 45, hand.lastHandScore.score);
    arduboy.drawFastVLine(x + 7, 44, 17, WHITE);
    x = x - Constants::HandScore_LineSpacing - Constants::HandScore_LineSpacing - 5;


    handScore_xMax = handScore_x - x;
    
    HandScore_BottomOptions bottomOptions_Idx = HandScore_BottomOptions::None;

    if (handScore_x == Constants::HandScore_StartingPos) {

        if (x > 16) {
            bottomOptions_Idx = HandScore_BottomOptions::None;  
        }
        else {
            bottomOptions_Idx = HandScore_BottomOptions::DownOnly;
        }

    }
    else {

        if (x >= 0) {
            bottomOptions_Idx = HandScore_BottomOptions::UpOnly;
        }
        else {
            bottomOptions_Idx = HandScore_BottomOptions::Both;
        }

    }


    FX::drawBitmap(128 - 48, 0, Images::HandScore_Top, 0, dbmNormal);
    FX::drawBitmap(0, 0, Images::HandScore_Bot, static_cast<uint8_t>(bottomOptions_Idx), dbmNormal);

}