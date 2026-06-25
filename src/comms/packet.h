#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <rs485.h>
#include <crc.h>

#include "config/settings.h"
#include "config/protocol_config.h"



struct Packet {

    uint8_t dest;
    uint8_t src;
    uint8_t type;

    uint8_t len;

    uint8_t data[MAX_PAYLOAD_SIZE];
};

const uint8_t PACKET_OVERHEAD_BYTES = 6; // start + dest + src + type + len + crc
const uint8_t MAX_PACKET_SIZE = MAX_PAYLOAD_SIZE + PACKET_OVERHEAD_BYTES;

uint8_t buildPacket(
    const Packet& pkt,
    uint8_t* buffer
);

bool parsePacket(
    const uint8_t* buffer,
    uint8_t size,
    Packet& pkt
);

class packetParser {

    public:

        packetParser(RS485Bus& bus);

        bool poll();

        Packet getPacket();

    private:

        RS485Bus& bus;

        uint8_t index = 0;

        uint8_t buffer[MAX_PACKET_SIZE];

        uint8_t expectedLength = 0;

        Packet packet;

        unsigned long timeoutCounter = 0;
};


#endif //PACKET_H