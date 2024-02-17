#include "lcd.h"

void LCD::setup()
{
    _lcd.begin();
    _lcd.setFontSize(FONT_SIZE_MEDIUM);
    _lcd.println("OBD DISPLAY");
}