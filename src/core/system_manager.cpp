#include "system_manager.h"
#include "utils/debug.h"
#include "config/protocol_config.h"

SystemManager::SystemManager() 
{
}

void SystemManager::begin() {
    _settings = EEPROMStore::loadSettings();
    NODE_ID = _settings.nodeId;
    _online = false;
    _lastHeartbeatAt = 0;
    _lastError = 0;
    _continuityController.begin();
    _continuityMask = _continuityController.getContinuityMask();
    _watchdog.begin();
    _watchdog.enable(2000);
    _faultManager.begin();
}

bool SystemManager::arm() {

    if(_armed)
        return false;

    _armed = true;
    _lastError = 0;
    DebugManager::println("=== SYSTEM ARMED ===");
    return true;
}

bool SystemManager::disarm() {

    if(!_armed)
        return false;

    _armed = false;
    _firingController.abortFire();
    _lastError = 0;
    DebugManager::println("=== SYSTEM DISARMED ===");
    return true;
}

SystemManager::FireResult SystemManager::requestFire(uint8_t channel) {
    _continuityController.update();
    _continuityMask = _continuityController.getContinuityMask();

    if (channel == 0 || channel > sizeof(FIRING_CHANNEL_PINS)) {
        DebugManager::println("ERROR: INVALID CHANNEL");
        _lastError = static_cast<uint8_t>(FireResult::InvalidChannel);
        return FireResult::InvalidChannel;
    }

    if(_armed) {
        if(_firingController.fireChannel(channel, _settings.firingTime)) {
            _lastError = static_cast<uint8_t>(FireResult::Success);
            return FireResult::Success;
        } else {
            _lastError = static_cast<uint8_t>(FireResult::InvalidChannel);
            return FireResult::InvalidChannel;
        }
    } else {
        DebugManager::println("ERROR: SYSTEM NOT ARMED");
        _lastError = static_cast<uint8_t>(FireResult::NotArmed);
        return FireResult::NotArmed;
    }
}

void SystemManager::noteHeartbeat() {
    _lastHeartbeatAt = millis();
    _online = true;
}

uint8_t SystemManager::getNodeId() const {
    return _settings.nodeId;
}

void SystemManager::setNodeId(uint8_t nodeId) {
    if (nodeId < MIN_NODE_ID || nodeId > MAX_NODE_ID) {
        return;
    }

    _settings.nodeId = nodeId;
    NODE_ID = nodeId;
    persistSettings();
}

uint8_t SystemManager::getUiBrightness() const {
    return _settings.uiBrightness;
}

void SystemManager::setUiBrightness(uint8_t brightness) {
    _settings.uiBrightness = brightness;
    persistSettings();
}

uint16_t SystemManager::getFiringTime() const {
    return _settings.firingTime;
}

void SystemManager::setFiringTime(uint16_t firingTimeMs) {
    _settings.firingTime = firingTimeMs;
    persistSettings();
}

uint16_t SystemManager::getDisplaySleepTimeout() const {
    return _settings.displaySleepTimeout;
}

void SystemManager::setDisplaySleepTimeout(uint16_t timeoutMs) {
    _settings.displaySleepTimeout = timeoutMs;
    persistSettings();
}

NodeState SystemManager::getStatus() {

    NodeState status;

    status.nodeId = _settings.nodeId;
    status.armed = _armed;
    status.online = _online;
    status.hasFault = _faultManager.hasFault();
    status.continuityMask = _continuityMask;
    status.firingMask = _firingController.getFiringMask();
    status.lastError = _lastError;

    return status;
}

void SystemManager::emergencyStop() {
    DebugManager::println("=== EMERGENCY STOP ACTIVATED ===");
    _armed = false;
    _firingController.abortFire();
    _faultManager.recordFault(FaultManager::FaultType::SystemError);
}

void SystemManager::update() {
    _watchdog.pet();

    if (_watchdog.isActive()) {
        emergencyStop();
        _faultManager.recordFault(FaultManager::FaultType::WatchdogTriggered);
    }

    if (_online && (millis() - _lastHeartbeatAt) > HEARTBEAT_TIMEOUT_MS) {
        _online = false;
    }

    _continuityController.update();
    _continuityMask = _continuityController.getContinuityMask();
    _firingController.update();
    _faultManager.update();
}

void SystemManager::persistSettings() {
    EEPROMStore::saveSettings(_settings);
}