// SWITCH.cpp
#include "SWITCH.h"

SWITCH::SWITCH(int switchPin) : _switchPin(switchPin)
{
    pinMode(_switchPin, INPUT);
}

bool SWITCH::Is_Ready()
{
    return digitalRead(_switchPin) == HIGH;
}
