#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "config/pins.h"

class ButtonHandler {
public:
    ButtonHandler();
    
    void begin();
    
    void update();
    
    bool isLeftPressed();
    
    bool isCenterPressed();
    
    bool isRightPressed();
    
    bool wasLeftClicked();
    
    bool wasCenterClicked();
    
    bool wasRightClicked();
    
    void setLEDBrightness(uint8_t brightness);

private:
    static const uint16_t DEBOUNCE_TIME = 0;
    static const uint16_t LONG_PRESS_TIME = 1000;
    
    uint8_t _leftState;
    uint8_t _centerState;
    uint8_t _rightState;

    uint8_t _lastLeftReading;
    uint8_t _lastCenterReading;
    uint8_t _lastRightReading;
    
    uint32_t _leftPressTime;
    uint32_t _centerPressTime;
    uint32_t _rightPressTime;
    
    bool _leftClicked;
    bool _centerClicked;
    bool _rightClicked;
    
    uint32_t _lastDebounceLeft;
    uint32_t _lastDebounceCentr;
    uint32_t _lastDebounceRight;
};

#endif // BUTTONS_H
