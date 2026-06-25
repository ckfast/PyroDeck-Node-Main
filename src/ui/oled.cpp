#include "oled.h"

#include <Wire.h>
#include <U8g2lib.h>

#include "config/pins.h"

OLEDDisplay::OLEDDisplay() 
  : _display(U8G2_R0, U8X8_PIN_NONE),
    _state(nullptr),
    _dirty(true),
    _lastRefresh(0)   
{
}

void OLEDDisplay::begin() {

    _display.begin();
    _display.clearBuffer();

    _dirty = true;
    _lastRefresh = millis();
}

void OLEDDisplay::setState(const NodeState* state) {

    _state = state;
    _dirty = true;
}

void OLEDDisplay::markDirty() {
    _dirty = true;
}

void OLEDDisplay::update() {
    if (!_dirty) return;
    
    uint32_t now = millis();
    
    // Throttle refresh rate to 30Hz max
    if (now - _lastRefresh < 33) return;
    
    _lastRefresh = now;

    // Render logic lives in page classes; this class only provides the display object.
    // Keep update lightweight for compatibility.
    (void)_state;
    
    _dirty = false;
}

void OLEDDisplay::drawHeader(const char* title) {
    _display.setFont(u8g2_font_7x13_tr);
    _display.clearBuffer();
    _display.drawStr(2, 12, title);
    _display.drawHLine(0, 14, 128);
    _display.sendBuffer();
}

void OLEDDisplay::clear() {
    _display.clearBuffer();
    _display.sendBuffer();
}