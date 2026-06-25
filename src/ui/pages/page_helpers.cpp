#include "page_helpers.h"

#include <stdio.h>

namespace ui_page_helpers {

void drawHeader(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const NodeState* state, const ProtocolStats* stats) {
    if (!display) {
        return;
    }

    display->drawBox(0, 0, OLED_WIDTH, HEADER_HEIGHT);
    display->setDrawColor(0);
    display->setFont(u8g2_font_5x7_tr);

    if (state && state->armed) {
        display->drawStr(UI_PADDING, 7, "ARMED");
    } else {
        display->drawStr(UI_PADDING, 7, "DISARM");
    }

    if (state && state->hasFault) {
        display->drawFrame(58, 1, 8, 8);
        display->drawStr(60, 7, "X");
    }

    if (stats) {
        char addrBuf[12];
        snprintf(addrBuf, sizeof(addrBuf), "ID:%u", stats->nodeAddress);
        display->drawStr(88, 7, addrBuf);
    }

    if (stats) {
        char ioBuf[20];
        if (state && !state->online) {
            snprintf(ioBuf, sizeof(ioBuf), "MASTER: OFFLINE");
        } else {
            snprintf(ioBuf, sizeof(ioBuf), "RX:%u TX:%u", stats->packetsRx % 100, stats->packetsTx % 100);
        }
        display->drawStr(UI_PADDING, 14, ioBuf);
    }

    display->setDrawColor(1);
    display->drawHLine(0, HEADER_HEIGHT - 1, OLED_WIDTH);
}

void drawProgressBar(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, int x, int y, int width, int height, uint8_t value) {
    if (!display) {
        return;
    }

    display->drawFrame(x, y, width, height);
    int fillWidth = map(value, 0, 100, 0, width - 2);
    display->drawBox(x + 1, y + 1, fillWidth, height - 2);
}

void drawCenteredText(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const char* text, int y, const uint8_t* font) {
    if (!display || !text || !font) {
        return;
    }

    display->setFont(font);
    int textWidth = display->getStrWidth(text);
    int x = (OLED_WIDTH - textWidth) / 2;
    display->drawStr(x, y, text);
}

}
