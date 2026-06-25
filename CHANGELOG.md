# Changelog
## [0.1.1] - 2026-06-25

### Changed

- **`main.cpp`** — OLED/UI update is now rate-limited to `OLED_REFRESH_RATE` (100 ms). Previously the display was refreshed every loop iteration, blocking RS485 reception for 10–30 ms per cycle.
- **`protocol.cpp`** — `logPacket()` replaced `String` heap concatenation with a fixed `char[48]` buffer and `snprintf`, eliminating per-packet heap allocations on AVR.
- **`packet.cpp`** — Removed four blocking serial debug prints from the packet receive hot path (CRC value logging and "Got full message" trace), saving ~3.5 ms of serial TX delay per packet.
- **`system_manager.cpp`** — Removed redundant `_continuityController.update()` call in `requestFire()`; continuity is already updated by `SystemManager::update()` each loop.
- **`firing.cpp`** — Removed `snprintf` + serial print from the channel-stop transition inside `FiringController::update()`.
- **`rs485.cpp`** — Reduced default `busIdleWindowMs` from 2 ms to 1 ms, halving the minimum pre-transmission wait time.

## [0.1.0] - 2026-06-25

_First release._

[0.1.0]: https://github.com/ckfast/PyroDeck-Node-Main/releases/tag/v0.1.0