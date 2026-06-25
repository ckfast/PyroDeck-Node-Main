#pragma once

#define UI_LEFT_PIN 18   // PC2
#define UI_ENTER_PIN 19  // PC3
#define UI_RIGHT_PIN 20  // PC4

#define UI_PWM 15

#define DISPLAY_SCL 22
#define DISPLAY_SDA 23

//RS485
#define RS485_DE_PIN 22

//Firing Pins
const uint8_t CONTINUITY_PINS[] = {24, 25, 26, 27, 28, 29, 30};
const uint8_t FIRING_CHANNEL_PINS[] = {12, 13, 14, 1, 3, 23, 21};

// Continuity is considered present when digitalRead(pin) matches this value.
const uint8_t CONTINUITY_ACTIVE_STATE = LOW;
const uint8_t CONTINUITY_PIN_MODE = INPUT;

