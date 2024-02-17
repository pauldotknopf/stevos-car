#pragma once
#include "OBD2UART.h"

class ODB
{
public:
    void setup();
    int getRPM();
private:
    COBD _obd;
};