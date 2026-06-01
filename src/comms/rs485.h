#ifndef RS485_H
#define RS485_H

#include <Arduino.h>

class RS485Bus {
    public:

        RS485Bus(HardwareSerial& port, uint8_t dePin);

        void begin(uint32_t baud);

        void sendByte(uint8_t byte);

        void sendBuffer(const uint8_t* data, uint8_t length);

        bool available();

        uint8_t readByte();
    
    private:

        HardwareSerial* serial;

        uint8_t driverEnablePin;

        void setTransmitMode();

        void setReceiveMode();
};


#endif // RS485_H