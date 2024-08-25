// LEDOption.cpp
#include "LEDOption.h"

extern void Blink(void *pvParameters); // Declare the Blink function

static int blinkLedPin; // let blinkLedPin be a global variable

void Blink(void *pvParameters)
{
    digitalWrite(blinkLedPin, HIGH); // Turn on the LED
    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5000 ms
    digitalWrite(blinkLedPin, LOW);  // Turn off the LED
    vTaskDelete(NULL);               // Delete the task
}
LEDOption::LEDOption(int ledPin) : _ledPin(ledPin)
{
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

void LEDOption::LED_Blink()
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

void LEDOption::LED_On()
{
    digitalWrite(_ledPin, HIGH);
}

void LEDOption::LED_Off()
{
    digitalWrite(_ledPin, LOW);
}
