/**
 * @file pin_config.h
 * @brief MorphNode Firmware - Hardware Pin Definitions
 *
 * =============================================================================
 * HARDWARE: ESP32-S3-WROOM-1 N16R8 (16MB Flash + 8MB PSRAM Octal)
 * =============================================================================
 * Reference:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html
 * Datasheet:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html
 *
 * Specifications:
 * - Controller: ESP32-S3-WROOM-1 N16R8
 * - CPU: Xtensa 32-Bit LX7 Dual Core @ 240MHz
 * - Flash: 16MB QIO (Quad)
 * - PSRAM: 8MB OPI (Octal)
 * - GPIOs: 36 available
 * - WiFi: 802.11 b/g/n 2.4GHz
 * - Bluetooth: 5.0 BLE + Mesh
 * - USB: Dual USB Type-C (COM + USB-OTG)
 * - Operating Voltage: 3.3V (5V via USB)
 *
 * =============================================================================
 * GPIO RESTRICTIONS (DO NOT USE)
 * =============================================================================
 * - GPIO 0   : Boot button (strapping pin)
 * - GPIO 3   : Strapping pin
 * - GPIO 19  : USB D- (Native USB)
 * - GPIO 20  : USB D+ (Native USB)
 * - GPIO 26-32: Used by SPI Flash
 * - GPIO 33-37: Used by Octal PSRAM (N16R8 - DO NOT USE)
 * - GPIO 45  : Strapping pin (VDD_SPI)
 * - GPIO 46  : Strapping pin (Boot mode)
 *
 * =============================================================================
 * AVAILABLE GPIOs FOR PROJECT
 * =============================================================================
 * Safe to use: 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
 *              21, 38, 39, 40, 41, 42, 43, 44, 47, 48
 *
 * Touch capable (T1-T14): GPIO 1-14
 * ADC1 channels: GPIO 1-10
 * ADC2 channels: GPIO 11-20
 *
 * =============================================================================
 * DISPLAY: MSP2402 - 2.4" ILI9341 240x320 SPI with XPT2046 Touch
 * =============================================================================
 * Reference: https://www.lcdwiki.com/2.4inch_SPI_Module_ILI9341_SKU:MSP2402
 *
 * @author MorphNode Team
 * @date 2025-12-23
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ============================================================================
// DISPLAY - MSP2402 ILI9341 240x320 TFT (SPI - FSPI)
// ============================================================================
// Uses FSPI bus for display + touch

#define PIN_TFT_CS 10   // Display Chip Select
#define PIN_TFT_RST 11  // Display Reset
#define PIN_TFT_DC 12   // Data/Command
#define PIN_TFT_MOSI 13 // SPI MOSI (to TFT SDI)
#define PIN_TFT_SCLK 14 // SPI Clock (to TFT SCK)
#define PIN_TFT_MISO -1 // Not used (display is write-only)
#define PIN_TFT_BL 21   // Backlight LED (PWM)

// ============================================================================
// TOUCH - XPT2046 Resistive Touch Controller
// ============================================================================
// Shares SPI bus with display (MOSI/SCK)

#define PIN_TOUCH_CS 15  // Touch Chip Select
#define PIN_TOUCH_IRQ 16 // Touch Interrupt (optional)

// Touch Calibration Values
#define TS_MINX 200
#define TS_MAXX 3700
#define TS_MINY 200
#define TS_MAXY 3800

// ============================================================================
// SD CARD - Dedicated SPI Bus (HSPI)
// ============================================================================
// Uses GPIOs compatible with JTAG pins (when JTAG not in use)

#define PIN_SD_CS 39   // SD Chip Select
#define PIN_SD_SCK 40  // SD Clock
#define PIN_SD_MOSI 41 // SD MOSI
#define PIN_SD_MISO 42 // SD MISO

// ============================================================================
// SUBGHZ - CC1101 (Shares SPI with SD)
// ============================================================================

#define PIN_CC1101_CS 38   // CC1101 Chip Select
#define PIN_CC1101_GDO0 47 // CC1101 GDO0 (Interrupt)
#define PIN_CC1101_EN 48   // Power Enable via MOSFET

// ============================================================================
// I2C BUS (PN532, DS3231, PAJ7620)
// ============================================================================

#define PIN_I2C_SDA 8 // I2C Data
#define PIN_I2C_SCL 9 // I2C Clock

// ============================================================================
// NFC - PN532 (I2C Mode)
// ============================================================================

#define PIN_PN532_EN 7   // Power Enable via MOSFET
#define PIN_PN532_IRQ -1 // Not used (polling mode)

// ============================================================================
// I2S AUDIO OUTPUT (PCM5102A DAC)
// ============================================================================

#define PIN_I2S_BCK 43 // I2S Bit Clock
#define PIN_I2S_WS 44  // I2S Word Select (LRCK)
#define PIN_I2S_DOUT 5 // I2S Data Out

// ============================================================================
// INFRARED - YS-IRTM Module (UART NEC)
// ============================================================================

#define PIN_YS_IR_TX 17 // ESP32 TX -> YS-IRTM RXD
#define PIN_YS_IR_RX 18 // ESP32 RX <- YS-IRTM TXD

// ============================================================================
// GPS - GY-NEO6MV2 (UART2)
// ============================================================================

#define PIN_GPS_TX 1 // ESP32 TX -> GPS RX
#define PIN_GPS_RX 2 // ESP32 RX <- GPS TX
#define GPS_BAUD 9600
#define GPS_ENABLED 1

// ============================================================================
// JOYSTICK / BUTTONS (ADC + Digital)
// ============================================================================

#define PIN_JOY_X 4 // ADC1_CH3 - Joystick X axis
#define PIN_JOY_Y 6 // ADC1_CH5 - Joystick Y axis

// Buttons (directly to GPIO, active LOW with internal pullup)
#define PIN_BTN_A 0     // Boot button (GPIO0) - Fault trigger
#define PIN_BTN_B -1    // Not available on N8R2
#define PIN_BTN_OK 0    // OK/Select = Boot button
#define PIN_BTN_BACK -1 // Not connected

// ============================================================================
// FAULT INJECTION - MOSFET IRF520
// ============================================================================

#define PIN_FAULT_GATE 45 // MOSFET IRF520 Gate - GPIO45 livre (strapping mas ok)

// ============================================================================
// TEMPERATURE SENSOR - DS18B20 (Overclock Protection)
// ============================================================================

#define PIN_DS18B20 3  // OneWire temperature sensor
#define TEMP_MAX_OC 70 // Max temp for overclock (fallback to 160MHz)

// ============================================================================
// PIEZO BUZZER
// ============================================================================

#define PIN_PIEZO 48 // Piezo buzzer (also RGB LED on DevKitC-1)

// ============================================================================
// ONBOARD RGB LED (WS2812 / Addressable)
// ============================================================================
// ESP32-S3-DevKitC-1 has RGB LED on GPIO48

#define PIN_RGB_LED 48 // Onboard RGB LED

// ============================================================================
// STATUS INDICATION (Using onboard RGB LED)
// ============================================================================

#define PIN_LED_RED_ATTACK_HI PIN_RGB_LED
#define PIN_LED_RED_ATTACK_LO PIN_RGB_LED
#define PIN_LED_BLUE_AI PIN_RGB_LED
#define PIN_LED_BLUE_SLEEP PIN_RGB_LED

// ============================================================================
// GESTURE SENSOR - PAJ7620U2 (I2C)
// ============================================================================

#define PIN_PAJ7620_INT -1    // Interrupt (-1 = polling mode)
#define PAJ7620_I2C_ADDR 0x73 // Default I2C address
#define PAJ7620_ENABLED 1

// ============================================================================
// RTC - DS3231 (I2C)
// ============================================================================

#define PIN_RTC_SQW -1          // SQW/INT pin (-1 = not used)
#define DS3231_I2C_ADDR 0x68    // RTC I2C address
#define DS3231_EEPROM_ADDR 0x57 // AT24C32 EEPROM (if present)
#define DS3231_ENABLED 1

// ============================================================================
// USB NATIVE (ESP32-S3 built-in)
// ============================================================================
// DO NOT USE - Reserved for USB functionality

#define PIN_USB_DN 19 // USB D- (Native)
#define PIN_USB_DP 20 // USB D+ (Native)

// ============================================================================
// LEGACY ALIASES (for backward compatibility)
// ============================================================================

#define PIN_BTN_FAULT PIN_BTN_A // Fault Trigger = Boot button
#define PIN_BTN_BOOT 0          // Boot button (GPIO0)

// ============================================================================
// MODULE ENABLE FLAGS
// ============================================================================

#define CC1101_ENABLED 1
#define AUDIO_ENABLED 1
#define MIC_ENABLED 0 // Not connected in this config
#define PIEZO_ENABLED 1

// ============================================================================
// PIN SUMMARY TABLE
// ============================================================================
/*
 * GPIO | Function          | Notes
 * -----|-------------------|----------------------------------
 *   0  | BOOT BUTTON       | Strapping pin - DO NOT USE
 *   1  | GPS TX            | UART to GPS module
 *   2  | GPS RX            | UART from GPS module
 *   3  | -                 | Strapping pin - AVOID
 *   4  | Joystick X        | ADC1_CH3
 *   5  | I2S DOUT          | Audio output
 *   6  | Joystick Y        | ADC1_CH5
 *   7  | PN532 Enable      | MOSFET gate
 *   8  | I2C SDA           | Shared I2C bus
 *   9  | I2C SCL           | Shared I2C bus
 *  10  | TFT CS            | Display chip select
 *  11  | TFT RST           | Display reset
 *  12  | TFT DC            | Display data/command
 *  13  | TFT MOSI          | SPI MOSI (shared with touch)
 *  14  | TFT SCLK          | SPI clock (shared with touch)
 *  15  | Touch CS          | XPT2046 chip select
 *  16  | Touch IRQ         | XPT2046 interrupt
 *  17  | IR TX             | YS-IRTM module
 *  18  | IR RX             | YS-IRTM module
 *  19  | USB D-            | RESERVED - Native USB
 *  20  | USB D+            | RESERVED - Native USB
 *  21  | TFT Backlight     | PWM controlled
 *  38  | CC1101 CS         | RF module chip select
 *  39  | SD CS             | SD card chip select (JTAG)
 *  40  | SD SCK            | SD card clock (JTAG)
 *  41  | SD MOSI           | SD card data out (JTAG)
 *  42  | SD MISO           | SD card data in (JTAG)
 *  43  | I2S BCK           | Audio bit clock
 *  44  | I2S WS            | Audio word select
 *  45  | -                 | Strapping - DO NOT USE
 *  46  | -                 | Strapping - DO NOT USE
 *  47  | CC1101 GDO0       | RF interrupt
 *  48  | RGB LED / Piezo   | Onboard LED + buzzer
 * -----|-------------------|----------------------------------
 */

#endif // PIN_CONFIG_H
