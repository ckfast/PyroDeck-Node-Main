#ifndef FAULTS_H
#define FAULTS_H

#include <Arduino.h>

class FaultManager {
public:
    enum class FaultType : uint8_t {
        None = 0,
        ContinuityFault,
        FireFailure,
        WatchdogTriggered,
        SystemError
    };

    FaultManager();
    
    void begin();
    
    void recordFault(FaultType fault);
    
    FaultType getLastFault() const;
    
    bool hasFault() const;
    
    void clearFaults();
    
    void update();

private:
    FaultType _lastFault;
    uint32_t _faultTime;
    static const uint32_t FAULT_RESET_TIMEOUT_MS = 5000;
    
    void logFault(FaultType fault) const;
};

#endif // FAULTS_H
