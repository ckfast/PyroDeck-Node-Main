#include "channel_detail_page.h"

#include <stdio.h>

#include "page_helpers.h"

ChannelDetailPage::ChannelDetailPage()
    : _selectedChannel(0)
{
}

UiPageId ChannelDetailPage::id() const {
    return UiPageId::ChannelDetail;
}

UiPageId ChannelDetailPage::parent() const {
    return UiPageId::MainMenu;
}

const char* ChannelDetailPage::navLabel() const {
    return "Fire";
}

void ChannelDetailPage::onEnter(MenuLibrary& nav, UIContext& ctx) {
    if (ctx.state && !ctx.state->armed) {
        nav.goToParent(ctx);
        return;
    }

    if (_selectedChannel > 7) {
        _selectedChannel = 0;
    }
}

bool ChannelDetailPage::onLeft(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    _selectedChannel = (_selectedChannel == 0) ? 7 : static_cast<uint8_t>(_selectedChannel - 1);
    return true;
}

bool ChannelDetailPage::onRight(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    _selectedChannel = (_selectedChannel == 7) ? 0 : static_cast<uint8_t>(_selectedChannel + 1);
    return true;
}

bool ChannelDetailPage::onCenter(MenuLibrary& nav, UIContext& ctx) {
    if (_selectedChannel == 7) {
        return nav.goToParent(ctx);
    }

    if (ctx.controller) {
        ctx.controller->requestFire(static_cast<uint8_t>(_selectedChannel + 1));
    }

    return nav.goTo(UiPageId::Status, ctx);
}

void ChannelDetailPage::render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) {
    (void)nav;
    if (!display || !ctx.state || !ctx.stats) {
        return;
    }

    display->clearBuffer();
    ui_page_helpers::drawHeader(display, ctx.state, ctx.stats);
    display->setFont(u8g2_font_5x7_tr);

    if (_selectedChannel == 7) {
        display->drawStr(0, 24, "BACK");
        display->drawStr(0, 36, "Return to menu");
        display->drawStr(0, 60, "C BACK");
        display->sendBuffer();
        return;
    }

    char title[12];
    snprintf(title, sizeof(title), "CH %u", static_cast<unsigned>(_selectedChannel + 1));
    display->drawStr(0, 24, title);

    if (ctx.state->firingMask & (1UL << _selectedChannel)) {
        display->drawStr(0, 36, "ACTIVE");
    } else {
        display->drawStr(0, 36, "READY");
    }

    display->drawStr(0, 48, "CONT:");
    if (ctx.state->continuityMask & (1UL << _selectedChannel)) {
        display->drawStr(40, 48, "OK");
    } else {
        display->drawStr(40, 48, "BAD");
    }

    display->drawStr(0, 60, "L/R SEL C FIRE BAK");
    display->sendBuffer();
}
