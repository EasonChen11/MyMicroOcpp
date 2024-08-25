#include <Arduino.h>
// LED
#define PermitsCharge 4
#define HERTBEAT 2

// SWITCH
#define RFID_PIN 35
#define ConnectorPlugged 12
#define EvseReady 14
#define EvReady 27

static int blinkLedPin;
void Blink(void *pvParameters)
{
    digitalWrite(blinkLedPin, HIGH); // Turn on the LED
    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 1000 ms
    digitalWrite(blinkLedPin, LOW);  // Turn off the LED
    vTaskDelete(NULL);               // Delete the task
}
class LEDOption
{
public:
    LEDOption(const int ledPin = 0) : _ledPin(ledPin)
    {
        pinMode(_ledPin, OUTPUT);
        digitalWrite(_ledPin, LOW);
    };
    void LED_Blink()
    {
        blinkLedPin = _ledPin;
        xTaskCreate(
            Blink,   // Function that should be called
            "Blink", // Name of the task (for debugging)
            1000,    // Stack size (bytes)
            NULL,    // Parameter to pass
            1,       // Task priority
            NULL);   // Task handle
    }

    void LED_On()
    {
        digitalWrite(_ledPin, HIGH);
    }
    void LED_Off()
    {
        digitalWrite(_ledPin, LOW);
    }

private:
    int _ledPin;
};

class SWITCH
{
public:
    SWITCH(const int switchPin = 0) : _switchPin(switchPin)
    {
        pinMode(_switchPin, INPUT);
    };
    bool Is_Ready()
    {
        return digitalRead(_switchPin) == HIGH;
    }

private:
    int _switchPin;
};