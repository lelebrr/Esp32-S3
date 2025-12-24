/**
 * @file fault_mosfet.h
 * @brief Fault Injection Magnético - MOSFET IRF520
 */

#ifndef FAULT_MOSFET_H
#define FAULT_MOSFET_H

#include <Arduino.h>

// Inicialização
void setup_fault_mosfet();

// Disparo de pulso
bool fault_pulse_trigger();
void fault_pulse_custom(uint16_t duration_us);

// Configuração
void fault_set_pulse_width(uint16_t us);
void fault_set_pulse_count(uint8_t count);

// Estado
uint32_t fault_get_count();
void fault_arm(bool arm);
bool fault_is_armed();

// Loop
void fault_check_button();

// Brute force
void fault_brute_force(uint16_t start_us, uint16_t end_us, uint16_t step_us, uint16_t delay_ms);

#endif // FAULT_MOSFET_H
