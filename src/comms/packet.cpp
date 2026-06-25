#include "packet.h"
#include "utils/debug.h"


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

    {
        uint8_t crc = calculateCRC(&buffer[1], index - 1);
        buffer[index++] = crc;
    }

    return index;
}

bool parsePacket(
    const uint8_t* buffer,
    uint8_t size,
    Packet& pkt
) {
    if(size < 6) {
        DebugManager::println("Packet Parse Failed due to invalid size, overall message too short");
        return false;
    }            

    if(buffer[0] != PROTOCOL_START_BYTE) {
        DebugManager::println("Packet Parse Failed due to invalid start byte at parsePacket()");
        return false;
    }        

    uint8_t receivedCRC = 
        buffer[size - 1];  

    if(!verifyCRC(&buffer[1], size - 2, receivedCRC)) {
        DebugManager::println("Packet Parse Failed due incorrect CRC");
        return false;
    }

    pkt.dest = buffer[1];
    pkt.src = buffer[2];
    pkt.type = buffer[3];
    pkt.len = buffer[4];

    if (pkt.len > MAX_PAYLOAD_SIZE) {
        DebugManager::println("Packet Parse Failed due to payload length overflow");
        return false;
    }

    if(size != pkt.len + 6) {
        DebugManager::println("Packet Parse Failed due to invalid size, overall message too short");
        return false;
    }

    for(uint8_t i = 0; i < pkt.len; i++) {
        pkt.data[i] = buffer[5 + i];
    }

    return true;
}

packetParser::packetParser(RS485Bus& bus) : bus(bus) {
}

bool packetParser::poll() {

    unsigned long now = millis();

    if(index > 0 && now - timeoutCounter > PROTOCOL_PACKET_TIMEOUT_MS) {
        DebugManager::println("Packet parser timeout, resetting state");
        index = 0;
        expectedLength = 0;
    }

    while(bus.available()) {

        uint8_t newByte = bus.readByte();
        
        if(index == 0 && newByte != PROTOCOL_START_BYTE)
            continue;

        if(index == 0) {
            timeoutCounter = millis();
        }

        if(index >= sizeof(buffer) || expectedLength > sizeof(buffer)) {
            
            index = 0;
            expectedLength = 0;
            return false;
        }

        buffer[index++] = newByte;

        if(index > 4) {

            expectedLength = buffer[4] + PACKET_OVERHEAD_BYTES;

            if(index >= expectedLength) {

                bool result =
                    parsePacket(buffer,
                                expectedLength,
                                packet);

                index = 0;
                expectedLength = 0;

                return result;
            }
        }
    }
    return false;
}

Packet packetParser::getPacket() {

    return packet;
}

