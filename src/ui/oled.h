#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>

#include "config/pins.h"
#include "core/node_state.h"

class OLEDDisplay {

    public:

        OLEDDisplay();

        void begin();

        void update();

        void markDirty();

        void setState(const NodeState* state);
        
        void setProtocolState(bool rxActive, bool txActive, uint8_t address);
        
        U8G2_SSD1306_128X64_NONAME_F_HW_I2C* getDisplay() {
            return &_display;
        }

    private:

        U8G2_SSD1306_128X64_NONAME_F_HW_I2C _display;

        const NodeState* _state;

        bool _dirty;

        uint32_t _lastRefresh;

        void drawHeader(const char* title);

        void clear();
};

#endif //OLED_H