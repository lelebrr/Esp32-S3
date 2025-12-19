/**
 * @file s3_hardware_init.h
 * @brief ESP32-S3-WROOM-1 N16R8 Complete Hardware Initialization
 * 
 * Integrates ALL hardware components:
 * - PCM5102A DAC (I2S)
 * - PAM8403 Audio Amplifier
 * - DS3231 RTC (I2C)
 * - PAJ7620U2 Gesture Sensor (I2C)
 * - PN532 NFC/RFID (I2C)
 * - CC1101 SubGHz (SPI)
 * - NRF24L01 2.4GHz (SPI)
 * - NEO-6M GPS (UART1)
 * - YS-IRTM IR Transceiver (UART2)
 * - TSOP38238 IR Receivers (5 zones)
 * - DS18B20 Temperature Sensors (1-Wire)
 * - WS2812B RGB LEDs (RMT)
 * - Buzzer (PWM)
 * - IRF520 MOSFET Power Gating
 * 
 * @author Lele Origin Project
 * @date 2025-12-19
 */

#ifndef S3_HARDWARE_INIT_H
#define S3_HARDWARE_INIT_H

#include <Arduino.h>
#include <Wire.h>
#include <driver/i2s.h>

#ifdef CYD_S3

// ============================================================================
// Hardware Status Structure
// ============================================================================
typedef struct {
    bool i2c_ok;
    bool i2s_dac_ok;
    bool rtc_ok;
    bool gesture_ok;
    bool nfc_ok;
    bool gps_ok;
    bool temp_sensors_ok;
    bool rgb_leds_ok;
    bool buzzer_ok;
    bool mosfet_ok;
    bool ir_zones_ok;
    bool ys_irtm_ok;
    uint8_t temp_sensor_count;
} HardwareStatus;

extern HardwareStatus hwStatus;

// ============================================================================
// I2C Device Addresses
// ============================================================================
#ifndef DS3231_ADDR
#define DS3231_ADDR 0x68
#endif

#ifndef PAJ7620_ADDR
#define PAJ7620_ADDR 0x73
#endif

#ifndef PN532_I2C_ADDR
#define PN532_I2C_ADDR 0x24
#endif

// ============================================================================
// I2S DAC Configuration (PCM5102A)
// ============================================================================
#ifndef I2S_BCLK_PIN
#define I2S_BCLK_PIN 40
#endif

#ifndef I2S_LRCK_PIN
#define I2S_LRCK_PIN 39
#endif

#ifndef I2S_DOUT_PIN
#define I2S_DOUT_PIN 41
#endif

#define I2S_SAMPLE_RATE 44100
#define I2S_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define I2S_DMA_BUF_COUNT 8
#define I2S_DMA_BUF_LEN 256

// ============================================================================
// PAM8403 Amplifier Control
// ============================================================================
#ifndef PAM8403_EN_PIN
#define PAM8403_EN_PIN 20
#endif

// ============================================================================
// 1-Wire (DS18B20A)
// ============================================================================
#ifndef ONEWIRE_PIN
#define ONEWIRE_PIN 19
#endif

#ifndef DS18B20_COUNT
#define DS18B20_COUNT 2
#endif

// ============================================================================
// IR Zones (TSOP38238)
// ============================================================================
#ifndef IR_RX_MAIN
#define IR_RX_MAIN 14
#endif

#ifndef IR_RX_ZONE1
#define IR_RX_ZONE1 21
#endif

#ifndef IR_RX_ZONE2
#define IR_RX_ZONE2 42
#endif

#ifndef IR_RX_ZONE3
#define IR_RX_ZONE3 45
#endif

#ifndef IR_RX_ZONE4
#define IR_RX_ZONE4 20
#endif

#ifndef IR_TX_PIN
#define IR_TX_PIN 47
#endif

// ============================================================================
// YS-IRTM (UART2)
// ============================================================================
#ifndef IRM_TX_PIN
#define IRM_TX_PIN 17
#endif

#ifndef IRM_RX_PIN
#define IRM_RX_PIN 18
#endif

#ifndef IRM_BAUD
#define IRM_BAUD 9600
#endif

// ============================================================================
// GPS NEO-6M (UART1)
// ============================================================================
#ifndef GPS_RX_PIN
#define GPS_RX_PIN 15
#endif

#ifndef GPS_TX_PIN
#define GPS_TX_PIN 16
#endif

#ifndef GPS_BAUD
#define GPS_BAUD 9600
#endif

// ============================================================================
// RGB LEDs (WS2812B)
// ============================================================================
#ifndef RGB_LED
#define RGB_LED 48
#endif

#ifndef RGB_LED_COUNT
#define RGB_LED_COUNT 10
#endif

// ============================================================================
// Buzzer (PWM)
// ============================================================================
#ifndef BUZZ_PIN
#define BUZZ_PIN 45
#endif

#ifndef BUZZER_CHANNEL
#define BUZZER_CHANNEL 1
#endif

#ifndef BUZZER_FREQ
#define BUZZER_FREQ 2700
#endif

// ============================================================================
// MOSFET Power Gate (IRF520)
// ============================================================================
#ifndef MOSFET_PIN
#define MOSFET_PIN 42
#endif

// ============================================================================
// Function Prototypes
// ============================================================================

/**
 * @brief Initialize all S3 hardware components
 * @return true if all critical components initialized successfully
 */
bool IRAM_ATTR init_all_s3_hardware(void);

/**
 * @brief Initialize I2C bus at 400kHz
 */
void init_i2c_bus(void);

/**
 * @brief Initialize I2S DAC (PCM5102A) with DMA
 * @return true on success
 */
bool init_i2s_dac(void);

/**
 * @brief Initialize DS3231 RTC
 * @return true if RTC responds on I2C
 */
bool init_rtc_ds3231(void);

/**
 * @brief Initialize PAJ7620U2 gesture sensor
 * @return true if sensor responds
 */
bool init_gesture_paj7620(void);

/**
 * @brief Initialize DS18B20 temperature sensors
 * @return number of sensors found (0-2)
 */
uint8_t init_temp_ds18b20(void);

/**
 * @brief Initialize WS2812B RGB LED strip
 */
void init_rgb_ws2812b(void);

/**
 * @brief Initialize GPS UART1
 */
void init_gps_neo6m(void);

/**
 * @brief Initialize YS-IRTM IR transceiver on UART2
 */
void init_ys_irtm(void);

/**
 * @brief Initialize all IR receiver zones
 */
void init_ir_zones(void);

/**
 * @brief Initialize buzzer PWM channel
 */
void init_buzzer(void);

/**
 * @brief Initialize MOSFET power gate
 */
void init_mosfet_gate(void);

/**
 * @brief Enable PAM8403 amplifier
 * @param enable true to enable, false to disable (power save)
 */
void pam8403_enable(bool enable);

/**
 * @brief Control MOSFET power gate
 * @param on true to enable power, false to cut power
 */
void mosfet_power(bool on);

/**
 * @brief Play a tone on the buzzer
 * @param frequency Hz
 * @param duration_ms milliseconds
 */
void buzzer_tone(uint16_t frequency, uint16_t duration_ms);

/**
 * @brief Get current temperature from DS18B20
 * @param sensor_index 0 or 1
 * @return temperature in Celsius, -127 if error
 */
float get_temperature(uint8_t sensor_index);

/**
 * @brief Print hardware status to Serial
 */
void print_hardware_status(void);

// ============================================================================
// Deep Sleep Wake Sources
// ============================================================================

/**
 * @brief Configure deep sleep wake sources
 * - Timer wake (configurable)
 * - GPIO wake (button)
 * - Gesture sensor wake (PAJ7620U2 interrupt)
 */
void configure_deep_sleep_wake(void);

/**
 * @brief Enter deep sleep mode
 * @param wake_time_us microseconds until wake (0 = indefinite)
 */
void enter_deep_sleep(uint64_t wake_time_us);

#endif // CYD_S3

#endif // S3_HARDWARE_INIT_H
