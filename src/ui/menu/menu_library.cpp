#include "menu_library.h"

namespace {
uint8_t maxU8(uint8_t a, uint8_t b) {
    return (a > b) ? a : b;
}

void drawDottedRect(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    if (!display || w < 2 || h < 2) {
        return;
    }

    for (uint8_t px = 0; px < w; px += 2) {
        display->drawPixel(static_cast<uint8_t>(x + px), y);
        display->drawPixel(static_cast<uint8_t>(x + px), static_cast<uint8_t>(y + h - 1));
    }

    for (uint8_t py = 0; py < h; py += 2) {
        display->drawPixel(x, static_cast<uint8_t>(y + py));
        display->drawPixel(static_cast<uint8_t>(x + w - 1), static_cast<uint8_t>(y + py));
    }
}
}

MenuPage::MenuPage(UiPageId pageId, UiPageId parentId, const char* title, const char* navLabel, bool includeBack)
    : _id(pageId),
      _parent(parentId),
      _title(title),
      _navLabel(navLabel),
      _itemCount(0),
      _includeBack(includeBack),
      _backLabel("Back"),
      _selected(0),
      _headerRenderer(nullptr),
      _listStartY(24)
{
}

UiPageId MenuPage::id() const {
    return _id;
}

UiPageId MenuPage::parent() const {
    return _parent;
}

const char* MenuPage::navLabel() const {
    return _navLabel;
}

bool MenuPage::addPageItem(const char* label, UiPageId targetPageId) {
    if (_itemCount >= MAX_ITEMS) {
        return false;
    }

    _items[_itemCount].type = ItemType::TargetPage;
    _items[_itemCount].label = label;
    _items[_itemCount].targetId = targetPageId;
    _items[_itemCount].enabled = true;
    _itemCount++;
    return true;
}

bool MenuPage::addSubMenuItem(const char* label, UiPageId subMenuId) {
    if (_itemCount >= MAX_ITEMS) {
        return false;
    }

    _items[_itemCount].type = ItemType::TargetMenu;
    _items[_itemCount].label = label;
    _items[_itemCount].targetId = subMenuId;
    _items[_itemCount].enabled = true;
    _itemCount++;
    return true;
}

bool MenuPage::setItemEnabled(uint8_t itemIndex, bool enabled) {
    if (itemIndex >= _itemCount) {
        return false;
    }

    _items[itemIndex].enabled = enabled;
    return true;
}

bool MenuPage::setItemEnabledByTarget(UiPageId targetId, bool enabled) {
    for (uint8_t i = 0; i < _itemCount; i++) {
        if (_items[i].targetId == targetId) {
            _items[i].enabled = enabled;
            return true;
        }
    }

    return false;
}

bool MenuPage::isItemEnabled(uint8_t itemIndex) const {
    if (itemIndex >= _itemCount) {
        return false;
    }

    return _items[itemIndex].enabled;
}

void MenuPage::setIncludeBack(bool enabled) {
    _includeBack = enabled;
}

void MenuPage::setBackLabel(const char* label) {
    _backLabel = label ? label : "Back";
}

void MenuPage::setHeaderRenderer(void (*renderer)(U8G2_SSD1306_128X64_NONAME_F_HW_I2C*, const UIContext&), uint8_t listStartY) {
    _headerRenderer = renderer;
    _listStartY = listStartY;
}

void MenuPage::onEnter(MenuLibrary& nav, UIContext& ctx) {
    (void)ctx;
    _selected = 0;
    const uint8_t total = totalItemCount();
    if (total == 0) {
        _selected = 0;
        return;
    }
    if (_selected >= total) {
        _selected = static_cast<uint8_t>(total - 1);
    }
}

bool MenuPage::onLeft(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;

    const uint8_t total = totalItemCount();
    if (total == 0) {
        return false;
    }

    _selected = (_selected == 0) ? static_cast<uint8_t>(total - 1) : static_cast<uint8_t>(_selected - 1);
    return true;
}

bool MenuPage::onRight(MenuLibrary& nav, UIContext& ctx) {
    (void)nav;
    (void)ctx;

    const uint8_t total = totalItemCount();
    if (total == 0) {
        return false;
    }

    _selected = static_cast<uint8_t>((_selected + 1) % total);
    return true;
}

bool MenuPage::onCenter(MenuLibrary& nav, UIContext& ctx) {
    const uint8_t total = totalItemCount();
    if (total == 0) {
        return false;
    }

    nav.setSelection(_id, _selected);

    if (isBackIndex(_selected)) {
        return nav.goToParent(ctx);
    }

    if (!isEnabledAt(_selected)) {
        return true;
    }

    return nav.goTo(targetAt(_selected), ctx);
}

void MenuPage::render(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const MenuLibrary& nav, const UIContext& ctx) {
    (void)nav;
    if (!display) {
        return;
    }

    display->clearBuffer();
    display->setFont(u8g2_font_5x7_tr);
    display->setDrawColor(1);

    if (_headerRenderer) {
        _headerRenderer(display, ctx);
    } else {
        display->drawStr(2, 10, _title ? _title : "Menu");
        display->drawHLine(0, 12, 128);
    }

    const uint8_t total = totalItemCount();
    const uint8_t scrollOffset = computeScrollOffset();

    for (uint8_t i = 0; i < VISIBLE_ITEMS; i++) {
        const uint8_t index = static_cast<uint8_t>(scrollOffset + i);
        if (index >= total) {
            break;
        }

        const uint8_t y = static_cast<uint8_t>(_listStartY + i * 10);
        const bool itemEnabled = isEnabledAt(index);
        if (index == _selected) {
            if (itemEnabled) {
                display->drawFrame(0, y - 8, 120, 10);
            } else {
                drawDottedRect(display, 0, static_cast<uint8_t>(y - 8), 120, 10);
            }
        }

        const char* label = labelAt(index);
        display->drawStr(2, y, label);
    }

    if (total > VISIBLE_ITEMS) {
        const uint8_t barX = 122;
        const uint8_t barY = static_cast<uint8_t>(_listStartY - 8);
        const uint8_t barH = static_cast<uint8_t>(VISIBLE_ITEMS * 10);

        display->drawFrame(barX, barY, 5, barH);

        const uint8_t thumbH = maxU8(6, static_cast<uint8_t>((barH * VISIBLE_ITEMS) / total));
        const uint8_t maxOffset = static_cast<uint8_t>(total - VISIBLE_ITEMS);
        const uint8_t travel = static_cast<uint8_t>(barH - thumbH - 2);
        const uint8_t thumbOffset = (maxOffset == 0)
            ? 0
            : static_cast<uint8_t>((static_cast<uint16_t>(scrollOffset) * travel) / maxOffset);

        display->drawBox(static_cast<uint8_t>(barX + 1), static_cast<uint8_t>(barY + 1 + thumbOffset), 3, thumbH);
    }

    display->sendBuffer();
}

uint8_t MenuPage::totalItemCount() const {
    return static_cast<uint8_t>(_itemCount + (_includeBack ? 1 : 0));
}

bool MenuPage::isBackIndex(uint8_t index) const {
    return _includeBack && index == _itemCount;
}

const char* MenuPage::labelAt(uint8_t index) const {
    if (isBackIndex(index)) {
        return _backLabel;
    }
    if (index < _itemCount && _items[index].label) {
        return _items[index].label;
    }
    return "";
}

UiPageId MenuPage::targetAt(uint8_t index) const {
    if (index < _itemCount) {
        return _items[index].targetId;
    }
    return UiPageId::None;
}

bool MenuPage::isEnabledAt(uint8_t index) const {
    if (isBackIndex(index)) {
        return true;
    }

    if (index < _itemCount) {
        return _items[index].enabled;
    }

    return false;
}

uint8_t MenuPage::computeScrollOffset() const {
    const uint8_t total = totalItemCount();
    if (total <= VISIBLE_ITEMS) {
        return 0;
    }
    if (_selected < VISIBLE_ITEMS) {
        return 0;
    }
    return static_cast<uint8_t>(_selected - (VISIBLE_ITEMS - 1));
}

MenuLibrary::MenuLibrary()
    : _count(0),
      _currentId(UiPageId::None),
      _defaultId(UiPageId::None)
{
    for (uint8_t i = 0; i < MAX_PAGES; i++) {
        _pages[i] = nullptr;
    }

    for (uint8_t i = 0; i < static_cast<uint8_t>(UiPageId::Count); i++) {
        _byId[i] = nullptr;
        _selectionByPage[i] = 0;
    }
}

bool MenuLibrary::registerPage(IUiPage* page) {
    if (!page || _count >= MAX_PAGES) {
        return false;
    }

    const uint8_t index = pageIndex(page->id());
    if (index >= static_cast<uint8_t>(UiPageId::Count) || _byId[index] != nullptr) {
        return false;
    }

    _pages[_count++] = page;
    _byId[index] = page;
    return true;
}

void MenuLibrary::setRoot(UiPageId rootId, UIContext& ctx) {
    _currentId = rootId;
    IUiPage* page = currentPage();
    if (page) {
        page->onEnter(*this, ctx);
    }
}

void MenuLibrary::setDefaultPage(UiPageId defaultId) {
    _defaultId = defaultId;
}

bool MenuLibrary::goDefault(UIContext& ctx) {
    if (_defaultId == UiPageId::None) {
        return false;
    }
    return goTo(_defaultId, ctx);
}

UiPageId MenuLibrary::currentPageId() const {
    return _currentId;
}

IUiPage* MenuLibrary::currentPage() const {
    if (_currentId == UiPageId::None) {
        return nullptr;
    }

    const uint8_t index = pageIndex(_currentId);
    if (index >= static_cast<uint8_t>(UiPageId::Count)) {
        return nullptr;
    }

    return _byId[index];
}

bool MenuLibrary::goTo(UiPageId targetId, UIContext& ctx) {
    if (targetId == UiPageId::None) {
        return false;
    }

    const uint8_t index = pageIndex(targetId);
    if (index >= static_cast<uint8_t>(UiPageId::Count) || _byId[index] == nullptr) {
        return false;
    }

    _currentId = targetId;
    _byId[index]->onEnter(*this, ctx);
    return true;
}

bool MenuLibrary::goToParent(UIContext& ctx) {
    IUiPage* page = currentPage();
    if (!page) {
        return false;
    }

    const UiPageId parentId = page->parent();
    if (parentId == UiPageId::None || parentId == page->id()) {
        return false;
    }

    return goTo(parentId, ctx);
}

uint8_t MenuLibrary::childCount(UiPageId parentId, const UIContext& ctx) const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < _count; i++) {
        IUiPage* page = _pages[i];
        if (!page) {
            continue;
        }
        if (page->parent() == parentId && page->showInParentMenu() && page->isEnabled(ctx)) {
            count++;
        }
    }
    return count;
}

IUiPage* MenuLibrary::childAt(UiPageId parentId, uint8_t index, const UIContext& ctx) const {
    uint8_t cursor = 0;
    for (uint8_t i = 0; i < _count; i++) {
        IUiPage* page = _pages[i];
        if (!page) {
            continue;
        }
        if (page->parent() != parentId || !page->showInParentMenu() || !page->isEnabled(ctx)) {
            continue;
        }

        if (cursor == index) {
            return page;
        }
        cursor++;
    }

    return nullptr;
}

uint8_t MenuLibrary::selection(UiPageId parentId) const {
    const uint8_t index = pageIndex(parentId);
    if (index >= static_cast<uint8_t>(UiPageId::Count)) {
        return 0;
    }
    return _selectionByPage[index];
}

void MenuLibrary::setSelection(UiPageId parentId, uint8_t index) {
    const uint8_t pageId = pageIndex(parentId);
    if (pageId >= static_cast<uint8_t>(UiPageId::Count)) {
        return;
    }

    _selectionByPage[pageId] = index;
}

bool MenuLibrary::moveSelection(UiPageId parentId, int8_t direction, const UIContext& ctx) {
    if (direction == 0) {
        return false;
    }

    const uint8_t pageId = pageIndex(parentId);
    if (pageId >= static_cast<uint8_t>(UiPageId::Count)) {
        return false;
    }

    const uint8_t count = childCount(parentId, ctx);
    if (count == 0) {
        _selectionByPage[pageId] = 0;
        return false;
    }

    uint8_t selected = _selectionByPage[pageId];
    if (selected >= count) {
        selected = 0;
    }

    if (direction > 0) {
        selected = static_cast<uint8_t>((selected + 1) % count);
    } else {
        selected = (selected == 0) ? static_cast<uint8_t>(count - 1) : static_cast<uint8_t>(selected - 1);
    }

    _selectionByPage[pageId] = selected;
    return true;
}

bool MenuLibrary::openSelectedChild(UiPageId parentId, UIContext& ctx) {
    return openChildAt(parentId, selection(parentId), ctx);
}

bool MenuLibrary::openChildAt(UiPageId parentId, uint8_t index, UIContext& ctx) {
    IUiPage* child = childAt(parentId, index, ctx);
    if (!child) {
        return false;
    }

    setSelection(parentId, index);
    return goTo(child->id(), ctx);
}

uint8_t MenuLibrary::pageIndex(UiPageId id) {
    return static_cast<uint8_t>(id);
}
