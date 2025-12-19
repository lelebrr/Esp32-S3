#ifndef ATTACKS_MANAGER_H
#define ATTACKS_MANAGER_H

#include <Arduino.h>
#include "YsIrtm.h"

// ============================================================================
// ATTACK TYPES - Complete Attack Suite
// ============================================================================
typedef enum {
    ATTACK_NONE = 0,
    
    // BLE Attacks
    ATTACK_BLE_SPAM,
    ATTACK_BLE_SOUR_APPLE,
    ATTACK_BLE_SWIFT_PAIR,
    ATTACK_BLE_FAST_PAIR,
    
    // WiFi Attacks
    ATTACK_WIFI_DEAUTH,
    ATTACK_WIFI_BEACON_SPAM,
    ATTACK_WIFI_EVIL_TWIN,
    ATTACK_WIFI_PMKID,
    ATTACK_WIFI_WARDRIVE,
    
    // RF SubGHz Attacks (CC1101)
    ATTACK_RF_JAMMER_433,
    ATTACK_RF_JAMMER_315,
    ATTACK_RF_JAMMER_868,
    ATTACK_RF_JAMMER_BURST,
    ATTACK_RF_JAMMER_SMART,
    ATTACK_RF_SCAN,
    ATTACK_RF_CAPTURE,
    ATTACK_RF_REPLAY,
    ATTACK_RF_GHOST_REPLAY,
    ATTACK_RF_BRUTE_FORCE,
    ATTACK_RF_DEBRUIJN,
    ATTACK_RF_SPECTRUM,
    
    // NFC Attacks
    ATTACK_NFC_CLONE,
    ATTACK_NFC_FAULT,
    ATTACK_NFC_PHISHING,
    
    // IR Attacks
    ATTACK_IR_BRUTE,
    ATTACK_IR_TV_OFF,
    
    // USB Attacks
    ATTACK_USB_BADUSB,
    ATTACK_USB_EXFIL
    
} AttackType;

// ============================================================================
// CONTROL FUNCTIONS
// ============================================================================
void attacks_init();
void attacks_start(AttackType type);
void attacks_stop();
bool attacks_is_running();
AttackType attacks_get_current();

// Update loop for time-based attacks
void attacks_update();

// ============================================================================
// CALLBACKS FOR AI LOGGING
// ============================================================================
void attacks_report_success();
void attacks_report_fail();

// ============================================================================
// ATTACK NAMES (for UI)
// ============================================================================
const char* attacks_get_name(AttackType type);

#endif // ATTACKS_MANAGER_H
