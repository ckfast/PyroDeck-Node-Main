#include "display_sleep_page.h"

#include <stdio.h>

UiPageId DisplaySleepPage::id() const {
    return UiPageId::DisplaySleep;
}

UiPageId DisplaySleepPage::parent() const {
    return UiPageId::SettingsMenu;
}

const char* DisplaySleepPage::navLabel() const {
    return "Display Sleep";
}

const char* DisplaySleepPage::settingTitle() const {
    return "Display Sleep";
}

uint16_t DisplaySleepPage::readValue(UIContext& ctx) const {
    return ctx.controller ? ctx.controller->getDisplaySleepTimeout() : 30000;
}

void DisplaySleepPage::writeValue(UIContext& ctx, uint16_t value) {
    if (ctx.controller) {
        ctx.controller->setDisplaySleepTimeout(value);
    }
}

uint16_t DisplaySleepPage::minValue() const {
    return 5000;
}

uint16_t DisplaySleepPage::maxValue() const {
    return 60000;
}

uint16_t DisplaySleepPage::step() const {
    return 1000;
}

void DisplaySleepPage::formatValue(char* out, size_t outSize, uint16_t value) const {
    snprintf(out, outSize, "%u s", static_cast<unsigned>(value / 1000));
}
