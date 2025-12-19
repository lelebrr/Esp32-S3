#ifndef THERMAL_MANAGER_H
#define THERMAL_MANAGER_H

/**
 * @file thermal_manager.h
 * @brief Dynamic thermal throttling based on DS18B20 temperature sensor
 * 
 * Monitors chip temperature and dynamically adjusts CPU frequency:
 * - Normal: 240MHz
 * - Throttled (>70°C): 160MHz
 * - Returns to normal when <60°C
 */

#include <esp_pm.h>

#define THERMAL_HIGH_THRESHOLD 70.0f  // °C - throttle above this
#define THERMAL_LOW_THRESHOLD  60.0f  // °C - restore speed below this
#define THERMAL_CHECK_INTERVAL 10000  // ms between checks

/**
 * @brief Initialize thermal manager and start monitoring task
 */
void thermal_manager_init();

/**
 * @brief FreeRTOS task for thermal monitoring (runs on SENSOR_TASK_CORE)
 */
void thermal_manager_task(void *pvParameters);

/**
 * @brief Get last measured temperature
 * @return Temperature in Celsius, or 0 if no sensor
 */
float thermal_get_temperature();

/**
 * @brief Check if CPU is currently throttled
 * @return true if running at reduced frequency
 */
bool thermal_is_throttled();

#endif // THERMAL_MANAGER_H
