/**
 * @file fault_mosfet.cpp
 * @brief Fault Injection Magnético - MOSFET IRF520
 * 
 * Hardware:
 * - MOSFET IRF520 no GPIO 37
 * - Capacitor 180uF 450V
 * - Bobina 2.5mm² 15-20 voltas (5cm diâmetro)
 * - Pulso 5ms para glitch em Mifare Classic
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "fault_mosfet.h"
#include "pin_config.h"
#include "tts_espeak.h"
#include <Arduino.h>

// Estado do fault injection
static bool fault_armed = false;
static uint32_t fault_count = 0;
static uint32_t last_fault_time = 0;

// Configurações de pulso
static uint16_t pulse_width_us = 5000;   // 5ms default
static uint8_t pulse_count = 1;          // Número de pulsos

/**
 * @brief Inicializa o sistema de fault injection
 */
void setup_fault_mosfet() {
    // GPIO do MOSFET
    pinMode(PIN_FAULT_GATE, OUTPUT);
    digitalWrite(PIN_FAULT_GATE, LOW);
    
    // Botão de trigger (GPIO0 = Boot button)
    pinMode(PIN_BTN_FAULT, INPUT_PULLUP);
    
    // LED de status (RGB onboard)
    pinMode(PIN_RGB_LED, OUTPUT);
    digitalWrite(PIN_RGB_LED, LOW);
    
    fault_armed = true;
    Serial.println("[FAULT] MOSFET IRF520 pronto no GPIO 37");
    Serial.println("[FAULT] Pressione BOOT para injetar falha");
}

/**
 * @brief Dispara pulso de fault injection
 * @return true se pulso foi disparado
 */
bool fault_pulse_trigger() {
    if (!fault_armed) {
        Serial.println("[FAULT] Sistema não armado!");
        return false;
    }
    
    // Desabilita interrupções para timing preciso
    portDISABLE_INTERRUPTS();
    
    // Sequência de pulsos
    for (int p = 0; p < pulse_count; p++) {
        // PULSO ON
        digitalWrite(PIN_FAULT_GATE, HIGH);
        
        // Delay preciso em microsegundos
        delayMicroseconds(pulse_width_us);
        
        // PULSO OFF
        digitalWrite(PIN_FAULT_GATE, LOW);
        
        // Pausa entre pulsos (se múltiplos)
        if (p < pulse_count - 1) {
            delayMicroseconds(1000);  // 1ms entre pulsos
        }
    }
    
    portENABLE_INTERRUPTS();
    
    // Atualiza contadores
    fault_count++;
    last_fault_time = millis();
    
    Serial.printf("[FAULT] Glitch #%lu disparado! Pulso: %uus x%d\n", 
                  fault_count, pulse_width_us, pulse_count);
    
    // LED vermelho pisca 3x
    for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_RGB_LED, HIGH);
        delay(100);
        digitalWrite(PIN_RGB_LED, LOW);
        delay(100);
    }
    
    // Voz: "Falha injetada"
    tts_speak("falha_injetada");
    
    return true;
}

/**
 * @brief Dispara pulso com duração customizada
 * @param duration_us Duração em microsegundos
 */
void fault_pulse_custom(uint16_t duration_us) {
    uint16_t old_width = pulse_width_us;
    pulse_width_us = duration_us;
    fault_pulse_trigger();
    pulse_width_us = old_width;
}

/**
 * @brief Configura largura do pulso
 * @param us Duração em microsegundos (100 - 50000)
 */
void fault_set_pulse_width(uint16_t us) {
    if (us >= 100 && us <= 50000) {
        pulse_width_us = us;
        Serial.printf("[FAULT] Largura do pulso: %uus\n", pulse_width_us);
    }
}

/**
 * @brief Configura número de pulsos
 * @param count Número de pulsos (1 - 10)
 */
void fault_set_pulse_count(uint8_t count) {
    if (count >= 1 && count <= 10) {
        pulse_count = count;
        Serial.printf("[FAULT] Pulsos por disparo: %d\n", pulse_count);
    }
}

/**
 * @brief Retorna contagem de faults disparados
 */
uint32_t fault_get_count() {
    return fault_count;
}

/**
 * @brief Arma/Desarma o sistema
 */
void fault_arm(bool arm) {
    fault_armed = arm;
    Serial.printf("[FAULT] Sistema %s\n", arm ? "ARMADO" : "desarmado");
}

/**
 * @brief Verifica se sistema está armado
 */
bool fault_is_armed() {
    return fault_armed;
}

/**
 * @brief Loop de verificação do botão de trigger
 * Chame no loop principal ou em task separada
 */
void fault_check_button() {
    static uint32_t last_press = 0;
    static bool was_pressed = false;
    
    bool pressed = (digitalRead(PIN_BTN_FAULT) == LOW);
    
    // Debounce 200ms
    if (pressed && !was_pressed && (millis() - last_press > 200)) {
        last_press = millis();
        fault_pulse_trigger();
    }
    
    was_pressed = pressed;
}

/**
 * @brief Brute force de pulsos para encontrar timing correto
 * @param start_us Início em us
 * @param end_us Fim em us
 * @param step_us Passo em us
 * @param delay_ms Delay entre tentativas
 */
void fault_brute_force(uint16_t start_us, uint16_t end_us, uint16_t step_us, uint16_t delay_ms) {
    Serial.printf("[FAULT] Brute Force: %u - %u us (step %u)\n", start_us, end_us, step_us);
    
    for (uint16_t width = start_us; width <= end_us; width += step_us) {
        fault_pulse_custom(width);
        delay(delay_ms);
        
        // TODO: Verificar se NFC respondeu diferente após glitch
        // Se cartão Mifare Classic retornar dados inesperados = sucesso
    }
    
    Serial.println("[FAULT] Brute Force concluído");
    tts_speak("brute_force_completo");
}
