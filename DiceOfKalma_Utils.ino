#include <ArduboyFX.h>  

const __FlashStringHelper* skullName(SkullType s) {

    switch (s) {
        case SkullType::Pair_Multiplier:      return F("PAIR MULT +1");
        case SkullType::Big_Multiplier:       return F("STR/FH MULT+1");
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




    void addLookLeftThenRight() {
    
        skullStack.push(0x0205);
        skullStack.push(0x0005);
        skullStack.push(0x0005);
        skullStack.push(0x0005);
        skullStack.push(0x0004);
        skullStack.push(0x0004);
        skullStack.push(0x0003);
        skullStack.push(0x0003);
        skullStack.push(0x0002);
        skullStack.push(0x0002);
        skullStack.push(0x0001);
        skullStack.push(0x0101);
    
    }

    void addLookDownThenUp() {
    
        skullStack.push(0x020B);
        skullStack.push(0x000B);
        skullStack.push(0x000A);
        skullStack.push(0x000A);
        skullStack.push(0x0009);
        skullStack.push(0x0009);
        skullStack.push(0x0008);
        skullStack.push(0x0008);
        skullStack.push(0x0007);
        skullStack.push(0x0007);
        skullStack.push(0x0006);
        skullStack.push(0x0106);
    
    }


    void addLongTalk() {
    
        skullStack.push(0x0201);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0001);
        skullStack.push(0x0101);
    
    }