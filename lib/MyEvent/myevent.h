#include <events.hpp>
#include <Arduino.h>
#include <string.h>
#include "LEDOption.h"
namespace esp32m
{

    class HeartBeatEvent : public Event
    {
    public:
        // use the constructor to set the LED pin
        HeartBeatEvent() : Event("heartbeat"), heartLed(HERTBEAT) {}
        void Blink()
        {
            heartLed.LED_Blink();
        }
        int getLedPin() const { return _ledPin; }

    private:
        LEDOption heartLed; // LED option
        int _ledPin;    // LED pin
    };

} // namespace esp32m
/*
enable event
.pio\libdeps\esp32dev\WebSockets\src\WebSockets.cpp 751 line
                esp32m::HeartBeatEvent ev;
                esp32m::EventManager::instance().publish(ev);
*/