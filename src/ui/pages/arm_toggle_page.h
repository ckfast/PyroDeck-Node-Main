#ifndef UI_ARM_TOGGLE_PAGE_H
#define UI_ARM_TOGGLE_PAGE_H

#include "ui/menu/menu_library.h"

class ArmTogglePage : public IUiPage {
public:
    UiPageId id() const override;
    UiPageId parent() const override;
    const char* navLabel() const override;

    bool showInParentMenu() const override;
    void onEnter(MenuLibrary& nav, UIContext& ctx) override;
    bool onLeft(MenuLibrary& nav, UIContext& ctx) override;
    bool onRight(MenuLibrary& nav, UIContext& ctx) override;
    bool onCenter(MenuLibrary& nav, UIContext& ctx) override;
    void render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) override;
};

#endif