#include <Arduino.h>

class GuineaPig_LedMatrix
{
public:
    GuineaPig_LedMatrix() = default;
    void init();
    void setText(String msg);
    void setScrollingDelay(int delay);
    void loop();
};