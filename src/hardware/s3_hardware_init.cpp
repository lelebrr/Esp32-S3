/**
 * @file s3_hardware_init.cpp
 * @brief ESP32-S3-WROOM-1 N16R8 Complete Hardware Initialization Implementation
 * 
 * @author Lele Origin Project
 * @date 2025-12-19
 */

#include "s3_hardware_init.h"

#ifdef CYD_S3

#include <OneWire.h>
#include <DallasTemperature.h>
#include <FastLED.h>
#include <driver/ledc.h>

// ============================================================================
// Global Hardware Status
// ============================================================================
HardwareStatus hwStatus = {
    .i2c_ok = false,
    .i2s_dac_ok = false,
    .rtc_ok = false,
    .gesture_ok = false,
    .nfc_ok = false,
    .gps_ok = false,
    .temp_sensors_ok = false,
    .rgb_leds_ok = false,
    .buzzer_ok = false,
    .mosfet_ok = false,
    .ir_zones_ok = false,
    .ys_irtm_ok = false,
    .temp_sensor_count = 0
};

// ============================================================================
// Static Objects
// ============================================================================
static OneWire oneWire(ONEWIRE_PIN);
static DallasTemperature tempSensors(&oneWire);
static CRGB leds[RGB_LED_COUNT];
static HardwareSerial gpsSerial(1);  // UART1
static HardwareSerial irmSerial(2);  // UART2

// ============================================================================
// I2C Bus Initialization
// ============================================================================
void init_i2c_bus(void) {
    Wire.begin(GROVE_SDA, GROVE_SCL, 400000);
    hwStatus.i2c_ok = true;
    Serial.println("[I2C] Bus initialized at 400kHz (SDA=" + String(GROVE_SDA) + ", SCL=" + String(GROVE_SCL) + ")");
}

// ============================================================================
// I2S DAC Initialization (PCM5102A)
// ============================================================================
bool init_i2s_dac(void) {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,
        .dma_buf_count = I2S_DMA_BUF_COUNT,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = true,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRCK_PIN,
        .data_out_num = I2S_DOUT_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.println("[I2S] FAIL - Driver install error: " + String(err));
        hwStatus.i2s_dac_ok = false;
        return false;
    }

    err = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (err != ESP_OK) {
        Serial.println("[I2S] FAIL - Pin config error: " + String(err));
        hwStatus.i2s_dac_ok = false;
        return false;
    }

    hwStatus.i2s_dac_ok = true;
    Serial.println("[I2S] OK - PCM5102A DAC ready (BCLK=" + String(I2S_BCLK_PIN) + 
                   ", LRCK=" + String(I2S_LRCK_PIN) + ", DOUT=" + String(I2S_DOUT_PIN) + ")");
    return true;
}

// ============================================================================
// DS3231 RTC Initialization
// ============================================================================
bool init_rtc_ds3231(void) {
    Wire.beginTransmission(DS3231_ADDR);
    if (Wire.endTransmission() == 0) {
        hwStatus.rtc_ok = true;
        Serial.println("[RTC] OK - DS3231 found at 0x" + String(DS3231_ADDR, HEX));
        return true;
    }
    Serial.println("[RTC] FAIL - DS3231 not responding at 0x" + String(DS3231_ADDR, HEX));
    hwStatus.rtc_ok = false;
    return false;
}

// ============================================================================
// PAJ7620U2 Gesture Sensor Initialization
// ============================================================================
bool init_gesture_paj7620(void) {
    Wire.beginTransmission(PAJ7620_ADDR);
    if (Wire.endTransmission() == 0) {
        // Basic initialization - in production, send full init sequence
        hwStatus.gesture_ok = true;
        Serial.println("[GESTURE] OK - PAJ7620U2 found at 0x" + String(PAJ7620_ADDR, HEX));
        return true;
    }
    Serial.println("[GESTURE] FAIL - PAJ7620U2 not responding at 0x" + String(PAJ7620_ADDR, HEX));
    hwStatus.gesture_ok = false;
    return false;
}

// ============================================================================
// DS18B20 Temperature Sensors Initialization
// ============================================================================
uint8_t init_temp_ds18b20(void) {
    tempSensors.begin();
    uint8_t count = tempSensors.getDeviceCount();
    hwStatus.temp_sensor_count = count;
    
    if (count > 0) {
        hwStatus.temp_sensors_ok = true;
        // Set resolution to 12-bit for maximum precision
        tempSensors.setResolution(12);
        // Enable async mode for non-blocking reads
        tempSensors.setWaitForConversion(false);
        Serial.println("[TEMP] OK - Found " + String(count) + " DS18B20 sensor(s) on GPIO " + String(ONEWIRE_PIN));
    } else {
        hwStatus.temp_sensors_ok = false;
        Serial.println("[TEMP] WARN - No DS18B20 sensors found on GPIO " + String(ONEWIRE_PIN));
    }
    
    return count;
}

// ============================================================================
// WS2812B RGB LED Initialization
// ============================================================================
void init_rgb_ws2812b(void) {
    FastLED.addLeds<WS2812B, RGB_LED, GRB>(leds, RGB_LED_COUNT);
    FastLED.setBrightness(50); // Start at 50% brightness
    FastLED.clear();
    FastLED.show();
    
    hwStatus.rgb_leds_ok = true;
    Serial.println("[LED] OK - WS2812B strip ready (" + String(RGB_LED_COUNT) + " LEDs on GPIO " + String(RGB_LED) + ")");
}

// ============================================================================
// GPS NEO-6M Initialization (UART1)
// ============================================================================
void init_gps_neo6m(void) {
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    hwStatus.gps_ok = true;
    Serial.println("[GPS] OK - UART1 configured at " + String(GPS_BAUD) + " baud (RX=" + 
                   String(GPS_RX_PIN) + ", TX=" + String(GPS_TX_PIN) + ")");
}

// ============================================================================
// YS-IRTM IR Transceiver Initialization (UART2)
// ============================================================================
void init_ys_irtm(void) {
#ifdef HAS_YS_IRTM
    irmSerial.begin(IRM_BAUD, SERIAL_8N1, IRM_RX_PIN, IRM_TX_PIN);
    hwStatus.ys_irtm_ok = true;
    Serial.println("[YS-IRTM] OK - UART2 configured at " + String(IRM_BAUD) + " baud (TX=" + 
                   String(IRM_TX_PIN) + ", RX=" + String(IRM_RX_PIN) + ")");
#ifdef IRM_LEVEL_SHIFTER
    Serial.println("[YS-IRTM] Note: Level shifter 3.3V-5V required!");
#endif
#endif
}

// ============================================================================
// IR Receiver Zones Initialization
// ============================================================================
void init_ir_zones(void) {
    // Configure all IR receiver pins as inputs
    pinMode(IR_RX_MAIN, INPUT);
    pinMode(IR_RX_ZONE1, INPUT);
    pinMode(IR_RX_ZONE2, INPUT);
    pinMode(IR_RX_ZONE3, INPUT);
#ifdef IR_RX_ZONE4
    pinMode(IR_RX_ZONE4, INPUT);
#endif
    
    // Configure IR transmitter
    pinMode(IR_TX_PIN, OUTPUT);
    digitalWrite(IR_TX_PIN, LOW);
    
    hwStatus.ir_zones_ok = true;
    Serial.println("[IR] OK - 5 receive zones + 1 TX configured");
    Serial.println("    Main=" + String(IR_RX_MAIN) + ", Z1=" + String(IR_RX_ZONE1) + 
                   ", Z2=" + String(IR_RX_ZONE2) + ", Z3=" + String(IR_RX_ZONE3) +
                   ", Z4=" + String(IR_RX_ZONE4) + ", TX=" + String(IR_TX_PIN));
}

// ============================================================================
// Buzzer Initialization
// ============================================================================
void init_buzzer(void) {
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, 8);
    ledcAttachPin(BUZZ_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0); // Start silent
    
    hwStatus.buzzer_ok = true;
    Serial.println("[BUZZER] OK - PWM channel " + String(BUZZER_CHANNEL) + 
                   " ready on GPIO " + String(BUZZ_PIN));
}

// ============================================================================
// MOSFET Power Gate Initialization
// ============================================================================
void init_mosfet_gate(void) {
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW); // Start with power OFF
    
    hwStatus.mosfet_ok = true;
    Serial.println("[MOSFET] OK - IRF520 gate configured on GPIO " + String(MOSFET_PIN) + " (initially OFF)");
}

// ============================================================================
// PAM8403 Amplifier Control
// ============================================================================
void pam8403_enable(bool enable) {
#ifdef HAS_PAM8403
    pinMode(PAM8403_EN_PIN, OUTPUT);
    digitalWrite(PAM8403_EN_PIN, enable ? HIGH : LOW);
    Serial.println("[PAM8403] " + String(enable ? "ENABLED" : "DISABLED"));
#endif
}

// ============================================================================
// MOSFET Power Control
// ============================================================================
void mosfet_power(bool on) {
    digitalWrite(MOSFET_PIN, on ? HIGH : LOW);
    Serial.println("[MOSFET] Power " + String(on ? "ON" : "OFF"));
}

// ============================================================================
// Buzzer Tone
// ============================================================================
void buzzer_tone(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) {
        ledcWrite(BUZZER_CHANNEL, 0);
        return;
    }
    
    ledcWriteTone(BUZZER_CHANNEL, frequency);
    if (duration_ms > 0) {
        delay(duration_ms);
        ledcWrite(BUZZER_CHANNEL, 0);
    }
}

// ============================================================================
// Get Temperature
// ============================================================================
float get_temperature(uint8_t sensor_index) {
    if (sensor_index >= hwStatus.temp_sensor_count) {
        return -127.0f;
    }
    
    tempSensors.requestTemperatures();
    return tempSensors.getTempCByIndex(sensor_index);
}

// ============================================================================
// Print Hardware Status
// ============================================================================
void print_hardware_status(void) {
    Serial.println("\n========== HARDWARE STATUS ==========");
    Serial.println("I2C Bus:        " + String(hwStatus.i2c_ok ? "OK" : "FAIL"));
    Serial.println("I2S DAC:        " + String(hwStatus.i2s_dac_ok ? "OK" : "FAIL"));
    Serial.println("RTC DS3231:     " + String(hwStatus.rtc_ok ? "OK" : "FAIL"));
    Serial.println("Gesture PAJ:    " + String(hwStatus.gesture_ok ? "OK" : "FAIL"));
    Serial.println("GPS NEO-6M:     " + String(hwStatus.gps_ok ? "OK" : "FAIL"));
    Serial.println("Temp Sensors:   " + String(hwStatus.temp_sensor_count) + " found");
    Serial.println("RGB LEDs:       " + String(hwStatus.rgb_leds_ok ? "OK" : "FAIL"));
    Serial.println("Buzzer:         " + String(hwStatus.buzzer_ok ? "OK" : "FAIL"));
    Serial.println("MOSFET Gate:    " + String(hwStatus.mosfet_ok ? "OK" : "FAIL"));
    Serial.println("IR Zones:       " + String(hwStatus.ir_zones_ok ? "OK" : "FAIL"));
    Serial.println("YS-IRTM:        " + String(hwStatus.ys_irtm_ok ? "OK" : "FAIL"));
    Serial.println("=====================================\n");
}

// ============================================================================
// Configure Deep Sleep Wake Sources
// ============================================================================
void configure_deep_sleep_wake(void) {
    // Wake on GPIO 0 (Boot button) - active LOW
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BTN_PIN, 0);
    
    // Wake on gesture sensor interrupt (if available)
#ifdef HAS_GESTURE
    // PAJ7620U2 can generate interrupt on gesture detection
    // Requires additional pin configuration
#endif
    
    Serial.println("[SLEEP] Wake sources configured (GPIO 0 button)");
}

// ============================================================================
// Enter Deep Sleep
// ============================================================================
void enter_deep_sleep(uint64_t wake_time_us) {
    Serial.println("[SLEEP] Entering deep sleep...");
    
    // Disable peripherals to minimize current
#ifdef HAS_PAM8403
    pam8403_enable(false);
#endif
    mosfet_power(false);
    FastLED.clear();
    FastLED.show();
    
    if (wake_time_us > 0) {
        esp_sleep_enable_timer_wakeup(wake_time_us);
        Serial.println("[SLEEP] Timer wake in " + String(wake_time_us / 1000000) + " seconds");
    }
    
    // Flush serial
    Serial.flush();
    delay(100);
    
    esp_deep_sleep_start();
}

// ============================================================================
// Master Initialization Function
// ============================================================================
bool IRAM_ATTR init_all_s3_hardware(void) {
    Serial.println("\n[S3] ====== HARDWARE INITIALIZATION ======");
    Serial.println("[S3] ESP32-S3-WROOM-1 N16R8 (16MB Flash + 8MB PSRAM)");
    Serial.println("[S3] Clock: 240MHz, PSRAM: 120MHz OPI");
    Serial.println("");
    
    // I2C Bus (required first for I2C devices)
    init_i2c_bus();
    
    // RTC DS3231
    init_rtc_ds3231();
    
    // Gesture Sensor PAJ7620U2
    init_gesture_paj7620();
    
    // I2S DAC PCM5102A
    init_i2s_dac();
    
    // PAM8403 Amplifier (start disabled for power save)
#ifdef HAS_PAM8403
    pam8403_enable(false);
    Serial.println("[PAM8403] OK - Amplifier configured (initially disabled)");
#endif
    
    // Temperature Sensors DS18B20
    init_temp_ds18b20();
    
    // RGB LEDs WS2812B
    init_rgb_ws2812b();
    
    // GPS NEO-6M
    init_gps_neo6m();
    
    // YS-IRTM IR Transceiver
    init_ys_irtm();
    
    // IR Receiver Zones
    init_ir_zones();
    
    // Buzzer
    init_buzzer();
    
    // MOSFET Power Gate
    init_mosfet_gate();
    
    // Configure deep sleep wake sources
    configure_deep_sleep_wake();
    
    // Print summary
    print_hardware_status();
    
    Serial.println("[S3] ====== INITIALIZATION COMPLETE ======\n");
    
    // Return true if critical components are OK
    return hwStatus.i2c_ok && hwStatus.i2s_dac_ok;
}

#endif // CYD_S3
