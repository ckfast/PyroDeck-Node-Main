#include "packet.h"
#include "crc.h"

uint8_t buildPacket(
    const Packet& pkt,
    uint8_t* buffer
) {
    
    uint8_t index = 0;

    buffer[index++] = PROTOCOL_START_BYTE;

    buffer[index++] = pkt.dest;
    buffer[index++] = pkt.src; 
    buffer[index++] = pkt.type;
    buffer[index++] = pkt.len;

    for(uint8_t i = 0; i < pkt.len; i++) {
        buffer[index++] = pkt.data[i];
    }

    buffer[index++] = calculateCRC(buffer, index);

    return index;
}

bool parsePacket(
    const uint8_t* buffer,
    uint8_t size,
    Packet& pkt
) {
    if(size < 5) 
        return false;

    if(buffer[0] != PROTOCOL_START_BYTE) 
        return false;

    uint8_t recievedCRC = 
        buffer[size - 1];  

    if(verifyCRC(&buffer[1], size - 2, recievedCRC))
        return false;

    pkt.dest = buffer[1];
    pkt.src = buffer[2];
    pkt.type = buffer[3];
    pkt.len = buffer[4];

    for(uint8_t i = 0; i < size; i++) {
        pkt.data[i] = buffer[5 + 1];
    }

    pkt.crc = buffer[size - 1];

    return true;
}