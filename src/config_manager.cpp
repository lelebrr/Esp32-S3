/**
 * @file config_manager.cpp
 * @brief Configuration Persistence System
 * 
 * Handles saving and loading of all device settings to the SD card
 * using JSON format. Manages runtime application of settings
 * to various hardware modules.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "config_manager.h"
#include <ArduinoJson.h>
#include <SD.h>
#include <esp_pm.h>
#include "rf_core.h"

// ============================================================================
// STATIC STORAGE
// ============================================================================

static DeviceConfig g_config = CONFIG_DEFAULTS;
static bool g_config_dirty = false;
static const char* CONFIG_PATH = "/config/settings.json";

// ============================================================================
// INITIALIZATION
// ============================================================================

/**
 * @brief Initialize Configuration Manager
 * 
 * Loads settings from SD card or falls back to defaults.
 * Validates and applies initial configuration to system.
 */
void config_init() {
    Serial.println("[CFG] Initializing configuration manager...");
    
    // Try to load from SD
    if (!config_load()) {
        Serial.println("[CFG] No saved config found, using defaults");
        config_reset_defaults();
    }
    
    // Validate all values
    config_validate();
    
    // Apply to system
    config_apply();
    
    g_config_dirty = false;
    Serial.println("[CFG] Configuration ready");
}

// ... (getters omitted for brevity) ...

// ============================================================================
// APPLY CONFIGURATION
// ============================================================================

/**
 * @brief Apply current configuration to hardware
 * 
 * Updates CPU frequency, RF module settings, and other
 * hardware parameters based on the current config state.
 */
void config_apply() {
    // Set CPU frequency
    #if CONFIG_PM_ENABLE
    esp_pm_config_esp32s3_t pm_config = {
        .max_freq_mhz = g_config.cpu_freq_mhz,
        .min_freq_mhz = g_config.cpu_freq_mhz,
        .light_sleep_enable = false
    };
    esp_pm_configure(&pm_config);
    #else
    setCpuFrequencyMhz(g_config.cpu_freq_mhz);
    #endif
    
    // Apply RF Configuration
    // Setters handle initialization checks internally
    if (g_config.rf_frequency > 0) RFCore::setFrequency(g_config.rf_frequency);
    RFCore::setTxPower(g_config.rf_power_dbm);
    RFCore::setModulation(g_config.rf_modulation);
    if (g_config.rf_bandwidth_khz > 0) RFCore::setBandwidth(g_config.rf_bandwidth_khz);
    if (g_config.rf_deviation_khz > 0) RFCore::setDeviation(g_config.rf_deviation_khz);
    if (g_config.rf_datarate_kbps > 0) RFCore::setDataRate(g_config.rf_datarate_kbps);
    if (g_config.rf_sync_word > 0) RFCore::setSyncWord(g_config.rf_sync_word);
    
    Serial.printf("[CFG] Applied: CPU=%dMHz, RF=%.2fMHz, Brightness=%d\n", 
                  g_config.cpu_freq_mhz, g_config.rf_frequency, g_config.display_brightness);
}

// ============================================================================
// SAVE TO SD CARD
// ============================================================================

bool config_save() {
    // Create config directory if needed
    if (!SD.exists("/config")) {
        SD.mkdir("/config");
    }
    
    // Create JSON document
    DynamicJsonDocument doc(4096);
    
    // System
    doc["wifi_enabled"] = g_config.wifi_enabled;
    doc["ble_enabled"] = g_config.ble_enabled;
    doc["gps_enabled"] = g_config.gps_enabled;
    doc["voice_enabled"] = g_config.voice_enabled;
    doc["mascot_enabled"] = g_config.mascot_enabled;
    doc["cpu_freq_mhz"] = g_config.cpu_freq_mhz;
    doc["display_brightness"] = g_config.display_brightness;
    doc["current_theme"] = g_config.current_theme;
    doc["auto_sleep_enabled"] = g_config.auto_sleep_enabled;
    doc["sleep_timeout_sec"] = g_config.sleep_timeout_sec;
    
    // Network
    doc["ap_ssid"] = g_config.ap_ssid;
    doc["ap_password"] = g_config.ap_password;
    doc["ap_hidden"] = g_config.ap_hidden;
    doc["ap_channel"] = g_config.ap_channel;
    doc["ap_max_conn"] = g_config.ap_max_conn;
    doc["web_username"] = g_config.web_username;
    doc["web_password"] = g_config.web_password;
    doc["require_auth"] = g_config.require_auth;
    
    // RF
    doc["rf_frequency"] = g_config.rf_frequency;
    doc["rf_power_dbm"] = g_config.rf_power_dbm;
    doc["rf_modulation"] = g_config.rf_modulation;
    doc["rf_bandwidth_khz"] = g_config.rf_bandwidth_khz;
    doc["rf_deviation_khz"] = g_config.rf_deviation_khz;
    doc["rf_datarate_kbps"] = g_config.rf_datarate_kbps;
    doc["rf_sync_word"] = g_config.rf_sync_word;
    
    // BLE
    doc["ble_spam_mode"] = g_config.ble_spam_mode;
    doc["ble_spam_interval_ms"] = g_config.ble_spam_interval_ms;
    doc["ble_random_mac"] = g_config.ble_random_mac;
    doc["ble_name_prefix"] = g_config.ble_name_prefix;
    
    // WiFi Attack
    doc["wifi_deauth_reason"] = g_config.wifi_deauth_reason;
    doc["wifi_deauth_count"] = g_config.wifi_deauth_count;
    doc["wifi_beacon_count"] = g_config.wifi_beacon_count;
    doc["wifi_beacon_interval_ms"] = g_config.wifi_beacon_interval_ms;
    doc["wifi_auto_target_strongest"] = g_config.wifi_auto_target_strongest;
    doc["wifi_capture_pmkid"] = g_config.wifi_capture_pmkid;
    doc["wifi_capture_handshake"] = g_config.wifi_capture_handshake;
    
    // NFC
    doc["nfc_protocol"] = g_config.nfc_protocol;
    doc["nfc_auto_read"] = g_config.nfc_auto_read;
    doc["nfc_beep_on_read"] = g_config.nfc_beep_on_read;
    
    // IR
    doc["ir_protocol"] = g_config.ir_protocol;
    doc["ir_repeat_enabled"] = g_config.ir_repeat_enabled;
    doc["ir_repeat_count"] = g_config.ir_repeat_count;
    doc["ir_repeat_delay_ms"] = g_config.ir_repeat_delay_ms;
    
    // AI
    doc["ai_epsilon"] = g_config.ai_epsilon;
    doc["ai_learning_rate"] = g_config.ai_learning_rate;
    doc["ai_discount_factor"] = g_config.ai_discount_factor;
    doc["ai_auto_learn"] = g_config.ai_auto_learn;
    doc["ai_voice_feedback"] = g_config.ai_voice_feedback;
    
    // Data
    doc["auto_save_captures"] = g_config.auto_save_captures;
    doc["auto_log_attacks"] = g_config.auto_log_attacks;
    doc["capture_folder"] = g_config.capture_folder;
    
    // Advanced
    doc["stealth_mode"] = g_config.stealth_mode;
    doc["debug_serial"] = g_config.debug_serial;
    doc["watchdog_timeout_sec"] = g_config.watchdog_timeout_sec;
    doc["attacker_ip"] = g_config.attacker_ip;
    
    // Write to file
    File configFile = SD.open(CONFIG_PATH, FILE_WRITE);
    if (!configFile) {
        Serial.println("[CFG] ERROR: Failed to open config file for writing");
        return false;
    }
    
    serializeJsonPretty(doc, configFile);
    configFile.close();
    
    g_config_dirty = false;
    Serial.println("[CFG] Configuration saved to SD");
    return true;
}

// ============================================================================
// LOAD FROM SD CARD
// ============================================================================

bool config_load() {
    if (!SD.exists(CONFIG_PATH)) {
        Serial.println("[CFG] Config file not found");
        return false;
    }
    
    File configFile = SD.open(CONFIG_PATH, FILE_READ);
    if (!configFile) {
        Serial.println("[CFG] ERROR: Failed to open config file");
        return false;
    }
    
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();
    
    if (error) {
        Serial.printf("[CFG] ERROR: JSON parse failed: %s\n", error.c_str());
        return false;
    }
    
    // System
    if (doc.containsKey("wifi_enabled")) g_config.wifi_enabled = doc["wifi_enabled"];
    if (doc.containsKey("ble_enabled")) g_config.ble_enabled = doc["ble_enabled"];
    if (doc.containsKey("gps_enabled")) g_config.gps_enabled = doc["gps_enabled"];
    if (doc.containsKey("voice_enabled")) g_config.voice_enabled = doc["voice_enabled"];
    if (doc.containsKey("mascot_enabled")) g_config.mascot_enabled = doc["mascot_enabled"];
    if (doc.containsKey("cpu_freq_mhz")) g_config.cpu_freq_mhz = doc["cpu_freq_mhz"];
    if (doc.containsKey("display_brightness")) g_config.display_brightness = doc["display_brightness"];
    if (doc.containsKey("current_theme")) g_config.current_theme = doc["current_theme"];
    if (doc.containsKey("auto_sleep_enabled")) g_config.auto_sleep_enabled = doc["auto_sleep_enabled"];
    if (doc.containsKey("sleep_timeout_sec")) g_config.sleep_timeout_sec = doc["sleep_timeout_sec"];
    
    // Network
    if (doc.containsKey("ap_ssid")) strlcpy(g_config.ap_ssid, doc["ap_ssid"], sizeof(g_config.ap_ssid));
    if (doc.containsKey("ap_password")) strlcpy(g_config.ap_password, doc["ap_password"], sizeof(g_config.ap_password));
    if (doc.containsKey("ap_hidden")) g_config.ap_hidden = doc["ap_hidden"];
    if (doc.containsKey("ap_channel")) g_config.ap_channel = doc["ap_channel"];
    if (doc.containsKey("ap_max_conn")) g_config.ap_max_conn = doc["ap_max_conn"];
    if (doc.containsKey("web_username")) strlcpy(g_config.web_username, doc["web_username"], sizeof(g_config.web_username));
    if (doc.containsKey("web_password")) strlcpy(g_config.web_password, doc["web_password"], sizeof(g_config.web_password));
    if (doc.containsKey("require_auth")) g_config.require_auth = doc["require_auth"];
    
    // RF
    if (doc.containsKey("rf_frequency")) g_config.rf_frequency = doc["rf_frequency"];
    if (doc.containsKey("rf_power_dbm")) g_config.rf_power_dbm = doc["rf_power_dbm"];
    if (doc.containsKey("rf_modulation")) g_config.rf_modulation = doc["rf_modulation"];
    if (doc.containsKey("rf_bandwidth_khz")) g_config.rf_bandwidth_khz = doc["rf_bandwidth_khz"];
    if (doc.containsKey("rf_deviation_khz")) g_config.rf_deviation_khz = doc["rf_deviation_khz"];
    if (doc.containsKey("rf_datarate_kbps")) g_config.rf_datarate_kbps = doc["rf_datarate_kbps"];
    if (doc.containsKey("rf_sync_word")) g_config.rf_sync_word = doc["rf_sync_word"];
    
    // BLE
    if (doc.containsKey("ble_spam_mode")) g_config.ble_spam_mode = doc["ble_spam_mode"];
    if (doc.containsKey("ble_spam_interval_ms")) g_config.ble_spam_interval_ms = doc["ble_spam_interval_ms"];
    if (doc.containsKey("ble_random_mac")) g_config.ble_random_mac = doc["ble_random_mac"];
    if (doc.containsKey("ble_name_prefix")) strlcpy(g_config.ble_name_prefix, doc["ble_name_prefix"], sizeof(g_config.ble_name_prefix));
    
    // WiFi Attack
    if (doc.containsKey("wifi_deauth_reason")) g_config.wifi_deauth_reason = doc["wifi_deauth_reason"];
    if (doc.containsKey("wifi_deauth_count")) g_config.wifi_deauth_count = doc["wifi_deauth_count"];
    if (doc.containsKey("wifi_beacon_count")) g_config.wifi_beacon_count = doc["wifi_beacon_count"];
    if (doc.containsKey("wifi_beacon_interval_ms")) g_config.wifi_beacon_interval_ms = doc["wifi_beacon_interval_ms"];
    if (doc.containsKey("wifi_auto_target_strongest")) g_config.wifi_auto_target_strongest = doc["wifi_auto_target_strongest"];
    if (doc.containsKey("wifi_capture_pmkid")) g_config.wifi_capture_pmkid = doc["wifi_capture_pmkid"];
    if (doc.containsKey("wifi_capture_handshake")) g_config.wifi_capture_handshake = doc["wifi_capture_handshake"];
    
    // NFC
    if (doc.containsKey("nfc_protocol")) g_config.nfc_protocol = doc["nfc_protocol"];
    if (doc.containsKey("nfc_auto_read")) g_config.nfc_auto_read = doc["nfc_auto_read"];
    if (doc.containsKey("nfc_beep_on_read")) g_config.nfc_beep_on_read = doc["nfc_beep_on_read"];
    
    // IR
    if (doc.containsKey("ir_protocol")) g_config.ir_protocol = doc["ir_protocol"];
    if (doc.containsKey("ir_repeat_enabled")) g_config.ir_repeat_enabled = doc["ir_repeat_enabled"];
    if (doc.containsKey("ir_repeat_count")) g_config.ir_repeat_count = doc["ir_repeat_count"];
    if (doc.containsKey("ir_repeat_delay_ms")) g_config.ir_repeat_delay_ms = doc["ir_repeat_delay_ms"];
    
    // AI
    if (doc.containsKey("ai_epsilon")) g_config.ai_epsilon = doc["ai_epsilon"];
    if (doc.containsKey("ai_learning_rate")) g_config.ai_learning_rate = doc["ai_learning_rate"];
    if (doc.containsKey("ai_discount_factor")) g_config.ai_discount_factor = doc["ai_discount_factor"];
    if (doc.containsKey("ai_auto_learn")) g_config.ai_auto_learn = doc["ai_auto_learn"];
    if (doc.containsKey("ai_voice_feedback")) g_config.ai_voice_feedback = doc["ai_voice_feedback"];
    
    // Data
    if (doc.containsKey("auto_save_captures")) g_config.auto_save_captures = doc["auto_save_captures"];
    if (doc.containsKey("auto_log_attacks")) g_config.auto_log_attacks = doc["auto_log_attacks"];
    if (doc.containsKey("capture_folder")) strlcpy(g_config.capture_folder, doc["capture_folder"], sizeof(g_config.capture_folder));
    
    // Advanced
    if (doc.containsKey("stealth_mode")) g_config.stealth_mode = doc["stealth_mode"];
    if (doc.containsKey("debug_serial")) g_config.debug_serial = doc["debug_serial"];
    if (doc.containsKey("watchdog_timeout_sec")) g_config.watchdog_timeout_sec = doc["watchdog_timeout_sec"];
    if (doc.containsKey("attacker_ip")) strlcpy(g_config.attacker_ip, doc["attacker_ip"], sizeof(g_config.attacker_ip));
    
    g_config_dirty = false;
    Serial.println("[CFG] Configuration loaded from SD");
    return true;
}

// ============================================================================
// JSON EXPORT
// ============================================================================

char* config_to_json() {
    DynamicJsonDocument doc(4096);
    
    // Same structure as config_save
    doc["wifi_enabled"] = g_config.wifi_enabled;
    doc["ble_enabled"] = g_config.ble_enabled;
    doc["gps_enabled"] = g_config.gps_enabled;
    doc["voice_enabled"] = g_config.voice_enabled;
    doc["mascot_enabled"] = g_config.mascot_enabled;
    doc["cpu_freq_mhz"] = g_config.cpu_freq_mhz;
    doc["display_brightness"] = g_config.display_brightness;
    doc["current_theme"] = g_config.current_theme;
    doc["auto_sleep_enabled"] = g_config.auto_sleep_enabled;
    doc["sleep_timeout_sec"] = g_config.sleep_timeout_sec;
    
    doc["ap_ssid"] = g_config.ap_ssid;
    doc["ap_password"] = g_config.ap_password;
    doc["ap_hidden"] = g_config.ap_hidden;
    doc["ap_channel"] = g_config.ap_channel;
    doc["ap_max_conn"] = g_config.ap_max_conn;
    doc["web_username"] = g_config.web_username;
    doc["web_password"] = g_config.web_password;
    doc["require_auth"] = g_config.require_auth;
    
    doc["rf_frequency"] = g_config.rf_frequency;
    doc["rf_power_dbm"] = g_config.rf_power_dbm;
    doc["rf_modulation"] = g_config.rf_modulation;
    doc["rf_bandwidth_khz"] = g_config.rf_bandwidth_khz;
    doc["rf_deviation_khz"] = g_config.rf_deviation_khz;
    doc["rf_datarate_kbps"] = g_config.rf_datarate_kbps;
    doc["rf_sync_word"] = g_config.rf_sync_word;
    
    doc["ble_spam_mode"] = g_config.ble_spam_mode;
    doc["ble_spam_interval_ms"] = g_config.ble_spam_interval_ms;
    doc["ble_random_mac"] = g_config.ble_random_mac;
    doc["ble_name_prefix"] = g_config.ble_name_prefix;
    
    doc["wifi_deauth_reason"] = g_config.wifi_deauth_reason;
    doc["wifi_deauth_count"] = g_config.wifi_deauth_count;
    doc["wifi_beacon_count"] = g_config.wifi_beacon_count;
    doc["wifi_beacon_interval_ms"] = g_config.wifi_beacon_interval_ms;
    doc["wifi_auto_target_strongest"] = g_config.wifi_auto_target_strongest;
    doc["wifi_capture_pmkid"] = g_config.wifi_capture_pmkid;
    doc["wifi_capture_handshake"] = g_config.wifi_capture_handshake;
    
    doc["nfc_protocol"] = g_config.nfc_protocol;
    doc["nfc_auto_read"] = g_config.nfc_auto_read;
    doc["nfc_beep_on_read"] = g_config.nfc_beep_on_read;
    
    doc["ir_protocol"] = g_config.ir_protocol;
    doc["ir_repeat_enabled"] = g_config.ir_repeat_enabled;
    doc["ir_repeat_count"] = g_config.ir_repeat_count;
    doc["ir_repeat_delay_ms"] = g_config.ir_repeat_delay_ms;
    
    doc["ai_epsilon"] = g_config.ai_epsilon;
    doc["ai_learning_rate"] = g_config.ai_learning_rate;
    doc["ai_discount_factor"] = g_config.ai_discount_factor;
    doc["ai_auto_learn"] = g_config.ai_auto_learn;
    doc["ai_voice_feedback"] = g_config.ai_voice_feedback;
    
    doc["auto_save_captures"] = g_config.auto_save_captures;
    doc["auto_log_attacks"] = g_config.auto_log_attacks;
    doc["capture_folder"] = g_config.capture_folder;
    
    doc["stealth_mode"] = g_config.stealth_mode;
    doc["debug_serial"] = g_config.debug_serial;
    doc["watchdog_timeout_sec"] = g_config.watchdog_timeout_sec;
    doc["attacker_ip"] = g_config.attacker_ip;
    
    size_t jsonSize = measureJsonPretty(doc) + 1;
    char* jsonStr = (char*)malloc(jsonSize);
    if (jsonStr) {
        serializeJsonPretty(doc, jsonStr, jsonSize);
    }
    return jsonStr;
}

// ============================================================================
// JSON IMPORT
// ============================================================================

bool config_from_json(const char* json) {
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
        Serial.printf("[CFG] ERROR: JSON import failed: %s\n", error.c_str());
        return false;
    }
    
    // Same logic as config_load, but from memory
    if (doc.containsKey("wifi_enabled")) g_config.wifi_enabled = doc["wifi_enabled"];
    if (doc.containsKey("ble_enabled")) g_config.ble_enabled = doc["ble_enabled"];
    if (doc.containsKey("gps_enabled")) g_config.gps_enabled = doc["gps_enabled"];
    if (doc.containsKey("voice_enabled")) g_config.voice_enabled = doc["voice_enabled"];
    if (doc.containsKey("mascot_enabled")) g_config.mascot_enabled = doc["mascot_enabled"];
    if (doc.containsKey("cpu_freq_mhz")) g_config.cpu_freq_mhz = doc["cpu_freq_mhz"];
    if (doc.containsKey("display_brightness")) g_config.display_brightness = doc["display_brightness"];
    if (doc.containsKey("current_theme")) g_config.current_theme = doc["current_theme"];
    if (doc.containsKey("auto_sleep_enabled")) g_config.auto_sleep_enabled = doc["auto_sleep_enabled"];
    if (doc.containsKey("sleep_timeout_sec")) g_config.sleep_timeout_sec = doc["sleep_timeout_sec"];
    
    if (doc.containsKey("ap_ssid")) strlcpy(g_config.ap_ssid, doc["ap_ssid"], sizeof(g_config.ap_ssid));
    if (doc.containsKey("ap_password")) strlcpy(g_config.ap_password, doc["ap_password"], sizeof(g_config.ap_password));
    if (doc.containsKey("ap_hidden")) g_config.ap_hidden = doc["ap_hidden"];
    if (doc.containsKey("ap_channel")) g_config.ap_channel = doc["ap_channel"];
    if (doc.containsKey("ap_max_conn")) g_config.ap_max_conn = doc["ap_max_conn"];
    if (doc.containsKey("web_username")) strlcpy(g_config.web_username, doc["web_username"], sizeof(g_config.web_username));
    if (doc.containsKey("web_password")) strlcpy(g_config.web_password, doc["web_password"], sizeof(g_config.web_password));
    if (doc.containsKey("require_auth")) g_config.require_auth = doc["require_auth"];
    
    if (doc.containsKey("rf_frequency")) g_config.rf_frequency = doc["rf_frequency"];
    if (doc.containsKey("rf_power_dbm")) g_config.rf_power_dbm = doc["rf_power_dbm"];
    if (doc.containsKey("rf_modulation")) g_config.rf_modulation = doc["rf_modulation"];
    if (doc.containsKey("rf_bandwidth_khz")) g_config.rf_bandwidth_khz = doc["rf_bandwidth_khz"];
    if (doc.containsKey("rf_deviation_khz")) g_config.rf_deviation_khz = doc["rf_deviation_khz"];
    if (doc.containsKey("rf_datarate_kbps")) g_config.rf_datarate_kbps = doc["rf_datarate_kbps"];
    if (doc.containsKey("rf_sync_word")) g_config.rf_sync_word = doc["rf_sync_word"];
    
    if (doc.containsKey("ble_spam_mode")) g_config.ble_spam_mode = doc["ble_spam_mode"];
    if (doc.containsKey("ble_spam_interval_ms")) g_config.ble_spam_interval_ms = doc["ble_spam_interval_ms"];
    if (doc.containsKey("ble_random_mac")) g_config.ble_random_mac = doc["ble_random_mac"];
    if (doc.containsKey("ble_name_prefix")) strlcpy(g_config.ble_name_prefix, doc["ble_name_prefix"], sizeof(g_config.ble_name_prefix));
    
    if (doc.containsKey("wifi_deauth_reason")) g_config.wifi_deauth_reason = doc["wifi_deauth_reason"];
    if (doc.containsKey("wifi_deauth_count")) g_config.wifi_deauth_count = doc["wifi_deauth_count"];
    if (doc.containsKey("wifi_beacon_count")) g_config.wifi_beacon_count = doc["wifi_beacon_count"];
    if (doc.containsKey("wifi_beacon_interval_ms")) g_config.wifi_beacon_interval_ms = doc["wifi_beacon_interval_ms"];
    if (doc.containsKey("wifi_auto_target_strongest")) g_config.wifi_auto_target_strongest = doc["wifi_auto_target_strongest"];
    if (doc.containsKey("wifi_capture_pmkid")) g_config.wifi_capture_pmkid = doc["wifi_capture_pmkid"];
    if (doc.containsKey("wifi_capture_handshake")) g_config.wifi_capture_handshake = doc["wifi_capture_handshake"];
    
    if (doc.containsKey("nfc_protocol")) g_config.nfc_protocol = doc["nfc_protocol"];
    if (doc.containsKey("nfc_auto_read")) g_config.nfc_auto_read = doc["nfc_auto_read"];
    if (doc.containsKey("nfc_beep_on_read")) g_config.nfc_beep_on_read = doc["nfc_beep_on_read"];
    
    if (doc.containsKey("ir_protocol")) g_config.ir_protocol = doc["ir_protocol"];
    if (doc.containsKey("ir_repeat_enabled")) g_config.ir_repeat_enabled = doc["ir_repeat_enabled"];
    if (doc.containsKey("ir_repeat_count")) g_config.ir_repeat_count = doc["ir_repeat_count"];
    if (doc.containsKey("ir_repeat_delay_ms")) g_config.ir_repeat_delay_ms = doc["ir_repeat_delay_ms"];
    
    if (doc.containsKey("ai_epsilon")) g_config.ai_epsilon = doc["ai_epsilon"];
    if (doc.containsKey("ai_learning_rate")) g_config.ai_learning_rate = doc["ai_learning_rate"];
    if (doc.containsKey("ai_discount_factor")) g_config.ai_discount_factor = doc["ai_discount_factor"];
    if (doc.containsKey("ai_auto_learn")) g_config.ai_auto_learn = doc["ai_auto_learn"];
    if (doc.containsKey("ai_voice_feedback")) g_config.ai_voice_feedback = doc["ai_voice_feedback"];
    
    if (doc.containsKey("auto_save_captures")) g_config.auto_save_captures = doc["auto_save_captures"];
    if (doc.containsKey("auto_log_attacks")) g_config.auto_log_attacks = doc["auto_log_attacks"];
    if (doc.containsKey("capture_folder")) strlcpy(g_config.capture_folder, doc["capture_folder"], sizeof(g_config.capture_folder));
    
    if (doc.containsKey("stealth_mode")) g_config.stealth_mode = doc["stealth_mode"];
    if (doc.containsKey("debug_serial")) g_config.debug_serial = doc["debug_serial"];
    if (doc.containsKey("watchdog_timeout_sec")) g_config.watchdog_timeout_sec = doc["watchdog_timeout_sec"];
    if (doc.containsKey("attacker_ip")) strlcpy(g_config.attacker_ip, doc["attacker_ip"], sizeof(g_config.attacker_ip));
    
    g_config_dirty = true;
    config_validate();
    
    Serial.println("[CFG] Configuration imported from JSON");
    return true;
}
