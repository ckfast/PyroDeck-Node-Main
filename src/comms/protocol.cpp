#include "protocol.h"
#include "utils/debug.h"
#include "ui/ui_handler.h"

Protocol::Protocol(uint8_t address, SystemManager& controller) 
    : _bus(RS485_SERIAL_PORT, RS485_DE_PIN), 
    _parser(_bus), 
    address(address),
    _controller(controller),
    _ui(nullptr)
    
{
}

void Protocol::begin() {
    _bus.begin(PROTOCOL_BAUD, PROTOCOL_SERIAL_CONFIG);
}

void Protocol::attachUI(UIHandler* ui) {
    _ui = ui;
}

void Protocol::update() {
    address = currentAddress();

    if(_parser.poll()) {
        if (_ui) {
            _ui->recordPacketRx();
        }

        const Packet& packet = _parser.getPacket();
        logPacket(packet);

        //Check address
        if(packet.dest != address && packet.dest != BROADCAST_ID) {
            
            DebugManager::println("Discarding Message (Not addressed to this node)");
            return;
        }

        processPacket(packet);
    }    
}

void Protocol::processPacket(const Packet& pkt) {
    switch(pkt.type) {
        case TYPE_PING:
            handlePing(pkt);
            break;

        case TYPE_STATUS_REQ:
            handleStatusReq(pkt);
            break;

        case TYPE_ARM:
            handleArm(pkt);
            break;

        case TYPE_DISARM:
            handleDisarm(pkt);
            break;

        case TYPE_FIRE:
            handleFire(pkt);
            break;

        case TYPE_PONG:
            handlePong(pkt);
            break;

        case TYPE_STATUS_RESP:
            handleStatusResp(pkt);
            break;

        case TYPE_FIRE_ACK:
            handleFireAck(pkt);
            break;

        case TYPE_ERROR:
            handleErrorPacket(pkt);
            break;

        case TYPE_HEARTBEAT:
            handleHeartbeat(pkt);
            break;

        default:
            handleUnknown(pkt);
            break;
    }
}

void Protocol::logPacket(const Packet& packet) const {
    if (!DebugManager::enabled()) return;

    char buf[48];
    snprintf(buf, sizeof(buf), "Pkt dst=%02X src=%02X t=%02X len=%u",
             packet.dest, packet.src, packet.type, packet.len);
    DebugManager::print(buf);

    for (uint8_t i = 0; i < packet.len && i < 8; i++) {
        snprintf(buf, sizeof(buf), " %02X", packet.data[i]);
        DebugManager::print(buf);
    }
    DebugManager::println("");
}

void Protocol::handlePing(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Ping ===");
    sendPong();
}

void Protocol::handleStatusReq(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Status req ===");
    sendStatus();
}

void Protocol::handleArm(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Arm ===");
    _controller.arm();
    sendStatus();
}

void Protocol::handleDisarm(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Disarm ===");
    _controller.disarm();
    sendStatus();
}

void Protocol::handleFire(const Packet& packet) {
    DebugManager::println("=== Message type: Fire Cmd ===");

    if (packet.len == 0) {
        DebugManager::println("Malformed FIRE packet: empty payload");
        sendError(ErrorType::PacketMalformed);
        return;
    }

    uint8_t results[packet.len];

    for(uint8_t i = 0; i < packet.len; i++) {
        results[i] = static_cast<uint8_t>(_controller.requestFire(packet.data[i]));
    }

    sendFireAck(results, packet.len);
}

void Protocol::handlePong(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Pong ===");
}

void Protocol::handleStatusResp(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Status Resp ===");
}

void Protocol::handleFireAck(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Fire Ack ===");
}

void Protocol::handleErrorPacket(const Packet& packet) {
    DebugManager::println("=== Message type: Error ===");
    if (packet.len > 0) {
        DebugManager::print("Error code from peer: ");
        DebugManager::println(packet.data[0]);
    }
}

void Protocol::handleHeartbeat(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Heartbeat ===");
    _controller.noteHeartbeat();
}

void Protocol::handleUnknown(const Packet& packet) {
    (void)packet;
    DebugManager::println("=== Message type: Unknown ===");
    sendError(ErrorType::Unknown);
}

void Protocol::sendPong() {

    DebugManager::println("Sending Pong");

    Packet pkt;

    pkt.dest = CONTROLLER_ID;
    pkt.src = currentAddress();
    pkt.type = TYPE_PONG;
    pkt.len = 0;
    
    sendRawPacket(pkt);

}

void Protocol::sendStatus() {
    DebugManager::println("Sending Status Response");

    NodeState state = _controller.getStatus();

    Packet pkt;

    pkt.dest = CONTROLLER_ID;
    pkt.src = currentAddress();
    pkt.type = TYPE_STATUS_RESP;
    pkt.len = 13;
    pkt.data[0] = state.nodeId;
    pkt.data[1] = state.armed ? 1 : 0;
    pkt.data[2] = state.online ? 1 : 0;
    pkt.data[3] = _controller.getUiBrightness();
    pkt.data[4] = state.lastError;
    pkt.data[5] = static_cast<uint8_t>(state.continuityMask & 0xFF);
    pkt.data[6] = static_cast<uint8_t>((state.continuityMask >> 8) & 0xFF);
    pkt.data[7] = static_cast<uint8_t>((state.continuityMask >> 16) & 0xFF);
    pkt.data[8] = static_cast<uint8_t>((state.continuityMask >> 24) & 0xFF);
    pkt.data[9] = static_cast<uint8_t>(state.firingMask & 0xFF);
    pkt.data[10] = static_cast<uint8_t>((state.firingMask >> 8) & 0xFF);
    pkt.data[11] = static_cast<uint8_t>((state.firingMask >> 16) & 0xFF);
    pkt.data[12] = static_cast<uint8_t>((state.firingMask >> 24) & 0xFF);

    sendRawPacket(pkt);
}

void Protocol::sendFireAck(uint8_t* fireStatus, uint8_t count) {

    DebugManager::println("Sending FireAck");

    Packet pkt;

    pkt.dest = CONTROLLER_ID;
    pkt.src = currentAddress();
    pkt.type = TYPE_FIRE_ACK;
    pkt.len = count;
    memcpy(pkt.data, fireStatus, count);

    sendRawPacket(pkt);
}

void Protocol::sendError(ErrorType error) {

    DebugManager::println("Sending Error Packet");

    Packet pkt;

    pkt.dest = CONTROLLER_ID;
    pkt.src = currentAddress();
    pkt.type = TYPE_ERROR;
    pkt.len = 1;
    pkt.data[0] = static_cast<uint8_t>(error);

    sendRawPacket(pkt);
}

void Protocol::sendRawPacket(Packet& pkt) {
    uint8_t buffer[MAX_PAYLOAD_SIZE];
    uint8_t bufferLen = buildPacket(pkt, buffer);
    _bus.sendBuffer(buffer, bufferLen);

    if (_ui) {
        _ui->recordPacketTx();
    }
}

uint8_t Protocol::currentAddress() const {
    return NODE_ID;
}