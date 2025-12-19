#pragma once

// ==========================================
// MONSTER ESP32-S3 PINOUT CONFIGURATION
// ==========================================

// --- POWER MANAGEMENT ---
#define PIN_FAULT_GATE          5  // User Request: GPIO5 IRF520
#define PIN_CC1101_EN           1  // Moved from 5
#define PIN_PN532_EN            2
#define PIN_BLE_EN              19 // Internal or unused

// --- STATUS LEDS (User Request) ---
#define PIN_LED_RED_ATTACK_HI   4
#define PIN_LED_RED_ATTACK_LO   0
#define PIN_LED_BLUE_AI         6
#define PIN_LED_BLUE_SLEEP      7 // Assuming Blue Sleep

// --- AUDIO (I2S - PCM5102A) ---
#define PIN_I2S_BCK             9
#define PIN_I2S_WS              10
#define PIN_I2S_DOUT            11

// --- SPI BUS (SD Card) ---
#define PIN_SD_MISO             13
#define PIN_SD_MOSI             12
#define PIN_SD_SCK              14
#define PIN_SD_CS               21 

// --- DISPLAY (ST7789 SPI) ---
#define PIN_TFT_MOSI            35
#define PIN_TFT_SCLK            36
#define PIN_TFT_CS              37
#define PIN_TFT_DC              38
#define PIN_TFT_RST             39
#define PIN_TFT_BL              40

// --- I2C BUS ---
#define PIN_I2C_SDA             8
#define PIN_I2C_SCL             48

// --- UART ---
#define PIN_GPS_TX              17
#define PIN_GPS_RX              18

#define PIN_IR_TX               15
#define PIN_IR_RX               16

// --- CONTROLS ---
#define PIN_BTN_FAULT           33 // Trigger
#define PIN_BTN_YES             41 // Moved from 34/35 conflict
#define PIN_BTN_NO              42
#define PIN_SWITCH_STEALTH      3  // Moved from 15/16 conflict (Using 3, careful of JTAG/UART0)
                                   // Actually S3 UART0 is 43/44. 3 is freeish.
