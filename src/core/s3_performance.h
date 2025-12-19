/**
 * @file s3_performance.h
 * @brief ESP32-S3 Ultra Performance Optimizations
 * 
 * Features:
 * - Dynamic clock management (240MHz with thermal/battery fallback)
 * - PSRAM allocation with 16-byte alignment
 * - Attack burst infrastructure
 * 
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef S3_PERFORMANCE_H
#define S3_PERFORMANCE_H

#include <Arduino.h>
#include <esp_pm.h>
#include <esp_adc_cal.h>

#ifdef CYD_S3

// ============================================================================
// PERFORMANCE THRESHOLDS
// ============================================================================

#ifndef THERMAL_THROTTLE_TEMP
#define THERMAL_THROTTLE_TEMP     65.0f   // Throttle if > 65°C
#endif

#ifndef THERMAL_RESTORE_TEMP
#define THERMAL_RESTORE_TEMP      60.0f   // Restore if < 60°C
#endif

#ifndef BAT_LOW_THRESHOLD
#define BAT_LOW_THRESHOLD         3.5f    // Throttle if < 3.5V
#endif

#ifndef BAT_RESTORE_THRESHOLD
#define BAT_RESTORE_THRESHOLD     3.6f    // Restore if > 3.6V
#endif

#ifndef BAT_ADC_DIVIDER
#define BAT_ADC_DIVIDER           2.0f    // Resistor divider ratio (10K/10K)
#endif

// Performance check interval (ms)
#define PERF_CHECK_INTERVAL       5000

// ============================================================================
// CLOCK CONFIGURATION
// ============================================================================

typedef enum {
    CLOCK_240MHZ = 240,
    CLOCK_160MHZ = 160,
    CLOCK_80MHZ  = 80
} s3_clock_freq_t;

typedef enum {
    THROTTLE_NONE = 0,
    THROTTLE_THERMAL,
    THROTTLE_BATTERY,
    THROTTLE_BOTH
} throttle_reason_t;

// ============================================================================
// PERFORMANCE STATE
// ============================================================================

typedef struct {
    s3_clock_freq_t currentFreq;
    throttle_reason_t throttleReason;
    float lastTemperature;
    float lastBatteryVoltage;
    uint32_t throttleCount;
    uint32_t restoreCount;
    bool isThrottled;
} s3_perf_state_t;

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

/**
 * @brief Initialize S3 performance manager
 * Sets up 240MHz clock with PM, ADC for battery, and monitoring task
 */
void s3_perf_init();

/**
 * @brief Get current performance state
 */
s3_perf_state_t* s3_perf_get_state();

/**
 * @brief Force clock frequency (use with caution)
 */
bool s3_perf_set_clock(s3_clock_freq_t freq);

/**
 * @brief Get battery voltage via calibrated ADC
 */
float s3_perf_get_battery_voltage();

/**
 * @brief Check if currently throttled
 */
bool s3_perf_is_throttled();

/**
 * @brief Get throttle reason string
 */
const char* s3_perf_get_throttle_reason();

// ============================================================================
// PSRAM UTILITIES
// ============================================================================

/**
 * @brief Allocate aligned PSRAM buffer (16-byte alignment for DMA)
 * @param size Size in bytes
 * @return Pointer to aligned buffer or NULL
 */
void* psram_alloc_aligned(size_t size);

/**
 * @brief Allocate buffer preferring PSRAM for large allocations
 * @param size Size in bytes
 * @param psramThreshold Use PSRAM if size >= threshold (default 4KB)
 * @return Pointer to buffer or NULL
 */
void* smart_alloc(size_t size, size_t psramThreshold = 4096);

/**
 * @brief Get PSRAM usage statistics
 * @param total Output: total PSRAM bytes
 * @param used Output: used PSRAM bytes
 */
void psram_get_stats(size_t* total, size_t* used);

// ============================================================================
// ATTACK BURST SUPPORT
// ============================================================================

/**
 * @brief Prepare for high-speed attack (boost clock, disable sleep)
 */
void s3_perf_attack_mode_enter();

/**
 * @brief Exit attack mode (restore normal operation)
 */
void s3_perf_attack_mode_exit();

/**
 * @brief Precision microsecond delay (non-yielding)
 */
#define BURST_DELAY_US(us) ets_delay_us(us)

#endif // CYD_S3

#endif // S3_PERFORMANCE_H
