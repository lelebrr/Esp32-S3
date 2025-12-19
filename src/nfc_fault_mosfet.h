#ifndef NFC_FAULT_MOSFET_H
#define NFC_FAULT_MOSFET_H

#include <stdint.h>
#include <stdbool.h>

// Funções públicas para controle do NFC fault injection com MOSFET
bool init_nfc_fault_mosfet();
void deinit_nfc_fault_mosfet();
void trigger_nfc_fault_manual();
uint32_t get_nfc_fault_count();
bool is_nfc_fault_active();
void reset_nfc_fault_count();

#endif // NFC_FAULT_MOSFET_H
