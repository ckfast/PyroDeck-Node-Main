#include "status_page.h"

#include <stdio.h>

#include "page_helpers.h"

StatusPage::StatusPage()
    : _styleIndex(0)
{
}

UiPageId StatusPage::id() const {
    return UiPageId::Status;
}

UiPageId StatusPage::parent() const {
    return UiPageId::None;
}

const char* StatusPage::navLabel() const {
    return "Status";
}

bool StatusPage::showInParentMenu() const {
    return false;
}

bool StatusPage::onLeft(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    _styleIndex = (_styleIndex == 0) ? 2 : static_cast<uint8_t>(_styleIndex - 1);
    return true;
}

bool StatusPage::onRight(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    _styleIndex = static_cast<uint8_t>((_styleIndex + 1) % 3);
    return true;
}

bool StatusPage::onCenter(MenuLibrary& nav, UIContext& ctx) {
    return nav.openSelectedChild(UiPageId::Status, ctx);
}

void StatusPage::render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) {
    (void)nav;
    if (!display || !ctx.state || !ctx.stats) {
        return;
    }

    display->clearBuffer();
    ui_page_helpers::drawHeader(display, ctx.state, ctx.stats);

    const uint8_t contentTop = ui_page_helpers::HEADER_HEIGHT + ui_page_helpers::UI_PADDING;
    display->setFont(u8g2_font_5x7_tr);
    display->setDrawColor(1);

    if (_styleIndex == 0) {
        drawGrid(display, ctx.state);
    } else if (_styleIndex == 1) {
        for (uint8_t i = 0; i < 7; i++) {
            const uint8_t row = i / 2;
            const uint8_t col = i % 2;
            const uint8_t x = static_cast<uint8_t>(ui_page_helpers::UI_PADDING + col * 62);
            const uint8_t y = static_cast<uint8_t>(contentTop + 8 + row * 11);
            const bool firing = (ctx.state->firingMask & (1UL << i)) != 0;
            const bool continuity = (ctx.state->continuityMask & (1UL << i)) != 0;

            char line[22];
            snprintf(line, sizeof(line), "C%u F:%c C:%c", i + 1, firing ? 'Y' : 'N', continuity ? 'Y' : 'N');
            display->drawStr(x, y, line);
        }
    } else {
        uint8_t continuityCount = 0;
        uint8_t firingCount = 0;
        for (uint8_t i = 0; i < 7; i++) {
            if (ctx.state->continuityMask & (1UL << i)) {
                continuityCount++;
            }
            if (ctx.state->firingMask & (1UL << i)) {
                firingCount++;
            }
        }

        char line[22];
        snprintf(line, sizeof(line), "CONT OK: %u/7", continuityCount);
        display->drawStr(ui_page_helpers::UI_PADDING, contentTop + 8, line);

        snprintf(line, sizeof(line), "FIRING: %u", firingCount);
        display->drawStr(ui_page_helpers::UI_PADDING, contentTop + 17, line);

        snprintf(line, sizeof(line), "ONLINE: %s", ctx.state->online ? "YES" : "NO");
        display->drawStr(ui_page_helpers::UI_PADDING, contentTop + 26, line);

        snprintf(line, sizeof(line), "FAULT: %s", ctx.state->hasFault ? "YES" : "NO");
        display->drawStr(ui_page_helpers::UI_PADDING, contentTop + 35, line);

        snprintf(line, sizeof(line), "ERR: %u", ctx.state->lastError);
        display->drawStr(ui_page_helpers::UI_PADDING, contentTop + 44, line);
    }

    const uint8_t indicatorY = ui_page_helpers::OLED_HEIGHT - 1;
    const uint8_t indicatorWidth = 26;
    const uint8_t indicatorGap = 10;
    const uint8_t totalPages = 3;
    const uint8_t totalWidth = static_cast<uint8_t>(totalPages * indicatorWidth + (totalPages - 1) * indicatorGap);
    const uint8_t startX = static_cast<uint8_t>((ui_page_helpers::OLED_WIDTH - totalWidth) / 2);

    for (uint8_t i = 0; i < totalPages; i++) {
        const uint8_t x = static_cast<uint8_t>(startX + i * (indicatorWidth + indicatorGap));
        if (i == _styleIndex) {
            display->drawHLine(x, indicatorY, indicatorWidth);
        } else {
            for (uint8_t px = 0; px < indicatorWidth; px += 2) {
                display->drawPixel(x + px, indicatorY);
            }
        }
    }

    display->sendBuffer();
}

void StatusPage::drawGrid(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const NodeState* state) const {
    const uint8_t boxWidth = 30;
    const uint8_t boxHeight = 21;
    const uint8_t gapX = 2;
    const uint8_t gapY = 0;
    const uint8_t columns = 4;

    const uint8_t gridWidth = static_cast<uint8_t>(columns * boxWidth + (columns - 1) * gapX);
    const uint8_t startX = static_cast<uint8_t>((ui_page_helpers::OLED_WIDTH - gridWidth) / 2);
    const uint8_t startY = 17;

    display->setFont(u8g2_font_5x7_tr);
    for (uint8_t i = 0; i < 7; i++) {
        const uint8_t column = i % 4;
        const uint8_t row = i / 4;
        const uint8_t x = startX + column * (boxWidth + gapX);
        const uint8_t y = startY + row * (boxHeight + gapY);

        //display->drawFrame(x, y, boxWidth, boxHeight);

        char label[4];
        snprintf(label, sizeof(label), "C%u", i + 1);
        uint8_t labelWidth = display->getStrWidth(label);
        uint8_t labelX = static_cast<uint8_t>(x + (boxWidth - labelWidth) / 2);
        display->drawStr(labelX, static_cast<uint8_t>(y + 8), label);

        // Firing indicator box
        const uint8_t fireX = x + 4;
        const uint8_t fireY = y + 9;
        const uint8_t indW = 10;
        const uint8_t indH = 10;

        if (state->firingMask & (1UL << i)) {
            display->drawBox(fireX, fireY, indW, indH);
            display->setDrawColor(0);
            display->drawStr(fireX + 3, fireY + 8, "F");
            display->setDrawColor(1);
        } else {
            display->drawFrame(fireX, fireY, indW, indH);
            display->drawStr(fireX + 3, fireY + 8, "F");
        }

        // Continuity indicator box
        const uint8_t contX = x + 16;
        const uint8_t contY = y + 9;

        if (state->continuityMask & (1UL << i)) {
            display->drawBox(contX, contY, indW, indH);
            display->setDrawColor(0);
            display->drawStr(contX + 3, contY + 8, "C");
            display->setDrawColor(1);
        } else {
            display->drawFrame(contX, contY, indW, indH);
            display->drawStr(contX + 3, contY + 8, "C");
        }
    }
}
