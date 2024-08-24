#pragma once

#include "Adafruit_NeoPixel.h"

#define LED_PIN1 5
#define LED_PIN2 6
#define MAX_LED_COUNT 300

class Pixels
{
public:
    Pixels();
    void setup();
    void clearAll();
    void setPixelColor(uint16_t index, uint32_t color);
    void fill(uint32_t color, uint16_t first, uint16_t count);
    void fillAll(uint32_t color);
    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    void show();
private:
    Adafruit_NeoPixel _strip1;
    #ifdef LED_PIN2
    Adafruit_NeoPixel _strip2;
    #endif
};