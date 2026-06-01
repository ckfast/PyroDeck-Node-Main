#include "rs485.h"

RS485Bus::RS485Bus(HardwareSerial& port, uint8_t dePin) {

    serial = &port;

    driverEnablePin = dePin;
}

void RS485Bus::begin(uint32_t baud) {

    serial->begin(baud);

    pinMode(driverEnablePin, OUTPUT);

    setReceiveMode();
}

void RS485Bus::sendByte(uint8_t byte) {

    setTransmitMode();

    serial->write(byte);

    serial->flush();

    setReceiveMode();
}

void RS485Bus::sendBuffer(const uint8_t* data, uint8_t length) {

    setTransmitMode();

    serial->write(data, length);

    serial->flush();

    setReceiveMode();
}

bool RS485Bus::available() {

    return serial->available() > 0;
}

uint8_t RS485Bus::readByte() {

    return serial->read();
}

void RS485Bus::setTransmitMode() {
    
    digitalWrite(driverEnablePin, HIGH);
}

void RS485Bus::setReceiveMode() {

    digitalWrite(driverEnablePin, LOW);
}