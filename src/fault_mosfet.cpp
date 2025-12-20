/**
 * @file fault_mosfet.cpp
 * Monster S3 Firmware - Fault Injection via MOSFET
 * 
 * Voltage glitching for NFC/smartcard attacks
 * Uses IRF520 MOSFET with variable pulse timing via trimmer cap
 */

#include "fault_mosfet.h"
#include "pin_config.h"
#include <Arduino.h>

// Configuration
static bool fault_initialized = false;
static uint16_t last_pulse_us = 0;
static uint32_t fault_count = 0;

// Timing bounds (microseconds)
static const uint16_t PULSE_MIN_US = 50;    // 50µs minimum
static const uint16_t PULSE_MAX_US = 5000;  // 5ms maximum
static const uint16_t PULSE_DEFAULT_US = 500; // 500µs default

/**
 * @brief Initialize fault injection hardware
 */
void setup_fault_mosfet() {
    // MOSFET gate output
    if (PIN_FAULT_GATE >= 0) {
        pinMode(PIN_FAULT_GATE, OUTPUT);
        digitalWrite(PIN_FAULT_GATE, LOW);
    }
    
    // Fault trigger button (hardware trigger)
    pinMode(PIN_BTN_FAULT, INPUT_PULLUP);
    
    // Trimmer cap ADC for variable timing (if configured)
    #ifdef PIN_TRIMMER_CAP
    if (PIN_TRIMMER_CAP >= 0) {
        pinMode(PIN_TRIMMER_CAP, INPUT);
    }
    #endif
    
    fault_initialized = true;
    fault_count = 0;
    Serial.println("[FAULT] Glitch injector initialized");
}

/**
 * @brief Read trimmer cap to determine pulse width
 * @return Pulse width in microseconds
 */
static uint16_t read_pulse_width() {
    #ifdef PIN_TRIMMER_CAP
    if (PIN_TRIMMER_CAP >= 0) {
        int adc_val = analogRead(PIN_TRIMMER_CAP);
        // Map 0-4095 ADC to pulse range
        uint16_t pulse = map(adc_val, 0, 4095, PULSE_MIN_US, PULSE_MAX_US);
        return pulse;
    }
    #endif
    return PULSE_DEFAULT_US;
}

/**
 * @brief Fire a single glitch pulse
 * Timing is determined by trimmer cap position
 */
void fault_pulse_trigger() {
    if (!fault_initialized || PIN_FAULT_GATE < 0) {
        Serial.println("[FAULT] Not initialized or no gate pin");
        return;
    }
    
    // Get pulse width from trimmer
    uint16_t pulse_us = read_pulse_width();
    last_pulse_us = pulse_us;
    
    // Critical section - disable interrupts for precise timing
    noInterrupts();
    
    // Fire glitch pulse
    digitalWrite(PIN_FAULT_GATE, HIGH);
    delayMicroseconds(pulse_us);
    digitalWrite(PIN_FAULT_GATE, LOW);
    
    interrupts();
    
    fault_count++;
    Serial.printf("[FAULT] Glitch fired! Pulse: %dµs (count: %d)\n", pulse_us, fault_count);
}

/**
 * @brief Fire a burst of glitch pulses
 * @param count Number of pulses
 * @param delay_ms Delay between pulses in milliseconds
 */
void fault_burst_trigger(uint8_t count, uint16_t delay_ms) {
    if (!fault_initialized) return;
    
    Serial.printf("[FAULT] Burst: %d pulses @ %dms interval\n", count, delay_ms);
    
    for (uint8_t i = 0; i < count && i < 100; i++) {
        fault_pulse_trigger();
        if (delay_ms > 0 && i < count - 1) {
            delay(delay_ms);
        }
    }
}

/**
 * @brief Fire pulse with specific width (override trimmer)
 * @param pulse_us Pulse width in microseconds
 */
void fault_pulse_custom(uint16_t pulse_us) {
    if (!fault_initialized || PIN_FAULT_GATE < 0) return;
    
    // Clamp to safe range
    if (pulse_us < PULSE_MIN_US) pulse_us = PULSE_MIN_US;
    if (pulse_us > PULSE_MAX_US) pulse_us = PULSE_MAX_US;
    
    noInterrupts();
    digitalWrite(PIN_FAULT_GATE, HIGH);
    delayMicroseconds(pulse_us);
    digitalWrite(PIN_FAULT_GATE, LOW);
    interrupts();
    
    fault_count++;
    last_pulse_us = pulse_us;
    Serial.printf("[FAULT] Custom glitch: %dµs\n", pulse_us);
}

/**
 * @brief Sweep pulse widths for calibration
 * Useful for finding optimal glitch timing
 */
void fault_sweep_calibration() {
    if (!fault_initialized) return;
    
    Serial.println("[FAULT] Starting calibration sweep...");
    
    for (uint16_t pulse = PULSE_MIN_US; pulse <= PULSE_MAX_US; pulse += 50) {
        fault_pulse_custom(pulse);
        delay(100); // Allow target to recover
    }
    
    Serial.println("[FAULT] Calibration sweep complete");
}

/**
 * @brief Get last pulse width used
 */
uint16_t fault_get_last_pulse() {
    return last_pulse_us;
}

/**
 * @brief Get total fault count
 */
uint32_t fault_get_count() {
    return fault_count;
}

/**
 * @brief Check if hardware fault trigger button is pressed
 */
bool fault_button_pressed() {
    return digitalRead(PIN_BTN_FAULT) == LOW;
}
