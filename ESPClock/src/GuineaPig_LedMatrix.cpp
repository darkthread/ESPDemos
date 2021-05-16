
#include <Arduino.h>
#include <LEDMatrixDriver.hpp>
#include <iostream>
#include <sstream>
#include "ldm-fonts.h"
#include "GuineaPig_LedMatrix.h"

/**
 * MOSI=DIN, SCK=CLK, CS=Chip Select
 * ESP32 MOSI=23, CLK=18, CS=15
 * ESP8266 MOSI=D7, CLK=D5 CS=D8
 */
#if ESP32
#define LEDMATRIX_CS 4
#else
#define LEDMATRIX_CS D8
#endif
#define LEDMATRIX_WIDTH 64

LEDMatrixDriver lmd(4, LEDMATRIX_CS);

/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void drawSprite(byte *sprite, int x, int y, int width, int height)
{
    // row bit mask
    byte mask;
    for (int ix = 0; ix < width; ix++)
    {
        mask = B10000000;
        for (int iy = height - 1; iy >= 0; iy--)
        {

            lmd.setPixel(x + ix, y + iy, (bool)(sprite[ix] & mask));

            // shift the mask by one pixel to the right
            mask = mask >> 1;
        }
    }
}
const int fontWidth = 6;
byte fontData[fontWidth];
void drawString(const char *text, int len, int x, int y)
{
    for (int idx = 0; idx < len; idx++)
    {
        int c = text[idx];

        // stop if char is outside visible area
        if (x + idx * fontWidth > LEDMATRIX_WIDTH)
            return;

        // only draw if char is visible
        if (8 + x + idx * fontWidth > 0)
        {
            memcpy_P(fontData, font5x8[c - 1], sizeof(fontData));
            drawSprite(fontData, x + idx * fontWidth, y, fontWidth, 8);
        }
    }
}

String message = "darkthread";
int ledX = 0;
//std::stringstream _sb;
void GuineaPig_LedMatrix::init()
{
    // init the display
    lmd.setEnabled(true);
    lmd.setIntensity(0); // 0 = low, 10 = high    
}

int msgLen;
int scrollDelay = 50;

void GuineaPig_LedMatrix::setScrollingDelay(int delay)
{
    scrollDelay = delay;
}

bool enableScrolling = false;

void GuineaPig_LedMatrix::clear() {
    lmd.clear();
    lmd.display();
}

void GuineaPig_LedMatrix::showText() {
    drawString(message.c_str(), message.length(), ledX, 0);
    lmd.display();
}

void GuineaPig_LedMatrix::toggleScroll(String flag) {
    enableScrolling = flag == "Y";
    clear();
    showText();
}

void GuineaPig_LedMatrix::loop()
{
    if (!enableScrolling) return;
    msgLen = message.length();
    drawString(message.c_str(), msgLen, ledX, 0);
    // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background...

    // Toggle display of the new framebuffer
    lmd.display();

    // Wait to let the human read the display
    delay(scrollDelay);

    // Advance to next coordinate
    if (--ledX < msgLen * -fontWidth)
    {
        ledX = LEDMATRIX_WIDTH;
    }
}

void GuineaPig_LedMatrix::setText(String text)
{
    clear();
    message = text;
    ledX =  8 * (1 - LEDMATRIX_WIDTH / 16);
    showText();
}

void drawDigit(byte n, int x) {
    memcpy_P(fontData, fontDigits[n], 4);
    if (n == 10)
        drawSprite(fontData, x, 0, 1, 8);
    else
        drawSprite(fontData, x, 0, 4, 8);
}

void GuineaPig_LedMatrix::printTime(int h, int m, int s) {
    int x = 0;
    lmd.clear();
    drawDigit(h / 10, x);
    x += 5;
    drawDigit(h % 10, x);
    x += 5;
    //drawDigit(10, x);
    x++;
    drawDigit(m / 10, x);
    x += 5;
    drawDigit(m % 10, x);
    x += 5;
    //drawDigit(10, x);
    x++;
    drawDigit(s / 10, x);
    x += 5;
    drawDigit(s % 10, x);
    lmd.display();
}

void GuineaPig_LedMatrix::setBrightness(int level) {
    if (level < 0) level = 0;
    if (level > 15) level = 15;
    lmd.setIntensity(level);
}