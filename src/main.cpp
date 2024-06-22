#include <Arduino.h>
#include <AceButton.h>
#include "config.h"
#include "odb.h"
#include "fakeodb.h"
#include "lcd.h"
#include "gpio.h"
#include <FastLED.h>

#define FAKE_ODB

using namespace ace_button;

#define LED_COUNT_METER 100
#define LED_COUNT_SMOKE 100

#define BUTTON_CALIBRATE_PIN 2 // D2
#define BUTTON_MODE_PIN 3      // D3
#define BUTTON_INC_PIN 10      // D10
#define BUTTON_DEC_PIN 11      // D11

#define THROTTLE_MODE_PIN 8

AceButton buttonCalibrate(BUTTON_CALIBRATE_PIN);
AceButton buttonMode(BUTTON_MODE_PIN);
AceButton buttonIncrement(BUTTON_INC_PIN);
AceButton buttonDecrement(BUTTON_DEC_PIN);

CRGB ledMeter[LED_COUNT_METER];
CRGB ledSmoke[LED_COUNT_SMOKE];

#ifdef FAKE_ODB
FakeODB odb;
#else
ODB odb;
#endif

LCD lcd;
Config config;

//SmokePixels smokePixels;
GPIO gpio;
int throttleMode = 0;

unsigned long lastShowTime;

bool calibrating = false;
unsigned long calibratingStart = 0;
int calibrateMin = -1;
int calibrateMax = -1;
int ledCount = 1;

void buttonEvent(AceButton *, uint8_t, uint8_t);

// clutch on, clutch off, brake
int trsColorIndex = 0;
CRGB trsColorSchemes[4][3] = { 
    { CRGB(0, 255, 0), CRGB(255, 255, 0), CRGB(255, 0, 0) },
    // TODO: Make these the colors
    { CRGB(255, 0, 0), CRGB(255, 0, 0), CRGB(255, 0, 0) },
    { CRGB(255, 0, 0), CRGB(255, 0, 0), CRGB(255, 0, 0) },
    { CRGB(255, 0, 0), CRGB(255, 0, 0), CRGB(255, 0, 0) }
};

int smokeMode = 0;

// CURRENT     MODE 2     MODE 3       MODE 4
// RED         VIOLET     ORANGE       RED/PURPLE (BURGUNDY)
// YELLOW      GREEN      BLUE         YELLOW/GREEN (CHARTREUSE)
// GREEN       ORANGE     VIOLET       GREEN/BLUE (TEAL)
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    64
#define DATA_PIN    3

// the setup routine runs once when you press reset:
void setup()
{
    lcd.setup();
    lcd.clear();
    lcd.println("connecting...");

    calibrateMin = config.getCalibrationMin();
    calibrateMax = config.getCalibrationMax();
    ledCount = config.getLedCount();
    if(ledCount < 3)
    {
        ledCount = 3;
    }
    if(ledCount > LED_COUNT_METER)
    {
        ledCount = LED_COUNT_METER;
    }
    
    pinMode(THROTTLE_MODE_PIN, INPUT);
    throttleMode = digitalRead(THROTTLE_MODE_PIN);

    lastShowTime = millis();
#ifdef FAKE_ODB
    Serial.begin(9600);
#endif

    FastLED.addLeds<WS2812, 5, RGB>(ledMeter, LED_COUNT_METER);
    FastLED.addLeds<WS2812, 6, RGB>(ledSmoke, LED_COUNT_SMOKE);
    gpio.setup();

    pinMode(BUTTON_CALIBRATE_PIN, INPUT);
    pinMode(BUTTON_MODE_PIN, INPUT);
    pinMode(BUTTON_INC_PIN, INPUT);
    pinMode(BUTTON_DEC_PIN, INPUT);
    
    ButtonConfig::getSystemButtonConfig()->setFeature(ButtonConfig::kFeatureLongPress);
    ButtonConfig::getSystemButtonConfig()->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
    ButtonConfig::getSystemButtonConfig()->setEventHandler(buttonEvent);

    if(throttleMode == 1)
    {
        odb.setup();
    }
    lcd.setup();
}

// Returns 0f-5f which represents where the pixel strip should light up to
float getAccelVoltage()
{
    float result = 0;
    if(throttleMode == 1)
    {
        int rpm = odb.getRPM();
        if(rpm < calibrateMin)
        {
            rpm = calibrateMin;
        }
        if(rpm > calibrateMax)
        {
            rpm = calibrateMax;
        }
        // This value is 0-1000, factoring in calibration min/max.
        long mapped = map(rpm, calibrateMin, calibrateMax, 0, 1000);
        return (mapped * 5) / 1000.0;
    }
    else
    {
        int accel = gpio.getAccelVoltage();
        if(accel < calibrateMin)
        {
            accel = calibrateMin;
        }
        if(accel > calibrateMax)
        {
            accel = calibrateMax;
        }
        long mapped = map(accel, calibrateMin, calibrateMax, 0, 1023);
        return (mapped * 5) / 1023.0;
    }
}

// the loop routine runs over and over again forever:
void loop()
{ 
    fill_solid(ledSmoke, LED_COUNT_SMOKE, CRGB(0, 255, 0));

    if (calibrating)
    {
        int rpm;
        if(throttleMode == 1)
        {
            rpm = odb.getRPM();
        }
        else
        {
            rpm = gpio.getAccelVoltage();
        }
        if(calibrateMax == -1)
        {
            calibrateMax = rpm;
        }
        else
        {
            if (rpm > calibrateMax)
            {
                calibrateMax = rpm;
            }
        }

        if(calibrateMin == -1)
        {
            calibrateMin = rpm;
        }
        else
        {
            if (rpm < calibrateMin)
            {
                calibrateMin = rpm;
            }
        }

        if (millis() - calibratingStart > 10000)
        {
            calibrating = false;

            #ifdef FAKE_ODB
            Serial.println("Calibration min: " + String(calibrateMin));
            Serial.println("Calibration max: " + String(calibrateMax));
            #endif

            config.setCalibrationMin(calibrateMin);
            config.setCalibrationMax(calibrateMax);
        }
    }

    buttonCalibrate.check();
    buttonMode.check();
    buttonIncrement.check();
    buttonDecrement.check();
    return;

    unsigned long currentTime = millis();
    if ((currentTime - lastShowTime) >= 10) // every 10 milliseconds
    {
        gpio.updateState();

        // set all pixels to black
        fill_solid(ledMeter, LED_COUNT_METER, CRGB::Black);

        // set the brake color, if it's pressed
        if (gpio.isBrakePressed())
        {
            fill_solid(ledMeter, LED_COUNT_METER, trsColorSchemes[trsColorIndex][2]);
        }

        float voltage = getAccelVoltage();
        lcd.println("v: " + String(voltage * 5000));
        
        int howManyLedsToLightUp = round((voltage * ledCount) / 3);

        for (int x = 0; x < howManyLedsToLightUp - 2; x++)
        {
            if (gpio.isClutchPressed())
            {
                // for reversed clutch switch change to 0, 255, 0
                ledMeter[x] = trsColorSchemes[trsColorIndex][0];
            }
            else
            {
                // for reversed clutch switch change to 255, 255, 0
                ledMeter[x] = trsColorSchemes[trsColorIndex][1];
            }
        }
        
        FastLED.show();
    }
}

void buttonEvent(AceButton *button, uint8_t eventType, uint8_t)
{
    Serial.print("event type: ");
    Serial.println(eventType);
    // increment/decrym changes number of LEDs used
    // cal, press down and for 10 seconds, it will read min/max values of accelerate

    if (eventType == AceButton::kEventPressed)
    {
        if (button == &buttonCalibrate)
        {
            //Serial.println("Calibrate button pressed");
            if (!calibrating)
            {
                #ifdef FAKE_ODB
                Serial.println("Calibrating...");
                #endif
                calibrating = true;
                calibrateMin = -1;
                calibrateMax = -1;
                calibratingStart = millis() - 1000;
            }
        }
        else if (button == &buttonIncrement)
        {
            ledCount++;
            if(ledCount > LED_COUNT_METER)
            {
                ledCount = LED_COUNT_METER;
            }
            config.setLedCount(ledCount);
            //Serial.println("ledCount: " + String(ledCount));
        }
        else if (button == &buttonDecrement)
        {
            ledCount--;
            if(ledCount < 3)
            {
                ledCount = 3;
            }
            config.setLedCount(ledCount);
            //Serial.println("ledCount: " + String(ledCount));
        }
        else if (button == &buttonMode)
        {
            smokeMode++;
            if(smokeMode > 3)
            {
                smokeMode = 0;
            }
            //Serial.print("smoke mode: ");
            //Serial.println(smokeMode);
        }
    }

    if(eventType == AceButton::kEventLongPressed)
    {
        if (button == &buttonMode)
        {
            trsColorIndex++;
            if(trsColorIndex > 3)
            {
                trsColorIndex = 0;
            }
            //Serial.print("trs color mode: ");
            //Serial.println(trsColorIndex);
        }
    }
}