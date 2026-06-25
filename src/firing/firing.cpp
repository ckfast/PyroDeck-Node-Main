#include "firing.h"

FiringController::FiringController() {

    for(uint8_t i = 0; i < sizeof(FIRING_CHANNEL_PINS); i++) {

        pinMode(FIRING_CHANNEL_PINS[i], OUTPUT);
        states[i] = {false, 0, false};
    }
}

bool FiringController::fireChannel(uint8_t channel, uint16_t durationMs) {

    if(channel == 0 || channel > sizeof(FIRING_CHANNEL_PINS)) {
        DebugManager::print("Invalid channel number: ");
        DebugManager::println(channel);
        return false;
    }

    {
        char buffer[48];
        snprintf(buffer, sizeof(buffer), "Firing channel %u for %ums", channel, durationMs);
        DebugManager::println(buffer);
    }

    states[channel - 1] = {true, millis(), false, durationMs};

    return true;
}

void FiringController::fireAllChannels(uint16_t durationMs) {

    for(uint8_t i = 0; i < sizeof(FIRING_CHANNEL_PINS); i++) {

        states[i] = {true, millis(), false, durationMs};
    }

    DebugManager::println("Firing All Channels");
}

void FiringController::abortFire() {

    for(uint8_t i = 0; i < sizeof(FIRING_CHANNEL_PINS); i++) {

        states[i].fired = false;
    }

    DebugManager::println("Aborting Fire Command");
}

void FiringController::update() {

    unsigned long thisMillis = millis();

    for(uint8_t i = 0; i < sizeof(FIRING_CHANNEL_PINS); i++) {

        bool &channelFired = states[i].fired;
        unsigned long &fireStartTime = states[i].fireStartTime;
        bool &pinState = states[i].pinState;
        uint16_t &duration = states[i].duration;

        if(channelFired == true && thisMillis > fireStartTime + duration) {

            channelFired = false;
            {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "Channel %u set to OFF", i + 1);
                DebugManager::println(buffer);
            }
        }
        
        uint8_t pin = FIRING_CHANNEL_PINS[i];

        if(pinState != channelFired) {

            pinState = channelFired;
            digitalWrite(pin, pinState);
        }

    }
}

uint32_t FiringController::getFiringMask() const {
    uint32_t mask = 0;
    for(uint8_t i = 0; i < sizeof(FIRING_CHANNEL_PINS); i++) {
        if(states[i].fired) {
            mask |= (1 << i);
        }
    }
    return mask;
}