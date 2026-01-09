/**
 * @file gesture_sensor.h
 * MorphNode Firmware - PAJ7620U2 Gesture Sensor Driver
 *
 * Provides gesture recognition using PAJ7620U2 sensor via I2C.
 * Supports 9 gestures, cursor mode, and wake from deep sleep.
 *
 * @author MorphNode Team
 * @date 2025-12-19
 */

#ifndef GESTURE_SENSOR_H
#define GESTURE_SENSOR_H

#include "pin_config.h"
#include <Arduino.h>

// ========================================
// Gesture Types Enum
// ========================================
enum GestureType {
    GESTURE_NONE = 0,
    GESTURE_UP,
    GESTURE_DOWN,
    GESTURE_LEFT,
    GESTURE_RIGHT,
    GESTURE_FORWARD,  // Push / approach
    GESTURE_BACKWARD, // Pull / retreat
    GESTURE_CLOCKWISE,
    GESTURE_ANTICLOCKWISE,
    GESTURE_WAVE
};

// ========================================
// Gesture Action Mapping
// ========================================
enum GestureAction {
    ACTION_NONE = 0,
    ACTION_MENU_UP,
    ACTION_MENU_DOWN,
    ACTION_MENU_LEFT,
    ACTION_MENU_RIGHT,
    ACTION_SELECT,
    ACTION_BACK,
    ACTION_SPECIAL_1, // Clockwise - e.g., volume up
    ACTION_SPECIAL_2, // Anti-clockwise - e.g., volume down
    ACTION_WAKE       // Wave - wake from sleep
};

// ========================================
// Gesture Sensor Configuration
// ========================================
struct GestureConfig {
    bool enabled;
    bool useInterrupt;
    bool gameMode;        // 240fps vs 120fps
    uint16_t entryTimeMs; // Time before gesture starts
    uint16_t exitTimeMs;  // Time after gesture ends
    uint8_t sensitivity;  // 0-100
};

// ========================================
// GestureSensor Class
// ========================================
class GestureSensor {
public:
    /**
     * @brief Initialize the gesture sensor
     * @return true if sensor found and initialized
     */
    static bool init();

    /**
     * @brief Update sensor readings (call in loop or task)
     * Should be called periodically (10-50ms interval)
     */
    static void update();

    /**
     * @brief Get the last detected gesture
     * @return GestureType enum value
     */
    static GestureType getLastGesture();

    /**
     * @brief Get the mapped action for a gesture
     * @param gesture The gesture type
     * @return GestureAction enum value
     */
    static GestureAction getActionForGesture(GestureType gesture);

    /**
     * @brief Check if sensor is available
     * @return true if sensor is initialized and working
     */
    static bool isAvailable();

    /**
     * @brief Enable or disable the sensor
     * @param enabled true to enable, false to disable
     */
    static void setEnabled(bool enabled);

    /**
     * @brief Set game mode (faster response)
     * @param enabled true for 240fps, false for 120fps
     */
    static void setGameMode(bool enabled);

    /**
     * @brief Get wave count from last wave gesture
     * @return Number of waves (0-15)
     */
    static int getWaveCount();

    /**
     * @brief Check if an object is in view
     * @return true if object detected
     */
    static bool isObjectInView();

    /**
     * @brief Get object brightness (IR reflection)
     * @return 0-255
     */
    static int getObjectBrightness();

    /**
     * @brief Get object size in pixels
     * @return 0-900 pixels
     */
    static int getObjectSize();

    // ---- Cursor Mode ----

    /**
     * @brief Switch to cursor tracking mode
     */
    static void enableCursorMode();

    /**
     * @brief Switch back to gesture mode
     */
    static void enableGestureMode();

    /**
     * @brief Get cursor X position
     * @return 0-3712
     */
    static int getCursorX();

    /**
     * @brief Get cursor Y position
     * @return 0-3712
     */
    static int getCursorY();

    // ---- Deep Sleep Wake ----

    /**
     * @brief Configure wake from deep sleep on gesture
     * Uses the INT pin (if connected) to wake ESP32
     */
    static void configureDeepSleepWake();

    /**
     * @brief Check if wakeup was caused by gesture
     * @return true if gesture triggered the wakeup
     */
    static bool wasWakeupByGesture();

    // ---- Configuration ----

    /**
     * @brief Get current configuration
     * @return GestureConfig struct
     */
    static GestureConfig getConfig();

    /**
     * @brief Set configuration
     * @param config GestureConfig struct
     */
    static void setConfig(const GestureConfig &config);

    /**
     * @brief Set gesture timing parameters
     * @param entryMs Entry time in milliseconds
     * @param exitMs Exit time in milliseconds
     */
    static void setTiming(uint16_t entryMs, uint16_t exitMs);

private:
    static bool _initialized;
    static bool _enabled;
    static bool _inCursorMode;
    static GestureType _lastGesture;
    static GestureConfig _config;
    static int _waveCount;
};

// ========================================
// Global Instance (optional access)
// ========================================
extern GestureSensor gestureSensor;

#endif // GESTURE_SENSOR_H
