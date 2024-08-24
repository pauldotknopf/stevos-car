#include "pixels.h"

Pixels::Pixels()
    : _strip1(MAX_LED_COUNT, LED_PIN1, NEO_RGB + NEO_KHZ800)
    #ifdef LED_PIN2
    , _strip2(MAX_LED_COUNT, LED_PIN2, NEO_RGB + NEO_KHZ800)
    #endif
{

}

void Pixels::setup()
{
    _strip1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    _strip1.show();            // Turn OFF all pixels ASAP
    _strip1.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

    #ifdef LED_PIN2
    _strip2.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    _strip2.show();            // Turn OFF all pixels ASAP
    _strip2.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
    #endif
}

void Pixels::clearAll()
{
    fillAll(0);
}

void Pixels::setPixelColor(uint16_t index, uint32_t color)
{
    _strip1.setPixelColor(index, color);
    #ifdef LED_PIN2
    _strip1.setPixelColor(index, color);
    #endif
}

void Pixels::fill(uint32_t color, uint16_t first, uint16_t count)
{
    int index = first;
    while(count > 0)
    {
        _strip1.setPixelColor(index, color);
        #ifdef LED_PIN2
        _strip2.setPixelColor(index, color);
        #endif
        index++;
        count--;
    }
}

void Pixels::fillAll(uint32_t color)
{
    for(int x = 0; x < MAX_LED_COUNT; x++)
    {
        _strip1.setPixelColor(x, color);
        #ifdef LED_PIN2
        _strip2.setPixelColor(x, color);
        #endif
    }
}

void Pixels::show()
{
    _strip1.show();
    #ifdef LED_PIN2
    _strip2.show();
    #endif
}