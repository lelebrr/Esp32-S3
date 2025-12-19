#ifndef POWER_GATING_H
#define POWER_GATING_H

/**
 * @file power_gating.h
 * @brief MOSFET-based power gating for high-power modules
 * 
 * Uses IRF520 MOSFET to cut power to inactive modules:
 * - PAM8403 audio amplifier
 * - CC1101 SubGHz RF
 * - NRF24L01 2.4GHz RF
 * - GPS NEO-6M
 * 
 * This significantly reduces idle power consumption.
 */

#include <Arduino.h>

/**
 * @brief Modules that can be power-gated
 */
enum PoweredModule {
    MODULE_PAM8403 = 0,  ///< Audio amplifier (speaker)
    MODULE_CC1101,       ///< SubGHz 433MHz RF
    MODULE_NRF24,        ///< 2.4GHz RF
    MODULE_GPS,          ///< NEO-6M GPS
    MODULE_PCM5102A,     ///< I2S DAC
    MODULE_SPEAKER,      ///< Speaker via PAM8403
    MODULE_PN532,        ///< NFC module
    MODULE_COUNT
};

/**
 * @brief Initialize power gating (MOSFET off by default)
 */
void power_gating_init();

/**
 * @brief Enable power to a specific module
 * @param module Module to power on
 */
void power_module_enable(PoweredModule module);

/**
 * @brief Disable power to a specific module
 * @param module Module to power off
 */
void power_module_disable(PoweredModule module);

/**
 * @brief Check if a module is currently powered
 * @param module Module to check
 * @return true if module is powered
 */
bool power_module_is_enabled(PoweredModule module);

/**
 * @brief Cut power to all gated modules
 */
void power_all_modules_off();

/**
 * @brief Get current power consumption estimate
 * @return Estimated current draw in mA
 */
uint16_t power_get_estimated_draw();

#endif // POWER_GATING_H
