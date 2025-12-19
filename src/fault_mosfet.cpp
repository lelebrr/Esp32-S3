#include "fault_mosfet.h"

// Pino do MOSFET de glitch (externo)
#define GLITCH_PIN 42 

void setup_fault_mosfet() {
    pinMode(GLITCH_PIN, OUTPUT);
    digitalWrite(GLITCH_PIN, LOW);
}

void fault_pulse_trigger() {
    // Sequência de Glitch:
    // 1. Prepara
    noInterrupts();
    
    // 2. Dispara Pulso Curto (5ms)
    digitalWrite(GLITCH_PIN, HIGH);
    delay(5); // 5ms
    digitalWrite(GLITCH_PIN, LOW);
    
    // 3. Restaura
    interrupts();
    
    Serial.println("[FAULT] Glitch INJECTED!");
}
