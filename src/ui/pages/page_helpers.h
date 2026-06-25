#ifndef UI_PAGE_HELPERS_H
#define UI_PAGE_HELPERS_H

#include <Arduino.h>
#include <U8g2lib.h>

#include "ui/menu/menu_library.h"

namespace ui_page_helpers {

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;
const uint8_t OLED_MAX_X = OLED_WIDTH - 1;
const uint8_t HEADER_HEIGHT = 16;
const uint8_t UI_PADDING = 2;

void drawHeader(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const NodeState* state, const ProtocolStats* stats);
void drawProgressBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, int x, int y, int width, int height, uint8_t value);\
void drawCenteredText(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const char* text, int y, const uint8_t* font);

}

#endif