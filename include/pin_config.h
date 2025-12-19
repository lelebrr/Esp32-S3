/**
 * @file pin_config.h
 * Monster S3 Firmware - Hardware Pin Definitions
 * ESP32-S3-WROOM-1 N8R8 + ILI9341 240x320 + Joystick Shield
 * 
 * IMPORTANT: Avoid GPIO 0 (BOOT), GPIO 45 (VSPI boot strap)
 * Updated: 2025-12-19
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ========================================
// DISPLAY - ILI9341 240x320 TFT SPI
// Reassigned to avoid joystick conflicts
// ========================================
#define PIN_TFT_CS      10     // Display Chip Select (was 5)
#define PIN_TFT_RST     11     // Display Reset (was 4)
#define PIN_TFT_DC      12     // Data/Command (was 6)
#define PIN_TFT_MOSI    13     // Display MOSI (SDI)
#define PIN_TFT_SCLK    14     // Display Clock (SCK)
#define PIN_TFT_MISO    -1     // Not used (set to -1)
#define PIN_TFT_BL      21     // Backlight LED (was 3)

// Screen Dimensions
#define TFT_WIDTH       240
#define TFT_HEIGHT      320

// ========================================
// TOUCH - XPT2046 (Resistive Touch)
// ========================================
#define PIN_TOUCH_CS    15     // Touch Chip Select (was 38)
#define PIN_TOUCH_IRQ   16     // Touch Interrupt (was 39)

// ========================================
// JOYSTICK SHIELD (Funduino V1.A)
// ========================================
// Analog Joystick
#define PIN_JOY_X       4      // ADC1_CH3 - Joystick X axis
#define PIN_JOY_Y       5      // ADC1_CH4 - Joystick Y axis
#define PIN_JOY_SW      6      // Joystick button (was GPIO0!)

// Directional Buttons
#define PIN_BTN_A       41     // Button A (Up)
#define PIN_BTN_B       42     // Button B (Right)
#define PIN_BTN_C       1      // Button C (Down) - was 14
#define PIN_BTN_D       2      // Button D (Left) - was 15

// Auxiliary Buttons
#define PIN_BTN_E       17     // Button E (Center-Left) - was 16
#define PIN_BTN_F       18     // Button F (Center-Right) - was 17

// ========================================
// SD CARD - Separate SPI Bus
// ========================================
#define PIN_SD_CS       38     // SD Chip Select
#define PIN_SD_MOSI     39     // SD MOSI
#define PIN_SD_MISO     40     // SD MISO  
#define PIN_SD_SCK      45     // SD Clock

// ========================================
// I2C BUS (PN532, Sensors)
// ========================================
#define PIN_I2C_SDA     8      // I2C Data
#define PIN_I2C_SCL     9      // I2C Clock

// ========================================
// NFC - PN532 (I2C Mode)
// ========================================
#define PIN_PN532_EN    7      // Power Enable (MOSFET Gate)
#define PIN_PN532_IRQ   -1     // Not used

// ========================================
// SUBGHZ - CC1101 (SPI) - Uses SD SPI bus
// ========================================
#define PIN_CC1101_CS   46     // CC1101 Chip Select
#define PIN_CC1101_GDO0 47     // CC1101 GDO0
#define PIN_CC1101_EN   48     // Power Enable

// ========================================
// I2S AUDIO OUTPUT
// ========================================
#define PIN_I2S_BCK     35     // I2S Bit Clock
#define PIN_I2S_WS      36     // I2S Word Select (LRCK)
#define PIN_I2S_DOUT    37     // I2S Data Out

// ========================================
// INFRARED
// ========================================
#define PIN_IR_TX       3      // IR Transmitter
#define PIN_IR_RX       -1     // Not used

// ========================================
// LEGACY BUTTON ALIASES (for compatibility)
// ========================================
#define PIN_BTN_FAULT   PIN_BTN_A   // Fault Trigger = Button A
#define PIN_BTN_BOOT    PIN_BTN_B   // Boot/User = Button B

// ========================================
// STATUS LEDs (Internal RGB or external)
// ========================================
#define PIN_LED_RED_ATTACK_HI   RGB_BUILTIN
#define PIN_LED_RED_ATTACK_LO   RGB_BUILTIN
#define PIN_LED_BLUE_AI         RGB_BUILTIN
#define PIN_LED_BLUE_SLEEP      RGB_BUILTIN

// ========================================
// FAULT INJECTION
// ========================================
#define PIN_FAULT_GATE  -1     // Not used in this config

#endif // PIN_CONFIG_H
