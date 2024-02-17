#pragma once
#include <MicroLCD.h>

class LCD : public Print
{
public:
    void setup();
    size_t write(uint8_t d)
    {
        _lcd.write(d);
    }
    void clear()
    {
        _lcd.clear();
    }
    void printInt(int value)
    {
        _lcd.printInt(value);
    }
private:
    LCD_SSD1306 _lcd;
};