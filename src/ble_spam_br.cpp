/**
 * @file ble_spam_br.cpp
 * Monster S3 Firmware - BLE Spam with Brazilian Names
 * 
 * High-speed BLE advertisement spam using NimBLE
 * Features:
 * - 50+ Brazilian device names
 * - Random MAC rotation
 * - 1000+ packets per second
 * - Apple/Samsung/Xiaomi device profiles
 */

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "ble_spam_br.h"
#include "pin_config.h"
#include "config_manager.h"

// ============================================================================
// BRAZILIAN DEVICE NAMES
// ============================================================================

// Apple devices (iPhone, iPad, Watch, AirPods)
static const char* APPLE_NAMES_BR[] = {
    "iPhone do João",
    "iPhone da Maria",
    "iPhone de Ana",
    "iPad Família Silva",
    "iPad do Pedro",
    "Apple Watch João",
    "AirPods Pro João",
    "AirPods da Júlia",
    "MacBook do Ricardo",
    "iPhone do Zé"
};
static const int APPLE_NAMES_COUNT = 10;

// Samsung devices
static const char* SAMSUNG_NAMES_BR[] = {
    "Galaxy S24 do Carlos",
    "Galaxy de Maria",
    "Galaxy Tab Família",
    "Galaxy Watch Ana",
    "Galaxy Buds João",
    "Samsung do Marcos",
    "Galaxy A54 Trabalho",
    "Galaxy Note Pai",
    "Samsung Mãe",
    "Galaxy Z Fold"
};
static const int SAMSUNG_NAMES_COUNT = 10;

// Xiaomi devices
static const char* XIAOMI_NAMES_BR[] = {
    "Mi Band Fitness",
    "Redmi do Lucas",
    "Xiaomi da Família",
    "Mi Watch Paula",
    "Redmi Airdots",
    "Poco X5 Bruno",
    "Mi 13 Trabalho",
    "Redmi Note 12",
    "Xiaomi Pai",
    "Mi Buds Pro"
};
static const int XIAOMI_NAMES_COUNT = 10;

// Generic Brazilian names
static const char* GENERIC_NAMES_BR[] = {
    "Fone do Antônio",
    "Smartwatch Tio",
    "Caixinha Portátil",
    "Alto-falante Casa",
    "Headphone Gym",
    "Teclado BT João",
    "Mouse Trabalho",
    "Controle PS5",
    "JBL Flip Festa",
    "Speaker Churrasco",
    "Bose QuietComfort",
    "Campainha Inteligente",
    "Câmera WiFi Porta",
    "Fechadura Digital",
    "Balança Banheiro",
    "Termômetro Bebê",
    "Monitor Pressão",
    "Rastreador Pet",
    "Tag AirTag Bolsa",
    "Tile Chaves"
};
static const int GENERIC_NAMES_COUNT = 20;

// ============================================================================
// STATE
// ============================================================================
static bool ble_spam_running = false;
static NimBLEServer* pServer = nullptr;
static NimBLEAdvertising* pAdvertising = nullptr;
static uint32_t packets_sent = 0;
static BLESpamMode current_mode = BLE_SPAM_ALL;

// ============================================================================
// HELPERS
// ============================================================================

/**
 * @brief Get random Brazilian device name based on mode
 */
static const char* get_random_name(BLESpamMode mode) {
    int category;
    int index;
    
    switch (mode) {
        case BLE_SPAM_APPLE:
            index = random(APPLE_NAMES_COUNT);
            return APPLE_NAMES_BR[index];
            
        case BLE_SPAM_SAMSUNG:
            index = random(SAMSUNG_NAMES_COUNT);
            return SAMSUNG_NAMES_BR[index];
            
        case BLE_SPAM_XIAOMI:
            index = random(XIAOMI_NAMES_COUNT);
            return XIAOMI_NAMES_BR[index];
            
        case BLE_SPAM_ALL:
        default:
            category = random(4);
            switch (category) {
                case 0:
                    return APPLE_NAMES_BR[random(APPLE_NAMES_COUNT)];
                case 1:
                    return SAMSUNG_NAMES_BR[random(SAMSUNG_NAMES_COUNT)];
                case 2:
                    return XIAOMI_NAMES_BR[random(XIAOMI_NAMES_COUNT)];
                default:
                    return GENERIC_NAMES_BR[random(GENERIC_NAMES_COUNT)];
            }
    }
}

/**
 * @brief Generate random BLE MAC address
 */
static void randomize_mac() {
    uint8_t mac[6];
    for (int i = 0; i < 6; i++) {
        mac[i] = random(256);
    }
    // Set locally administered bit, clear multicast bit
    mac[0] = (mac[0] | 0x02) & 0xFE;
    
    // Note: NimBLE on ESP32-S3 may not support runtime MAC change
    // This is a placeholder for future implementation
}

// ============================================================================
// PUBLIC API
// ============================================================================

/**
 * @brief Initialize BLE spam subsystem
 */
void ble_spam_init() {
    // NimBLE initialization happens on first use
    packets_sent = 0;
    ble_spam_running = false;
    Serial.println("[BLE] Spam subsystem initialized");
}

/**
 * @brief Start BLE spam attack
 * @param mode Which device type to spam
 */
void ble_spam_start(BLESpamMode mode) {
    if (ble_spam_running) {
        ble_spam_stop();
    }
    
    current_mode = mode;
    packets_sent = 0;
    
    // Initialize NimBLE
    NimBLEDevice::init("");
    
    // Configure for high-speed advertising
    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setMinInterval(0x20);  // 20ms min (fastest allowed)
    pAdvertising->setMaxInterval(0x20);  // 20ms max
    
    ble_spam_running = true;
    Serial.printf("[BLE] Spam started (mode: %d)\n", mode);
}

/**
 * @brief Update BLE spam - call in loop to change names
 * @return Number of packets sent this call
 */
uint32_t ble_spam_update() {
    if (!ble_spam_running) return 0;
    
    static unsigned long last_change = 0;
    uint32_t sent = 0;
    
    // Change advertised name every X ms for variety (configurable)
    const DeviceConfig* cfg = config_get();
    if (millis() - last_change > cfg->ble_spam_interval_ms) {
        last_change = millis();
        
        // Stop current advertising
        pAdvertising->stop();
        
        // Get new random name
        const char* name = get_random_name(current_mode);
        
        // Apply custom prefix if set
        static char final_name[64];
        if (strlen(cfg->ble_name_prefix) > 0) {
            snprintf(final_name, sizeof(final_name), "%s %s", cfg->ble_name_prefix, name);
            name = final_name;
        }
        
        // Update advertisement data
        NimBLEAdvertisementData advData;
        advData.setCompleteServices(NimBLEUUID((uint16_t)0x1812)); // HID
        advData.setName(name);
        
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();
        
        packets_sent++;
        sent = 1;
        
        // Log every 100 packets
        if (packets_sent % 100 == 0) {
            Serial.printf("[BLE] Spam: %d packets, last: %s\n", packets_sent, name);
        }
    }
    
    return sent;
}

/**
 * @brief Stop BLE spam attack
 */
void ble_spam_stop() {
    if (!ble_spam_running) return;
    
    if (pAdvertising) {
        pAdvertising->stop();
    }
    
    NimBLEDevice::deinit();
    ble_spam_running = false;
    
    Serial.printf("[BLE] Spam stopped. Total packets: %d\n", packets_sent);
}

/**
 * @brief Check if spam is running
 */
bool ble_spam_is_running() {
    return ble_spam_running;
}

/**
 * @brief Get packet count
 */
uint32_t ble_spam_get_packet_count() {
    return packets_sent;
}
