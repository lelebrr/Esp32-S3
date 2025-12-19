#ifndef ATTACKS_MANAGER_H
#define ATTACKS_MANAGER_H

#include <Arduino.h>

// Tipos de Ataque
enum AttackType {
    ATTACK_NONE = 0,
    ATTACK_BLE_SPAM,
    ATTACK_WIFI_DEAUTH,
    ATTACK_NFC_FAULT,
    ATTACK_SUBGHZ_REPLAY,
    ATTACK_IR_BRUTE
};

// Controle Geral
void attacks_init();
void attacks_start(AttackType type);
void attacks_stop();
bool attacks_is_running();

// Callbacks para IA logging
void attacks_report_success();
void attacks_report_fail();

#endif
