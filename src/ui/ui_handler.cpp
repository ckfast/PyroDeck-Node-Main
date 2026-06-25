#include "ui_handler.h"

#include "config/pins.h"
#include "config/settings.h"
#include "ui/pages/page_helpers.h"

extern uint8_t NODE_ID;

namespace {
void drawStandardHeader(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* display, const UIContext& ctx) {
    ui_page_helpers::drawHeader(display, ctx.state, ctx.stats);
}
}

UIHandler::UIHandler()
    : _controller(nullptr),
      _mainMenuPage(UiPageId::MainMenu, UiPageId::Status, "Main", "Menu", true),
      _settingsMenuPage(UiPageId::SettingsMenu, UiPageId::MainMenu, "Settings", "Settings", true),
      _lastLEDUpdate(0),
      _lastUserActivity(0),
    _leftHoldStart(0),
    _rightHoldStart(0),
    _leftRepeatAt(0),
    _rightRepeatAt(0),
      _currentLEDBrightness(128),
      _displayAsleep(false)
{
    _protocolStats.packetsRx = 0;
    _protocolStats.packetsTx = 0;
    _protocolStats.nodeAddress = NODE_ID;
}

void UIHandler::begin(SystemManager* controller) {
    _controller = controller;

    _display.begin();
    _buttons.begin();

    _splash.start();

    _protocolStats.packetsRx = 0;
    _protocolStats.packetsTx = 0;
    _protocolStats.nodeAddress = controller ? controller->getNodeId() : NODE_ID;

    _lastUserActivity = millis();
    _displayAsleep = false;
    _display.getDisplay()->setContrast(controller ? controller->getUiBrightness() : DEFAULT_UI_BRIGHTNESS);

    registerPages();

    UIContext ctx = {_controller, nullptr, &_protocolStats, millis()};
    _menu.setDefaultPage(UiPageId::Status);
    _menu.setRoot(UiPageId::Status, ctx);
    _menu.setSelection(UiPageId::Status, 0);
}

void UIHandler::update(const NodeState* state) {
    _buttons.update();

    const uint32_t now = millis();
    const uint16_t displaySleepTimeout = _controller ? _controller->getDisplaySleepTimeout() : 30000;

    const bool leftPressed = _buttons.isLeftPressed();
    const bool rightPressed = _buttons.isRightPressed();
    const bool centerPressed = _buttons.isCenterPressed();
    const bool anyPressed = leftPressed || rightPressed || centerPressed;

    if (anyPressed) {
        _lastUserActivity = now;
        if (_displayAsleep) {
            _displayAsleep = false;
            _display.getDisplay()->setPowerSave(0);
            _display.getDisplay()->setContrast(_controller ? _controller->getUiBrightness() : DEFAULT_UI_BRIGHTNESS);
            _leftHoldStart = 0;
            _rightHoldStart = 0;
            _leftRepeatAt = 0;
            _rightRepeatAt = 0;
            // Consume wake-up clicks so waking the display does not trigger UI actions.
            (void)_buttons.wasLeftClicked();
            (void)_buttons.wasRightClicked();
            (void)_buttons.wasCenterClicked();
            return;
        }
    }

    if (!_displayAsleep && _lastUserActivity > 0 && (now - _lastUserActivity) > displaySleepTimeout) {
        _displayAsleep = true;
        _display.getDisplay()->setPowerSave(1);
        _display.getDisplay()->setContrast(0);
        _leftHoldStart = 0;
        _rightHoldStart = 0;
        _leftRepeatAt = 0;
        _rightRepeatAt = 0;
        _currentLEDBrightness = 0;
        _buttons.setLEDBrightness(0);
        return;
    }

    const bool leftClick = _buttons.wasLeftClicked();
    const bool rightClick = _buttons.wasRightClicked();
    const bool centerClick = _buttons.wasCenterClicked();

    if (leftClick || rightClick || centerClick) {
        _lastUserActivity = now;
    }

    if (state) {
        _protocolStats.nodeAddress = state->nodeId;
    }

    if (_splash.isActive()) {
        _splash.draw(_display.getDisplay());
        _display.setState(state);
        return;
    }

    UIContext ctx = {_controller, state, &_protocolStats, now};
    _mainMenuPage.setItemEnabledByTarget(UiPageId::ChannelDetail, state && state->armed);

    bool repeatLeft = false;
    bool repeatRight = false;

    IUiPage* currentPage = _menu.currentPage();
    const bool allowRepeat = currentPage && currentPage->allowHoldRepeat(ctx);
    const uint32_t holdStartDelayMs = 350;

    if (leftClick) {
        _leftHoldStart = now;
        _leftRepeatAt = now + holdStartDelayMs;
    } else if (!leftPressed) {
        _leftHoldStart = 0;
        _leftRepeatAt = 0;
    }

    if (rightClick) {
        _rightHoldStart = now;
        _rightRepeatAt = now + holdStartDelayMs;
    } else if (!rightPressed) {
        _rightHoldStart = 0;
        _rightRepeatAt = 0;
    }

    if (allowRepeat) {
        if (leftPressed && !leftClick) {
            if (_leftHoldStart == 0) {
                _leftHoldStart = now;
                _leftRepeatAt = now + holdStartDelayMs;
            } else if (_leftRepeatAt != 0 && now >= _leftRepeatAt) {
                repeatLeft = true;

                const uint32_t holdDuration = now - _leftHoldStart;
                uint32_t intervalMs = 140;
                if (holdDuration > 4000) {
                    intervalMs = 10;
                } else if(holdDuration > 2500) {
                    intervalMs = 35;
                } else if (holdDuration > 1200) {
                    intervalMs = 70;
                }

                _leftRepeatAt = now + intervalMs;
            }
        }

        if (rightPressed && !rightClick) {
            if (_rightHoldStart == 0) {
                _rightHoldStart = now;
                _rightRepeatAt = now + holdStartDelayMs;
            } else if (_rightRepeatAt != 0 && now >= _rightRepeatAt) {
                repeatRight = true;

                const uint32_t holdDuration = now - _rightHoldStart;
                uint32_t intervalMs = 140;
                if (holdDuration > 4000) {
                    intervalMs = 10;
                } else if(holdDuration > 2500) {
                    intervalMs = 35;
                } else if (holdDuration > 1200) {
                    intervalMs = 70;
                }

                _rightRepeatAt = now + intervalMs;
            }
        }
    }

    dispatchInput(leftClick || repeatLeft, rightClick || repeatRight, centerClick, ctx);

    updateLEDBrightness(state);
    renderCurrentPage(state, ctx);
    _display.setState(state);
}

void UIHandler::setState(const NodeState* state) {
    _display.setState(state);
}

void UIHandler::recordPacketTx() {
    _protocolStats.packetsTx++;
    if (_protocolStats.packetsTx > 65535) {
        _protocolStats.packetsTx = 0;
    }
}

void UIHandler::recordPacketRx() {
    _protocolStats.packetsRx++;
    if (_protocolStats.packetsRx > 65535) {
        _protocolStats.packetsRx = 0;
    }
}

void UIHandler::setNodeAddress(uint8_t address) {
    NODE_ID = address;
    _protocolStats.nodeAddress = address;
}

void UIHandler::registerPages() {
    _mainMenuPage.setHeaderRenderer(drawStandardHeader, 24);
    _settingsMenuPage.setHeaderRenderer(drawStandardHeader, 24);

    _mainMenuPage.addPageItem("Fire", UiPageId::ChannelDetail);
    _mainMenuPage.addPageItem("Arm/Disarm", UiPageId::ArmToggle);
    _mainMenuPage.addSubMenuItem("Settings", UiPageId::SettingsMenu);

    _settingsMenuPage.addPageItem("Network", UiPageId::Network);
    _settingsMenuPage.addPageItem("Brightness", UiPageId::Brightness);
    _settingsMenuPage.addPageItem("Relay Time", UiPageId::RelayTime);
    _settingsMenuPage.addPageItem("Display Sleep", UiPageId::DisplaySleep);

    _menu.registerPage(&_statusPage);
    _menu.registerPage(&_mainMenuPage);
    _menu.registerPage(&_armTogglePage);
    _menu.registerPage(&_channelDetailPage);
    _menu.registerPage(&_settingsMenuPage);
    _menu.registerPage(&_networkPage);
    _menu.registerPage(&_brightnessPage);
    _menu.registerPage(&_relayTimePage);
    _menu.registerPage(&_displaySleepPage);
}

void UIHandler::dispatchInput(bool leftClick, bool rightClick, bool centerClick, UIContext& ctx) {
    IUiPage* page = _menu.currentPage();
    if (!page) {
        return;
    }

    if (leftClick) {
        page->onLeft(_menu, ctx);
        return;
    }

    if (rightClick) {
        page->onRight(_menu, ctx);
        return;
    }

    if (centerClick) {
        page->onCenter(_menu, ctx);
    }
}

void UIHandler::renderCurrentPage(const NodeState* state, const UIContext& ctx) {
    (void)state;
    IUiPage* page = _menu.currentPage();
    if (!page) {
        return;
    }

    page->render(_display.getDisplay(), _menu, ctx);
}

void UIHandler::updateLEDBrightness(const NodeState* state) {
    
    const uint32_t now = millis();
    if (now - _lastLEDUpdate < 100) {
        return;
    }
    _lastLEDUpdate = now;
    
    if(_displayAsleep) {
        _currentLEDBrightness = 0;
        _buttons.setLEDBrightness(0);
        return;
    }

    const uint8_t configuredBrightness = _controller ? _controller->getUiBrightness() : DEFAULT_UI_BRIGHTNESS;
    uint8_t targetBrightness = configuredBrightness;

    if (state) {
        if (!state->armed) {
            targetBrightness = static_cast<uint8_t>(configuredBrightness / 2);
        }

        if (state->firingMask != 0) {
            static uint8_t pulsePhase = 0;
            pulsePhase = (pulsePhase + 1) % 20;
            if (pulsePhase < 10) {
                targetBrightness = configuredBrightness;
            } else {
                targetBrightness = static_cast<uint8_t>(configuredBrightness / 3);
            }
        }
    }

    if (_currentLEDBrightness < targetBrightness) {
        _currentLEDBrightness = (_currentLEDBrightness + 5 > targetBrightness)
            ? targetBrightness
            : static_cast<uint8_t>(_currentLEDBrightness + 5);
    } else if (_currentLEDBrightness > targetBrightness) {
        _currentLEDBrightness = (_currentLEDBrightness - 5 < targetBrightness)
            ? targetBrightness
            : static_cast<uint8_t>(_currentLEDBrightness - 5);
    }

    _buttons.setLEDBrightness(_currentLEDBrightness);
}
