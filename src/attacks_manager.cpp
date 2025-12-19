#include "attacks_manager.h"
#include "s3_driver.h"
#include "pin_config.h"

// External Dependencies
#include <WiFi.h>
#include <esp_wifi.h>
#include <NimBLEDevice.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Globals
AttackType current_attack = ATTACK_NONE;
TaskHandle_t attack_task_handle = NULL;
volatile bool stop_requested = false;

// IR Object
IRsend irsend(PIN_IR_TX);

// --- HELPER WRAPPERS ---
void set_module_power(int mod_type) {
    // 0=None, 1=CC1101, 2=PN532, 3=BLE
    bool cc = (mod_type == 1);
    bool nfc = (mod_type == 2);
    bool ble = (mod_type == 3);
    MonsterDriver::powerOnModules(false, ble, nfc, cc);
}

// =================================================================================
// TASKS (Simplified for Compilation, Logic remains similar to original)
// =================================================================================

void task_ble_spam(void *pvParameters) {
    Serial.println("[ATTACK] Starting BLE Spam...");
    set_module_power(3); // BLE
    NimBLEDevice::init("MonsterS3");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    // ... payload logic ...
    pAdvertising->start(); 
    while(!stop_requested) { delay(100); }
    NimBLEDevice::deinit();
    vTaskDelete(NULL);
}

void task_wifi_deauth(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Deauth...");
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    // ... logic ...
    while(!stop_requested) { delay(100); }
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// ... other tasks stubs that call specific HW ...

// =================================================================================
// MANAGER
// =================================================================================

void attacks_init() {
    stop_requested = false;
    current_attack = ATTACK_NONE;
}

void attacks_stop() {
    if (current_attack != ATTACK_NONE) {
        stop_requested = true;
        delay(200);
        if (attack_task_handle != NULL) {
            vTaskDelete(attack_task_handle);
            attack_task_handle = NULL;
        }
        set_module_power(0); // Off
        current_attack = ATTACK_NONE;
    }
}

void attacks_start(AttackType type) {
    if (current_attack == type) return;
    attacks_stop();
    stop_requested = false;
    current_attack = type;
    
    if (type == ATTACK_BLE_SPAM) {
        xTaskCreate(task_ble_spam, "Att_BLE", 4096, NULL, 1, &attack_task_handle);
    } else if (type == ATTACK_WIFI_DEAUTH) {
        xTaskCreate(task_wifi_deauth, "Att_WiFi", 4096, NULL, 1, &attack_task_handle);
    }
    // Add others
}

bool attacks_is_running() { return current_attack != ATTACK_NONE; }
void attacks_report_success() {}
void attacks_report_fail() {}
