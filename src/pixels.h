#pragma once

#include "Adafruit_NeoPixel.h"
#include "FastLED.h"

#define MAX_LED_COUNT_METER 150
#define MAX_LED_COUNT_SMOKE 256

enum PixelType
{
    Meter,
    SmokeLights
};

class Pixels
{
public:
    Pixels(int numberOfPixels, int pin);
    void setup();
    void clearAll();
    void setPixelColor(uint16_t index, uint32_t color);
    void fill(uint32_t color, uint16_t first, uint16_t count);
    void fillAll(uint32_t color);
    int ledCount() { return _numberOfPixels; }
    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    void show();
private:
    CRGB* _leds;
    int _numberOfPixels;
    int _pin;
};