#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include <Arduino.h>
#include <U8g2lib.h>

#include "oled.h"
#include "buttons.h"
#include "splash.h"
#include "core/node_state.h"
#include "core/system_manager.h"
#include "ui/menu/menu_library.h"
#include "ui/pages/status_page.h"
#include "ui/pages/arm_toggle_page.h"
#include "ui/pages/channel_detail_page.h"
#include "ui/pages/network_page.h"
#include "ui/pages/brightness_page.h"
#include "ui/pages/relay_time_page.h"
#include "ui/pages/display_sleep_page.h"

class UIHandler {
public:
    UIHandler();
    
    void begin(SystemManager* controller);
    
    void update(const NodeState* state);
    
    void setState(const NodeState* state);
    
    void recordPacketTx();
    
    void recordPacketRx();
    
    void setNodeAddress(uint8_t address);

private:
    OLEDDisplay _display;
    ButtonHandler _buttons;
    SplashScreen _splash;
    ProtocolStats _protocolStats;
    SystemManager* _controller;

    MenuLibrary _menu;
    StatusPage _statusPage;
    MenuPage _mainMenuPage;
    ArmTogglePage _armTogglePage;
    ChannelDetailPage _channelDetailPage;
    MenuPage _settingsMenuPage;
    NetworkPage _networkPage;
    BrightnessPage _brightnessPage;
    RelayTimePage _relayTimePage;
    DisplaySleepPage _displaySleepPage;
    
    uint32_t _lastLEDUpdate;
    uint32_t _lastUserActivity;
    uint32_t _leftHoldStart;
    uint32_t _rightHoldStart;
    uint32_t _leftRepeatAt;
    uint32_t _rightRepeatAt;
    uint8_t _currentLEDBrightness;
    bool _displayAsleep;

    void registerPages();
    void dispatchInput(bool leftClick, bool rightClick, bool centerClick, UIContext& ctx);
    void renderCurrentPage(const NodeState* state, const UIContext& ctx);
    
    void updateLEDBrightness(const NodeState* state);
};

#endif // UI_HANDLER_H
