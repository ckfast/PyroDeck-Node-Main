#include "core/scheduler.h"
#include "core/system_manager.h"
#include "comms/protocol.h"
#include "utils/debug.h"
#include "safety/watchdog.h"
#include "safety/faults.h"

#include "config/settings.h"
#include "config/pins.h"

#include "ui/ui_handler.h"

//for testing
#include "firing/firing.h"

SystemManager controller;
Protocol protocol(NODE_ID, controller);
UIHandler ui;

uint8_t buffer[MAX_PAYLOAD_SIZE];

void setup() {
  // Initialize Serial for debugging

  // Status LED Setup
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);

  controller.begin();
  protocol.begin();

  DebugManager::begin();
  DebugManager::println("Node Started");
  
  // Initialize UI
  ui.begin(&controller);
  protocol.attachUI(&ui);
}

void loop() {

  protocol.update();

  controller.update();

  // Rate-limit UI refresh — OLED I2C renders take 10-30ms and must not
  // starve the RS485 receive loop between packets.
  static uint32_t lastUIUpdate = 0;
  const uint32_t now = millis();
  if ((uint32_t)(now - lastUIUpdate) >= OLED_REFRESH_RATE) {
    lastUIUpdate = now;
    NodeState state = controller.getStatus();
    ui.update(&state);
  }

}