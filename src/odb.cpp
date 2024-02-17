#include "odb.h"

void ODB::setup()
{
    _obd.begin();
    // TODO: Add error handling
    while (!_obd.init()) ;
}

int ODB::getRPM()
{
    int value;
    if(!_obd.readPID(PID_RPM, value))
    {
        return -1;
    }
    return value;
}
