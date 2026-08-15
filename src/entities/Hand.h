#pragma once

#include <Arduino.h>

#include "../utils/Constants.h"
#include "../utils/Structs.h"
#include "DeckEntry.h"

class Hand {
    
    public:

        uint8_t dice[5];
        uint8_t deckCount = 0;
        Marked marked[5];
        DeckEntry deck[MAX_DECK];
        HandType upgradeHand = HandType::None;
        
        bool rerollUsedThisHand = false;
        bool firstHandOfLevel = true;
        uint8_t rerollHighlight = Constants::RerollHighlight_None;
        uint8_t playHandHighlight = Constants::PlayHandHighlight_None;

        HandScore lastHandScore;


    private:

        // Returns the dice value (not index) of the nominated qty.  If dice are 2, 3, 3, 5, 1 and you call getDice_OfaKind(2, 0) returns a 3.
        
        uint8_t getDice_OfaKind(uint8_t number, uint8_t higherThan) { 

            uint8_t counts[7] = {0, 0, 0, 0, 0, 0, 0};
            for (uint8_t i = 0; i < 5; i++) counts[this->dice[i]]++;

            for (uint8_t i = higherThan; i < 7; i++) {
                if (counts[i] == number && i > higherThan) return i;
            }

            return 0;

        }

    public:

        void markAllCards(Marked markedVal){
        
            for (uint8_t i = 0; i < 5; i++) this->marked[i] = markedVal;

        }

        void clearDeck() {
        
            for (uint8_t i = 0; i < MAX_DECK; i++) {
                this->deck[i].bones = 0;
                this->deck[i].multiplier = 0;
            }

        }

        void markWiningHand(HandScore handScore) {
        
            switch (handScore.handType) {
            
                case HandType::Five_of_a_Kind:
                case HandType::Full_House:
                case HandType::Straight:
                
                    this->markAllCards(Marked::True);
                    break;
            
                case HandType::Four_of_a_Kind:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(4, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }
                    }
                    break;
            
                case HandType::Three_of_a_Kind:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(3, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                    }
                    break;

                case HandType::Two_Pair:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(2, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                        diceValue = this->getDice_OfaKind(2, diceValue);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                    }
                    break;

                case HandType::Pair:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(2, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                    }
                    break;

                case HandType::High_Roll:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(1, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                        diceValue = this->getDice_OfaKind(1, diceValue);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = Marked::True;
                            }
                        }

                    }
                    break;

            }
        
        }

        void addSkullToDeck(SkullType id) {

            if (this->deckCount < MAX_DECK) {
                this->deck[this->deckCount++].skullType = id;
            }

        }


        uint8_t countSkull(SkullType id) {

            uint8_t n = 0;

            for (uint8_t i = 0; i < this->deckCount; i++) {

                if (this->deck[i].skullType == id) {
                    n++;
                }

            }

            return n;

        }

        void updateDeckEntry_Bones(SkullType id, uint8_t bones) {

            uint8_t n = 0;

            for (uint8_t i = 0; i < this->deckCount; i++) {

                if (this->deck[i].skullType == id) {

                    this->deck[i].bones = bones;
                    
                }

            }

        }

        void updateDeckEntry_Multiplier(SkullType id, uint8_t multiplier) {

            uint8_t n = 0;

            for (uint8_t i = 0; i < this->deckCount; i++) {

                if (this->deck[i].skullType == id) {

                    this->deck[i].multiplier = multiplier;
                    
                }

            }

        }

        void resetDice() {

            for (uint8_t i = 0; i < 5; i++) {
                this->dice[i] = 7;
                this->marked[i] = Marked::False;
            }

        }

        void rollAll() {
        
            for (uint8_t i = 0; i < 5; i++) {

                this->marked[i] = Marked::True_NoHighlight;

            }
        
        }

        void rerollDice(uint8_t i) {

            this->dice[i] = random(1, 7);

        }

        uint8_t diceSum() {

            uint8_t s = 0;
            for (uint8_t i = 0; i < 5; i++) s += this->dice[i];
            return s;

        }

        uint16_t evaluateHand() {

            #ifdef DEBUG_HAND
                DEBUG_PRINTLN("\n--------------------------");
            #endif

            this->clearDeck();


            // How many of each dice do we have?

            uint8_t counts[7] = {0, 0, 0, 0, 0, 0, 0};
            for (uint8_t i = 0; i < 5; i++) counts[this->dice[i]]++;

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
                
                if (this->dice[i] % 2 == 0) {
                    allOdd = false; 
                }
                else {
                    allEven = false;
                }

            }

            uint8_t sixCount = counts[6];
            uint8_t aceCount = counts[1];

            uint16_t handBones = 0;
            uint16_t skullBones = 0;
            uint16_t upgradeBones = 0;
            uint8_t handMultiplier = 0;
            uint8_t skullMultiplier = 0;
            uint8_t upgradeMultiplier = 0;
            uint8_t handCategory; // 0=high,1=pair-ish,2=big,3=kind
            
            HandType handType = HandType::None; 

            if (maxCount == 5) {

                handBones = 30; 
                handMultiplier = 14; 
                handCategory = 3;
                handType = HandType::Five_of_a_Kind;

                if (this->upgradeHand == HandType::Five_of_a_Kind) {
                
                    upgradeBones = 30;
                    upgradeMultiplier = 14;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Five of a kind: B 30, M 14");
                #endif

            } 
            else if (isStraight) {

                handBones = 30; 
                handMultiplier = 11; 
                handCategory = 2;
                handType = HandType::Straight;

                if (this->upgradeHand == HandType::Straight) {
                
                    upgradeBones = 15;
                    upgradeMultiplier = 0;
                    
                }
                
                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Straight: B 30, M 11");
                #endif

            } 
            else if (maxCount == 4) {

                handBones = 25; 
                handMultiplier = 8; 
                handCategory = 3;
                handType = HandType::Four_of_a_Kind;

                if (this->upgradeHand == HandType::Four_of_a_Kind) {
                
                    upgradeBones = 25;
                    upgradeMultiplier = 8;
                    
                }
                
                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Four of a Kind: B 25, M 8");
                #endif

            } 
            else if (maxCount == 3 && pairCount == 1) {

                handBones = 20; 
                handMultiplier = 6; 
                handCategory = 2;
                handType = HandType::Full_House;
                
                if (this->upgradeHand == HandType::Full_House) {
                
                    upgradeBones = 20;
                    upgradeMultiplier = 6;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Full House: B 20, M 6");
                #endif
            } 
            else if (threeCount == 1) {

                handBones = 15; 
                handMultiplier = 4; 
                handCategory = 3;
                handType = HandType::Three_of_a_Kind;
                
                if (this->upgradeHand == HandType::Three_of_a_Kind) {
                
                    upgradeBones = 15;
                    upgradeMultiplier = 4;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Three of a Kind: B 15, M 4");
                #endif

            } 
            else if (pairCount == 2) {

                handBones = 10; 
                handMultiplier = 3; 
                handCategory = 1;
                handType = HandType::Two_Pair;
                
                if (this->upgradeHand == HandType::Two_Pair) {
                
                    upgradeBones = 10;
                    upgradeMultiplier = 3;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Two Pair: B 10, M 3");
                #endif

            } 
            else if (pairCount == 1) {

                handBones = 5; 
                handMultiplier = 2; 
                handCategory = 1;
                handType = HandType::Pair;
                
                if (this->upgradeHand == HandType::Pair) {
                
                    upgradeBones = 5;
                    upgradeMultiplier = 2;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Pair: B 5, M 2");
                #endif

            } 
            else if (sixCount == 1 && aceCount == 1) {

                handBones = 5; 
                handMultiplier = 2; 
                handCategory = 0;
                handType = HandType::High_Roll;
                
                if (this->upgradeHand == HandType::High_Roll) {
                
                    upgradeBones = 5;
                    upgradeMultiplier = 2;
                    
                }

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("High: B 5, M 2");
                #endif

            } 
            else {

                handBones = 0; 
                handMultiplier = 1; 
                handCategory = 255;
                handType = HandType::None;
                
                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Nothing: B 0, M 0");
                #endif

            }

            if (handCategory == 0 && this->countSkull(SkullType::High_Roll_Save) > 0) {

                handBones = 15;

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Nothing + Save: B 15, M 0");
                #endif

            }



            // Skulls ---------------------------------------------------------------------------------------

            // skull-driven mult bonuses

            if (handCategory == 1) {

                skullMultiplier += this->countSkull(SkullType::Pair_Multiplier);
                updateDeckEntry_Multiplier(SkullType::Pair_Multiplier, 1);

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Sk Pair_Multiplier: B 0, M 1");
                #endif

            }

            if (handCategory == 2) {

                skullMultiplier += this->countSkull(SkullType::Big_Multiplier);
                updateDeckEntry_Multiplier(SkullType::Big_Multiplier, 1);

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Sk Big_Multiplier: B 0, M 1");
                #endif

            }

            if (handCategory == 3) {

                skullMultiplier += this->countSkull(SkullType::Kind_3or4_Multiplier);
                updateDeckEntry_Multiplier(SkullType::Kind_3or4_Multiplier, 1);

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Sk Kind_3or4_Multiplier: B 0, M 1");
                #endif

            }

            if (allEven) {

                skullMultiplier += this->countSkull(SkullType::Even_Mulitplier);
                updateDeckEntry_Multiplier(SkullType::Even_Mulitplier, 1);

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Sk Even_Multiplier: B 0, M 1");
                #endif

            }

            if (allOdd)  {

                skullMultiplier += this->countSkull(SkullType::Odd_Mulitplier);
                updateDeckEntry_Multiplier(SkullType::Odd_Mulitplier, 1);

                #ifdef DEBUG_HAND
                    DEBUG_PRINTLN("Sk Odd_Mulitplier: B 0, M 1");
                #endif

            }

            // skull-driven chip bonuses

            if (this->countSkull(SkullType::Flat_Bones) > 0) {

                skullBones += this->countSkull(SkullType::Flat_Bones) * 10;
                updateDeckEntry_Bones(SkullType::Flat_Bones, 10);

                #ifdef DEBUG_HAND
                    DEBUG_PRINT("Sk Flat_Bones: B ");
                    DEBUG_PRINT(this->countSkull(SkullType::Flat_Bones) * 10);
                    DEBUG_PRINTLN(", M 0");
                #endif

            }

            if (this->countSkull(SkullType::Six_Bonus) > 0) {

                skullBones += this->countSkull(SkullType::Six_Bonus) * 3 * sixCount;
                updateDeckEntry_Bones(SkullType::Six_Bonus, 3 * sixCount);

                #ifdef DEBUG_HAND
                    DEBUG_PRINT("Sk Six_Bonus: B ");
                    DEBUG_PRINT(this->countSkull(SkullType::Six_Bonus) * 3 * sixCount);
                    DEBUG_PRINTLN(", M 0");
                #endif

            }

            if (this->countSkull(SkullType::Ace_Bonus) > 0) {

                skullBones += this->countSkull(SkullType::Ace_Bonus) * 3 * aceCount;
                updateDeckEntry_Bones(SkullType::Ace_Bonus, 3 * aceCount);

                #ifdef DEBUG_HAND
                    DEBUG_PRINT("Sk Six_Bonus: B ");
                    DEBUG_PRINT(this->countSkull(SkullType::Six_Bonus) * 3 * sixCount);
                    DEBUG_PRINTLN(", M 0");
                #endif

            }

            if (!this->rerollUsedThisHand) {

                if (this->countSkull(SkullType::No_Reroll_Bonus) > 0) {

                    skullBones += this->countSkull(SkullType::No_Reroll_Bonus) * 15;
                    updateDeckEntry_Bones(SkullType::No_Reroll_Bonus, 15);

                    #ifdef DEBUG_HAND
                        DEBUG_PRINT("Sk No_Reroll_Bonus: B ");
                        DEBUG_PRINT(this->countSkull(SkullType::No_Reroll_Bonus) * 15);
                        DEBUG_PRINTLN(", M 0");
                    #endif

                }

            }
// skullMultiplier = 5;
// skullBones = 20;

// upgradeMultiplier = 5;
// upgradeBones = 20;

            this->lastHandScore.baseBones = this->diceSum();
            this->lastHandScore.handBones = handBones;
            this->lastHandScore.handMultiplier = handMultiplier;
            this->lastHandScore.skullBones = skullBones;
            this->lastHandScore.skullMultiplier = skullMultiplier;
            this->lastHandScore.upgradeBones = upgradeBones;
            this->lastHandScore.upgradeMultiplier = upgradeMultiplier;
            this->lastHandScore.totalBones = this->lastHandScore.baseBones + this->lastHandScore.handBones + this->lastHandScore.skullBones + this->lastHandScore.upgradeBones;
            this->lastHandScore.totalMultiplier = this->lastHandScore.handMultiplier + this->lastHandScore.skullMultiplier + this->lastHandScore.upgradeMultiplier;
            this->lastHandScore.handType = handType;

            #ifdef DEBUG_HAND
                DEBUG_PRINT("Base Bones ");
                DEBUG_PRINTLN(this->diceSum());
                DEBUG_PRINT("Hand Bones / Multiplier ");
                DEBUG_PRINT(handBones);
                DEBUG_PRINT(", ");
                DEBUG_PRINTLN(handMultiplier);
                DEBUG_PRINT("Skull Bones / Multiplier ");
                DEBUG_PRINT(skullBones);
                DEBUG_PRINT(", ");
                DEBUG_PRINTLN(skullMultiplier);
                DEBUG_PRINT("Upgrade Bones / Multiplier ");
                DEBUG_PRINT(upgradeBones);
                DEBUG_PRINT(", ");
                DEBUG_PRINTLN(upgradeMultiplier);
                DEBUG_PRINT("Total Bones / Multiplier ");
                DEBUG_PRINT(this->lastHandScore.totalBones);
                DEBUG_PRINT(", ");
                DEBUG_PRINTLN(this->lastHandScore.totalMultiplier);
            #endif

            uint16_t score = this->lastHandScore.totalBones * this->lastHandScore.totalMultiplier;
            if (this->firstHandOfLevel && this->countSkull(SkullType::Double_First) > 0) score *= 2;
            this->lastHandScore.score = score;

            return score;

        }


};