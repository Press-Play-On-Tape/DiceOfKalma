
// ---------------------------------------------------------------------
// State draws
// ---------------------------------------------------------------------
void drawTitle() {

    FX::drawBitmap(0, 0, Images::Title_Options, 0, dbmNormal);
    FX::drawBitmap(76, 0, Images::Title_Skull, 0, dbmNormal);
    FX::drawBitmap(25, 0, Images::Title_Text, 0, dbmMasked);
    drawNumber(11, 46, bestLevel);

}


// ---------------------------------------------------------------------
// State updates
// ---------------------------------------------------------------------
void updateTitle() {
  if (arduboy.justPressed(A_BUTTON)) {
    level = 1;
    hand.deckCount = 0;
    startLevel();
  }
}