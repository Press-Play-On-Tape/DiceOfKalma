#include <ArduboyFX.h>  

#include "fxdata/fxdata.h"
#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Constants.h"
#include "src/utils/Structs.h"
#include "src/utils/Stack.h"
#include "src/entities/Hand.h"
#include "src/entities/Particle.h"
#include "src/entities/SkullData.h"
#include "src/entities/Cookie.h"

#include "images.h"

Arduboy2Ext arduboy;


uint8_t renderHandResult_Counter = 0;
uint8_t renderHandResult_Timer = 0;
uint8_t countdownDiv = 0;

GameState gameState = GameState::Splash;
GameState returnState =GameState::Splash; // where DECK_VIEW returns to

uint8_t stateTimer = 0;
uint8_t cursor = 0;
uint8_t rollDice_Counter = 0;

// ---------------------------------------------------------------------
// Run state
// ---------------------------------------------------------------------

uint8_t  level = 1;
int16_t threshold = 0;
int16_t thresholdMin = 0;
uint8_t  handsLeft = 0;
uint8_t  handsMax  = 0;
uint8_t  rerollsLeft = 0;
uint8_t  rerollsMax  = 0;
uint16_t bCounter = 0;

Hand hand;

// last hand result, for the result screen

Cookie cookie;
HandScore tempHandScore;
Particle particles[Constants::ParticlesMax];

SkullType skullChoiceA, skullChoiceB;
SkullType skullInfoType;
SkullType pendingSkull = SkullType::None;   

uint8_t skullCursor = 0;
uint8_t upgradeCursor = 0;
uint8_t upgradeTop = 0;
uint8_t deckViewCursor = 0;
uint8_t deckViewTop = 0;
uint8_t messageIdx = 0;
uint16_t takeTooLong = 0;
uint8_t takeTooLong_Type = 0;

Stack <uint16_t, 60> skullStack;
SkullData skullData;

void setup() {
    
    arduboy.boot();
    arduboy.setFrameRate(30);

    FX::display(CLEAR_BUFFER);
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    FX::loadGameState((uint8_t*)&cookie, sizeof(cookie));

}

void loop() {

    if (!arduboy.nextFrame()) return;
    arduboy.pollButtons();
    arduboy.clear();

    switch (gameState) {

        case GameState::Splash:         
            splashScreen();         
            break;

        case GameState::Title:         
            title();         
            break;

        case GameState::Game_Level_Intro:   
            levelIntro();    
            break;

        case GameState::Game_Roll_Dice:          
            renderRollDice();          
            break;

        case GameState::Game_Roll:          
            updateRoll();          
            drawSkull();
            drawLevelAndTarget(hand.getLastHandScore(), level, threshold);

            if (bCounter > 16) {
                drawBonesMultTotal(hand.getLastHandScore());
            }
            else {
                FX::drawBitmap(0, 0, Images::Background_00, 0, dbmNormal);
            }

            drawDice();
            drawFooterRoll();    
            break;

        case GameState::Game_Hand_Result_Init:  
            thresholdMin = (threshold > hand.getLastHandScore().score ? threshold - hand.getLastHandScore().score : 0); 
            renderHandResult_Counter = 0;
            tempHandScore.reset();
            gameState = GameState::Game_Hand_Result_Base;
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

        case GameState::Game_Skull_Choice_Init:  
            skullChoice_Init();   
            [[fallthrough]]

        case GameState::Game_Skull_Choice:  
            skullChoice(); 
            break;

        case GameState::Game_Upgrade_Choice_Init:  
            upgradeHand_Init();   
            [[fallthrough]]

        case GameState::Game_Upgrade_Choice:  
            upgradeHand();   
            break;

        case GameState::Game_Skull_Info:  
            drawSkullInfo();   
            break;

        case GameState::Game_Hand_Info_Init:  
            handInfo_Init();   
            [[fallthrough]]

        case GameState::Game_Hand_Info:  
            handInfo();   
            break;

        case GameState::Game_Deck_Full_Swap:
        case GameState::Game_Deck_View:     
            deckView();      
            break;

        case GameState::Game_Over:      
            gameOver();      
            break;

        case GameState::Game_Win:           
            win();           
            break;
    }

    updateAndRenderParticles();
    FX::display(CLEAR_BUFFER);

    if (!skullStack.isEmpty() && arduboy.isFrameCount(2)) {
        uint16_t data = skullStack.pop();
        skullData.setData(data);
    }

}
