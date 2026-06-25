#ifndef NODE_STATE_H
#define NODE_STATE_H

#include <Arduino.h>

struct NodeState {
    
    uint8_t nodeId;

    bool armed;

    bool online;

    bool hasFault;

    uint32_t continuityMask;

    uint32_t firingMask;  // Bitmask of currently firing channels

    uint8_t lastError;
};

#endif