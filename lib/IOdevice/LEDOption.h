// LEDOption.h
#ifndef LEDOPTION_H
#define LEDOPTION_H
// LED
#define PermitsCharge 4
#define HERTBEAT 2
#include <Arduino.h>

class LEDOption
{
public:
    explicit LEDOption(int ledPin = 0);
    void LED_Blink();
    void LED_On();
    void LED_Off();

private:
    int _ledPin;
};

#endif // LEDOPTION_H
