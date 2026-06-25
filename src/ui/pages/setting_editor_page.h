#ifndef UI_SETTING_EDITOR_PAGE_H
#define UI_SETTING_EDITOR_PAGE_H

#include "ui/menu/menu_library.h"

class SettingEditorPage : public IUiPage {
public:
    SettingEditorPage();

    void onEnter(MenuLibrary& nav, UIContext& ctx) override;
    bool onLeft(MenuLibrary& nav, UIContext& ctx) override;
    bool onRight(MenuLibrary& nav, UIContext& ctx) override;
    bool onCenter(MenuLibrary& nav, UIContext& ctx) override;
    bool allowHoldRepeat(const UIContext& ctx) const override;

    void render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) override;

protected:
    virtual const char* settingTitle() const = 0;
    virtual uint16_t readValue(UIContext& ctx) const = 0;
    virtual void writeValue(UIContext& ctx, uint16_t value) = 0;
    virtual uint16_t minValue() const = 0;
    virtual uint16_t maxValue() const = 0;
    virtual uint16_t step() const = 0;
    virtual void formatValue(char* out, size_t outSize, uint16_t value) const = 0;

private:
    uint8_t _selectedOption;
    bool _editing;
    bool _dirty;
    uint16_t _value;

    void adjustValue(int8_t direction);
};

#endif