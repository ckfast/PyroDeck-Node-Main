#include "crc.h"

uint8_t calculateCRC(
    const uint8_t* data,
    uint8_t length
 ) {

    uint8_t crc = 0x00;

    for(uint8_t i = 0; i < length; i++) {
        crc ^= data[i];

        for(uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    return crc;
 }

 bool verifyCRC(
    const uint8_t* data,
    uint8_t length,
    uint8_t crc
) {

    return calculateCRC(data, length) == crc;
}