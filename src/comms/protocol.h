#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include <rs485.h>

#include "core/system_manager.h"
#include "packet.h"

#include "config/protocol_config.h"
#include "config/pins.h"

class UIHandler;

class Protocol {

    public:

        enum class packetTypes : uint8_t{
            PING = 0x01, //Check if node alive
            PONG = 0x02, //Node Responce to PING
            STATUS_REQ = 0x10, //Request Status
            STATUS_RESP = 0x11, //Status Resp
            ARM = 0x20, //Arm Node
            DISARM = 0x21, //Disarm Node
            FIRE_CMD = 0x30, //Trigger output
            FIRE_ACK = 0x31, //Confirm output triggered
            ERROR = 0xE0, //Error packet
            HEARTBEAT = 0xF0 //Periodic alive message (from master)
        };

        enum class ErrorType : uint8_t
        {
            Unknown          = 0x00,
            NotArmed         = 0x01,
            PartialFireError = 0x02,
            InvalidChannel   = 0x03,
            NoContinuity     = 0x04,
            EStopActive      = 0x05,
            PacketMalformed  = 0x06
        };

        Protocol(uint8_t address, SystemManager& controller);

        void begin();

        void attachUI(UIHandler* ui);

        void update();

        void sendPong();

        void sendStatus();

        void sendFireAck(uint8_t* fireStatus, uint8_t count);

        void sendError(ErrorType error);

        void processPacket(const Packet& pkt);

    private:

        RS485Bus _bus;
        packetParser _parser;
        uint8_t address;

        SystemManager& _controller;
        UIHandler* _ui;

        void logPacket(const Packet& packet) const;
        void handlePing(const Packet& packet);
        void handleStatusReq(const Packet& packet);
        void handleArm(const Packet& packet);
        void handleDisarm(const Packet& packet);
        void handleFire(const Packet& packet);
        void handlePong(const Packet& packet);
        void handleStatusResp(const Packet& packet);
        void handleFireAck(const Packet& packet);
        void handleErrorPacket(const Packet& packet);
        void handleHeartbeat(const Packet& packet);
        void handleUnknown(const Packet& packet);

        void sendRawPacket(Packet& pkt);

        uint8_t currentAddress() const;

};

#endif //PROTOCOL_H