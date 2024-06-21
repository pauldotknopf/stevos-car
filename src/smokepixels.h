#pragma once

#include "Adafruit_NeoPixel.h"

class SmokePixels
{
public:
    SmokePixels();
    void setup();
    void clearAll();
    void setPixelColor(uint16_t index, uint32_t color);
    void fill(uint32_t color, uint16_t first, uint16_t count);
    void fillAll(uint32_t color);
    int ledCount() { return _strip.numPixels(); }
    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    void show();
private:
    Adafruit_NeoPixel _strip;
};