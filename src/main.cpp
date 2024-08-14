#include <Arduino.h>
#include <AceButton.h>
#include "config.h"
#include "odb.h"
#include "fakeodb.h"
#include "lcd.h"
#include "pixels.h"
#include "gpio.h"

#define MAX_LED_COUNT 300
//#define FAKE_ODB

using namespace ace_button;

#define BUTTON_CALIBRATE_PIN 2 // D2
#define BUTTON_MODE_PIN 3      // D3
#define BUTTON_INC_PIN 10      // D10
#define BUTTON_DEC_PIN 11      // D11

#define THROTTLE_MODE_PIN 8

AceButton buttonCalibrate(BUTTON_CALIBRATE_PIN);
AceButton buttonMode(BUTTON_MODE_PIN);
AceButton buttonIncrement(BUTTON_INC_PIN);
AceButton buttonDecrement(BUTTON_DEC_PIN);

#ifdef FAKE_ODB
FakeODB odb;
#else
ODB odb;
#endif

LCD lcd;
Config config;
Pixels pixels;
GPIO gpio;
int throttleMode = 0;

unsigned long lastShowTime;

bool calibrating = false;
unsigned long calibratingStart = 0;
int calibrateMin = -1;
int calibrateMax = -1;
int ledCount = 1;

void buttonEvent(AceButton *, uint8_t, uint8_t);

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
    if(ledCount > MAX_LED_COUNT)
    {
        ledCount = MAX_LED_COUNT;
    }
    
    pinMode(THROTTLE_MODE_PIN, INPUT);
    throttleMode = digitalRead(THROTTLE_MODE_PIN);

    lastShowTime = millis();
#ifdef FAKE_ODB
    Serial.begin(9600);
#endif

    pixels.setup();
    gpio.setup();

    pinMode(BUTTON_CALIBRATE_PIN, INPUT);
    pinMode(BUTTON_MODE_PIN, INPUT);
    pinMode(BUTTON_INC_PIN, INPUT);
    pinMode(BUTTON_DEC_PIN, INPUT);
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
    lcd.clear();
    lcd.println("loop");
    lcd.println("led: " + String(ledCount));
    // lcd.println(odb.getRPM());
    // lcd.println("test");
    // return;
   
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

    unsigned long currentTime = millis();
    if ((currentTime - lastShowTime) >= 10) // every 10 milliseconds
    {
        gpio.updateState();

        // set all pixels to black
        pixels.clearAll();

        // set the brake color, if it's pressed
        if (gpio.isBrakePressed())
        {
            pixels.fillAll(Pixels::Color(255, 0, 0));
        }

        float voltage = getAccelVoltage();
        lcd.println("v: " + String(voltage * 5000));
        
        int howManyLedsToLightUp = round((voltage * ledCount) / 3);

        for (int x = 0; x < howManyLedsToLightUp - 2; x++)
        {
            if (gpio.isClutchPressed())
            {
                // for reversed clutch switch change to 0, 255, 0
                pixels.setPixelColor(x, Pixels::Color(255, 255, 0));
            }
            else
            {
                // for reversed clutch switch change to 255, 255, 0
                pixels.setPixelColor(x, Pixels::Color(0, 255, 0));
            }
        }

        pixels.show();
    }
}

void buttonEvent(AceButton *button, uint8_t eventType, uint8_t)
{
    // increment/decrym changes number of LEDs used
    // cal, press down and for 10 seconds, it will read min/max values of accelerate

    if (eventType != AceButton::kEventReleased)
        return;

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
    else if (button == &buttonMode)
    {
        //Serial.println("Mode button pressed");
        //Serial.println("ledCount: " + String(ledCount));
    }
    else if (button == &buttonIncrement)
    {
        ledCount++;
        if(ledCount > MAX_LED_COUNT)
        {
            ledCount = MAX_LED_COUNT;
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
}