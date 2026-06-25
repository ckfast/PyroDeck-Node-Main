#include "network_page.h"

#include <stdio.h>

UiPageId NetworkPage::id() const {
    return UiPageId::Network;
}

UiPageId NetworkPage::parent() const {
    return UiPageId::SettingsMenu;
}

const char* NetworkPage::navLabel() const {
    return "Network";
}

const char* NetworkPage::settingTitle() const {
    return "Node ID";
}

uint16_t NetworkPage::readValue(UIContext& ctx) const {
    return ctx.controller ? ctx.controller->getNodeId() : DEFAULT_NODE_ID;
}

void NetworkPage::writeValue(UIContext& ctx, uint16_t value) {
    if (ctx.controller) {
        ctx.controller->setNodeId(static_cast<uint8_t>(value));
    }
    if (ctx.stats) {
        ctx.stats->nodeAddress = static_cast<uint8_t>(value);
    }
}

uint16_t NetworkPage::minValue() const {
    return MIN_NODE_ID;
}

uint16_t NetworkPage::maxValue() const {
    return MAX_NODE_ID;
}

uint16_t NetworkPage::step() const {
    return 1;
}

void NetworkPage::formatValue(char* out, size_t outSize, uint16_t value) const {
    snprintf(out, outSize, "ID %u", static_cast<unsigned>(value));
}
