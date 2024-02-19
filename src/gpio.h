#pragma once

class GPIO
{
public:
    void setup();
    void updateState();
    bool isBrakePressed();
    bool isClutchPressed();
    float getAccelVoltage();
private:
    bool _brakePressed;
    bool _clutchPressed;
    float _accelVoltage;
};