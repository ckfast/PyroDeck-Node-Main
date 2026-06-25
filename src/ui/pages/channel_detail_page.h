#ifndef UI_CHANNEL_DETAIL_PAGE_H
#define UI_CHANNEL_DETAIL_PAGE_H

#include "ui/menu/menu_library.h"

class ChannelDetailPage : public IUiPage {
public:
    ChannelDetailPage();

    UiPageId id() const override;
    UiPageId parent() const override;
    const char* navLabel() const override;

    void onEnter(MenuLibrary& nav, UIContext& ctx) override;
    bool onLeft(MenuLibrary& nav, UIContext& ctx) override;
    bool onRight(MenuLibrary& nav, UIContext& ctx) override;
    bool onCenter(MenuLibrary& nav, UIContext& ctx) override;

    void render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) override;

private:
    uint8_t _selectedChannel;
};

#endif