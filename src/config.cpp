#include "config.h"
#include "EEPROM.h"

void Config::clear()
{
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }
}

int Config::getCalibrationMin()
{
    int result = 0;
    EEPROM.get(sizeof(int) * 0, result);
    return result;
}

void Config::setCalibrationMin(int value)
{
    EEPROM.put(sizeof(int) * 0, value);
}

int Config::getCalibrationMax()
{
    int result = 0;
    EEPROM.get(sizeof(int) * 1, result);
    return result;
}

void Config::setCalibrationMax(int value)
{
    EEPROM.put(sizeof(int) * 1, value);
}

int Config::getLedCount()
{
    int result = 0;
    EEPROM.get(sizeof(int) * 2, result);
    return result;
}

void Config::setLedCount(int value)
{
    EEPROM.put(sizeof(int) * 2, value);
}