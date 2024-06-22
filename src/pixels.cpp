#include "pixels.h"

#define MAX_LED_COUNT 300
#define LED_PIN 6

Pixels::Pixels(int pixelCount, int pin) : _numberOfPixels(pixelCount), _pin(pin)
{
    
    
}

void Pixels::setup()
{
    _leds = new CRGB[_numberOfPixels];
    if(_pin == 5)
    {
        FastLED.addLeds<NEOPIXEL, 5>(_leds, _numberOfPixels);
    }
    else if(_pin == 6)
    {
        FastLED.addLeds<NEOPIXEL, 6>(_leds, _numberOfPixels);
    }
}

void Pixels::clearAll()
{
    fillAll(0);
}

void Pixels::setPixelColor(uint16_t index, uint32_t color)
{
    _leds[index] = color;
}

void Pixels::fill(uint32_t color, uint16_t first, uint16_t count)
{
    int index = first;
    while(count > 0)
    {
        _leds[index] = color;
        index++;
        count--;
    }
}

void Pixels::fillAll(uint32_t color)
{
    for(int x = 0; x < 5; x++)
    {
        _leds[x] = color;
    }
}

void Pixels::show()
{
    FastLED.show();
}