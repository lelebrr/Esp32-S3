/**
 * @file module_manager.cpp
 * @brief Implementação do Gerenciador de Módulos Exclusivos
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "module_manager.h"
#include "pin_config.h"
#include "rf_core.h"

// ESP-IDF includes para controle de rádios
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <NimBLEDevice.h>
#include <WiFi.h>

// Static member initialization
SemaphoreHandle_t ModuleManager::_mutex = nullptr;
ModuleType ModuleManager::_activeModule = MODULE_NONE;

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void ModuleManager::init() {
    if (_mutex == nullptr) {
        _mutex = xSemaphoreCreateMutex();
    }
    _activeModule = MODULE_NONE;
    Serial.println("[MOD] Module Manager initialized");
}

// ============================================================================
// CONTROLE DE MUTEX
// ============================================================================

bool ModuleManager::takeMutex(uint32_t timeout_ms) {
    if (_mutex == nullptr) {
        init();
    }
    return xSemaphoreTake(_mutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

void ModuleManager::giveMutex() {
    if (_mutex != nullptr) {
        xSemaphoreGive(_mutex);
    }
}

// ============================================================================
// DESLIGAR MÓDULOS INDIVIDUAIS
// ============================================================================

void ModuleManager::desligaBLE() {
    Serial.println("[MOD] Desligando BLE...");
    
    // NimBLE deinit
    if (NimBLEDevice::getInitialized()) {
        NimBLEDevice::deinit(true);
    }
    
    // Desabilita controller BT
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_bt_controller_disable();
    }
    
    Serial.println("[MOD] BLE desligado");
}

void ModuleManager::desligaWiFi() {
    Serial.println("[MOD] Desligando WiFi...");
    
    // Para modo promíscuo se ativo
    esp_wifi_set_promiscuous(false);
    
    // Desconecta AP/STA
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
    
    // Para WiFi
    esp_wifi_stop();
    WiFi.mode(WIFI_OFF);
    
    Serial.println("[MOD] WiFi desligado");
}

void ModuleManager::desligaSubGHz() {
    Serial.println("[MOD] Desligando SubGHz CC1101...");
    
    // Para qualquer operação ativa
    if (RFCore::isInitialized()) {
        RFCore::stopJammer();
        RFCore::stopReceive();
        RFCore::stop();  // Coloca CC1101 em sleep
    }
    
    Serial.println("[MOD] SubGHz desligado");
}

void ModuleManager::desligaIR() {
    Serial.println("[MOD] Desligando IR YS-IRTM...");
    
    // Desabilita UART do YS-IRTM
    Serial1.end();
    
    // Pinos em low power
    pinMode(PIN_YS_IR_TX, INPUT);
    pinMode(PIN_YS_IR_RX, INPUT);
    
    Serial.println("[MOD] IR desligado");
}

void ModuleManager::desligaNFC() {
    Serial.println("[MOD] Desligando NFC PN532...");
    
    // PN532 não tem sleep via software no Adafruit lib
    // Apenas para comunicação I2C
    // Wire.end(); // Cuidado: pode afetar outros dispositivos I2C
    
    Serial.println("[MOD] NFC em idle");
}

void ModuleManager::desligaFault() {
    Serial.println("[MOD] Desligando MOSFET Fault...");
    
    // Garante que MOSFET está desligado
    digitalWrite(PIN_FAULT_GATE, LOW);
    
    Serial.println("[MOD] Fault desligado");
}

// ============================================================================
// DESLIGAR TODOS OS MÓDULOS
// ============================================================================

void ModuleManager::desligaModulos() {
    Serial.println("[MOD] ===== DESLIGANDO TODOS OS MÓDULOS =====");
    
    desligaBLE();
    desligaWiFi();
    desligaSubGHz();
    desligaIR();
    desligaNFC();
    desligaFault();
    
    _activeModule = MODULE_NONE;
    
    Serial.println("[MOD] Todos os módulos desligados");
}

// ============================================================================
// ATIVAR MÓDULO (COM EXCLUSÃO MÚTUA)
// ============================================================================

bool ModuleManager::ativaModulo(ModuleType module) {
    if (!takeMutex(1000)) {
        Serial.println("[MOD] ERRO: Não conseguiu mutex!");
        return false;
    }
    
    // Se já está ativo, retorna
    if (_activeModule == module) {
        giveMutex();
        return true;
    }
    
    Serial.printf("[MOD] Ativando módulo: %d\n", module);
    
    // Lógica de exclusão
    switch (module) {
        case MODULE_BLE:
            // BLE exclui WiFi
            desligaWiFi();
            break;
            
        case MODULE_WIFI:
            // WiFi exclui BLE
            desligaBLE();
            break;
            
        case MODULE_SUBGHZ:
            // SubGHz exclui IR (conflito de timing)
            desligaIR();
            break;
            
        case MODULE_IR:
            // IR exclui SubGHz
            desligaSubGHz();
            break;
            
        case MODULE_FAULT:
            // Fault desliga TUDO por segurança
            desligaModulos();
            break;
            
        case MODULE_NFC:
            // NFC pode coexistir, mas desliga fault
            desligaFault();
            break;
            
        default:
            break;
    }
    
    _activeModule = module;
    
    giveMutex();
    return true;
}

void ModuleManager::desativaModulo() {
    if (takeMutex(1000)) {
        _activeModule = MODULE_NONE;
        giveMutex();
    }
}

ModuleType ModuleManager::getActiveModule() {
    return _activeModule;
}

bool ModuleManager::isModuleActive(ModuleType module) {
    return _activeModule == module;
}
