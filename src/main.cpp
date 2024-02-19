#include <Arduino.h>
#include <AceButton.h>
#include "config.h"
#include "odb.h"
#include "fakeodb.h"
#include "lcd.h"
#include "pixels.h"
#include "gpio.h"

#define MAX_LED_COUNT 20
#define FAKE_ODB

using namespace ace_button;

#define BUTTON_CALIBRATE_PIN 2 // D2
#define BUTTON_MODE_PIN 3      // D3
#define BUTTON_INC_PIN 10      // D10
#define BUTTON_DEC_PIN 11      // D11

#define LED_COUNT 10

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

unsigned long lastShowTime;

bool calibrating = false;
int calibratingStart = 0;
int calibrateMinRpm = 0;
int calibrateMaxRpm = 0;

void buttonEvent(AceButton *, uint8_t, uint8_t);

// the setup routine runs once when you press reset:
void setup()
{
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

    odb.setup();
    lcd.setup();
}

// the loop routine runs over and over again forever:
void loop()
{
    if (calibrating)
    {
        int rpm = odb.getRPM();
        if (rpm < calibrateMinRpm)
        {
            calibrateMinRpm = rpm;
        }
        if (rpm > calibrateMaxRpm)
        {
            calibrateMaxRpm = rpm;
        }

        if (millis() - calibratingStart > 10000)
        {
            calibrating = false;
            Serial.println("Calibration done");
            Serial.print("Min RPM: ");
            Serial.println(calibrateMinRpm);
            Serial.print("Max RPM: ");
            Serial.println(calibrateMaxRpm);
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

        int howManyLedsToLightUp = round((gpio.getAccelVoltage() * LED_COUNT) / 3);

        for (int x = 0; x < howManyLedsToLightUp - 2; x++)
        {
            if (gpio.isClutchPressed())
            {
                // yellow
                pixels.setPixelColor(x, Pixels::Color(255, 255, 0));
            }
            else
            {
                // green
                pixels.setPixelColor(x, Pixels::Color(0, 0, 255));
            }
        }

        if (gpio.getAccelVoltage() < 0.7f /*volts*/)
        {
            int ledsToLightup = 4;
            for (int x = LED_COUNT - ledsToLightup; x < LED_COUNT; x++)
            {
                pixels.setPixelColor(x, Pixels::Color(0, 255, 0));
            }
        }

        //Serial.println(analogRead(ACCEL_INPUT_VOL_PIN));

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
        Serial.println("Calibrate button pressed");
        if (!calibrating)
        {
            calibrating = true;
            calibratingStart = millis();
        }
    }
    else if (button == &buttonMode)
    {
        Serial.println("Mode button pressed");
    }
    else if (button == &buttonIncrement)
    {
        Serial.println("Increment button pressed");
    }
    else if (button == &buttonDecrement)
    {
        Serial.println("Decrement button pressed");
    }
}