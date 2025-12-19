/**
 * @file joystick_driver.cpp
 * @brief Analog Joystick Driver Implementation for ESP32-S3
 * @details FreeRTOS task with 50ms polling, moving average filter,
 *          dead-zone handling, and navigation mapping
 * @version 1.0
 * @date 2025-12-19
 */

#include "joystick_driver.h"
#include <globals.h>
#include <Preferences.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#ifdef HAS_JOYSTICK

// ==============================================================================
// Global Driver Instance
// ==============================================================================
JoystickDriver joystick = {0};

// ADC Calibration handle
static esp_adc_cal_characteristics_t adc_chars;
static bool adc_calibrated = false;

// Debounce tracking
static uint32_t button_press_time = 0;
static bool button_last_state = false;

// ==============================================================================
// Private Helper Functions
// ==============================================================================

/**
 * @brief Initialize ADC for joystick reading
 */
static void init_adc(void) {
    // Configure ADC1 for VRx and VRy
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    // VRx on GPIO4 = ADC1_CHANNEL_3
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_12);
    
    // VRy on GPIO5 = ADC1_CHANNEL_4
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12);
    
    // Characterize ADC for better accuracy
    esp_adc_cal_value_t cal_type = esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_12,
        ADC_WIDTH_BIT_12,
        1100,  // Default Vref
        &adc_chars
    );
    
    if (cal_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        log_i("[JOYSTICK] ADC calibrated with eFuse Vref");
        adc_calibrated = true;
    } else if (cal_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        log_i("[JOYSTICK] ADC calibrated with Two Point");
        adc_calibrated = true;
    } else {
        log_w("[JOYSTICK] ADC using default calibration");
    }
}

/**
 * @brief Apply moving average filter
 */
static uint16_t apply_filter(uint16_t *buffer, uint8_t *idx, uint16_t new_value) {
    buffer[*idx] = new_value;
    *idx = (*idx + 1) % JOYSTICK_FILTER_SIZE;
    
    uint32_t sum = 0;
    for (int i = 0; i < JOYSTICK_FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / JOYSTICK_FILTER_SIZE;
}

/**
 * @brief Map value to normalized range with dead-zone
 */
static int8_t normalize_axis(uint16_t value, uint16_t center, uint16_t min, uint16_t max) {
    int16_t centered = (int16_t)value - (int16_t)center;
    
    // Apply dead-zone
    if (abs(centered) < JOYSTICK_DEADZONE) {
        return 0;
    }
    
    // Normalize to -100 to +100
    if (centered > 0) {
        // Positive direction
        int16_t range = max - center - JOYSTICK_DEADZONE;
        if (range <= 0) range = 1;
        int16_t adjusted = centered - JOYSTICK_DEADZONE;
        return (int8_t)constrain((adjusted * 100) / range, 0, 100);
    } else {
        // Negative direction
        int16_t range = center - min - JOYSTICK_DEADZONE;
        if (range <= 0) range = 1;
        int16_t adjusted = centered + JOYSTICK_DEADZONE;
        return (int8_t)constrain((adjusted * 100) / range, -100, 0);
    }
}

/**
 * @brief Determine direction flags from normalized values
 */
static void determine_direction(JoystickState *state) {
    // Calculate threshold in normalized units
    const int8_t threshold = 30;  // 30% of range
    
    state->up = state->norm_y > threshold;
    state->down = state->norm_y < -threshold;
    state->right = state->norm_x > threshold;
    state->left = state->norm_x < -threshold;
    
    state->any = state->up || state->down || state->left || state->right || state->button;
}

/**
 * @brief Process button with debounce and long-press detection
 */
static void process_button(JoystickState *state) {
    bool current = (digitalRead(JOYSTICK_SW_PIN) == LOW);  // Active low
    uint32_t now = millis();
    
    // Debounce
    static uint32_t last_change = 0;
    if (current != button_last_state) {
        if (now - last_change < JOYSTICK_DEBOUNCE_MS) {
            return;  // Ignore bouncing
        }
        last_change = now;
    }
    
    // Edge detection
    state->button_pressed = (current && !button_last_state);
    state->button_released = (!current && button_last_state);
    
    if (state->button_pressed) {
        button_press_time = now;
    }
    
    // Long press detection
    state->long_press = false;
    if (current && (now - button_press_time > JOYSTICK_LONGPRESS_MS)) {
        state->long_press = true;
    }
    
    state->button = current;
    button_last_state = current;
}

// ==============================================================================
// FreeRTOS Task
// ==============================================================================

/**
 * @brief Joystick polling task (runs on Core 1)
 */
static void taskJoystickHandler(void *parameter) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(JOYSTICK_POLL_MS);
    
    log_i("[JOYSTICK] Task started on Core %d", xPortGetCoreID());
    
    while (true) {
        if (joystick.enabled) {
            joystick_read(&joystick.state);
            
            // Map to global navigation variables
            if (joystick.state.up && !UpPress) UpPress = true;
            if (joystick.state.down && !DownPress) DownPress = true;
            if (joystick.state.left && !PrevPress) PrevPress = true;
            if (joystick.state.right && !NextPress) NextPress = true;
            if (joystick.state.button_pressed && !SelPress) SelPress = true;
            if (joystick.state.long_press && !EscPress) EscPress = true;
            if (joystick.state.any && !AnyKeyPress) AnyKeyPress = true;
        }
        
        // Wait for next cycle (precise timing)
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ==============================================================================
// Public API Implementation
// ==============================================================================

bool joystick_init(void) {
    log_i("[JOYSTICK] Initializing driver...");
    
    // Initialize ADC
    init_adc();
    
    // Configure button pin with internal pull-up
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
    
    // Initialize filter buffers with center values
    for (int i = 0; i < JOYSTICK_FILTER_SIZE; i++) {
        joystick.filter_x[i] = JOYSTICK_ADC_CENTER;
        joystick.filter_y[i] = JOYSTICK_ADC_CENTER;
    }
    joystick.filter_idx = 0;
    
    // Set default calibration
    joystick_reset_calibration();
    
    // Try to load saved calibration
    joystick_load_calibration();
    
    // Enable driver
    joystick.enabled = true;
    
    // Create FreeRTOS task on Core 1 (RF/Sensor core)
    BaseType_t result = xTaskCreatePinnedToCore(
        taskJoystickHandler,
        "Joystick",
        2048,                           // Stack size
        NULL,
        2,                              // Priority (above idle)
        &joystick.task_handle,
        1                               // Core 1
    );
    
    if (result != pdPASS) {
        log_e("[JOYSTICK] Failed to create task!");
        return false;
    }
    
    log_i("[JOYSTICK] Driver initialized successfully");
    log_i("  VRx: GPIO%d, VRy: GPIO%d, SW: GPIO%d", 
          JOYSTICK_VRX_PIN, JOYSTICK_VRY_PIN, JOYSTICK_SW_PIN);
    log_i("  Dead-zone: %d, Poll: %dms", JOYSTICK_DEADZONE, JOYSTICK_POLL_MS);
    
    return true;
}

void joystick_deinit(void) {
    if (joystick.task_handle != NULL) {
        vTaskDelete(joystick.task_handle);
        joystick.task_handle = NULL;
    }
    joystick.enabled = false;
    log_i("[JOYSTICK] Driver de-initialized");
}

void IRAM_ATTR joystick_read(JoystickState *state) {
    // Read raw ADC values
    uint16_t raw_x = adc1_get_raw(ADC1_CHANNEL_3);  // VRx
    uint16_t raw_y = adc1_get_raw(ADC1_CHANNEL_4);  // VRy
    
    state->raw_x = raw_x;
    state->raw_y = raw_y;
    
    // Apply moving average filter
    state->filtered_x = apply_filter(joystick.filter_x, &joystick.filter_idx, raw_x);
    state->filtered_y = apply_filter(joystick.filter_y, &joystick.filter_idx, raw_y);
    
    // Normalize with calibration
    state->norm_x = normalize_axis(
        state->filtered_x,
        joystick.calibration.center_x,
        joystick.calibration.min_x,
        joystick.calibration.max_x
    );
    
    state->norm_y = normalize_axis(
        state->filtered_y,
        joystick.calibration.center_y,
        joystick.calibration.min_y,
        joystick.calibration.max_y
    );
    
    // Determine directions
    determine_direction(state);
    
    // Process button
    process_button(state);
    
    // Update timestamp
    state->last_update = millis();
}

void joystick_get_state(JoystickState *state) {
    // Thread-safe copy
    taskENTER_CRITICAL(&mux);
    memcpy(state, &joystick.state, sizeof(JoystickState));
    taskEXIT_CRITICAL(&mux);
}

void joystick_calibrate(void) {
    log_i("[JOYSTICK] Starting calibration...");
    
    // Disable task during calibration
    bool was_enabled = joystick.enabled;
    joystick.enabled = false;
    vTaskDelay(pdMS_TO_TICKS(100));
    
    JoystickCalibration cal = {0};
    
    // Read center position (average of 10 samples)
    uint32_t sum_x = 0, sum_y = 0;
    for (int i = 0; i < 10; i++) {
        sum_x += adc1_get_raw(ADC1_CHANNEL_3);
        sum_y += adc1_get_raw(ADC1_CHANNEL_4);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    cal.center_x = sum_x / 10;
    cal.center_y = sum_y / 10;
    
    log_i("[JOYSTICK] Center: X=%d, Y=%d", cal.center_x, cal.center_y);
    
    // Set reasonable defaults for min/max based on center
    // User should move stick to extremes for full calibration
    cal.min_x = max(0, (int)cal.center_x - 2000);
    cal.max_x = min(4095, (int)cal.center_x + 2000);
    cal.min_y = max(0, (int)cal.center_y - 2000);
    cal.max_y = min(4095, (int)cal.center_y + 2000);
    
    cal.calibrated = true;
    
    // Apply calibration
    joystick.calibration = cal;
    
    // Save to NVS
    joystick_save_calibration();
    
    // Re-enable if was enabled
    joystick.enabled = was_enabled;
    
    log_i("[JOYSTICK] Calibration complete");
}

bool joystick_load_calibration(void) {
    Preferences prefs;
    if (!prefs.begin("joystick", true)) {  // Read-only
        return false;
    }
    
    if (!prefs.isKey("calibrated")) {
        prefs.end();
        return false;
    }
    
    joystick.calibration.calibrated = prefs.getBool("calibrated", false);
    joystick.calibration.center_x = prefs.getUShort("center_x", JOYSTICK_ADC_CENTER);
    joystick.calibration.center_y = prefs.getUShort("center_y", JOYSTICK_ADC_CENTER);
    joystick.calibration.min_x = prefs.getUShort("min_x", 0);
    joystick.calibration.max_x = prefs.getUShort("max_x", JOYSTICK_ADC_MAX);
    joystick.calibration.min_y = prefs.getUShort("min_y", 0);
    joystick.calibration.max_y = prefs.getUShort("max_y", JOYSTICK_ADC_MAX);
    
    prefs.end();
    
    log_i("[JOYSTICK] Loaded calibration from NVS");
    return joystick.calibration.calibrated;
}

bool joystick_save_calibration(void) {
    Preferences prefs;
    if (!prefs.begin("joystick", false)) {  // Read-write
        log_e("[JOYSTICK] Failed to open NVS for writing");
        return false;
    }
    
    prefs.putBool("calibrated", joystick.calibration.calibrated);
    prefs.putUShort("center_x", joystick.calibration.center_x);
    prefs.putUShort("center_y", joystick.calibration.center_y);
    prefs.putUShort("min_x", joystick.calibration.min_x);
    prefs.putUShort("max_x", joystick.calibration.max_x);
    prefs.putUShort("min_y", joystick.calibration.min_y);
    prefs.putUShort("max_y", joystick.calibration.max_y);
    
    prefs.end();
    
    log_i("[JOYSTICK] Saved calibration to NVS");
    return true;
}

void joystick_reset_calibration(void) {
    joystick.calibration.calibrated = false;
    joystick.calibration.center_x = JOYSTICK_ADC_CENTER;
    joystick.calibration.center_y = JOYSTICK_ADC_CENTER;
    joystick.calibration.min_x = 0;
    joystick.calibration.max_x = JOYSTICK_ADC_MAX;
    joystick.calibration.min_y = 0;
    joystick.calibration.max_y = JOYSTICK_ADC_MAX;
    log_i("[JOYSTICK] Calibration reset to defaults");
}

void joystick_enable(bool enable) {
    joystick.enabled = enable;
}

bool joystick_is_enabled(void) {
    return joystick.enabled;
}

uint16_t joystick_get_raw_x(void) {
    return adc1_get_raw(ADC1_CHANNEL_3);
}

uint16_t joystick_get_raw_y(void) {
    return adc1_get_raw(ADC1_CHANNEL_4);
}

bool joystick_button_pressed(void) {
    return digitalRead(JOYSTICK_SW_PIN) == LOW;
}

void joystick_debug_print(void) {
    JoystickState state;
    joystick_get_state(&state);
    
    Serial.printf("[JOYSTICK] Raw: X=%4d Y=%4d | Filt: X=%4d Y=%4d | Norm: X=%+4d Y=%+4d | ",
        state.raw_x, state.raw_y,
        state.filtered_x, state.filtered_y,
        state.norm_x, state.norm_y);
    
    Serial.print("Dir: ");
    if (state.up) Serial.print("UP ");
    if (state.down) Serial.print("DN ");
    if (state.left) Serial.print("LT ");
    if (state.right) Serial.print("RT ");
    if (state.button) Serial.print("BTN ");
    if (state.long_press) Serial.print("LONG ");
    Serial.println();
}

#else
// Stub implementations when JOYSTICK is not enabled
bool joystick_init(void) { return false; }
void joystick_deinit(void) {}
void joystick_read(JoystickState *state) { memset(state, 0, sizeof(JoystickState)); }
void joystick_get_state(JoystickState *state) { memset(state, 0, sizeof(JoystickState)); }
void joystick_calibrate(void) {}
bool joystick_load_calibration(void) { return false; }
bool joystick_save_calibration(void) { return false; }
void joystick_reset_calibration(void) {}
void joystick_enable(bool enable) {}
bool joystick_is_enabled(void) { return false; }
uint16_t joystick_get_raw_x(void) { return 0; }
uint16_t joystick_get_raw_y(void) { return 0; }
bool joystick_button_pressed(void) { return false; }
void joystick_debug_print(void) {}
#endif // HAS_JOYSTICK
