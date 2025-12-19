#include "fault_mosfet.h"
#include "pin_config.h"

// PIN_FAULT_GATE defined in pin_config.h (GPIO 5)

void setup_fault_mosfet() {
    pinMode(PIN_FAULT_GATE, OUTPUT);
    digitalWrite(PIN_FAULT_GATE, LOW);
    
    // Configura botões aqui se não for feito no main
    pinMode(PIN_BTN_FAULT, INPUT_PULLUP);
    pinMode(PIN_BTN_YES, INPUT_PULLUP);
    pinMode(PIN_BTN_NO, INPUT_PULLUP);
}

void fault_pulse_trigger() {
    // Sequência de Glitch
    noInterrupts();
    digitalWrite(PIN_FAULT_GATE, HIGH);
    // Delay muito curto (microsegundos talvez? O prompt pediu 5ms)
    // 5ms = 5000us. Para fault injection real costuma ser ns/us.
    // Mas atendendo pedido:
    delay(5); 
    digitalWrite(PIN_FAULT_GATE, LOW);
    interrupts();
    
    Serial.println("[FAULT] Glitch Fired!");
}
