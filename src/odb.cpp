#include "odb.h"

void ODB::setup()
{
    _obd.begin();
    // TODO: Add error handling
    while (!_obd.init()) ;
}

int ODB::getRPM()
{
    return 0;
    int value;
    if(!_obd.readPID(PID_THROTTLE, value))
    {
        return -1;
    }
    return value;
}
