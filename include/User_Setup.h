/**
 * @file User_Setup.h
 * @brief TFT_eSPI User Setup for Monster S3 Firmware
 * 
 * =============================================================================
 * DISPLAY: MSP2402 - 2.4" ILI9341 TFT 240x320 with XPT2046 Touch
 * =============================================================================
 * Reference: https://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
 * 
 * MCU: ESP32-S3-DevKitC-1 N8R2 (8MB Flash, 2MB PSRAM)
 * 
 * This file is used by TFT_eSPI library. Most settings are also defined
 * in platformio.ini build_flags for consistency.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef _USER_SETUP_H_
#define _USER_SETUP_H_

// =============================================================================
// DRIVER SELECTION
// =============================================================================
// MSP2402 uses ILI9341 controller
#define ILI9341_2_DRIVER

// =============================================================================
// DISPLAY DIMENSIONS
// =============================================================================
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// =============================================================================
// ESP32-S3 SPI PIN DEFINITIONS
// =============================================================================
// Using FSPI (SPI3) on ESP32-S3

// Display SPI Pins
#define TFT_MOSI  13   // SPI Master Out Slave In
#define TFT_SCLK  14   // SPI Clock
#define TFT_MISO  -1   // Not used (display is write-only)

// Display Control Pins
#define TFT_CS    10   // Chip Select
#define TFT_DC    12   // Data/Command
#define TFT_RST   11   // Reset
#define TFT_BL    21   // Backlight (PWM controlled)

// =============================================================================
// TOUCH CONTROLLER - XPT2046
// =============================================================================
#define TOUCH_CS  15   // Touch Chip Select

// =============================================================================
// SPI FREQUENCIES
// =============================================================================
#define SPI_FREQUENCY        40000000   // 40MHz for display
#define SPI_READ_FREQUENCY   20000000   // 20MHz for reading
#define SPI_TOUCH_FREQUENCY   2500000   // 2.5MHz for touch

// =============================================================================
// COLOR AND DISPLAY SETTINGS
// =============================================================================
// Color order: RGB (normal) or BGR (swapped)
#define TFT_RGB_ORDER TFT_RGB

// Color inversion: OFF for MSP2402
#define TFT_INVERSION_OFF

// =============================================================================
// FONT LOADING
// =============================================================================
#define LOAD_GLCD   // Font 1 - 8x8 pixels
#define LOAD_FONT2  // Font 2 - 16x16 pixels
#define LOAD_FONT4  // Font 4 - 26x26 pixels
#define LOAD_FONT6  // Font 6 - 48x48 pixels (numbers only)
#define LOAD_FONT7  // Font 7 - 48x48 pixels (7-segment style)
#define LOAD_FONT8  // Font 8 - 75x75 pixels (numbers only)
#define LOAD_GFXFF  // FreeFont support
#define SMOOTH_FONT // Anti-aliased fonts

// =============================================================================
// ESP32-S3 SPECIFIC CONFIGURATION
// =============================================================================
// Use FSPI (SPI3) on ESP32-S3 for display
// HSPI (SPI2) is used for SD card and CC1101
#define USE_FSPI_PORT

// =============================================================================
// BACKLIGHT CONTROL
// =============================================================================
// Backlight is controlled via PWM on TFT_BL pin
// HIGH = backlight ON
#define TFT_BACKLIGHT_ON HIGH

// =============================================================================
// PIN SUMMARY FOR MSP2402 DISPLAY
// =============================================================================
/*
 * MSP2402 Pin | ESP32-S3 GPIO | Function
 * ------------|---------------|------------------
 * VCC         | 3.3V          | Power supply
 * GND         | GND           | Ground
 * CS          | GPIO 10       | Chip Select
 * RESET       | GPIO 11       | Reset
 * DC/RS       | GPIO 12       | Data/Command
 * SDI(MOSI)   | GPIO 13       | SPI MOSI
 * SCK         | GPIO 14       | SPI Clock
 * LED         | GPIO 21       | Backlight (via PWM)
 * SDO(MISO)   | -1            | Not connected
 * T_CLK       | GPIO 14       | Touch SPI Clock (shared)
 * T_CS        | GPIO 15       | Touch Chip Select
 * T_DIN       | GPIO 13       | Touch MOSI (shared)
 * T_DO        | GPIO 13       | Touch MISO (shared)
 * T_IRQ       | GPIO 16       | Touch Interrupt
 */

#endif // _USER_SETUP_H_
