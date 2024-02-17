#include <Arduino.h>
#include <AceButton.h>
#include "config.h"
#include "odb.h"
#include "fakeodb.h"
#include "lcd.h"

#define FAKE_ODB

using namespace ace_button;

#define BUTTON_CALIBRATE_PIN 2 // D2
#define BUTTON_MODE_PIN 3      // D3
#define BUTTON_INC_PIN 10      // D10
#define BUTTON_DEC_PIN 11      // D11

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

bool calibrating = false;
int calibratingStart = 0;
int calibrateMinRpm = 0;
int calibrateMaxRpm = 0;

void buttonEvent(AceButton *, uint8_t, uint8_t);

// the setup routine runs once when you press reset:
void setup()
{
#ifdef FAKE_ODB
    Serial.begin(9600);
#endif

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