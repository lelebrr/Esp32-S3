#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// ==============================================================================
// ESP32-S3 CYD N16R8 Pin Definitions
// Migrated from ESP32 WROOM-32 CYD-2USB with extended hardware
// PCM5102A DAC, DS3231 RTC, PAJ7620U2 Gesture, Multi-IR, DS18B20
// ==============================================================================
// NOTE: All pins use #ifndef guards to allow override via -D flags in platformio.ini

// Default UART (USB CDC)
#ifndef TX
static const uint8_t TX = 43;
#endif
#ifndef RX
static const uint8_t RX = 44;
#endif

// ==============================================================================
// I2C Bus (400kHz) - PN532 NFC, DS3231 RTC, PAJ7620U2 Gesture
// ==============================================================================
#ifndef SDA
static const uint8_t SDA = 1;
#endif
#ifndef SCL
static const uint8_t SCL = 2;
#endif

// I2C Device Addresses
#ifndef DS3231_ADDR
#define DS3231_ADDR    0x68
#endif
#ifndef PAJ7620_ADDR
#define PAJ7620_ADDR   0x73
#endif
#ifndef PN532_I2C_ADDR
#define PN532_I2C_ADDR 0x24
#endif

// ==============================================================================
// SPI2 - Display ILI9488 + Touch XPT2046
// ==============================================================================
#ifndef TFT_MOSI
static const uint8_t TFT_MOSI = 11;
#endif
#ifndef TFT_MISO
static const uint8_t TFT_MISO = 13;
#endif
#ifndef TFT_SCK
static const uint8_t TFT_SCK = 12;
#endif
#ifndef TFT_CS
static const uint8_t TFT_CS = 10;
#endif
#ifndef TFT_DC
static const uint8_t TFT_DC = 46;
#endif
#ifndef TFT_RST
static const uint8_t TFT_RST = 9;
#endif
#ifndef TFT_BL
static const uint8_t TFT_BL = 48;
#endif

// Touch XPT2046 (shared SPI2)
#ifndef TOUCH_CS
static const uint8_t TOUCH_CS = 3;
#endif
#ifndef TOUCH_IRQ
static const uint8_t TOUCH_IRQ = 8;
#endif

// ==============================================================================
// SPI3 - SD Card + RF Modules
// ==============================================================================
#ifndef SS
static const uint8_t SS = 38;
#endif
#ifndef MOSI
static const uint8_t MOSI = 35;
#endif
#ifndef MISO
static const uint8_t MISO = 37;
#endif
#ifndef SCK
static const uint8_t SCK = 36;
#endif

// SD Card Aliases
#ifndef SDCARD_CS
static const uint8_t SDCARD_CS = 38;
#endif
#ifndef SDCARD_MOSI
static const uint8_t SDCARD_MOSI = 35;
#endif
#ifndef SDCARD_MISO
static const uint8_t SDCARD_MISO = 37;
#endif
#ifndef SDCARD_SCK
static const uint8_t SDCARD_SCK = 36;
#endif

// ==============================================================================
// RF Modules - Shared SPI3 with SD
// ==============================================================================
// CC1101 SubGHz 433MHz
#ifndef CC1101_GDO0
static const uint8_t CC1101_GDO0 = 4;
#endif
#ifndef CC1101_SS
static const uint8_t CC1101_SS = 5;
#endif

// NRF24L01+PA+LNA
#ifndef NRF24_CE
static const uint8_t NRF24_CE = 6;
#endif
#ifndef NRF24_SS
static const uint8_t NRF24_SS = 7;
#endif

// ==============================================================================
// I2S DAC - PCM5102A High Quality Audio
// ==============================================================================
#ifndef I2S_BCLK
static const uint8_t I2S_BCLK = 40;
#endif
#ifndef I2S_LRCK
static const uint8_t I2S_LRCK = 39;  // Word Select
#endif
#ifndef I2S_DOUT
static const uint8_t I2S_DOUT = 41;
#endif

// ==============================================================================
// IR TX/RX Multi-Zone (5x TSOP38238)
// ==============================================================================
#ifndef IR_TX
static const uint8_t IR_TX = 47;
#endif
#ifndef IR_RX_MAIN
static const uint8_t IR_RX_MAIN = 14;
#endif
#ifndef IR_RX_ZONE1
static const uint8_t IR_RX_ZONE1 = 21;
#endif
#ifndef IR_RX_ZONE2
static const uint8_t IR_RX_ZONE2 = 42;
#endif
#ifndef IR_RX_ZONE3
static const uint8_t IR_RX_ZONE3 = 45;
#endif
#ifndef IR_RX_ZONE4
static const uint8_t IR_RX_ZONE4 = 0;  // Shared with boot button
#endif

// IR RX Zones Array - only if not defined
#ifndef IR_RX_ZONE_COUNT
#define IR_RX_ZONE_COUNT 5
#endif

// ==============================================================================
// UART1 - GPS NEO-6M
// ==============================================================================
#ifndef GPS_RX
static const uint8_t GPS_RX = 15;
#endif
#ifndef GPS_TX
static const uint8_t GPS_TX = 16;
#endif

// ==============================================================================
// UART2 - YS-IRTM (Requires Level Shifter 3.3V->5V)
// ==============================================================================
#ifndef IRM_TX
static const uint8_t IRM_TX = 17;
#endif
#ifndef IRM_RX
static const uint8_t IRM_RX = 18;
#endif

// ==============================================================================
// 1-Wire - DS18B20 Temperature Sensors
// ==============================================================================
#ifndef ONEWIRE_PIN
static const uint8_t ONEWIRE_PIN = 19;  // Dedicated pin for 1-Wire
#endif

// ==============================================================================
// WS2812B RGB LED Strip
// ==============================================================================
#ifndef RGB_LED
static const uint8_t RGB_LED = 48;
#endif
#ifndef RGB_LED_COUNT
#define RGB_LED_COUNT 10
#endif

// ==============================================================================
// Buzzer (Active/Passive)
// ==============================================================================
#ifndef BUZZER_PIN
static const uint8_t BUZZER_PIN = 45;
#endif
#ifndef BUZZ_PIN
static const uint8_t BUZZ_PIN = 45;  // Alias
#endif

// ==============================================================================
// MOSFET Driver IRF520 (High Power Loads)
// ==============================================================================
#ifndef MOSFET_PIN
static const uint8_t MOSFET_PIN = 42;
#endif

// ==============================================================================
// Battery ADC
// ==============================================================================
#ifndef VBAT_ADC
static const uint8_t VBAT_ADC = 14;  // ADC1_CH3
#endif

// ==============================================================================
// PAM8403 Audio Amplifier
// ==============================================================================
#ifndef PAM8403_EN_PIN
static const uint8_t PAM8403_EN_PIN = 20;
#endif

// ==============================================================================
// ADC Pins (ESP32-S3 has ADC1: GPIO1-10, ADC2: GPIO11-20)
// ==============================================================================
static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
static const uint8_t A7 = 8;
static const uint8_t A8 = 9;
static const uint8_t A9 = 10;

// ==============================================================================
// Touch Pins (ESP32-S3 capacitive touch)
// ==============================================================================
static const uint8_t T1 = 1;
static const uint8_t T2 = 2;
static const uint8_t T3 = 3;
static const uint8_t T4 = 4;
static const uint8_t T5 = 5;
static const uint8_t T6 = 6;
static const uint8_t T7 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;
static const uint8_t T14 = 14;

// ==============================================================================
// Boot Button
// ==============================================================================
#ifndef BTN_PIN
static const uint8_t BTN_PIN = 0;
#endif

// ==============================================================================
// Deep Sleep Wake Sources
// ==============================================================================
#ifndef DEEPSLEEP_WAKEUP_PIN
#define DEEPSLEEP_WAKEUP_PIN 8     // Touch interrupt
#endif
#ifndef DEEPSLEEP_PIN_ACT
#define DEEPSLEEP_PIN_ACT LOW
#endif

// ==============================================================================
// USB Native
// ==============================================================================
#ifndef USB_DN
static const uint8_t USB_DN = 19;
#endif
#ifndef USB_DP
static const uint8_t USB_DP = 20;
#endif

#endif /* Pins_Arduino_h */
