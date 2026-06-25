#include "debug.h"
#include <avr/io.h>

// -----------------------------
bool DebugManager::debugEnabled = false;

// must live in RAM, but survives reset
uint8_t DebugManager::cpmcusr __attribute__((section(".noinit")));

// -----------------------------
// EARLY BOOT CAPTURE (runs BEFORE main)
// -----------------------------
void capture_reset(void)
    __attribute__((naked, used, section(".init0")));

void capture_reset(void)
{
    __asm__ __volatile__(
        "mov %0, r2\n"
        : "=r"(DebugManager::cpmcusr)
    );
}

// -----------------------------
void DebugManager::begin()
{
    uint8_t flags = cpmcusr;

    if (flags & _BV(WDRF))
    {
        debugEnabled = true;

        DEBUG_SERIAL.begin(115200);
        DEBUG_SERIAL.println(F("=== DEBUG ENABLED (WDRF) ==="));
    }
    else
    {
        debugEnabled = false;
    }
}

// -----------------------------
bool DebugManager::enabled()
{
    return debugEnabled;
}