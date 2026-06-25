#pragma once

#include <Arduino.h>

extern const char* NODE_FIRMWARE_VERSION;

const uint16_t HEARTBEAT_INTERVAL = 1000;
const uint32_t HEARTBEAT_TIMEOUT_MS = 3000;

const uint8_t OLED_REFRESH_RATE = 100;

const uint8_t DEFAULT_NODE_ID = 0x01;
const uint8_t DEFAULT_UI_BRIGHTNESS = 160;
const uint8_t MIN_NODE_ID = 1;
const uint8_t MAX_NODE_ID = 254;
const uint32_t SETTINGS_SPLASH_DURATION_MS = 500;

#define DEBUG_SERIAL Serial

const unsigned long FIRING_TIME = 1000;



