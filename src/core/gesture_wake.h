#ifndef GESTURE_WAKE_H
#define GESTURE_WAKE_H

/**
 * @file gesture_wake.h
 * @brief Deep sleep with gesture-based wake using PAJ7620U2
 * 
 * Enables ultra-low power deep sleep (<5µA) with wake sources:
 * - Gesture sensor interrupt (PAJ7620U2)
 * - Timer-based periodic wake
 * - Touch interrupt wake
 */

#include <esp_sleep.h>

/**
 * @brief Initialize gesture wake capability
 * @note Sets up ext0 wakeup on touch/gesture interrupt pin
 */
void gesture_wake_init();

/**
 * @brief Enter deep sleep with gesture-based wake
 * @note Will wake on PAJ7620U2 gesture detection
 */
void enter_deep_sleep_with_gesture();

/**
 * @brief Enter deep sleep with timer wake
 * @param sleep_us Sleep duration in microseconds
 */
void enter_deep_sleep_with_timer(uint64_t sleep_us);

/**
 * @brief Enter deep sleep with multiple wake sources
 * @param sleep_us Maximum sleep duration (0 for infinite)
 * @param enable_gesture Enable gesture-based wake
 */
void enter_deep_sleep_multi(uint64_t sleep_us, bool enable_gesture);

/**
 * @brief Get the wake cause after waking from deep sleep
 * @return Wake cause enum value
 */
esp_sleep_wakeup_cause_t get_wake_cause();

/**
 * @brief Prepare peripherals for deep sleep (power down, save state)
 */
void prepare_for_deep_sleep();

#endif // GESTURE_WAKE_H
