/**
 * @file wifi_advanced_attacks.h
 * @brief Ataques WiFi Avançados 2024-2025
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Módulo de ataques WiFi baseados em CVEs recentes e técnicas avançadas:
 *
 * @section cve_based CVE-Based Attacks
 * - MediaTek SoftAP Zero-Click DoS (CVE-2024-20017)
 * - SSID Confusion Downgrade (CVE-2023-52424)
 * - TP-Link Auth Denial (CVE-2023-1389 inspired)
 * - FragAttacks-like Management Frame Injection
 *
 * @section flood_attacks Flood Attacks
 * - Authentication Flood DoS
 * - Association Flood Overload
 * - Disassociation Flood DoS
 * - Probe Response Flood
 * - EAP Fail Flood DoS
 * - Reassociation Flood DoS
 * - Probe Request Spoof Flood
 *
 * @section advanced Advanced Techniques
 * - Deauth Targeted com Reason Code Crafted
 * - Beacon Frame Fuzzing DoS
 * - Channel Hopping Deauth Storm
 * - Evil Twin AP com Beacon Spam
 * - Custom Management Frame Crash
 *
 * @warning Use apenas para fins educacionais e em redes autorizadas.
 */

#ifndef __WIFI_ADVANCED_ATTACKS_H__
#define __WIFI_ADVANCED_ATTACKS_H__

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ============================================================================
// ENUMS E CONSTANTES
// ============================================================================

/**
 * @brief Tipos de ataque WiFi avançado
 */
enum WiFiAdvancedAttack {
    // CVE-Based
    WIFI_ATK_MEDIATEK_DOS = 0,    ///< CVE-2024-20017 MediaTek SoftAP DoS
    WIFI_ATK_SSID_CONFUSION,      ///< CVE-2023-52424 SSID Confusion
    WIFI_ATK_TPLINK_AUTH_DENIAL,  ///< CVE-2023-1389 inspired
    WIFI_ATK_FRAGATTACKS,         ///< FragAttacks-like 2024

    // Flood Attacks
    WIFI_ATK_AUTH_FLOOD,          ///< Authentication Flood DoS
    WIFI_ATK_ASSOC_FLOOD,         ///< Association Flood Overload
    WIFI_ATK_DISASSOC_FLOOD,      ///< Disassociation Flood DoS
    WIFI_ATK_PROBE_RESP_FLOOD,    ///< Probe Response Flood
    WIFI_ATK_EAP_FAIL_FLOOD,      ///< EAP Fail Flood DoS
    WIFI_ATK_REASSOC_FLOOD,       ///< Reassociation Flood DoS
    WIFI_ATK_PROBE_REQ_SPOOF,     ///< Probe Request Spoof Flood

    // Advanced
    WIFI_ATK_DEAUTH_CRAFTED,      ///< Deauth com Reason Code Crafted
    WIFI_ATK_BEACON_FUZZING,      ///< Beacon Frame Fuzzing DoS
    WIFI_ATK_CHANNEL_HOPPING,     ///< Channel Hopping Deauth Storm
    WIFI_ATK_EVIL_TWIN_SPAM,      ///< Evil Twin AP + Beacon Spam
    WIFI_ATK_RICKROLL_BEACON,     ///< Rickroll Beacon Attack
    WIFI_ATK_NULL_PROBE_RESP,     ///< Null Probe Response DoS
    WIFI_ATK_LOUD_BEACON,         ///< Loud Beacon Attack
    WIFI_ATK_PMKID_REPLAY,        ///< PMKID Capture + Forced Replay
    WIFI_ATK_MGMT_FRAME_CRASH     ///< Custom Management Frame Crash
};

/**
 * @brief Status do ataque
 */
enum WiFiAdvAttackStatus {
    WIFI_ADV_IDLE = 0,
    WIFI_ADV_SCANNING,
    WIFI_ADV_RUNNING,
    WIFI_ADV_SUCCESS,
    WIFI_ADV_FAILED
};

/**
 * @brief Estrutura de alvo WiFi
 */
struct WiFiAdvTarget {
    uint8_t bssid[6];
    char ssid[33];
    uint8_t channel;
    int8_t rssi;
    bool is_wpa3;
    bool is_enterprise;
    bool is_mediatek;  ///< Chipset MediaTek detectado
    bool is_realtek;   ///< Chipset Realtek detectado
};

// ============================================================================
// FUNÇÕES PRINCIPAIS
// ============================================================================

/**
 * @brief Inicializa o módulo de ataques WiFi avançados
 */
void wifi_adv_init();

/**
 * @brief Escaneia redes WiFi para identificar alvos
 * @param scan_time_ms Tempo de scan em ms
 * @return Número de redes encontradas
 */
int wifi_adv_scan(uint32_t scan_time_ms);

/**
 * @brief Obtém lista de alvos
 * @return Ponteiro para array de WiFiAdvTarget
 */
WiFiAdvTarget* wifi_adv_get_targets();

/**
 * @brief Obtém número de alvos
 * @return Quantidade
 */
int wifi_adv_get_target_count();

// ============================================================================
// CVE-BASED ATTACKS
// ============================================================================

/**
 * @brief MediaTek SoftAP Zero-Click DoS (CVE-2024-20017)
 * @param target_bssid BSSID do AP alvo
 * @param channel Canal WiFi
 * @return true se iniciou
 * @note Efetivo em Netgear, Xiaomi, Ubiquiti com chipset MediaTek
 */
bool wifi_adv_mediatek_dos(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief SSID Confusion Downgrade Assist (CVE-2023-52424)
 * @param target_ssid SSID alvo
 * @param channel Canal
 * @return true se iniciou
 * @note Força downgrade de segurança em redes mistas
 */
bool wifi_adv_ssid_confusion(const char* target_ssid, uint8_t channel);

/**
 * @brief FragAttacks-like Management Frame Injection (2024)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_fragattacks(uint8_t* target_bssid, uint8_t channel);

// ============================================================================
// FLOOD ATTACKS
// ============================================================================

/**
 * @brief Authentication Flood DoS (variante 2024)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @param duration_ms Duração do flood
 * @return true se iniciou
 * @note 100% sucesso em APs vulneráveis (TP-Link/Asus antigos)
 */
bool wifi_adv_auth_flood(uint8_t* target_bssid, uint8_t channel, uint32_t duration_ms);

/**
 * @brief Association Flood Overload
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_assoc_flood(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Disassociation Flood DoS
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note Mais efetivo em APs WPA3
 */
bool wifi_adv_disassoc_flood(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Probe Response Flood
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_probe_resp_flood(uint8_t channel);

/**
 * @brief EAP Fail Flood DoS
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_eap_fail_flood(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Reassociation Flood DoS
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_reassoc_flood(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Probe Request Spoof Flood
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_probe_req_spoof(uint8_t channel);

// ============================================================================
// ADVANCED TECHNIQUES
// ============================================================================

/**
 * @brief Deauth Targeted com Reason Code Crafted
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @param reason_code Reason code (valores 6-8 mais destrutivos)
 * @return true se iniciou
 */
bool wifi_adv_deauth_crafted(uint8_t* target_bssid, uint8_t channel, uint16_t reason_code);

/**
 * @brief Beacon Frame Fuzzing DoS
 * @param channel Canal
 * @return true se iniciou
 * @note Explora parsing flaws em MediaTek/Realtek
 */
bool wifi_adv_beacon_fuzzing(uint8_t channel);

/**
 * @brief Channel Hopping Deauth Storm
 * @param target_bssid BSSID do AP (ou broadcast)
 * @return true se iniciou
 * @note Deauth em todos canais (1-14)
 */
bool wifi_adv_channel_hopping_deauth(uint8_t* target_bssid);

/**
 * @brief Evil Twin AP com Beacon Spam
 * @param target_ssid SSID a clonar
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_evil_twin_spam(const char* target_ssid, uint8_t channel);

/**
 * @brief Rickroll Beacon Attack (high rate DoS)
 * @return true se iniciou
 */
bool wifi_adv_rickroll_beacon();

/**
 * @brief Null Probe Response DoS
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_null_probe_resp(uint8_t channel);

/**
 * @brief Loud Beacon Attack (max power interference)
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_loud_beacon(uint8_t channel);

/**
 * @brief PMKID Capture + Forced Replay
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_adv_pmkid_replay(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Custom Management Frame Crash (fuzzing 2025)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note Alta chance de crash em Realtek/MediaTek
 */
bool wifi_adv_mgmt_frame_crash(uint8_t* target_bssid, uint8_t channel);

// ============================================================================
// CONTROLE
// ============================================================================

/**
 * @brief Para todos os ataques
 */
void wifi_adv_stop_all();

/**
 * @brief Obtém status
 * @return WiFiAdvAttackStatus
 */
WiFiAdvAttackStatus wifi_adv_get_status();

/**
 * @brief Atualiza ataques (chamar no loop)
 * @return Pacotes enviados nesta iteração
 */
int wifi_adv_update();

/**
 * @brief Obtém estatísticas
 * @param packets_sent Total de pacotes
 * @param aps_crashed APs que crasharam (detectados)
 */
void wifi_adv_get_stats(uint32_t* packets_sent, uint32_t* aps_crashed);

#endif // __WIFI_ADVANCED_ATTACKS_H__
