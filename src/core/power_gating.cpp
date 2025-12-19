/**
 * @file power_gating.cpp
 * @brief MOSFET-based power gating implementation
 * 
 * Uses IRF520 N-channel MOSFET to control high-side switching
 * for power-hungry peripheral modules.
 * 
 * Hardware notes:
 * - MOSFET gate connects to MOSFET_PIN
 * - VCC of modules goes through MOSFET drain
 * - LOW = all modules off, HIGH = modules on (if enabled)
 */

#include "power_gating.h"
#include <globals.h>

#ifdef HAS_MOSFET

// Bitmask of currently enabled modules
static uint8_t moduleStates = 0;

// Estimated current draw per module (mA)
static const uint16_t MODULE_CURRENT[] = {
    50,   // MODULE_PAM8403 (idle)
    30,   // MODULE_CC1101
    15,   // MODULE_NRF24
    35,   // MODULE_GPS
    10,   // MODULE_PCM5102A (DAC)
    150,  // MODULE_SPEAKER (PAM8403 active)
    25    // MODULE_PN532 (NFC)
};

void power_gating_init() {
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);  // Start with all modules off
    moduleStates = 0;
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[POWER] Power gating initialized, all modules off");
    #endif
}

void power_module_enable(PoweredModule module) {
    if (module >= MODULE_COUNT) return;
    
    moduleStates |= (1 << module);
    
    // Turn on MOSFET if any module needs power
    if (moduleStates != 0) {
        digitalWrite(MOSFET_PIN, HIGH);
    }
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.printf("[POWER] Module %d enabled (mask: 0x%02X)\n", module, moduleStates);
    #endif
}

void power_module_disable(PoweredModule module) {
    if (module >= MODULE_COUNT) return;
    
    moduleStates &= ~(1 << module);
    
    // Turn off MOSFET only if no modules need power
    if (moduleStates == 0) {
        digitalWrite(MOSFET_PIN, LOW);
    }
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.printf("[POWER] Module %d disabled (mask: 0x%02X)\n", module, moduleStates);
    #endif
}

bool power_module_is_enabled(PoweredModule module) {
    if (module >= MODULE_COUNT) return false;
    return (moduleStates & (1 << module)) != 0;
}

void power_all_modules_off() {
    moduleStates = 0;
    digitalWrite(MOSFET_PIN, LOW);
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[POWER] All modules powered off");
    #endif
}

uint16_t power_get_estimated_draw() {
    uint16_t total = 0;
    
    for (int i = 0; i < MODULE_COUNT; i++) {
        if (moduleStates & (1 << i)) {
            total += MODULE_CURRENT[i];
        }
    }
    
    return total;
}

#else
// Stub implementations for devices without MOSFET

void power_gating_init() {}

void power_module_enable(PoweredModule module) {
    (void)module;
}

void power_module_disable(PoweredModule module) {
    (void)module;
}

bool power_module_is_enabled(PoweredModule module) {
    (void)module;
    return true;  // Assume always on
}

void power_all_modules_off() {}

uint16_t power_get_estimated_draw() {
    return 0;
}

#endif // HAS_MOSFET
