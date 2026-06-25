#ifndef SYSTEM_MANAGER
#define SYSTEM_MANAGER

#include <Arduino.h>

#include "config/settings.h"
#include "firing/continuity.h"
#include "firing/firing.h"
#include "hardware/eeprom_store.h"
#include "node_state.h"
#include "../safety/watchdog.h"
#include "../safety/faults.h"


class SystemManager {

    public:

        enum class FireResult : uint8_t
        {
            Success = 0,
            NotArmed,
            InvalidChannel,
            NoContinuity,
            EStopActive
        };

        SystemManager();

        void begin();

        bool arm();

        bool disarm();

        FireResult requestFire(uint8_t channel);

        void noteHeartbeat();

        uint8_t getNodeId() const;

        void setNodeId(uint8_t nodeId);

        uint8_t getUiBrightness() const;

        void setUiBrightness(uint8_t brightness);

        uint16_t getFiringTime() const;

        void setFiringTime(uint16_t firingTimeMs);

        uint16_t getDisplaySleepTimeout() const;

        void setDisplaySleepTimeout(uint16_t timeoutMs);

        NodeState getStatus();

        void emergencyStop();
        
        void update();

    private:

        PersistedSettings _settings = {DEFAULT_NODE_ID, DEFAULT_UI_BRIGHTNESS};

        bool _armed = false;
        bool _online = false;
        uint32_t _lastHeartbeatAt = 0;
        uint8_t _lastError = 0;
        uint32_t _continuityMask = 0;

        FiringController _firingController;
        ContinuityController _continuityController;
        WatchdogTimer _watchdog;
        FaultManager _faultManager;

        void persistSettings();
        
};

#endif //SYSTEM_MANAGER