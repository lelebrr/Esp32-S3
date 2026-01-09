/**
 * @file interface.h
 * @brief Hardware Interface Abstraction - MorphNode Firmware
 *
 * Provides stubs for interface functions.
 * The main firmware uses pin_config.h and lvgl_menu for UI.
 */

#pragma once
#include <Arduino.h>

// ============================================================================
// INTERFACE FUNCTION DECLARATIONS
// ============================================================================

/**
 * @brief Initial GPIO setup for the device
 */
void _setup_gpio();

/**
 * @brief Second stage GPIO setup
 */
void _post_setup_gpio();

/**
 * @brief Get battery percentage (0-100)
 */
int getBattery();

/**
 * @brief Set display brightness
 * @param brightval Brightness value (0-255)
 */
void _setBrightness(uint8_t brightval);

/**
 * @brief Handle input buttons/touch events
 */
void InputHandler(void);

/**
 * @brief Power off the device
 */
void powerOff();

/**
 * @brief Put device into deep sleep
 */
void goToDeepSleep();

/**
 * @brief Check if reboot is requested
 */
void checkReboot();

/**
 * @brief Check if device is charging
 */
bool isCharging();
