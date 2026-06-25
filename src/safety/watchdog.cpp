#include "watchdog.h"
#include "utils/debug.h"

WatchdogTimer::WatchdogTimer()
    : _timeoutMs(DEFAULT_TIMEOUT_MS), _lastPetTime(0), _isEnabled(false)
{
}

void WatchdogTimer::begin() {
    _lastPetTime = millis();
    _isEnabled = false;
    DebugManager::println("Watchdog initialized");
}

void WatchdogTimer::pet() {
    if (_isEnabled) {
        _lastPetTime = millis();
    }
}

bool WatchdogTimer::isActive() const {
    if (!_isEnabled) {
        return false;
    }

    uint32_t elapsed = millis() - _lastPetTime;
    if (elapsed > _timeoutMs) {
        DebugManager::println("WATCHDOG TIMEOUT TRIGGERED");
        return true;
    }

    return false;
}

void WatchdogTimer::enable(uint16_t timeoutMs) {
    _timeoutMs = timeoutMs;
    _lastPetTime = millis();
    _isEnabled = true;
    DebugManager::print("Watchdog enabled: ");
    DebugManager::print(_timeoutMs);
    DebugManager::println("ms timeout");
}

void WatchdogTimer::disable() {
    _isEnabled = false;
    DebugManager::println("Watchdog disabled");
}
