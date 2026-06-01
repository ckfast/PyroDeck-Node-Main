#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

#include "../config/protocol_config.h"

struct Packet {

    uint8_t dest;
    uint8_t src;
    uint8_t type;

    uint8_t len;

    uint8_t data[MAX_PAYLOAD_SIZE];

    uint8_t crc;
};

uint8_t buildPacket(
    const Packet& pkt,
    uint8_t* buffer
);

bool parsePacket(
    const uint8_t* buffer,
    uint8_t size,
    Packet& pkt
);



#endif //PACKET_H