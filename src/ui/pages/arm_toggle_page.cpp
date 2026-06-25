#include "arm_toggle_page.h"

UiPageId ArmTogglePage::id() const {
    return UiPageId::ArmToggle;
}

UiPageId ArmTogglePage::parent() const {
    return UiPageId::MainMenu;
}

const char* ArmTogglePage::navLabel() const {
    return "Arm/Disarm";
}

bool ArmTogglePage::showInParentMenu() const {
    return false;
}

void ArmTogglePage::onEnter(MenuLibrary& nav, UIContext& ctx) {
    if (ctx.controller) {
        if (ctx.state && ctx.state->armed) {
            ctx.controller->disarm();
        } else {
            ctx.controller->arm();
        }
    }

    nav.goDefault(ctx);
}

bool ArmTogglePage::onLeft(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    return false;
}

bool ArmTogglePage::onRight(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    return false;
}

bool ArmTogglePage::onCenter(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;
    return false;
}

void ArmTogglePage::render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) {
    (void)display;
    (void)nav;
    (void)ctx;
}
