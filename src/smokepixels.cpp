#include "smokepixels.h"

#define LED_PIN 5
#define MAX_LED_COUNT 300

SmokePixels::SmokePixels()
    : _strip(MAX_LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800)
{

}

void SmokePixels::setup()
{
    _strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    _strip.show();            // Turn OFF all pixels ASAP
    _strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void SmokePixels::clearAll()
{
    fillAll(0);
}

void SmokePixels::setPixelColor(uint16_t index, uint32_t color)
{
    _strip.setPixelColor(index, color);
}

void SmokePixels::fill(uint32_t color, uint16_t first, uint16_t count)
{
    int index = first;
    while(count > 0)
    {
        _strip.setPixelColor(index, color);
        index++;
        count--;
    }
}

void SmokePixels::fillAll(uint32_t color)
{
    for(int x = 0; x < MAX_LED_COUNT; x++)
    {
        _strip.setPixelColor(x, color);
    }
}

void SmokePixels::show()
{
    _strip.show();
}