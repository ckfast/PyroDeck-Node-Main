#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>

class WatchdogTimer {
public:
    WatchdogTimer();
    
    void begin();
    
    void pet();
    
    bool isActive() const;
    
    void enable(uint16_t timeoutMs = 2000);
    
    void disable();

private:
    static const uint16_t DEFAULT_TIMEOUT_MS = 2000;
    
    uint16_t _timeoutMs;
    uint32_t _lastPetTime;
    bool _isEnabled;
};

#endif // WATCHDOG_H
