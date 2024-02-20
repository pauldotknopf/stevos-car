#pragma once

class GPIO
{
public:
    void setup();
    void updateState();
    bool isBrakePressed();
    bool isClutchPressed();
    int getAccelVoltage();
private:
    bool _brakePressed;
    bool _clutchPressed;
    int _accelVoltage;
};