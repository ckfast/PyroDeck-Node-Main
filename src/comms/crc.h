#ifndef CRC_H
#define CRC_H

#include <Arduino.h>

uint8_t calculateCRC(
    const uint8_t* data,
    uint8_t length
);

bool verifyCRC(
    const uint8_t* data,
    uint8_t length,
    uint8_t crc
);


#endif //CRC_H