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




void addLookLeftThenRight(bool talkBubble) {

    skullStack.push(0x000B);
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
    skullStack.push(0x0006);

    if (talkBubble) {
        skullStack.push(0x0205);
    }
    else {
        skullStack.push(0x0005);
    }

    skullStack.push(0x0005);
    skullStack.push(0x0004);
    skullStack.push(0x0004);
    skullStack.push(0x0003);
    skullStack.push(0x0003);
    skullStack.push(0x0002);
    skullStack.push(0x0002);
    skullStack.push(0x0001);
    skullStack.push(0x0001);
    skullStack.push(0x0000);

    if (talkBubble) {
        skullStack.push(0x0100);
    }
    else {
        skullStack.push(0x0000);
    }

}


void addLookLeft(bool talkBubble) {

    skullStack.push(0x000B);
    skullStack.push(0x000B);
    skullStack.push(0x000A);
    skullStack.push(0x000A);

    if (talkBubble) {
        skullStack.push(0x0205);
    }
    else {
        skullStack.push(0x0005);
    }

    skullStack.push(0x0005);
    skullStack.push(0x0004);
    skullStack.push(0x0004);
    skullStack.push(0x0003);
    skullStack.push(0x0003);
    skullStack.push(0x0002);
    skullStack.push(0x0002);
    skullStack.push(0x0001);
    skullStack.push(0x0001);
    skullStack.push(0x0000);

    if (talkBubble) {
        skullStack.push(0x0100);
    }
    else {
        skullStack.push(0x0000);
    }

}


void addLookRight(bool talkBubble) {

    skullStack.push(0x000B);
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
    skullStack.push(0x0006);

    if (talkBubble) {
        skullStack.push(0x0205);
    }
    else {
        skullStack.push(0x0005);
    }

    skullStack.push(0x0005);
    skullStack.push(0x0000);

    if (talkBubble) {
        skullStack.push(0x0100);
    }
    else {
        skullStack.push(0x0000);
    }

}


void addLookDownThenUp(bool talkBubble) {

    skullStack.push(0x0017);
    skullStack.push(0x0017);
    skullStack.push(0x0016);
    skullStack.push(0x0016);
    skullStack.push(0x0015);
    skullStack.push(0x0015);
    skullStack.push(0x0014);
    skullStack.push(0x0014);
    skullStack.push(0x0013);
    skullStack.push(0x0013);
    skullStack.push(0x0012);
    skullStack.push(0x0012);

    if (talkBubble) {
        skullStack.push(0x0211);
    }
    else {
        skullStack.push(0x0011);
    }    

    skullStack.push(0x0011);
    skullStack.push(0x0010);
    skullStack.push(0x0010);
    skullStack.push(0x000F);
    skullStack.push(0x000F);
    skullStack.push(0x000E);
    skullStack.push(0x000E);
    skullStack.push(0x000D);
    skullStack.push(0x000D);
    skullStack.push(0x000C);

    if (talkBubble) {
        skullStack.push(0x010C);
    }
    else {
        skullStack.push(0x000C);
    }    

}


void addLookDown(bool talkBubble) {

    skullStack.push(0x0017);
    skullStack.push(0x0017);
    skullStack.push(0x0016);
    skullStack.push(0x0016);

    if (talkBubble) {
        skullStack.push(0x0211);
    }
    else {
        skullStack.push(0x0011);
    }   

    skullStack.push(0x0011);
    skullStack.push(0x0010);
    skullStack.push(0x0010);
    skullStack.push(0x000F);
    skullStack.push(0x000F);
    skullStack.push(0x000E);
    skullStack.push(0x000E);
    skullStack.push(0x000D);
    skullStack.push(0x000D);
    skullStack.push(0x000C);
 
    if (talkBubble) {
        skullStack.push(0x010C);
    }
    else {
        skullStack.push(0x000C);
    } 

}


void addLookUp(bool talkBubble) {

    skullStack.push(0x0017);
    skullStack.push(0x0016);
    skullStack.push(0x0015);
    skullStack.push(0x0015);
    skullStack.push(0x0014);
    skullStack.push(0x0014);

    if (talkBubble) {
        skullStack.push(0x0213);
    }
    else {
        skullStack.push(0x0013);
    } 

    skullStack.push(0x0013);
    skullStack.push(0x0012);
    skullStack.push(0x0012);
    skullStack.push(0x0011);
    skullStack.push(0x0011);

    if (talkBubble) {
        skullStack.push(0x010C);
    }
    else {
        skullStack.push(0x000C);
    } 

}
void addLongTalk() {

    skullStack.push(0x000B);
    skullStack.push(0x000B);
    skullStack.push(0x0000);
    skullStack.push(0x0000);
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
    skullStack.push(0x0000);
    skullStack.push(0x0100);

}

void addDeathStare(bool talkBubble) {

    skullStack.push(0x000B);
    skullStack.push(0x000B);
    skullStack.push(0x0000);
    skullStack.push(0x0000);

    if (talkBubble) {
        skullStack.push(0x0201);
    }
    else {
        skullStack.push(0x0001);
    } 

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
    skullStack.push(0x0000);

    if (talkBubble) {
        skullStack.push(0x0100);
    }
    else {
        skullStack.push(0x0000);
    } 

}

void addPinPrick() {

    skullStack.push(0x000B);
    skullStack.push(0x000B);
    skullStack.push(0x0000);
    skullStack.push(0x0000);
    skullStack.push(0x0001);
    skullStack.push(0x0001);
    skullStack.push(0x0001);
    skullStack.push(0x0001);
    skullStack.push(0x0000);
    skullStack.push(0x0000);

}



void addWideEyes() {

    skullStack.push(0x0021);
    skullStack.push(0x0021);
    skullStack.push(0x0020);
    skullStack.push(0x0020);
    skullStack.push(0x001F);
    skullStack.push(0x001F);
    skullStack.push(0x001E);
    skullStack.push(0x001E);
    skullStack.push(0x001D);
    skullStack.push(0x001D);
    skullStack.push(0x001C);
    skullStack.push(0x001C);
    skullStack.push(0x001B);
    skullStack.push(0x001B);
    skullStack.push(0x001A);
    skullStack.push(0x001A);
    skullStack.push(0x0019);
    skullStack.push(0x0019);
    skullStack.push(0x0018);
    skullStack.push(0x0018);

}


void addRollEyes() {

    // skullStack.push(0x0021);
    skullStack.push(0x0021);
    // skullStack.push(0x002F);
    skullStack.push(0x002F);
    // skullStack.push(0x002E);
    skullStack.push(0x002E);
    // skullStack.push(0x002D);
    skullStack.push(0x002D);
    // skullStack.push(0x002C);
    skullStack.push(0x002C);
    // skullStack.push(0x002B);
    skullStack.push(0x002B);
    // skullStack.push(0x002A);
    skullStack.push(0x002A);
    // skullStack.push(0x0029);
    skullStack.push(0x0029);
    // skullStack.push(0x0028);
    skullStack.push(0x0028);
    // skullStack.push(0x0027);
    skullStack.push(0x0027);
    // skullStack.push(0x0026);
    skullStack.push(0x0026);
    // skullStack.push(0x0025);
    skullStack.push(0x0025);
    // skullStack.push(0x0024);
    skullStack.push(0x0024);

}