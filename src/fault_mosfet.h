/**
 * @file fault_mosfet.h
 * Monster S3 Firmware - Fault Injection Header
 */

#ifndef FAULT_MOSFET_H
#define FAULT_MOSFET_H

#include <stdint.h>

// Initialize fault injection hardware
void setup_fault_mosfet();

// Fire single glitch pulse (timing from trimmer)
void fault_pulse_trigger();

// Fire burst of pulses
void fault_burst_trigger(uint8_t count, uint16_t delay_ms);

// Fire pulse with custom width (µs)
void fault_pulse_custom(uint16_t pulse_us);

// Sweep for calibration
void fault_sweep_calibration();

// Get last pulse width (µs)
uint16_t fault_get_last_pulse();

// Get total fault count
uint32_t fault_get_count();

// Check hardware button
bool fault_button_pressed();

#endif // FAULT_MOSFET_H
