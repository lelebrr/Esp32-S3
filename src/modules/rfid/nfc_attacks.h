#pragma once

// NFC Attack Functions - Advanced Pulse Injection Techniques

// Traditional attacks
void nfc_clone_card();
void nfc_phishing_tag();
void nfc_ota_rewrite();
void nfc_fake_apple_pay();
void nfc_audio_injection();

// Advanced pulse injection attacks
void nfc_pulse_injection_claw_machine();  // Crédito fantasma em máquinas de ursinho
void nfc_pulse_injection_time_clock();     // Choque em relógio de ponto
void nfc_pulse_injection_bus_card();       // Overflow em cartão de ônibus
void nfc_pulse_injection_reverse_credit(); // Zera saldo alheio (ética limitada)
void nfc_pulse_injection_ghost_reader();   // Simula máquina de validação

// Pulse injection configuration
#define BURST_PIN 28           // GPIO para controlar antena externa
#define BURST_FREQUENCY 50     // 50 bursts por segundo
#define BURST_DURATION_MS 1000 // 1 segundo de burst
#define PULSE_INTENSITY_MAX 100 // Intensidade máxima
