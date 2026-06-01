#include "core/scheduler.h"
#include "comms/protocol.h"
//#include "display/oled.h"
//#include "firing/continuity.h"
//#include "safety/watchdog.h"

#include "config/settings.h"
#include "config/pins.h"

//for testing
#include "comms/rs485.h"
#include "comms/packet.h"


RS485Bus bus(RS485_SERIAL_PORT, RS485_DE_PIN);

uint8_t buffer[MAX_PAYLOAD_SIZE + 5];

void setup() {

  bus.begin(PROTOCOL_BAUD);

  Serial.begin(115200);

  Packet pkt;
  pkt.dest = 0x00;
  pkt.src = 2;
  pkt.type = 4;
  pkt.len=5;

  for(int i = 0; i < 5; i++) {
    pkt.data[i] = i;
  }

  buildPacket(pkt, buffer);

  Packet decode;

  Serial.println(parsePacket(buffer, 10, decode));

  Serial.println("Parsed as: ");
  Serial.println(decode.dest);
  Serial.println(decode.src);
  Serial.println(decode.type);
  Serial.println(decode.len);

  for(int i = 0; i < decode.len; i++) {
    Serial.println(pkt.data[i]);
  }

  Serial.println();
    
}

void loop() {

    
}