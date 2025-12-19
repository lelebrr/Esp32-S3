/**
 * @file pin_config.h
 * Monster S3 Firmware - Hardware Pin Definitions
 * ESP32-S3-WROOM-1 N16R8 + ILI9341 240x320 LCD with XPT2046 Touch
 * 
 * Updated: 2025-12-19
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ========================================
// DISPLAY - ILI9341 240x320 TFT SPI
// ========================================
#define PIN_TFT_CS      5      // Display Chip Select
#define PIN_TFT_RST     4      // Display Reset
#define PIN_TFT_DC      6      // Data/Command
#define PIN_TFT_MOSI    36     // Display MOSI (SDI)
#define PIN_TFT_SCLK    35     // Display Clock (SCK)
#define PIN_TFT_MISO    37     // Display MISO (SDO) - optional
#define PIN_TFT_BL      3      // Backlight LED

// Screen Dimensions
#define TFT_WIDTH       240
#define TFT_HEIGHT      320

// ========================================
// TOUCH - XPT2046 (Resistive Touch)
// ========================================
#define PIN_TOUCH_CS    38     // Touch Chip Select
#define PIN_TOUCH_IRQ   39     // Touch Interrupt
// Touch shares SPI bus with display (MOSI, MISO, CLK)

// ========================================
// SD CARD - Separate SPI Bus (HSPI)
// ========================================
#define PIN_SD_CS       10     // SD Chip Select
#define PIN_SD_MOSI     11     // SD MOSI
#define PIN_SD_MISO     13     // SD MISO  
#define PIN_SD_SCK      12     // SD Clock

// ========================================
// I2C BUS (PN532, Sensors)
// ========================================
#define PIN_I2C_SDA     8      // I2C Data
#define PIN_I2C_SCL     9      // I2C Clock

// ========================================
// NFC - PN532 (I2C Mode)
// ========================================
#define PIN_PN532_EN    7      // Power Enable (MOSFET Gate)
#define PIN_PN532_IRQ   15     // Interrupt (optional)

// ========================================
// SUBGHZ - CC1101 (SPI)
// ========================================
#define PIN_CC1101_CS   1      // CC1101 Chip Select
#define PIN_CC1101_GDO0 2      // CC1101 GDO0
#define PIN_CC1101_EN   14     // Power Enable

// ========================================
// I2S AUDIO OUTPUT
// ========================================
#define PIN_I2S_BCK     16     // I2S Bit Clock
#define PIN_I2S_WS      17     // I2S Word Select (LRCK)
#define PIN_I2S_DOUT    18     // I2S Data Out

// ========================================
// INFRARED
// ========================================
#define PIN_IR_TX       21     // IR Transmitter
#define PIN_IR_RX       47     // IR Receiver

// ========================================
// JOYSTICK SHIELD V1.A (Funduino)
// ========================================
// Joystick analógico (2 eixos + botão)
#define PIN_JOY_X       4      // ADC1_CH3 - Eixo X (range 0-4095, centro ~2048)
#define PIN_JOY_Y       5      // ADC1_CH4 - Eixo Y (range 0-4095, centro ~2048)
#define PIN_JOY_SW      0      // Botão do joystick (click) - LOW=pressionado
                               // ATENÇÃO: GPIO0 é Boot button! Não pressionar durante upload

// Botões direcionais (todos LOW quando pressionados, com pull-up interno)
#define PIN_BTN_A       41     // Botão A (posição: alto/cima)
#define PIN_BTN_B       42     // Botão B (posição: direita)
#define PIN_BTN_C       14     // Botão C (posição: baixo)
#define PIN_BTN_D       15     // Botão D (posição: esquerda)

// Botões auxiliares (pequenos, centro inferior do shield)
#define PIN_BTN_E       16     // Botão E (esquerdo)
#define PIN_BTN_F       17     // Botão F (direito)

// Conectores externos do shield (já mapeados):
// - I2C: SDA=GPIO8, SCL=GPIO9 (compatível com PN532, DS3231, PAJ7620U2)
// - Serial: TX=GPIO43, RX=GPIO44 (para módulo Bluetooth)
// - nRF24L01: Já configurado separadamente (CC1101/NRF24 section)
// - Nokia 5110: Não suportado (conflita com display TFT)

// ========================================
// STATUS LEDs
// ========================================
#define PIN_LED_RED_ATTACK_HI   45   // Attack Active (High Power)
#define PIN_LED_RED_ATTACK_LO   46   // Attack Standby (Low Power)
#define PIN_LED_BLUE_AI         48   // AI Processing
#define PIN_LED_BLUE_SLEEP      RGB_BUILTIN  // Deep Sleep Indicator

// ========================================
// FAULT INJECTION
// ========================================
#define PIN_FAULT_GATE  40     // MOSFET Gate for Voltage Glitching

#endif // PIN_CONFIG_H
