#include "config.h"
#include "EEPROM.h"

void Config::clear()
{
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }
}

int Config::getRpmMin()
{
    int result = 0;
    EEPROM.get(sizeof(int) * 0, result);
    return result;
}

void Config::setRpmMin(int value)
{
    EEPROM.put(sizeof(int) * 0, value);
}

int Config::getRpmMax()
{
    int result = 0;
    EEPROM.get(sizeof(int) * 1, result);
    return result;
}

void Config::setRpmMax(int value)
{
    EEPROM.write(sizeof(int) * 1, value);
}