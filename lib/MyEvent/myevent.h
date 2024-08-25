#include <events.hpp>
#include <Arduino.h>
#include <string.h>
#include "LEDOption.h"
namespace esp32m
{

    class HeartBeatEvent : public Event
    {
    public:
        // 使用默认的 LED 引脚
        HeartBeatEvent() : Event("heartbeat"), heartLed(HERTBEAT) {}
        void Blink()
        {
            heartLed.LED_Blink();
        }
        // Getter 方法获取 LED 引脚
        int getLedPin() const { return _ledPin; }

    private:
        LEDOption heartLed; // 创建 LED 对象
        int _ledPin;        // 实际使用的 LED 引脚号
    };

} // namespace esp32m
