#include <Arduino.h>

class GuineaPig_LedMatrix
{
public:
    GuineaPig_LedMatrix() = default;
    void init();
    void clear();
    void setText(String msg);
    void toggleScroll(bool enabled);
    void setScrollingDelay(int delay);
    void loop();
    void printTime(int h, int m, int s);
    void setBrightness(int level);
    void showText();
private:
    String message = "darkthread";
    int msgLen;
    int scrollDelay = 50;
    int ledX = 0;
    byte fontData[8];    
    void drawDigit(byte n, int x);
    void drawString(const char *text, int len, int x, int y);
    void drawSprite(byte *sprite, int x, int y, int width, int height);
};