/**
 * @file gesture_wake.cpp
 * @brief Deep sleep implementation with gesture and timer wake sources
 * 
 * Uses ESP32-S3 deep sleep capabilities for <5µA idle consumption.
 * Wake sources:
 * - EXT0: Touch/gesture interrupt (low-level trigger)
 * - Timer: Periodic wake for housekeeping
 */

#include "gesture_wake.h"
#include "power_gating.h"
#include <globals.h>
#include <driver/gpio.h>

// Wake pin configuration
#ifdef TOUCH_IRQ
    #define WAKE_GPIO_PIN ((gpio_num_t)TOUCH_IRQ)
#else
    #define WAKE_GPIO_PIN GPIO_NUM_8
#endif

void gesture_wake_init() {
    // Configure wake pin as input with pull-up
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << WAKE_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    
    // Enable EXT0 wakeup on low level (gesture interrupt is active-low)
    esp_sleep_enable_ext0_wakeup(WAKE_GPIO_PIN, 0);
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.printf("[SLEEP] Gesture wake configured on GPIO %d\n", WAKE_GPIO_PIN);
    #endif
}

void prepare_for_deep_sleep() {
    #ifdef HAS_MOSFET
    // Power off all gated modules
    power_all_modules_off();
    #endif
    
    #ifdef TFT_BL
    // Turn off display backlight
    digitalWrite(TFT_BL, LOW);
    #endif
    
    #ifdef RGB_LED
    // Turn off RGB LEDs
    digitalWrite(RGB_LED, LOW);
    #endif
    
    // Flush serial buffers
    Serial.flush();
    delay(10);
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[SLEEP] Entering deep sleep...");
    Serial.flush();
    #endif
}

void enter_deep_sleep_with_gesture() {
    prepare_for_deep_sleep();
    gesture_wake_init();
    esp_deep_sleep_start();
}

void enter_deep_sleep_with_timer(uint64_t sleep_us) {
    prepare_for_deep_sleep();
    esp_sleep_enable_timer_wakeup(sleep_us);
    esp_deep_sleep_start();
}

void enter_deep_sleep_multi(uint64_t sleep_us, bool enable_gesture) {
    prepare_for_deep_sleep();
    
    if (enable_gesture) {
        gesture_wake_init();
    }
    
    if (sleep_us > 0) {
        esp_sleep_enable_timer_wakeup(sleep_us);
    }
    
    esp_deep_sleep_start();
}

esp_sleep_wakeup_cause_t get_wake_cause() {
    return esp_sleep_get_wakeup_cause();
}

`nvoid process_gesture_wake() {`n    esp_sleep_wakeup_cause_t cause = get_wake_cause();`n    `n    if (cause == ESP_SLEEP_WAKEUP_EXT0) {`n        // Wake word detected via gesture`n        if (is_wake_word_enabled()) {`n            process_wake_word();`n        }`n    } else if (cause == ESP_SLEEP_WAKEUP_TIMER) {`n        // Timer wake - check for pending speech`n        // Silent wake, no TTS`n    }`n}`n`nvoid gesture_wake_with_tts() {`n    enter_deep_sleep_with_gesture();`n    process_gesture_wake();`n}
