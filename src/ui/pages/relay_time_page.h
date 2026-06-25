#ifndef UI_RELAY_TIME_PAGE_H
#define UI_RELAY_TIME_PAGE_H

#include "setting_editor_page.h"

class RelayTimePage : public SettingEditorPage {
public:
    UiPageId id() const override;
    UiPageId parent() const override;
    const char* navLabel() const override;

protected:
    const char* settingTitle() const override;
    uint16_t readValue(UIContext& ctx) const override;
    void writeValue(UIContext& ctx, uint16_t value) override;
    uint16_t minValue() const override;
    uint16_t maxValue() const override;
    uint16_t step() const override;
    void formatValue(char* out, size_t outSize, uint16_t value) const override;
};

#endif