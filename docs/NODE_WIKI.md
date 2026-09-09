# PyroDeck Node Wiki

## Purpose
PyroDeck Node is an ATmega1284P-based distributed fireworks firing node that communicates with a master controller over RS485 and provides a local OLED/button interface.

## Repository Map
- `src/main.cpp` — startup and main control loop
- `src/core/` — node state and system orchestration
- `src/comms/` — packet format, parser, protocol handlers
- `src/firing/` — continuity sensing and channel firing control
- `src/safety/` — watchdog and fault lifecycle
- `src/ui/` — OLED rendering, buttons, menus, settings pages
- `src/hardware/` — EEPROM-backed persistent settings
- `src/config/` — global settings, pin assignments, protocol constants

## Hardware (from code + README)
### Confirmed components
- ATmega1284P MCU
- MAX485 (RS485 transceiver)
- SSD1306 OLED display (128x64 via U8g2)
- Custom boards listed in README:
  - MCU Board
  - Relay Board
  - UI Board
  - Power Board (WIP)

### Pin usage (`src/config/pins.h`)
- UI buttons:
  - Left: pin `18` (PC2)
  - Enter: pin `19` (PC3)
  - Right: pin `20` (PC4)
- UI PWM (button LED brightness): pin `15`
- Display I2C:
  - SCL: pin `22`
  - SDA: pin `23`
- RS485:
  - Driver Enable (`DE`): pin `22`
- Continuity inputs: `{24, 25, 26, 27, 28, 29, 30}`
- Firing outputs: `{12, 13, 14, 1, 3, 23, 21}`

> Current status: pin assignments in firmware overlap (`DISPLAY_SCL` with `RS485_DE_PIN` on `22`, and `DISPLAY_SDA` with firing channel 6 on `23`). In this repository, this is unresolved and should be treated as a hardware/firmware conflict until schematics or board-level mux/gating details confirm intended sharing.

## Firmware architecture
## 1) Startup and runtime loop
- `setup()`:
  - Initializes status LED
  - Boots `SystemManager`, `Protocol`, and UI
  - Attaches UI to protocol so packet TX/RX counters can be shown
- `loop()` order:
  1. `protocol.update()` (receive/process RS485 packets)
  2. `controller.update()` (safety, continuity, active firing updates)
  3. UI refresh every `OLED_REFRESH_RATE` (100 ms) to avoid starving comms

## 2) SystemManager (`src/core/system_manager.*`)
Owns and coordinates:
- Persistent settings (`EEPROMStore`)
- Arming/disarming state
- Continuity controller
- Firing controller
- Software watchdog
- Fault manager
- Online/heartbeat status

Main behavior:
- Loads settings from EEPROM at boot and sets global `NODE_ID`
- Arms/disarms node
- Accepts fire requests (with armed + channel validation)
- Tracks heartbeat; disarms if timeout (`HEARTBEAT_TIMEOUT_MS = 3000`)
- Updates continuity and firing masks exposed through `NodeState`

## 3) Persistent settings (`src/hardware/eeprom_store.*`)
Stored with:
- magic (`0x5044`)
- version (`1`)
- checksum

Persisted fields:
- `nodeId`
- `uiBrightness`
- `firingTime`
- `displaySleepTimeout`

Defaults:
- node ID: `1`
- brightness: `160`
- firing time: `1000 ms`
- display sleep: `30000 ms`

## 4) Communications protocol (`src/comms/*`, `src/config/protocol_config.h`)
### Frame format
`[start][dest][src][type][len][payload...][crc]`
- Start byte: `0x7E`
- Max payload: `64`
- Overhead: `6` bytes total framing
- CRC computed over bytes from `dest` through payload

### Addressing
- Controller (master) fixed address: `0x00`
- Broadcast: `0xFF`
- Node ID is mutable (`NODE_ID`) and persisted

### Packet types
- `PING (0x01)` / `PONG (0x02)`
- `STATUS_REQ (0x10)` / `STATUS_RESP (0x11)`
- `ARM (0x20)` / `DISARM (0x21)`
- `FIRE (0x30)` / `FIRE_ACK (0x31)`
- `ERROR (0xE0)`
- `HEARTBEAT (0xF0)`

### Status response payload (`len=13`)
1. nodeId
2. armed flag
3. online flag
4. UI brightness
5. last error (`uint8`, 1 byte)
6-9. continuity bitmask (little-endian uint32)
10-13. firing bitmask (little-endian uint32)

## 5) Firing + continuity subsystem (`src/firing/*`)
### Continuity
- Polls 7 continuity pins each update cycle
- A channel is continuous when pin reads `LOW` (`CONTINUITY_ACTIVE_STATE`)
- Exposes mask for UI and status packets

### Firing
- Supports 7 channels
- Fire command marks channel active with a timed duration
- `update()` deactivates channels when duration expires and writes output pins
- `abortFire()` immediately clears all channel active flags

## 6) Safety model (`src/safety/*`)
### Watchdog (software)
- Enabled at `2000 ms` timeout in `SystemManager::begin()`
- `SystemManager::update()` pets watchdog every cycle
- If timeout detected, triggers emergency stop + fault record

### Fault manager
- Records first non-`None` fault
- Auto-clears fault after `5000 ms`
- Fault types:
  - ContinuityFault
  - FireFailure
  - WatchdogTriggered
  - SystemError

### Heartbeat safety
- Master is marked online when heartbeat received
- If heartbeat lapses >3s:
  - Master set offline
  - Node disarmed
  - System fault recorded

## 7) UI/UX (`src/ui/*`)
### Input/Display
- 3-button navigation (left/right/center)
- OLED with top status header (armed state, fault, node ID, RX/TX or master offline)
- Splash screen on boot with version
- Display sleep timeout and wake-on-button behavior

### Pages
- Status page (3 view styles)
- Main menu
- Arm/Disarm action page
- Channel detail/fire page
- Settings menu:
  - Network (Node ID)
  - Brightness
  - Relay Time
  - Display Sleep

### UX details
- Hold-repeat acceleration for setting edits
- Fire menu is disabled unless node is armed
- Button backlight brightness reflects state (dim when disarmed, pulse when firing)

## Build and flash
Using PlatformIO (`platformio.ini`):
- Platform: `atmelavr`
- Board: `ATmega1284P`
- Framework: `arduino`
- CPU: `16 MHz`
- Upload protocol: `urclock` at `115200`
- Libraries:
  - `U8g2` (`olikraus/U8g2@^2.36.18`)

Build command:
```bash
pio run
```

## Operational flow summary
1. Node boots, loads persisted settings, initializes subsystems.
2. Master sends periodic heartbeat and commands.
3. Node answers status/ping requests and enforces address filtering.
4. Arming is required for firing commands to succeed.
5. UI continuously reflects continuity, firing, and comms state.
6. Loss of heartbeat or watchdog fault leads to safe disarm behavior.

## Open questions for hardware/software completion
1. Are shared pin assignments (`22` and `23`) intentional via external gating/mux, or are they unresolved conflicts?
2. What is the exact electrical design for continuity sensing (pull-ups/pull-downs, expected field wiring)?
3. Are firing outputs directly driving relays or controlling transistor/MOSFET stages on a relay board?
4. Should watchdog logic represent MCU-reset recovery state only, or also runtime loop-stall detection?
5. Is there an authoritative packet protocol spec for the master panel (field meanings, error-code contract, retries)?
6. Are there required production safety interlocks not yet represented in firmware (physical key switch, dead-man, dual-confirm fire)?
7. Should this wiki be split into multiple pages (Hardware, Protocol, UI, Safety) for GitHub Wiki publishing?
