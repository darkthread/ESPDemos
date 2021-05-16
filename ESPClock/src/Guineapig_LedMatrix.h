#include <Arduino.h>

class GuineaPig_LedMatrix
{
public:
    GuineaPig_LedMatrix() = default;
    void init();
    void clear();
    void setText(String msg);
    void toggleScroll(String enabled);
    void setScrollingDelay(int delay);
    void loop();
    void printTime(int h, int m, int s);
    void setBrightness(int level);
    void showText();
};