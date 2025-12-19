/**
 * @file thermal_manager.cpp
 * @brief Dynamic CPU throttling based on DS18B20 temperature sensor
 * 
 * Uses ESP-IDF power management to dynamically adjust CPU frequency
 * based on real-time temperature readings from DS18B20 sensor.
 */

#include "thermal_manager.h"
#include <globals.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef HAS_TEMP_SENSOR

#include <OneWire.h>
#include <DallasTemperature.h>

static OneWire oneWire(ONEWIRE_PIN);
static DallasTemperature sensors(&oneWire);
static bool isThrottled = false;
static float lastTemp = 0.0f;
static TaskHandle_t thermalTaskHandle = NULL;

void thermal_manager_init() {
    sensors.begin();
    
    // Get initial temperature reading
    sensors.requestTemperatures();
    lastTemp = sensors.getTempCByIndex(0);
    
    // Create thermal monitoring task on sensor core
    xTaskCreatePinnedToCore(
        thermal_manager_task,
        "ThermalMgr",
        2048,
        NULL,
        5,  // Mid priority
        &thermalTaskHandle,
#ifdef SENSOR_TASK_CORE
        SENSOR_TASK_CORE
#else
        1  // Default to core 1
#endif
    );
}

void thermal_manager_task(void *pvParameters) {
    esp_pm_config_t pm_config;
    
    for (;;) {
        // Request temperature conversion
        sensors.requestTemperatures();
        float temp = sensors.getTempCByIndex(0);
        
        // Validate reading (DS18B20 returns -127 on error)
        if (temp > -100.0f && temp < 125.0f) {
            lastTemp = temp;
            
            if (temp > THERMAL_HIGH_THRESHOLD && !isThrottled) {
                // Throttle down to prevent overheating
                pm_config.max_freq_mhz = 160;
                pm_config.min_freq_mhz = 80;
                pm_config.light_sleep_enable = true;
                
                if (esp_pm_configure(&pm_config) == ESP_OK) {
                    isThrottled = true;
                    #if CORE_DEBUG_LEVEL > 0
                    Serial.printf("[THERMAL] Throttled to 160MHz (%.1f°C)\n", temp);
                    #endif
                }
            } else if (temp < THERMAL_LOW_THRESHOLD && isThrottled) {
                // Restore full performance
                pm_config.max_freq_mhz = 240;
                pm_config.min_freq_mhz = 80;
                pm_config.light_sleep_enable = true;
                
                if (esp_pm_configure(&pm_config) == ESP_OK) {
                    isThrottled = false;
                    #if CORE_DEBUG_LEVEL > 0
                    Serial.printf("[THERMAL] Restored to 240MHz (%.1f°C)\n", temp);
                    #endif
                }
            }
        }
        
        vTaskDelay(THERMAL_CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
}

float thermal_get_temperature() {
    return lastTemp;
}

bool thermal_is_throttled() {
    return isThrottled;
}

#else
// Stub implementations for devices without temperature sensor

void thermal_manager_init() {
    // No-op
}

void thermal_manager_task(void *pvParameters) {
    vTaskDelete(NULL);
}

float thermal_get_temperature() {
    return 0.0f;
}

bool thermal_is_throttled() {
    return false;
}

#endif // HAS_TEMP_SENSOR
