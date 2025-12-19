#ifndef BLE_SPAM_BR_H
#define BLE_SPAM_BR_H

#include <stdint.h>
#include <stdbool.h>

// Funções públicas para controle do BLE spam brasileiro otimizado
bool start_ble_spam_br();
void stop_ble_spam_br();
uint32_t get_ble_spam_packets_sent();
bool is_ble_spam_active();

#endif // BLE_SPAM_BR_H
