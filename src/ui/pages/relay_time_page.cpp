#include "relay_time_page.h"

#include <stdio.h>

UiPageId RelayTimePage::id() const {
    return UiPageId::RelayTime;
}

UiPageId RelayTimePage::parent() const {
    return UiPageId::SettingsMenu;
}

const char* RelayTimePage::navLabel() const {
    return "Relay Time";
}

const char* RelayTimePage::settingTitle() const {
    return "Relay Time";
}

uint16_t RelayTimePage::readValue(UIContext& ctx) const {
    return ctx.controller ? ctx.controller->getFiringTime() : 1000;
}

void RelayTimePage::writeValue(UIContext& ctx, uint16_t value) {
    if (ctx.controller) {
        ctx.controller->setFiringTime(value);
    }
}

uint16_t RelayTimePage::minValue() const {
    return 50;
}

uint16_t RelayTimePage::maxValue() const {
    return 5000;
}

uint16_t RelayTimePage::step() const {
    return 50;
}

void RelayTimePage::formatValue(char* out, size_t outSize, uint16_t value) const {
    snprintf(out, outSize, "%u ms", static_cast<unsigned>(value));
}
