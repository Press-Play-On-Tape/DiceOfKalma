#pragma once

#include <Arduino.h>

#include "../utils/Constants.h"
#include "../utils/Structs.h"

class Hand {
    
    public:

        uint8_t dice[5];
        bool    marked[5];
        SkullType deck[MAX_DECK];
        uint8_t deckCount = 0;
        
        bool    rerollUsedThisHand = false;
        bool    firstHandOfLevel = true;

        HandScore lastHandScore;


    private:

        uint8_t getDice_OfaKind(uint8_t number, uint8_t higherThan) { // Returns the dice value (not index) of the nominated qty 

            uint8_t counts[7] = {0, 0, 0, 0, 0, 0, 0};
            for (uint8_t i = 0; i < 5; i++) counts[this->dice[i]]++;

            for (uint8_t i = 0; i < 7; i++) {
                if (counts[i] == number && i > higherThan) return i;
            }

            return 0;

        }

    public:

        void markWiningHand(HandScore handScore) {
        
            switch (handScore.handType) {
            
                case HandType::Five_of_a_Kind:
                case HandType::Full_House:
                case HandType::Straight:
                
                    for (uint8_t i = 0; i < 5; i++) {
                        this->marked[i] = true;
                    }
                    break;
            
                case HandType::Four_of_a_Kind:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(4, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }
                    }
                    break;
            
                case HandType::Three_of_a_Kind:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(3, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                    }
                    break;

                case HandType::Two_Pair:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(2, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                        diceValue = this->getDice_OfaKind(2, diceValue);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                    }
                    break;

                case HandType::Pair:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(2, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                    }
                    break;

                case HandType::High_Roll:
                    {
                        uint8_t diceValue = this->getDice_OfaKind(1, 0);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                        diceValue = this->getDice_OfaKind(6, diceValue);
                        for (uint8_t i = 0; i < 5; i++) {
                            if (this->dice[i] == diceValue) {
                                this->marked[i] = true;
                            }
                        }

                    }
                    break;

            }
        
        }

        void addSkullToDeck(SkullType id) {

            if (this->deckCount < MAX_DECK) {
                this->deck[this->deckCount++] = id;
            }

        }


        uint8_t countSkull(SkullType id) {

            uint8_t n = 0;

            for (uint8_t i = 0; i < this->deckCount; i++) {
                if (this->deck[i] == id) {
                    n++;
                }
            }

            return n;

        }

        void rollAllDice() {

            for (uint8_t i = 0; i < 5; i++) {
                this->dice[i] = random(1, 7);
                this->marked[i] = false;
            }

        }

        void rerollMarked() {

            for (uint8_t i = 0; i < 5; i++) {

                if (this->marked[i]) {
                    this->dice[i] = random(1, 7);
                    this->marked[i] = false;
                }

            }

        }

        uint8_t diceSum() {

            uint8_t s = 0;
            for (uint8_t i = 0; i < 5; i++) s += this->dice[i];
            return s;

        }


        uint16_t evaluateHand() {

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
            } 
            else if (isStraight) {
                handBones = 30; 
                handMultiplier = 11; 
                handCategory = 2;
                handType = HandType::Straight;
            } 
            else if (maxCount == 4) {
                handBones = 25; 
                handMultiplier = 8; 
                handCategory = 3;
                handType = HandType::Four_of_a_Kind;
            } 
            else if (maxCount == 3 && pairCount == 1) {
                handBones = 20; 
                handMultiplier = 6; 
                handCategory = 2;
                handType = HandType::Full_House;
            } 
            else if (threeCount == 1) {
                handBones = 15; 
                handMultiplier = 4; 
                handCategory = 2;
                handType = HandType::Three_of_a_Kind;
            } 
            else if (pairCount == 2) {
                handBones = 10; 
                handMultiplier = 3; 
                handCategory = 1;
                handType = HandType::Two_Pair;
            } 
            else if (pairCount == 1) {
                handBones = 5; 
                handMultiplier = 2; 
                handCategory = 1;
                handType = HandType::Pair;
            } 
            else if (sixCount == 1 && aceCount == 1) {
                handBones = 5; 
                handMultiplier = 2; 
                handCategory = 0;
                handType = HandType::High_Roll;
            } 
            else {
                handBones = 0; 
                handMultiplier = 1; 
                handCategory = 255;
                handType = HandType::None;
            }

            if (handCategory == 0 && this->countSkull(SkullType::High_Roll_Save) > 0) {
                handBones = 15;
            }


            // skull-driven mult bonuses
            if (handCategory == 1) skullMultiplier += this->countSkull(SkullType::Pair_Multiplier);
            if (handCategory == 2) skullMultiplier += this->countSkull(SkullType::Big_Multiplier);
            if (handCategory == 3) skullMultiplier += this->countSkull(SkullType::Kind_3or4_Multiplier);
            if (allEven) skullMultiplier += this->countSkull(SkullType::Even_Mulitplier);
            if (allOdd)  skullMultiplier += this->countSkull(SkullType::Odd_Mulitplier);

            // skull-driven chip bonuses

            skullBones += this->countSkull(SkullType::Flat_Bones) * 10;
            skullBones += this->countSkull(SkullType::Six_Bonus) * 3 * sixCount;
            skullBones += this->countSkull(SkullType::Ace_Bonus) * 3 * aceCount;
            if (!this->rerollUsedThisHand) skullBones += this->countSkull(SkullType::No_Reroll_Bonus) * 15;

skullMultiplier = 5;
skullBones = 20;

upgradeMultiplier = 5;
upgradeBones = 20;

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

            uint16_t score = this->lastHandScore.totalBones * this->lastHandScore.totalMultiplier;
            if (this->firstHandOfLevel && this->countSkull(SkullType::Double_First) > 0) score *= 2;
            this->lastHandScore.score = score;

            return score;

        }


};