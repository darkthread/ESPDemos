
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
    // The mask is used to get the column bit from the sprite row
    byte mask = B10000000;

    for (int iy = 0; iy < height; iy++)
    {
        for (int ix = 0; ix < width; ix++)
        {

            lmd.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask));

            // shift the mask by one pixel to the right
            mask = mask >> 1;
        }

        // reset column mask
        mask = B10000000;
    }
}

byte fontData[8];
void drawString(const char *text, int len, int x, int y)
{
    for (int idx = 0; idx < len; idx++)
    {
        int c = text[idx];
        if (c > 127)
        {
            c = c < 160 ? 32 : c - 32;
        }

        // stop if char is outside visible area
        if (x + idx * 8 > LEDMATRIX_WIDTH)
            return;

        // only draw if char is visible
        if (8 + x + idx * 8 > 0)
        {
            memcpy_P(fontData, font8x8[c], sizeof(fontData));
            drawSprite(fontData, x + idx * 8, y, 8, 8);
        }
    }
}

String message = "darkthread ";
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

void GuineaPig_LedMatrix::setScrollingDelay(int delay) {
    scrollDelay = delay;
}

void GuineaPig_LedMatrix::loop()
{
    msgLen = message.length();
    //drawStringIBM(text, len, x, 0);
    drawString(message.c_str(), msgLen, ledX, 0);
    // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background...

    // Toggle display of the new framebuffer
    lmd.display();

    // Wait to let the human read the display
    delay(scrollDelay);

    // Advance to next coordinate
    if (--ledX < msgLen * -8)
    {
        ledX = LEDMATRIX_WIDTH;
    }
}

void GuineaPig_LedMatrix::setText(String text) {
    drawString("    ", 4, 0, 0);
    message = text + " ";
    ledX = 0;
}