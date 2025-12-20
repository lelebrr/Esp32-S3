/**
 * @file attacks_manager.h
 * @brief Central Attack Management System Definition
 * 
 * Defines the complete suite of attack vectors managed by the firmware,
 * including their enums and control interfaces.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#ifndef ATTACKS_MANAGER_H
#define ATTACKS_MANAGER_H

#include <Arduino.h>
#include "YsIrtm.h"

// ============================================================================
// ATTACK TYPES - Complete Attack Suite (30+ Attacks)
// ============================================================================
typedef enum {
    ATTACK_NONE = 0,
    
    // BLE Attacks (4)
    ATTACK_BLE_SPAM,        // Brazilian device names
    ATTACK_BLE_SOUR_APPLE,  // iOS popup spam
    ATTACK_BLE_SWIFT_PAIR,  // Windows popup spam
    ATTACK_BLE_FAST_PAIR,   // Android popup spam
    
    // WiFi Attacks (5)
    ATTACK_WIFI_DEAUTH,       // 802.11 deauth frames
    ATTACK_WIFI_BEACON_SPAM,  // Fake network spam
    ATTACK_WIFI_EVIL_TWIN,    // Clone strongest network
    ATTACK_WIFI_PMKID,        // WPA2 PMKID capture
    ATTACK_WIFI_WARDRIVE,     // GPS network logging
    
    // RF SubGHz Attacks (12) - via CC1101
    ATTACK_RF_JAMMER_433,
    ATTACK_RF_JAMMER_315,
    ATTACK_RF_JAMMER_868,
    ATTACK_RF_JAMMER_BURST,
    ATTACK_RF_JAMMER_SMART,   // Detect & jam
    ATTACK_RF_SCAN,
    ATTACK_RF_CAPTURE,
    ATTACK_RF_REPLAY,
    ATTACK_RF_GHOST_REPLAY,   // Random delay replay
    ATTACK_RF_BRUTE_FORCE,
    ATTACK_RF_DEBRUIJN,
    ATTACK_RF_ROLLJAM,    // Jam + Capture Rolling Code
    ATTACK_RF_ROLLBACK,   // Resync Exploit
    ATTACK_RF_SPECTRUM,
    
    // NFC Attacks (3) - via PN532
    ATTACK_NFC_CLONE,      // Read & clone MiFare
    ATTACK_NFC_FAULT,      // Glitch during auth
    ATTACK_NFC_PHISHING,   // Write NDEF URL
    
    // IR Attacks (3) - via YS-IRTM
    ATTACK_IR_BRUTE,       // Brute force NEC codes
    ATTACK_IR_TV_OFF,      // TV-B-Gone (15+ TVs)
    ATTACK_IR_CLONE,       // Learn & replay
    
    // NRF24 Attacks (2) - via NRF24L01
    ATTACK_NRF_MOUSEJACK,   // Injection
    ATTACK_NRF_KEYBOARD,    // Sniffing
    
    // USB Attacks (2)
    ATTACK_USB_BADUSB,     // HID injection (Generic)
    ATTACK_USB_WAX,        // WAX NTLM Hash Capture
    ATTACK_USB_EXFIL,      // Data exfiltration
    ATTACK_HOAXSHELL,      // C2 Reverse Shell
    ATTACK_POSHC2,         // PoshC2 Framework Payloads
    
    // Total: 34 attack types
    ATTACK_TYPE_COUNT
} AttackType;

// ============================================================================
// CONTROL FUNCTIONS
// ============================================================================

// Initialize attack manager
void attacks_init();

// Start specific attack
void attacks_start(AttackType type);

// Stop all attacks
void attacks_stop();

// Check if any attack is running
bool attacks_is_running();

// Get current attack type
AttackType attacks_get_current();

// Update loop (call from main)
void attacks_update();

// Get packet counter
uint32_t attacks_get_packet_count();

// ============================================================================
// AI FEEDBACK CALLBACKS
// ============================================================================
void attacks_report_success();
void attacks_report_fail();

// ============================================================================
// UTILITY
// ============================================================================
const char* attacks_get_name(AttackType type);

#endif // ATTACKS_MANAGER_H
