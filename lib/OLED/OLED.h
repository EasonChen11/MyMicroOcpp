#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <MicroOcpp/Operations/StatusNotification.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

class OLED : public Adafruit_SSD1306
{
public:
    OLED();
    bool begin();
    void showSplashScreen();
    void testDrawPixel();
    void testDrawLine();
    void testDrawRect();
    void testFillRect();
    void testDrawCircle();
    void testFillCircle();
    void testDrawRoundRect();
    void testFillRoundRect();
    void testDrawTriangle();
    void testFillTriangle();
    void testDrawChar();
    void testDrawStyles();
    void testScrollText();
    void testDrawBitmap();
    void testAnimate();
    void updateStatusDisplay(const ChargePointStatus &status);
    void updateEnergy(const float &energy);
    void updateIdTag(const String &idTag);
    void haveTxDoUpdateDisplay();
    void notHaveTxDoUpdateDisplay();

private:
    static const unsigned char PROGMEM logo_bmp[];
    String m_status;
    String m_idTag;
    float m_energy;
    SemaphoreHandle_t m_mutex;
};

#endif