#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include "config/settings.h"

class DebugManager
{
public:
    static void begin();
    static bool enabled();

    template<typename T>
    static bool print(const T& value)
    {
        if (!debugEnabled) return false;
        DEBUG_SERIAL.print(value);
        return true;
    }

    template<typename T>
    static bool println(const T& value)
    {
        if (!debugEnabled) return false;
        DEBUG_SERIAL.println(value);
        return true;
    }

    template<typename T>
    static bool print(const T& value, int format)
    {
        if (!debugEnabled) return false;
        DEBUG_SERIAL.print(value, format);
        return true;
    }

    template<typename T>
    static bool println(const T& value, int format)
    {
        if (!debugEnabled) return false;
        DEBUG_SERIAL.println(value, format);
        return true;
    }

private:
    static bool debugEnabled;

public:
    static uint8_t cpmcusr;
    
};

#endif