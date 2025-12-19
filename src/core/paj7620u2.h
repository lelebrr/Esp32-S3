/**
 * @file paj7620u2.h
 * @brief PAJ7620U2 Gesture Sensor Driver
 * 
 * Features:
 * - I2C interface (addr 0x73)
 * - 9 gesture detection (up/down/left/right/forward/backward/clockwise/counterclockwise/wave)
 * - Menu navigation integration
 * - Deep sleep wake source via interrupt
 * 
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef PAJ7620U2_H
#define PAJ7620U2_H

#include <Arduino.h>
#include <Wire.h>

#ifdef HAS_GESTURE

// ============================================================================
// I2C CONFIGURATION
// ============================================================================

#ifndef PAJ7620_ADDR
#define PAJ7620_ADDR          0x73
#endif

// Register addresses
#define PAJ7620_BANK_SEL      0xEF
#define PAJ7620_CHIP_ID       0x00
#define PAJ7620_INT_FLAG1     0x43
#define PAJ7620_INT_FLAG2     0x44

// ============================================================================
// GESTURE CODES
// ============================================================================

typedef enum {
    GESTURE_NONE              = 0x00,
    GESTURE_RIGHT             = 0x01,
    GESTURE_LEFT              = 0x02,
    GESTURE_UP                = 0x04,
    GESTURE_DOWN              = 0x08,
    GESTURE_FORWARD           = 0x10,
    GESTURE_BACKWARD          = 0x20,
    GESTURE_CLOCKWISE         = 0x40,
    GESTURE_COUNTERCLOCKWISE  = 0x80,
    GESTURE_WAVE              = 0x100
} paj7620_gesture_t;

// ============================================================================
// SENSOR STATE
// ============================================================================

typedef struct {
    bool initialized;
    uint8_t chipId;
    paj7620_gesture_t lastGesture;
    uint32_t gestureCount;
} paj7620_state_t;

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

/**
 * @brief Initialize PAJ7620U2 sensor
 * @param wire I2C instance (default Wire)
 * @return true on success
 */
bool paj7620_init(TwoWire* wire = &Wire);

/**
 * @brief Read current gesture (non-blocking)
 * @return Detected gesture or GESTURE_NONE
 */
paj7620_gesture_t paj7620_get_gesture();

/**
 * @brief Get last detected gesture
 */
paj7620_gesture_t paj7620_get_last_gesture();

/**
 * @brief Get gesture name as string
 */
const char* paj7620_gesture_name(paj7620_gesture_t gesture);

/**
 * @brief Enable interrupt output for wake source
 * Configures INT pin to go low on gesture detection
 */
void paj7620_enable_interrupt();

/**
 * @brief Disable interrupt output
 */
void paj7620_disable_interrupt();

/**
 * @brief Enter low power mode
 */
void paj7620_sleep();

/**
 * @brief Wake from low power mode
 */
void paj7620_wake();

/**
 * @brief Get sensor state
 */
paj7620_state_t* paj7620_get_state();

/**
 * @brief Map gesture to navigation action
 * @return 0=none, 1=up, 2=down, 3=left/back, 4=right/select
 */
uint8_t paj7620_to_nav_action(paj7620_gesture_t gesture);

#endif // HAS_GESTURE

#endif // PAJ7620U2_H
