#ifndef SPLASH_H
#define SPLASH_H

#include <Arduino.h>
#include <U8g2lib.h>

class SplashScreen {
public:
    SplashScreen();
    
    void start(const char* title = "PyroDeck", const char* subtitle = "Node", uint32_t duration = SPLASH_DURATION);
    
    bool isActive() const;
    
    void draw(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display);
    
    void stop();

private:
    static const uint32_t SPLASH_DURATION = 3000; // 3 seconds
    
    uint32_t _startTime;
    uint32_t _duration;
    bool _isActive;
    char _title[17];
    char _subtitle[17];
};

#endif // SPLASH_H
