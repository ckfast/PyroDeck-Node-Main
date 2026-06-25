#include "rs485.h"

RS485Bus::RS485Bus(HardwareSerial& port, uint8_t dePin) {

    serial = &port;

    driverEnablePin = dePin;

    busClearTimeoutMs = 5;
    busIdleWindowMs = 1;
}

void RS485Bus::begin(uint32_t baud, uint16_t config) {

    serial->begin(baud, config);

    pinMode(driverEnablePin, OUTPUT);

    setReceiveMode();
}

void RS485Bus::sendByte(uint8_t byte) {

    waitForBusClear();

    setTransmitMode();

    serial->write(byte);

    serial->flush();

    setReceiveMode();
}

void RS485Bus::sendBuffer(const uint8_t* data, uint8_t length) {

    waitForBusClear();

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

void RS485Bus::setBusClearTiming(uint16_t timeoutMs, uint16_t idleWindowMs) {
    busClearTimeoutMs = timeoutMs;
    busIdleWindowMs = idleWindowMs;
}

bool RS485Bus::waitForBusClear() {
    const unsigned long startMs = millis();
    unsigned long lastChangeMs = startMs;
    int lastAvail = serial->available();

    while ((millis() - startMs) < busClearTimeoutMs) {
        int avail = serial->available();

        if (avail != lastAvail) {
            lastAvail = avail;
            lastChangeMs = millis();
        }

        if ((millis() - lastChangeMs) >= busIdleWindowMs) {
            return true;
        }

        delayMicroseconds(100);
    }

    return false;
}

void RS485Bus::setTransmitMode() {
    
    digitalWrite(driverEnablePin, HIGH);
}

void RS485Bus::setReceiveMode() {

    digitalWrite(driverEnablePin, LOW);
}