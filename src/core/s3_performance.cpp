/**
 * @file s3_performance.cpp
 * @brief ESP32-S3 Ultra Performance Optimizations Implementation
 */

#include "s3_performance.h"

#ifdef CYD_S3

#include <globals.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_heap_caps.h>
#include "thermal_manager.h"

// ============================================================================
// STATIC VARIABLES
// ============================================================================

static s3_perf_state_t perfState = {
    .currentFreq = CLOCK_240MHZ,
    .throttleReason = THROTTLE_NONE,
    .lastTemperature = 25.0f,
    .lastBatteryVoltage = 4.2f,
    .throttleCount = 0,
    .restoreCount = 0,
    .isThrottled = false
};

static esp_adc_cal_characteristics_t *adc_chars = nullptr;
static TaskHandle_t perfTaskHandle = nullptr;
static bool attackModeActive = false;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

static void s3_perf_monitor_task(void *pvParameters);
static bool apply_clock_config(s3_clock_freq_t freq);

// ============================================================================
// INITIALIZATION
// ============================================================================

void s3_perf_init() {
    // Configure ADC for battery monitoring
    adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    if (adc_chars) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        
        #ifdef VBAT_ADC_PIN
        adc1_channel_t channel = (adc1_channel_t)((VBAT_ADC_PIN < 10) ? VBAT_ADC_PIN : VBAT_ADC_PIN - 10);
        adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars);
        #endif
    }
    
    // Apply initial 240MHz configuration
    apply_clock_config(CLOCK_240MHZ);
    
    // Create performance monitoring task on Core 1
    xTaskCreatePinnedToCore(
        s3_perf_monitor_task,
        "S3Perf",
        2048,
        NULL,
        5,  // Mid priority
        &perfTaskHandle,
        1   // Core 1
    );
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[S3PERF] Performance manager initialized @ 240MHz");
    Serial.printf("[S3PERF] PSRAM: %d KB total, %d KB free\n", 
                  ESP.getPsramSize() / 1024, 
                  ESP.getFreePsram() / 1024);
    #endif
}

// ============================================================================
// CLOCK MANAGEMENT
// ============================================================================

static bool apply_clock_config(s3_clock_freq_t freq) {
    esp_pm_config_t pm_config = {
        .max_freq_mhz = (int)freq,
        .min_freq_mhz = 80,
        .light_sleep_enable = !attackModeActive
    };
    
    esp_err_t err = esp_pm_configure(&pm_config);
    if (err == ESP_OK) {
        perfState.currentFreq = freq;
        return true;
    }
    return false;
}

bool s3_perf_set_clock(s3_clock_freq_t freq) {
    return apply_clock_config(freq);
}

// ============================================================================
// BATTERY MONITORING
// ============================================================================

float s3_perf_get_battery_voltage() {
    #ifdef VBAT_ADC_PIN
    if (!adc_chars) return 4.2f;
    
    adc1_channel_t channel = (adc1_channel_t)((VBAT_ADC_PIN < 10) ? VBAT_ADC_PIN : VBAT_ADC_PIN - 10);
    int raw = adc1_get_raw(channel);
    uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, adc_chars);
    
    // Apply resistor divider compensation
    float voltage = (voltage_mv * BAT_ADC_DIVIDER) / 1000.0f;
    return voltage;
    #else
    return 4.2f;  // Assume full battery if no ADC
    #endif
}

// ============================================================================
// MONITORING TASK
// ============================================================================

static void s3_perf_monitor_task(void *pvParameters) {
    for (;;) {
        // Skip monitoring in attack mode
        if (attackModeActive) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        
        // Read sensors
        #ifdef HAS_TEMP_SENSOR
        perfState.lastTemperature = thermal_get_temperature();
        #endif
        
        perfState.lastBatteryVoltage = s3_perf_get_battery_voltage();
        
        // Determine throttle conditions
        bool shouldThrottle = false;
        throttle_reason_t reason = THROTTLE_NONE;
        
        if (perfState.lastTemperature > THERMAL_THROTTLE_TEMP) {
            shouldThrottle = true;
            reason = THROTTLE_THERMAL;
        }
        
        if (perfState.lastBatteryVoltage < BAT_LOW_THRESHOLD) {
            shouldThrottle = true;
            reason = (reason == THROTTLE_THERMAL) ? THROTTLE_BOTH : THROTTLE_BATTERY;
        }
        
        // Apply throttling
        if (shouldThrottle && !perfState.isThrottled) {
            if (apply_clock_config(CLOCK_160MHZ)) {
                perfState.isThrottled = true;
                perfState.throttleReason = reason;
                perfState.throttleCount++;
                
                #if CORE_DEBUG_LEVEL > 0
                Serial.printf("[S3PERF] Throttled to 160MHz (T:%.1f°C, V:%.2fV)\n",
                              perfState.lastTemperature, perfState.lastBatteryVoltage);
                #endif
            }
        }
        
        // Check restore conditions
        bool canRestore = true;
        if (perfState.lastTemperature > THERMAL_RESTORE_TEMP) canRestore = false;
        if (perfState.lastBatteryVoltage < BAT_RESTORE_THRESHOLD) canRestore = false;
        
        if (canRestore && perfState.isThrottled) {
            if (apply_clock_config(CLOCK_240MHZ)) {
                perfState.isThrottled = false;
                perfState.throttleReason = THROTTLE_NONE;
                perfState.restoreCount++;
                
                #if CORE_DEBUG_LEVEL > 0
                Serial.printf("[S3PERF] Restored to 240MHz (T:%.1f°C, V:%.2fV)\n",
                              perfState.lastTemperature, perfState.lastBatteryVoltage);
                #endif
            }
        }
        
        vTaskDelay(PERF_CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
}

// ============================================================================
// STATE ACCESSORS
// ============================================================================

s3_perf_state_t* s3_perf_get_state() {
    return &perfState;
}

bool s3_perf_is_throttled() {
    return perfState.isThrottled;
}

const char* s3_perf_get_throttle_reason() {
    switch (perfState.throttleReason) {
        case THROTTLE_THERMAL: return "Temperature";
        case THROTTLE_BATTERY: return "Battery";
        case THROTTLE_BOTH:    return "Temp+Battery";
        default:               return "None";
    }
}

// ============================================================================
// PSRAM UTILITIES
// ============================================================================

void* psram_alloc_aligned(size_t size) {
    if (!psramFound()) return nullptr;
    
    // Allocate with 16-byte alignment for DMA compatibility
    return heap_caps_aligned_alloc(16, size, MALLOC_CAP_SPIRAM);
}

void* smart_alloc(size_t size, size_t psramThreshold) {
    if (size >= psramThreshold && psramFound()) {
        void* ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (ptr) return ptr;
    }
    return malloc(size);
}

void psram_get_stats(size_t* total, size_t* used) {
    if (total) *total = ESP.getPsramSize();
    if (used) *used = ESP.getPsramSize() - ESP.getFreePsram();
}

// ============================================================================
// ATTACK MODE
// ============================================================================

void s3_perf_attack_mode_enter() {
    attackModeActive = true;
    
    // Force 240MHz, disable light sleep
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 240,  // Lock at max
        .light_sleep_enable = false
    };
    esp_pm_configure(&pm_config);
    perfState.currentFreq = CLOCK_240MHZ;
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[S3PERF] Attack mode ENGAGED - 240MHz locked");
    #endif
}

void s3_perf_attack_mode_exit() {
    attackModeActive = false;
    
    // Restore normal PM config
    apply_clock_config(perfState.isThrottled ? CLOCK_160MHZ : CLOCK_240MHZ);
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[S3PERF] Attack mode DISENGAGED");
    #endif
}

#endif // CYD_S3
