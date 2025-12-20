/**
 * @file config_manager.h
 * @brief Central Configuration Manager for Monster S3
 * 
 * Handles storage, persistence, and validation of all device settings.
 * Serializes configuration to/from JSON for Web UI interaction and
 * saves binary/JSON data to SD card for persistence.
 * 
 * @author Monster S3 Team
 * @version 2.0
 * @date 2025
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

// ============================================================================
// DEVICE CONFIGURATION STRUCTURE
// ============================================================================

struct DeviceConfig {
    // ===== SYSTEM SETTINGS =====
    bool wifi_enabled;
    bool ble_enabled;
    bool gps_enabled;
    bool voice_enabled;
    bool mascot_enabled;
    int cpu_freq_mhz;           // 80, 160, 240
    int display_brightness;     // 0-255
    int current_theme;          // 0-N
    bool auto_sleep_enabled;
    int sleep_timeout_sec;      // 30-600
    
    // ===== NETWORK/AP SETTINGS =====
    char ap_ssid[32];
    char ap_password[64];
    bool ap_hidden;
    int ap_channel;             // 1-13
    int ap_max_conn;            // 1-4
    char web_username[32];
    char web_password[64];
    bool require_auth;
    
    // ===== RF SUBGHZ SETTINGS =====
    float rf_frequency;         // 300.0 - 928.0 MHz
    int rf_power_dbm;           // -30 to +10
    int rf_modulation;          // 0=2FSK, 1=GFSK, 2=ASK/OOK, 3=4FSK, 4=MSK
    float rf_bandwidth_khz;     // 58 - 812
    float rf_deviation_khz;     // 1 - 380
    float rf_datarate_kbps;     // 0.6 - 600
    uint16_t rf_sync_word;
    
    // ===== BLE ATTACK SETTINGS =====
    int ble_spam_mode;          // 0=All, 1=Apple, 2=Samsung, 3=Xiaomi
    int ble_spam_interval_ms;   // 20 - 500
    bool ble_random_mac;
    char ble_name_prefix[16];
    
    // ===== WIFI ATTACK SETTINGS =====
    int wifi_deauth_reason;     // 1-7
    int wifi_deauth_count;      // Packets per target
    int wifi_beacon_count;      // Number of fake networks
    int wifi_beacon_interval_ms;// 20 - 500
    bool wifi_auto_target_strongest;
    bool wifi_capture_pmkid;
    bool wifi_capture_handshake;
    
    // ===== NFC SETTINGS =====
    int nfc_protocol;           // 0=Auto, 1=MiFare Classic, 2=Ultralight, 3=NTAG
    bool nfc_auto_read;
    bool nfc_beep_on_read;
    
    // ===== IR SETTINGS =====
    int ir_protocol;            // 0=NEC, 1=RC5, 2=RC6, 3=Sony, 4=Samsung, 5=LG
    bool ir_repeat_enabled;
    int ir_repeat_count;        // 1-10
    int ir_repeat_delay_ms;     // 50-500
    
    // ===== AI Q-LEARNING SETTINGS =====
    float ai_epsilon;           // 0.0 - 1.0 exploration rate
    float ai_learning_rate;     // 0.01 - 1.0
    float ai_discount_factor;   // 0.0 - 1.0
    bool ai_auto_learn;
    bool ai_voice_feedback;
    
    // ===== DATA SETTINGS =====
    bool auto_save_captures;
    bool auto_log_attacks;
    char capture_folder[32];
    
    // ===== ADVANCED =====
    bool stealth_mode;
    bool debug_serial;
    int watchdog_timeout_sec;
    char attacker_ip[16];       // For WAX/BadUSB attacks
};

// ============================================================================
// DEFAULT CONFIGURATION VALUES
// ============================================================================

#define CONFIG_DEFAULTS { \
    /* System */ \
    .wifi_enabled = true, \
    .ble_enabled = true, \
    .gps_enabled = true, \
    .voice_enabled = false, \
    .mascot_enabled = false, \
    .cpu_freq_mhz = 240, \
    .display_brightness = 200, \
    .current_theme = 0, \
    .auto_sleep_enabled = true, \
    .sleep_timeout_sec = 120, \
    /* Network */ \
    .ap_ssid = "Monster_S3_AP", \
    .ap_password = "lele2025", \
    .ap_hidden = true, \
    .ap_channel = 1, \
    .ap_max_conn = 4, \
    .web_username = "admin", \
    .web_password = "lele2025", \
    .require_auth = true, \
    /* RF */ \
    .rf_frequency = 433.92f, \
    .rf_power_dbm = 10, \
    .rf_modulation = 2, \
    .rf_bandwidth_khz = 200.0f, \
    .rf_deviation_khz = 47.6f, \
    .rf_datarate_kbps = 4.8f, \
    .rf_sync_word = 0xD391, \
    /* BLE */ \
    .ble_spam_mode = 0, \
    .ble_spam_interval_ms = 100, \
    .ble_random_mac = true, \
    .ble_name_prefix = "", \
    /* WiFi Attack */ \
    .wifi_deauth_reason = 7, \
    .wifi_deauth_count = 50, \
    .wifi_beacon_count = 20, \
    .wifi_beacon_interval_ms = 100, \
    .wifi_auto_target_strongest = true, \
    .wifi_capture_pmkid = true, \
    .wifi_capture_handshake = true, \
    /* NFC */ \
    .nfc_protocol = 0, \
    .nfc_auto_read = true, \
    .nfc_beep_on_read = true, \
    /* IR */ \
    .ir_protocol = 0, \
    .ir_repeat_enabled = true, \
    .ir_repeat_count = 3, \
    .ir_repeat_delay_ms = 100, \
    /* AI */ \
    .ai_epsilon = 0.15f, \
    .ai_learning_rate = 0.1f, \
    .ai_discount_factor = 0.95f, \
    .ai_auto_learn = true, \
    .ai_voice_feedback = false, \
    /* Data */ \
    .auto_save_captures = true, \
    .auto_log_attacks = true, \
    .capture_folder = "/captures", \
    /* Advanced */ \
    .stealth_mode = false, \
    .debug_serial = true, \
    .watchdog_timeout_sec = 30, \
    .attacker_ip = "192.168.1.100" \
}

// ============================================================================
// PUBLIC API
// ============================================================================

// Initialize configuration manager (loads from SD if available)
void config_init();

// Get current configuration (read-only pointer)
const DeviceConfig* config_get();

// Get mutable configuration for modifications
DeviceConfig* config_get_mutable();

// Save current configuration to SD card
bool config_save();

// Load configuration from SD card (overwrites current)
bool config_load();

// Reset to factory defaults
void config_reset_defaults();

// Export configuration as JSON string (caller must free)
char* config_to_json();

// Import configuration from JSON string
bool config_from_json(const char* json);

// Validate configuration values (fix out-of-range)
void config_validate();

// Apply configuration to system (set CPU freq, etc)
void config_apply();

// Get config file path
const char* config_get_path();

// Check if config was modified since last save
bool config_is_dirty();

// Mark config as modified
void config_set_dirty();

#endif // CONFIG_MANAGER_H
