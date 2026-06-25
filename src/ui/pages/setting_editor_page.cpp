#include "setting_editor_page.h"

#include <stdio.h>

#include "page_helpers.h"

SettingEditorPage::SettingEditorPage()
    : _selectedOption(0),
      _editing(false),
      _dirty(false),
      _value(0)
{
}

void SettingEditorPage::onEnter(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    _value = readValue(ctx);
    _editing = false;
    _dirty = false;
    _selectedOption = 0;
}

bool SettingEditorPage::onLeft(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    if (_editing) {
        adjustValue(-1);
    } else {
        _selectedOption = (_selectedOption == 0) ? 2 : static_cast<uint8_t>(_selectedOption - 1);
    }
    return true;
}

bool SettingEditorPage::onRight(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    if (_editing) {
        adjustValue(1);
    } else {
        _selectedOption = static_cast<uint8_t>((_selectedOption + 1) % 3);
    }
    return true;
}

bool SettingEditorPage::onCenter(MenuLibrary& nav, UIContext& ctx) {
    if (_editing) {
        _editing = false;
        _selectedOption = _dirty ? 1 : 2;
        return true;
    }

    if (_selectedOption == 0) {
        _editing = true;
        return true;
    }

    if (_selectedOption == 1) {
        if (_dirty) {
            writeValue(ctx, _value);
            _dirty = false;
        }
        return nav.goToParent(ctx);
    }

    _value = readValue(ctx);
    _dirty = false;
    return nav.goToParent(ctx);
}

bool SettingEditorPage::allowHoldRepeat(const UIContext& ctx) const {
    (void)ctx;
    return _editing;
}

void SettingEditorPage::render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) {
    (void)nav;
    if (!display) {
        return;
    }

    display->clearBuffer();
    ui_page_helpers::drawHeader(display, ctx.state, ctx.stats);
    display->setFont(u8g2_font_5x7_tr);

    display->drawStr(0, 24, settingTitle());

    char valueBuf[20];
    formatValue(valueBuf, sizeof(valueBuf), _value);

    if (_editing) {
        display->drawBox(0, 30, 75, 12);
        display->setDrawColor(0);
        display->drawStr(4, 39, valueBuf);
        display->setDrawColor(1);
        display->drawStr(0, 56, "L/R ADJUST C DONE");
        display->sendBuffer();
        return;
    }

    if (_selectedOption == 0) {
        display->drawFrame(0, 30, 75, 12);
    }
    display->drawStr(4, 39, valueBuf);

    const bool saveEnabled = _dirty;

    if (_selectedOption == 1) {
        display->drawFrame(0, 46, 40, 10);
        if (!saveEnabled) {
            display->setDrawColor(0);
        }
        display->drawStr(4, 54, "Save");
        display->setDrawColor(1);
    } else {
        if (!saveEnabled) {
            display->setDrawColor(0);
        }
        display->drawStr(4, 54, "Save");
        display->setDrawColor(1);
    }

    if (_selectedOption == 2) {
        display->drawFrame(45, 46, 40, 10);
    }
    display->drawStr(49, 54, "Back");

    display->sendBuffer();
}

void SettingEditorPage::adjustValue(int8_t direction) {
    if (direction == 0) {
        return;
    }

    const uint16_t stepValue = step();
    const uint16_t min = minValue();
    const uint16_t max = maxValue();

    uint16_t previous = _value;

    if (direction < 0) {
        if (_value > min) {
            int32_t next = static_cast<int32_t>(_value) - static_cast<int32_t>(stepValue);
            _value = static_cast<uint16_t>(next < min ? min : next);
        }
    } else {
        if (_value < max) {
            uint32_t next = static_cast<uint32_t>(_value) + stepValue;
            _value = static_cast<uint16_t>(next > max ? max : next);
        }
    }

    if (_value != previous) {
        _dirty = true;
    }
}
