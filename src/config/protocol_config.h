#pragma once

#include <Arduino.h>

const uint8_t PROTOCOL_START_BYTE = 0x7E;
const uint8_t MAX_PAYLOAD_SIZE = 64;
const uint32_t PROTOCOL_BAUD = 115200;
const uint16_t PROTOCOL_SERIAL_CONFIG = SERIAL_8N1;
const uint16_t PROTOCOL_PACKET_TIMEOUT_MS = 100;

#define RS485_SERIAL_PORT Serial1

const uint8_t TYPE_PING =         0x01;
const uint8_t TYPE_PONG =         0x02;
const uint8_t TYPE_STATUS_REQ =   0x10;
const uint8_t TYPE_STATUS_RESP =  0x11;
const uint8_t TYPE_ARM =          0x20;
const uint8_t TYPE_DISARM =       0x21;
const uint8_t TYPE_FIRE =         0x30;
const uint8_t TYPE_FIRE_ACK =     0x31;
const uint8_t TYPE_ERROR =        0xE0;
const uint8_t TYPE_HEARTBEAT =    0xF0;

const uint8_t CONTROLLER_ID = 0x00; // Constant address of Master Panel
extern uint8_t NODE_ID; // Initial startup ID for a node. Changed in UI.
const uint8_t BROADCAST_ID = 0xFF; // ID for broadcasted message
