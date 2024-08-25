// SWITCH.h
#ifndef SWITCH_H
#define SWITCH_H
// SWITCH
#define RFID_PIN 35
#define ConnectorPlugged 12
#define EvseReady 14
#define EvReady 27
#include <Arduino.h>

class SWITCH
{
public:
    explicit SWITCH(int switchPin = 0);
    bool Is_Ready();

private:
    int _switchPin;
};

#endif // SWITCH_H
