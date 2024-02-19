#include "pixels.h"

#define LED_PIN 6

Pixels::Pixels()
    : _strip(MAX_LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ400)
{

}

void Pixels::setup()
{
    _strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    _strip.show();            // Turn OFF all pixels ASAP
    _strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void Pixels::clearAll()
{
    fillAll(0);
}

void Pixels::setPixelColor(uint16_t index, uint32_t color)
{
    _strip.setPixelColor(index, color);
}

void Pixels::fill(uint32_t color, uint16_t first, uint16_t count)
{
    int index = first;
    while(count > 0)
    {
        _strip.setPixelColor(index, color);
        index++;
        count--;
    }
}

void Pixels::fillAll(uint32_t color)
{
    for(int x = 0; x < MAX_LED_COUNT; x++)
    {
        _strip.setPixelColor(x, color);
    }
}

void Pixels::show()
{
    _strip.show();
}