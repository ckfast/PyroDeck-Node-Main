#include "faults.h"
#include "utils/debug.h"

FaultManager::FaultManager()
    : _lastFault(FaultType::None), _faultTime(0)
{
}

void FaultManager::begin() {
    _lastFault = FaultType::None;
    _faultTime = 0;
    DebugManager::println("Fault manager initialized");
}

void FaultManager::recordFault(FaultType fault) {
    if (fault != FaultType::None && _lastFault == FaultType::None) {
        _lastFault = fault;
        _faultTime = millis();
        logFault(fault);
    }
}

FaultManager::FaultType FaultManager::getLastFault() const {
    return _lastFault;
}

bool FaultManager::hasFault() const {
    return _lastFault != FaultType::None;
}

void FaultManager::clearFaults() {
    if (_lastFault != FaultType::None) {
        DebugManager::println("Faults cleared");
    }
    _lastFault = FaultType::None;
    _faultTime = 0;
}

void FaultManager::update() {
    if (_lastFault != FaultType::None) {
        uint32_t elapsed = millis() - _faultTime;
        if (elapsed > FAULT_RESET_TIMEOUT_MS) {
            clearFaults();
        }
    }
}

void FaultManager::logFault(FaultType fault) const {
    DebugManager::print("FAULT RECORDED: ");
    
    switch (fault) {
        case FaultType::ContinuityFault:
            DebugManager::println("Continuity fault detected");
            break;
        case FaultType::FireFailure:
            DebugManager::println("Fire failure detected");
            break;
        case FaultType::WatchdogTriggered:
            DebugManager::println("Watchdog timeout triggered");
            break;
        case FaultType::SystemError:
            DebugManager::println("System error occurred");
            break;
        default:
            DebugManager::println("Unknown fault");
            break;
    }
}
