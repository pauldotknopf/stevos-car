#include "fakeodb.h"
#include <Arduino.h>

void FakeODB::setup()
{
    
}

int FakeODB::getRPM()
{
    return random(10, 4000);
}
