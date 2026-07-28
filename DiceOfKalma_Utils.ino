#include <ArduboyFX.h>  

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


void saveHighScore() {
  if (level > bestLevel) {
    bestLevel = level;
    EEPROM.update(Constants::EEPROM_Addr, (Constants::EEPROM_Magic));
    EEPROM.update(Constants::EEPROM_Addr + 1, bestLevel);
  }
}

void loadHighScore() {
  if (EEPROM.read(Constants::EEPROM_Addr) == Constants::EEPROM_Magic) {
    bestLevel = EEPROM.read(Constants::EEPROM_Addr + 1);
    if (bestLevel == 0) bestLevel = 1;
  } else {
    bestLevel = 1;
  }
}

