/**
 * @file joystick_driver.h
 * @brief Analog Joystick Driver for ESP32-S3
 * @details FreeRTOS-based driver with 50ms polling, dead-zone filtering,
 *          calibration support, and IRAM optimization
 * @version 1.0
 * @date 2025-12-19
 */

#ifndef JOYSTICK_DRIVER_H
#define JOYSTICK_DRIVER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef __cplusplus
extern "C" {
#endif

// ==============================================================================
// Pin Definitions (overridable via platformio.ini)
// ==============================================================================
#ifndef JOYSTICK_VRX_PIN
#define JOYSTICK_VRX_PIN    4       // ADC1_CH3 (GPIO4)
#endif

#ifndef JOYSTICK_VRY_PIN
#define JOYSTICK_VRY_PIN    5       // ADC1_CH4 (GPIO5)
#endif

#ifndef JOYSTICK_SW_PIN
#define JOYSTICK_SW_PIN     0       // Boot button (internal pull-up)
#endif

// ==============================================================================
// Configuration Constants
// ==============================================================================
#ifndef JOYSTICK_DEADZONE
#define JOYSTICK_DEADZONE   150     // Dead-zone radius from center
#endif

#ifndef JOYSTICK_POLL_MS
#define JOYSTICK_POLL_MS    50      // Polling interval in milliseconds
#endif

#define JOYSTICK_ADC_MAX    4095    // 12-bit ADC resolution
#define JOYSTICK_ADC_CENTER 2048    // Default center value
#define JOYSTICK_THRESHOLD  800     // Threshold for direction detection

#define JOYSTICK_DEBOUNCE_MS    50  // Button debounce time
#define JOYSTICK_LONGPRESS_MS   500 // Long press threshold

// Filter settings
#define JOYSTICK_FILTER_SIZE    4   // Moving average filter size

// ==============================================================================
// Data Structures
// ==============================================================================

/**
 * @brief Joystick calibration data
 */
typedef struct {
    uint16_t center_x;      // Center X value (default 2048)
    uint16_t center_y;      // Center Y value (default 2048)
    uint16_t min_x;         // Minimum X (left)
    uint16_t max_x;         // Maximum X (right)
    uint16_t min_y;         // Minimum Y (down)
    uint16_t max_y;         // Maximum Y (up)
    bool calibrated;        // Calibration complete flag
} JoystickCalibration;

/**
 * @brief Joystick raw and processed state
 */
typedef struct {
    // Raw ADC values
    uint16_t raw_x;
    uint16_t raw_y;
    
    // Filtered values (moving average)
    uint16_t filtered_x;
    uint16_t filtered_y;
    
    // Normalized values (-100 to +100)
    int8_t norm_x;
    int8_t norm_y;
    
    // Direction flags
    bool up;
    bool down;
    bool left;
    bool right;
    
    // Button states
    bool button;            // Current button state
    bool button_pressed;    // Rising edge (just pressed)
    bool button_released;   // Falling edge (just released)
    bool long_press;        // Long press detected
    
    // Meta
    bool any;               // Any direction or button active
    uint32_t last_update;   // Last update timestamp (millis)
} JoystickState;

/**
 * @brief Joystick driver handle
 */
typedef struct {
    JoystickState state;
    JoystickCalibration calibration;
    TaskHandle_t task_handle;
    volatile bool enabled;
    uint16_t filter_x[JOYSTICK_FILTER_SIZE];
    uint16_t filter_y[JOYSTICK_FILTER_SIZE];
    uint8_t filter_idx;
} JoystickDriver;

// ==============================================================================
// Global Driver Instance
// ==============================================================================
extern JoystickDriver joystick;

// ==============================================================================
// Public API Functions
// ==============================================================================

/**
 * @brief Initialize joystick driver
 * @return true if successful, false on error
 */
bool joystick_init(void);

/**
 * @brief De-initialize joystick driver
 */
void joystick_deinit(void);

/**
 * @brief Read joystick state (called by FreeRTOS task)
 * @param state Pointer to state structure to fill
 * @note Uses IRAM_ATTR for speed
 */
void IRAM_ATTR joystick_read(JoystickState *state);

/**
 * @brief Get current joystick state (thread-safe copy)
 * @param state Pointer to state structure to fill
 */
void joystick_get_state(JoystickState *state);

/**
 * @brief Start interactive calibration
 * @note Will update internal calibration values
 */
void joystick_calibrate(void);

/**
 * @brief Load calibration from NVS
 * @return true if calibration loaded, false if using defaults
 */
bool joystick_load_calibration(void);

/**
 * @brief Save calibration to NVS
 * @return true if successful
 */
bool joystick_save_calibration(void);

/**
 * @brief Reset calibration to defaults
 */
void joystick_reset_calibration(void);

/**
 * @brief Enable/disable joystick input processing
 * @param enable true to enable, false to disable
 */
void joystick_enable(bool enable);

/**
 * @brief Check if joystick is enabled
 * @return true if enabled
 */
bool joystick_is_enabled(void);

/**
 * @brief Get raw X value (no filtering)
 * @return 0-4095 raw ADC value
 */
uint16_t joystick_get_raw_x(void);

/**
 * @brief Get raw Y value (no filtering)
 * @return 0-4095 raw ADC value
 */
uint16_t joystick_get_raw_y(void);

/**
 * @brief Check if button is currently pressed
 * @return true if pressed
 */
bool joystick_button_pressed(void);

// ==============================================================================
// Debug Functions
// ==============================================================================

/**
 * @brief Print joystick debug info to serial
 */
void joystick_debug_print(void);

#ifdef __cplusplus
}
#endif

#endif // JOYSTICK_DRIVER_H
