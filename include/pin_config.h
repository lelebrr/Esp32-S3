/**
 * @file pin_config.h
 * Monster S3 Firmware - Hardware Pin Definitions
 * ESP32-S3-WROOM-1 N8R8 + ILI9341 240x320 + Joystick Shield + MicroSD
 *
 * ARCHITECTURE: Dual SPI Bus
 * - SPI A (FSPI): Display + Touch (Pins 10-16)
 * - SPI B (HSPI): SD Card + CC1101 RF (Pins 39-42)
 *
 * IMPORTANT: Avoid GPIO 0 (BOOT), GPIO 45 (VSPI boot strap)
 * Updated: 2025-12-19
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ========================================
// DISPLAY - ILI9341 240x320 TFT (SPI A - FSPI)
// ========================================
#define PIN_TFT_CS 10   // Display Chip Select
#define PIN_TFT_RST 11  // Display Reset
#define PIN_TFT_DC 12   // Data/Command
#define PIN_TFT_MOSI 13 // Display MOSI (SDI)
#define PIN_TFT_SCLK 14 // Display Clock (SCK)
#define PIN_TFT_MISO -1 // Not used (set to -1)
#define PIN_TFT_BL 21   // Backlight LED (PWM)

// Screen Dimensions
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// ========================================
// TOUCH - XPT2046 (Shares SPI A bus)
// ========================================
#define PIN_TOUCH_CS 15   // Touch Chip Select
#define PIN_TOUCH_IRQ -1  // Touch Interrupt (not used - polling mode)
#define PIN_TOUCH_DO 16   // Touch Data Out (T_DO) - MISO for touch

// ========================================
// JOYSTICK SHIELD (Funduino V1.A)
// ========================================
// Analog Joystick
#define PIN_JOY_X 4  // ADC1_CH3 - Joystick X axis
#define PIN_JOY_Y 5  // ADC1_CH4 - Joystick Y axis
#define PIN_JOY_SW 6 // Joystick button

// Directional Buttons
#define PIN_BTN_A 35 // Button A (Up) - MOVED from 41
#define PIN_BTN_B 36 // Button B (Right) - MOVED from 42
#define PIN_BTN_C 1  // Button C (Down)
#define PIN_BTN_D 2  // Button D (Left)

// Auxiliary Buttons (On GPIO 19/20 - clear of GPS UART2)
#define PIN_BTN_E 19 // Button E (Center-Left)
// #define PIN_BTN_F       20  // Button F (Center-Right) - REMOVED for Piezo
#define PIN_PIEZO 20 // Piezo Positive Pin

// ========================================
// SD CARD - Dedicated SPI Bus (SPI B - HSPI)
// Pins 39-42 are safe for O PI PSRAM and provide dedicated bus
// ========================================
#define PIN_SD_CS 39   // SD Chip Select
#define PIN_SD_SCK 40  // SD Clock
#define PIN_SD_MOSI 41 // SD MOSI (Data to SD)
#define PIN_SD_MISO 42 // SD MISO (Data from SD)

// ========================================
// SUBGHZ - CC1101 (SPI B - Shares with SD)
// ========================================
#define PIN_CC1101_CS 37   // CC1101 Chip Select (moved from 46 - strap pin)
#define PIN_CC1101_GDO0 47 // CC1101 GDO0 (Interrupt)
#define PIN_CC1101_EN 48   // Power Enable (MOSFET Gate)

// ========================================
// I2C BUS (PN532, DS3231, PAJ7620)
// ========================================
#define PIN_I2C_SDA 8 // I2C Data
#define PIN_I2C_SCL 9 // I2C Clock

// ========================================
// NFC - PN532 (I2C Mode)
// ========================================
#define PIN_PN532_EN 7   // Power Enable (MOSFET Gate)
#define PIN_PN532_IRQ -1 // Not used

// ========================================
// I2S AUDIO OUTPUT (PCM5102A DAC)
// ========================================
#define PIN_I2S_BCK 43  // I2S Bit Clock
#define PIN_I2S_WS 44   // I2S Word Select (LRCK)
#define PIN_I2S_DOUT 38 // I2S Data Out

// ========================================
// INFRARED - YS-IRTM Module (UART NEC)
// ========================================
#define PIN_YS_IR_TX 3  // ESP32 TX -> YS-IRTM RXD
#define PIN_YS_IR_RX 27 // ESP32 RX <- YS-IRTM TXD

// ========================================
// LEGACY BUTTON ALIASES (for compatibility)
// ========================================
#define PIN_BTN_FAULT PIN_BTN_A // Fault Trigger = Button A
#define PIN_BTN_BOOT PIN_BTN_B  // Boot/User = Button B

// ========================================
// STATUS LEDs (Internal RGB LED)
// ========================================
#define PIN_LED_RED_ATTACK_HI RGB_BUILTIN
#define PIN_LED_RED_ATTACK_LO RGB_BUILTIN
#define PIN_LED_BLUE_AI RGB_BUILTIN
#define PIN_LED_BLUE_SLEEP RGB_BUILTIN

// ========================================
// FAULT INJECTION (Optional)
// ========================================
#define PIN_FAULT_GATE -1 // Not used in this config

// ========================================
// GESTURE SENSOR - PAJ7620U2 (I2C Bus)
// ========================================
#define PIN_PAJ7620_INT -1    // Interrupt pin (-1 = polling mode)
#define PAJ7620_I2C_ADDR 0x73 // Default I2C address
#define PAJ7620_ENABLED 1     // Enable gesture sensor

// ========================================
// GPS - GY-NEO6MV2 (UART2)
// ========================================
#define PIN_GPS_RX 17 // ESP32 RX <- GPS TX (UART2)
#define PIN_GPS_TX 18 // ESP32 TX -> GPS RX (UART2)
#define GPS_BAUD 9600 // Default baud rate
#define GPS_ENABLED 1 // Enable GPS module

// ========================================
// RTC - DS3231 (I2C Bus)
// ========================================
#define PIN_RTC_SQW -1          // SQW/INT pin (-1 = not used)
#define DS3231_I2C_ADDR 0x68    // RTC I2C address
#define DS3231_EEPROM_ADDR 0x57 // AT24C32 EEPROM address (if present)
#define DS3231_ENABLED 1        // Enable RTC module

// ========================================
// MICROPHONE - INMP441 (I2S Input)
// ========================================
#define PIN_I2S_MIC_SCK 28 // I2S Bit Clock (BCLK)
#define PIN_I2S_MIC_WS 29  // I2S Word Select (LRCK)
#define PIN_I2S_MIC_SD 30  // I2S Serial Data (DIN)

#endif // PIN_CONFIG_H
