#include "OLED.h"

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
#define NUMFLAKES 10

const unsigned char OLED::logo_bmp[] PROGMEM = {
    0b00000000, 0b11000000,
    0b00000001, 0b11000000,
    0b00000001, 0b11000000,
    0b00000011, 0b11100000,
    0b11110011, 0b11100000,
    0b11111110, 0b11111000,
    0b01111110, 0b11111111,
    0b00110011, 0b10011111,
    0b00011111, 0b11111100,
    0b00001101, 0b01110000,
    0b00011011, 0b10100000,
    0b00111111, 0b11100000,
    0b00111111, 0b11110000,
    0b01111100, 0b11110000,
    0b01110000, 0b01110000,
    0b00000000, 0b00110000};

OLED::OLED() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

bool OLED::begin()
{
    if (!Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        return false;
    }
    return true;
}

void OLED::showSplashScreen()
{
    display();
    // delay(2000);
    clearDisplay();
}

void OLED::testDrawPixel()
{
    clearDisplay();
    drawPixel(10, 10, SSD1306_WHITE);
    display();
    delay(2000);
}

void OLED::testDrawLine()
{
    int16_t i;
    clearDisplay();

    for (i = 0; i < width(); i += 4)
    {
        drawLine(0, 0, i, height() - 1, SSD1306_WHITE);
        display();
        delay(1);
    }
    for (i = 0; i < height(); i += 4)
    {
        drawLine(0, 0, width() - 1, i, SSD1306_WHITE);
        display();
        delay(1);
    }
    delay(250);

    clearDisplay();

    for (i = 0; i < width(); i += 4)
    {
        drawLine(0, height() - 1, i, 0, SSD1306_WHITE);
        display();
        delay(1);
    }
    for (i = height() - 1; i >= 0; i -= 4)
    {
        drawLine(0, height() - 1, width() - 1, i, SSD1306_WHITE);
        display();
        delay(1);
    }
    delay(250);

    clearDisplay();

    for (i = width() - 1; i >= 0; i -= 4)
    {
        drawLine(width() - 1, height() - 1, i, 0, SSD1306_WHITE);
        display();
        delay(1);
    }
    for (i = height() - 1; i >= 0; i -= 4)
    {
        drawLine(width() - 1, height() - 1, 0, i, SSD1306_WHITE);
        display();
        delay(1);
    }
    delay(250);

    clearDisplay();

    for (i = 0; i < height(); i += 4)
    {
        drawLine(width() - 1, 0, 0, i, SSD1306_WHITE);
        display();
        delay(1);
    }
    for (i = 0; i < width(); i += 4)
    {
        drawLine(width() - 1, 0, i, height() - 1, SSD1306_WHITE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testDrawRect()
{
    clearDisplay();

    for (int16_t i = 0; i < height() / 2; i += 2)
    {
        drawRect(i, i, width() - 2 * i, height() - 2 * i, SSD1306_WHITE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testFillRect()
{
    clearDisplay();

    for (int16_t i = 0; i < height() / 2; i += 3)
    {
        fillRect(i, i, width() - i * 2, height() - i * 2, SSD1306_INVERSE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testDrawCircle()
{
    clearDisplay();

    for (int16_t i = 0; i < max(width(), height()) / 2; i += 2)
    {
        drawCircle(width() / 2, height() / 2, i, SSD1306_WHITE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testFillCircle()
{
    clearDisplay();

    for (int16_t i = max(width(), height()) / 2; i > 0; i -= 3)
    {
        fillCircle(width() / 2, height() / 2, i, SSD1306_INVERSE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testDrawRoundRect()
{
    clearDisplay();

    for (int16_t i = 0; i < height() / 2 - 2; i += 2)
    {
        drawRoundRect(i, i, width() - 2 * i, height() - 2 * i, height() / 4, SSD1306_WHITE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testFillRoundRect()
{
    clearDisplay();

    for (int16_t i = 0; i < height() / 2 - 2; i += 2)
    {
        fillRoundRect(i, i, width() - 2 * i, height() - 2 * i, height() / 4, SSD1306_INVERSE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testDrawTriangle()
{
    clearDisplay();

    for (int16_t i = 0; i < max(width(), height()) / 2; i += 5)
    {
        drawTriangle(width() / 2, height() / 2 - i,
                     width() / 2 - i, height() / 2 + i,
                     width() / 2 + i, height() / 2 + i, SSD1306_WHITE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testFillTriangle()
{
    clearDisplay();

    for (int16_t i = max(width(), height()) / 2; i > 0; i -= 5)
    {
        fillTriangle(width() / 2, height() / 2 - i,
                     width() / 2 - i, height() / 2 + i,
                     width() / 2 + i, height() / 2 + i, SSD1306_INVERSE);
        display();
        delay(1);
    }

    delay(2000);
}

void OLED::testDrawChar()
{
    clearDisplay();

    setTextSize(1);
    setTextColor(SSD1306_WHITE);
    setCursor(0, 0);
    cp437(true);

    for (int16_t i = 0; i < 256; i++)
    {
        if (i == '\n')
            write(' ');
        else
            write(i);
    }

    display();
    delay(2000);
}

void OLED::testDrawStyles()
{
    clearDisplay();

    setTextSize(1);
    setTextColor(SSD1306_WHITE);
    setCursor(0, 0);
    println(F("Hello, world!"));

    setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    println(3.141592);

    setTextSize(2);
    setTextColor(SSD1306_WHITE);
    print(F("0x"));
    println(0xDEADBEEF, HEX);

    display();
    delay(2000);
}

void OLED::testScrollText()
{
    clearDisplay();

    setTextSize(2);
    setTextColor(SSD1306_WHITE);
    setCursor(10, 0);
    println(F("scroll"));
    display();
    delay(100);

    startscrollright(0x00, 0x0F);
    delay(2000);
    stopscroll();
    delay(1000);
    startscrollleft(0x00, 0x0F);
    delay(2000);
    stopscroll();
    delay(1000);
    startscrolldiagright(0x00, 0x07);
    delay(2000);
    startscrolldiagleft(0x00, 0x07);
    delay(2000);
    stopscroll();
    delay(1000);
}

void OLED::testDrawBitmap()
{
    clearDisplay();

    drawBitmap((width() - LOGO_WIDTH) / 2, (height() - LOGO_HEIGHT) / 2,
               logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display();
    delay(1000);
}

void OLED::testAnimate()
{
    int8_t f, icons[NUMFLAKES][3];

    for (f = 0; f < NUMFLAKES; f++)
    {
        icons[f][0] = random(1 - LOGO_WIDTH, width());
        icons[f][1] = -LOGO_HEIGHT;
        icons[f][2] = random(1, 6);
    }

    for (;;)
    {
        clearDisplay();

        for (f = 0; f < NUMFLAKES; f++)
        {
            drawBitmap(icons[f][0], icons[f][1], logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
        }

        display();
        delay(200);

        for (f = 0; f < NUMFLAKES; f++)
        {
            icons[f][1] += icons[f][2];

            if (icons[f][1] >= height())
            {
                icons[f][0] = random(1 - LOGO_WIDTH, width());
                icons[f][1] = -LOGO_HEIGHT;
                icons[f][2] = random(1, 6);
            }
        }
    }
}

void OLED::updateStatusDisplay(const ChargePointStatus &status)
{
    m_status = MicroOcpp::cstrFromOcppEveState(status);
    // if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE)
    // {
    //     m_status = MicroOcpp::cstrFromOcppEveState(status);
    //     xSemaphoreGive(m_mutex);
    // }
}

void OLED::updateEnergy(const float &energy)
{
    m_energy = energy;
}

void OLED::updateIdTag(const String &idTag)
{
    m_idTag = idTag;
}

void OLED::haveTxDoUpdateDisplay()
{
    clearDisplay();
    setTextSize(1);
    setTextColor(SSD1306_WHITE);

    setCursor(0, 0);
    println(m_status);

    setCursor(0, 10);
    println(m_idTag);

    setCursor(0, 20);
    print(m_energy);
    println(" Wh");

    display();
}

void OLED::notHaveTxDoUpdateDisplay()
{
    clearDisplay();
    setTextSize(2);
    setTextColor(SSD1306_WHITE);

    setCursor(0, 0);
    println(m_status);

    display();
}