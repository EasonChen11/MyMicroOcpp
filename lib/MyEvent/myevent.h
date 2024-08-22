#include <events.hpp>
#include <Arduino.h>
#include <string.h>

namespace esp32m
{

    class HeartBeatEvent : public Event
    {
    public:
        // 使用默认的 LED 引脚
        HeartBeatEvent() : Event("heartbeat"), _ledPin(DEFAULT_LED_PIN)
        {
            pinMode(_ledPin, OUTPUT); // 设置 LED_PIN 为输出
        }

        // Getter 方法获取 LED 引脚
        int getLedPin() const { return _ledPin; }

    private:
        static const int DEFAULT_LED_PIN = 2; // 默认的 LED 引脚号
        int _ledPin;                          // 实际使用的 LED 引脚号
    };

} // namespace esp32m
