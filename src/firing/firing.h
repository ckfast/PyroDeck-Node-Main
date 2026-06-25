#ifndef FIRING_H
#define FIRING_H

#include <Arduino.h>

#include "config/settings.h"
#include "utils/debug.h"
#include "config/pins.h"

class FiringController {

    public:

        FiringController();

        bool fireChannel(uint8_t channel, uint16_t durationMs = 1000);

        void fireAllChannels(uint16_t durationMs = 1000);

        void abortFire();

        void update();

        // Get bitmask of currently firing channels
        uint32_t getFiringMask() const;

    private:
        struct FiringState {
                    bool fired;
                    unsigned long fireStartTime;
                    bool pinState;
                    uint16_t duration;
        };

        FiringState states[sizeof(FIRING_CHANNEL_PINS)];
        uint16_t _defaultDuration;

        
};

#endif //FIRING_H