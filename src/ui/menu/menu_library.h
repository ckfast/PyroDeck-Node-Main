#ifndef UI_MENU_LIBRARY_H
#define UI_MENU_LIBRARY_H

#include <Arduino.h>
#include <U8g2lib.h>

#include "core/node_state.h"
#include "core/system_manager.h"

struct ProtocolStats {
    uint16_t packetsRx;
    uint16_t packetsTx;
    uint8_t nodeAddress;
};

enum class UiPageId : uint8_t {
    Status = 0,
    MainMenu,
    ArmToggle,
    ChannelDetail,
    SettingsMenu,
    Network,
    Brightness,
    RelayTime,
    DisplaySleep,
    Count,
    None = 255
};

struct UIContext {
    SystemManager* controller;
    const NodeState* state;
    ProtocolStats* stats;
    uint32_t nowMs;
};

class MenuLibrary;

class IUiPage {
public:
    virtual ~IUiPage() {}

    virtual UiPageId id() const = 0;
    virtual UiPageId parent() const = 0;
    virtual const char* navLabel() const = 0;

    virtual bool showInParentMenu() const { return true; }
    virtual bool isEnabled(const UIContext& ctx) const {
        (void)ctx;
        return true;
    }

    virtual void onEnter(MenuLibrary& nav, UIContext& ctx) {
        (void)nav;
        (void)ctx;
    }

    virtual bool onLeft(MenuLibrary& nav, UIContext& ctx) {
        (void)nav;
        (void)ctx;
        return false;
    }

    virtual bool onRight(MenuLibrary& nav, UIContext& ctx) {
        (void)nav;
        (void)ctx;
        return false;
    }

    virtual bool onCenter(MenuLibrary& nav, UIContext& ctx) {
        (void)nav;
        (void)ctx;
        return false;
    }

    virtual bool allowHoldRepeat(const UIContext& ctx) const {
        (void)ctx;
        return false;
    }

    virtual void render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) = 0;
};

class MenuPage : public IUiPage {
public:
    enum class ItemType : uint8_t {
        TargetPage = 0,
        TargetMenu
    };

    struct MenuItem {
        ItemType type;
        const char* label;
        UiPageId targetId;
        bool enabled;
    };

    static const uint8_t MAX_ITEMS = 12;

    MenuPage(UiPageId pageId, UiPageId parentId, const char* title, const char* navLabel, bool includeBack = true);

    UiPageId id() const override;
    UiPageId parent() const override;
    const char* navLabel() const override;

    bool addPageItem(const char* label, UiPageId targetPageId);
    bool addSubMenuItem(const char* label, UiPageId subMenuId);
    bool setItemEnabled(uint8_t itemIndex, bool enabled);
    bool setItemEnabledByTarget(UiPageId targetId, bool enabled);
    bool isItemEnabled(uint8_t itemIndex) const;

    void setIncludeBack(bool enabled);
    void setBackLabel(const char* label);

    void setHeaderRenderer(void (*renderer)(U8G2_SSD1306_128X64_NONAME_F_HW_I2C*, const UIContext&), uint8_t listStartY);

    void onEnter(MenuLibrary& nav, UIContext& ctx) override;
    bool onLeft(MenuLibrary& nav, UIContext& ctx) override;
    bool onRight(MenuLibrary& nav, UIContext& ctx) override;
    bool onCenter(MenuLibrary& nav, UIContext& ctx) override;
    void render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) override;

private:
    static const uint8_t VISIBLE_ITEMS = 4;

    UiPageId _id;
    UiPageId _parent;
    const char* _title;
    const char* _navLabel;

    MenuItem _items[MAX_ITEMS];
    uint8_t _itemCount;

    bool _includeBack;
    const char* _backLabel;
    uint8_t _selected;

    void (*_headerRenderer)(U8G2_SSD1306_128X64_NONAME_F_HW_I2C*, const UIContext&);
    uint8_t _listStartY;

    uint8_t totalItemCount() const;
    bool isBackIndex(uint8_t index) const;
    const char* labelAt(uint8_t index) const;
    UiPageId targetAt(uint8_t index) const;
    bool isEnabledAt(uint8_t index) const;
    uint8_t computeScrollOffset() const;
};

class MenuLibrary {
public:
    static const uint8_t MAX_PAGES = 16;

    MenuLibrary();

    bool registerPage(IUiPage* page);
    void setRoot(UiPageId rootId, UIContext& ctx);
    void setDefaultPage(UiPageId defaultId);
    bool goDefault(UIContext& ctx);

    UiPageId currentPageId() const;
    IUiPage* currentPage() const;

    bool goTo(UiPageId targetId, UIContext& ctx);
    bool goToParent(UIContext& ctx);

    uint8_t childCount(UiPageId parentId, const UIContext& ctx) const;
    IUiPage* childAt(UiPageId parentId, uint8_t index, const UIContext& ctx) const;

    uint8_t selection(UiPageId parentId) const;
    void setSelection(UiPageId parentId, uint8_t index);
    bool moveSelection(UiPageId parentId, int8_t direction, const UIContext& ctx);

    bool openSelectedChild(UiPageId parentId, UIContext& ctx);
    bool openChildAt(UiPageId parentId, uint8_t index, UIContext& ctx);

private:
    IUiPage* _pages[MAX_PAGES];
    uint8_t _count;
    IUiPage* _byId[static_cast<uint8_t>(UiPageId::Count)];
    uint8_t _selectionByPage[static_cast<uint8_t>(UiPageId::Count)];
    UiPageId _currentId;
    UiPageId _defaultId;

    static uint8_t pageIndex(UiPageId id);
};

#endif