#include "gpio.h"
#include <Arduino.h>

#define ACCEL_INPUT_VOL_PIN A7
#define BREAK_INPUT_PIN     7
#define CLUTCH_INPUT_PIN    12

void GPIO::setup()
{
    pinMode(ACCEL_INPUT_VOL_PIN, INPUT);
    pinMode(BREAK_INPUT_PIN, INPUT);
    pinMode(CLUTCH_INPUT_PIN, INPUT);
}

void GPIO::updateState()
{
    _accelVoltage = analogRead(ACCEL_INPUT_VOL_PIN) * (3.7f / 790);
    _brakePressed = digitalRead(BREAK_INPUT_PIN) == HIGH;
    _clutchPressed = digitalRead(CLUTCH_INPUT_PIN) == HIGH;
}

float GPIO::getAccelVoltage()
{
    return _accelVoltage;
}

bool GPIO::isBrakePressed()
{
    return _brakePressed;
}

bool GPIO::isClutchPressed()
{
    return _clutchPressed;
}

