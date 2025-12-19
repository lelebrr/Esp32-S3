/**
 * @file gesture_sensor.cpp
 * Monster S3 Firmware - PAJ7620U2 Gesture Sensor Implementation
 * 
 * Full implementation of gesture recognition using PAJ7620U2 sensor.
 * Supports 9 gestures, cursor mode, game mode, and deep sleep wake.
 * 
 * @author Monster S3 Team
 * @date 2025-12-19
 */

#include "gesture_sensor.h"
#include "pin_config.h"
#include <Wire.h>

// Include the RevEng_PAJ7620 library
#include "RevEng_PAJ7620.h"

// ========================================
// Static Member Initialization
// ========================================
bool GestureSensor::_initialized = false;
bool GestureSensor::_enabled = false;
bool GestureSensor::_inCursorMode = false;
GestureType GestureSensor::_lastGesture = GESTURE_NONE;
int GestureSensor::_waveCount = 0;

GestureConfig GestureSensor::_config = {
    .enabled = true,
    .useInterrupt = false,
    .gameMode = false,
    .entryTimeMs = 0,
    .exitTimeMs = 200,
    .sensitivity = 50
};

// Internal sensor object
static RevEng_PAJ7620 _sensor;

// ========================================
// Initialization
// ========================================
bool GestureSensor::init() {
#if PAJ7620_ENABLED == 0
    Serial.println("[GESTURE] Sensor disabled in config");
    return false;
#endif

    Serial.println("[GESTURE] Initializing PAJ7620U2...");
    
    // Sensor uses Wire (I2C) which should already be initialized
    // by s3_driver.cpp on GPIO 8 (SDA) and GPIO 9 (SCL)
    
    if (!_sensor.begin()) {
        Serial.println("[GESTURE] ERROR: PAJ7620 not found!");
        Serial.println("[GESTURE] Check I2C connections (SDA=GPIO8, SCL=GPIO9)");
        _initialized = false;
        return false;
    }
    
    Serial.println("[GESTURE] PAJ7620U2 initialized successfully!");
    Serial.printf("[GESTURE] I2C Address: 0x%02X\n", PAJ7620_I2C_ADDR);
    
    // Apply default configuration
    _sensor.setGestureEntryTime(_config.entryTimeMs);
    _sensor.setGestureExitTime(_config.exitTimeMs);
    
    // Set game mode if configured
    if (_config.gameMode) {
        _sensor.setGameSpeed();
        Serial.println("[GESTURE] Game mode enabled (240fps)");
    } else {
        _sensor.setNormalSpeed();
        Serial.println("[GESTURE] Normal mode (120fps)");
    }
    
    // Configure interrupt pin if used
    if (PIN_PAJ7620_INT >= 0 && _config.useInterrupt) {
        pinMode(PIN_PAJ7620_INT, INPUT_PULLUP);
        Serial.printf("[GESTURE] Interrupt pin configured: GPIO %d\n", PIN_PAJ7620_INT);
    }
    
    _initialized = true;
    _enabled = _config.enabled;
    
    Serial.println("[GESTURE] Ready for gesture detection!");
    return true;
}

// ========================================
// Update (Polling)
// ========================================
void GestureSensor::update() {
    if (!_initialized || !_enabled) {
        return;
    }
    
    if (_inCursorMode) {
        // In cursor mode, don't poll for gestures
        return;
    }
    
    // Read gesture from sensor
    Gesture gesture = _sensor.readGesture();
    
    // Map library gesture to our enum
    switch (gesture) {
        case GES_UP:
            _lastGesture = GESTURE_UP;
            Serial.println("[GESTURE] ⬆️ UP detected");
            break;
        case GES_DOWN:
            _lastGesture = GESTURE_DOWN;
            Serial.println("[GESTURE] ⬇️ DOWN detected");
            break;
        case GES_LEFT:
            _lastGesture = GESTURE_LEFT;
            Serial.println("[GESTURE] ⬅️ LEFT detected");
            break;
        case GES_RIGHT:
            _lastGesture = GESTURE_RIGHT;
            Serial.println("[GESTURE] ➡️ RIGHT detected");
            break;
        case GES_FORWARD:
            _lastGesture = GESTURE_FORWARD;
            Serial.println("[GESTURE] ↪️ FORWARD detected");
            break;
        case GES_BACKWARD:
            _lastGesture = GESTURE_BACKWARD;
            Serial.println("[GESTURE] ↩️ BACKWARD detected");
            break;
        case GES_CLOCKWISE:
            _lastGesture = GESTURE_CLOCKWISE;
            Serial.println("[GESTURE] 🔄 CLOCKWISE detected");
            break;
        case GES_ANTICLOCKWISE:
            _lastGesture = GESTURE_ANTICLOCKWISE;
            Serial.println("[GESTURE] 🔃 ANTI-CLOCKWISE detected");
            break;
        case GES_WAVE:
            _lastGesture = GESTURE_WAVE;
            _waveCount = _sensor.getWaveCount();
            Serial.printf("[GESTURE] 👋 WAVE detected (%d waves)\n", _waveCount);
            break;
        default:
            // GES_NONE - no gesture
            _lastGesture = GESTURE_NONE;
            break;
    }
}

// ========================================
// Gesture Retrieval
// ========================================
GestureType GestureSensor::getLastGesture() {
    GestureType gesture = _lastGesture;
    _lastGesture = GESTURE_NONE;  // Clear after reading
    return gesture;
}

GestureAction GestureSensor::getActionForGesture(GestureType gesture) {
    switch (gesture) {
        case GESTURE_UP:        return ACTION_MENU_UP;
        case GESTURE_DOWN:      return ACTION_MENU_DOWN;
        case GESTURE_LEFT:      return ACTION_MENU_LEFT;
        case GESTURE_RIGHT:     return ACTION_MENU_RIGHT;
        case GESTURE_FORWARD:   return ACTION_SELECT;
        case GESTURE_BACKWARD:  return ACTION_BACK;
        case GESTURE_CLOCKWISE: return ACTION_SPECIAL_1;
        case GESTURE_ANTICLOCKWISE: return ACTION_SPECIAL_2;
        case GESTURE_WAVE:      return ACTION_WAKE;
        default:                return ACTION_NONE;
    }
}

// ========================================
// Status Methods
// ========================================
bool GestureSensor::isAvailable() {
    return _initialized;
}

void GestureSensor::setEnabled(bool enabled) {
    _enabled = enabled;
    if (_initialized) {
        if (enabled) {
            _sensor.enable();
            Serial.println("[GESTURE] Sensor enabled");
        } else {
            _sensor.disable();
            Serial.println("[GESTURE] Sensor disabled");
        }
    }
}

void GestureSensor::setGameMode(bool enabled) {
    if (!_initialized) return;
    
    _config.gameMode = enabled;
    if (enabled) {
        _sensor.setGameSpeed();
        Serial.println("[GESTURE] Game mode ON (240fps)");
    } else {
        _sensor.setNormalSpeed();
        Serial.println("[GESTURE] Normal mode (120fps)");
    }
}

int GestureSensor::getWaveCount() {
    return _waveCount;
}

// ========================================
// Object Detection
// ========================================
bool GestureSensor::isObjectInView() {
    if (!_initialized) return false;
    return _sensor.isObjectInView();
}

int GestureSensor::getObjectBrightness() {
    if (!_initialized) return 0;
    return _sensor.getObjectBrightness();
}

int GestureSensor::getObjectSize() {
    if (!_initialized) return 0;
    return _sensor.getObjectSize();
}

// ========================================
// Cursor Mode
// ========================================
void GestureSensor::enableCursorMode() {
    if (!_initialized) return;
    
    _sensor.setCursorMode();
    _inCursorMode = true;
    Serial.println("[GESTURE] Cursor mode enabled");
}

void GestureSensor::enableGestureMode() {
    if (!_initialized) return;
    
    _sensor.setGestureMode();
    _inCursorMode = false;
    Serial.println("[GESTURE] Gesture mode enabled");
}

int GestureSensor::getCursorX() {
    if (!_initialized || !_inCursorMode) return 0;
    return _sensor.getCursorX();
}

int GestureSensor::getCursorY() {
    if (!_initialized || !_inCursorMode) return 0;
    return _sensor.getCursorY();
}

// ========================================
// Deep Sleep Wake
// ========================================
void GestureSensor::configureDeepSleepWake() {
#if PIN_PAJ7620_INT >= 0
    // Enable ext0 wakeup on gesture interrupt (active LOW)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_PAJ7620_INT, 0);
    Serial.printf("[GESTURE] Deep sleep wake configured on GPIO %d\n", PIN_PAJ7620_INT);
#else
    Serial.println("[GESTURE] WARNING: No interrupt pin configured, deep sleep wake unavailable");
#endif
}

bool GestureSensor::wasWakeupByGesture() {
#if PIN_PAJ7620_INT >= 0
    esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
    return (wakeupCause == ESP_SLEEP_WAKEUP_EXT0);
#else
    return false;
#endif
}

// ========================================
// Configuration
// ========================================
GestureConfig GestureSensor::getConfig() {
    return _config;
}

void GestureSensor::setConfig(const GestureConfig& config) {
    _config = config;
    
    if (_initialized) {
        _sensor.setGestureEntryTime(config.entryTimeMs);
        _sensor.setGestureExitTime(config.exitTimeMs);
        
        if (config.gameMode) {
            _sensor.setGameSpeed();
        } else {
            _sensor.setNormalSpeed();
        }
        
        setEnabled(config.enabled);
    }
}

void GestureSensor::setTiming(uint16_t entryMs, uint16_t exitMs) {
    _config.entryTimeMs = entryMs;
    _config.exitTimeMs = exitMs;
    
    if (_initialized) {
        _sensor.setGestureEntryTime(entryMs);
        _sensor.setGestureExitTime(exitMs);
        Serial.printf("[GESTURE] Timing set: entry=%dms, exit=%dms\n", entryMs, exitMs);
    }
}

// ========================================
// Global Instance
// ========================================
GestureSensor gestureSensor;
