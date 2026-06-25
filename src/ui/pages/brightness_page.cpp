#include "brightness_page.h"

#include <stdio.h>

UiPageId BrightnessPage::id() const {
    return UiPageId::Brightness;
}

UiPageId BrightnessPage::parent() const {
    return UiPageId::SettingsMenu;
}

const char* BrightnessPage::navLabel() const {
    return "Brightness";
}

const char* BrightnessPage::settingTitle() const {
    return "Brightness";
}

uint16_t BrightnessPage::readValue(UIContext& ctx) const {
    return ctx.controller ? ctx.controller->getUiBrightness() : DEFAULT_UI_BRIGHTNESS;
}

void BrightnessPage::writeValue(UIContext& ctx, uint16_t value) {
    if (ctx.controller) {
        ctx.controller->setUiBrightness(static_cast<uint8_t>(value));
    }
}

uint16_t BrightnessPage::minValue() const {
    return 0;
}

uint16_t BrightnessPage::maxValue() const {
    return 255;
}

uint16_t BrightnessPage::step() const {
    return 5;
}

void BrightnessPage::formatValue(char* out, size_t outSize, uint16_t value) const {
    snprintf(out, outSize, "%u", static_cast<unsigned>(value));
}
