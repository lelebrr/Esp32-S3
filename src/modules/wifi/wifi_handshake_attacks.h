/**
 * @file wifi_handshake_attacks.h
 * @brief Ataques de Captura PMKID e Handshake WiFi
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Módulo de ataques de captura para cracking posterior:
 *
 * @section pmkid PMKID Attacks
 * - PMKID Clientless Capture
 * - Rogue AP PMKID Solicitation
 * - Targeted Association Request PMKID
 * - Fast Roaming PMKID Harvest (802.11r)
 * - Multi-Target PMKID Batch Capture
 *
 * @section handshake Handshake Capture
 * - WPA2 4-Way Handshake Capture Passivo
 * - Deauth + Handshake Capture
 * - Deauth Storm + Multi-Handshake
 * - Channel Hopping Handshake Sniff
 * - EAPOL Logoff + Reconnect Capture
 *
 * @section downgrade Downgrade Attacks
 * - Evil Twin WPA2 Downgrade
 * - Transition Mode Exploit (Dragonblood 2024)
 * - Mixed Mode Downgrade SAE to PSK
 * - WPA3 SAE Partial Handshake Capture
 *
 * @warning Use apenas para fins educacionais e em redes autorizadas.
 */

#ifndef __WIFI_HANDSHAKE_ATTACKS_H__
#define __WIFI_HANDSHAKE_ATTACKS_H__

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ============================================================================
// ENUMS E CONSTANTES
// ============================================================================

/**
 * @brief Tipos de ataque de captura
 */
enum WiFiCaptureAttack {
    // PMKID Attacks
    WIFI_CAP_PMKID_CLIENTLESS = 0,  ///< PMKID Clientless Capture
    WIFI_CAP_ROGUE_AP_PMKID,         ///< Rogue AP PMKID Solicitation
    WIFI_CAP_ASSOC_REQ_PMKID,        ///< Targeted Association Request PMKID
    WIFI_CAP_FAST_ROAMING_PMKID,     ///< Fast Roaming PMKID Harvest (802.11r)
    WIFI_CAP_MULTI_TARGET_PMKID,     ///< Multi-Target PMKID Batch Capture
    WIFI_CAP_INVALID_PMKID_REPLAY,   ///< Invalid PMKID Replay Assist

    // Handshake Capture
    WIFI_CAP_PASSIVE_HANDSHAKE,      ///< WPA2 4-Way Handshake Passivo
    WIFI_CAP_DEAUTH_HANDSHAKE,       ///< Deauth + Handshake Capture
    WIFI_CAP_DEAUTH_STORM_MULTI,     ///< Deauth Storm + Multi-Handshake
    WIFI_CAP_CHANNEL_HOP_SNIFF,      ///< Channel Hopping Handshake Sniff
    WIFI_CAP_EAPOL_LOGOFF,           ///< EAPOL Logoff + Reconnect Capture
    WIFI_CAP_BEACON_SPAM_FORCE,      ///< Beacon Spam + Handshake Force
    WIFI_CAP_MALFORMED_EAPOL,        ///< Malformed EAPOL Start Flood
    WIFI_CAP_KARMA_PROBE_RESP,       ///< Karma-like Probe Response

    // Downgrade Attacks
    WIFI_CAP_EVIL_TWIN_DOWNGRADE,    ///< Evil Twin WPA2 Downgrade
    WIFI_CAP_TRANSITION_EXPLOIT,     ///< Transition Mode Exploit (Dragonblood)
    WIFI_CAP_SAE_TO_PSK,             ///< Mixed Mode Downgrade SAE to PSK
    WIFI_CAP_WPA3_SAE_PARTIAL,       ///< WPA3 SAE Partial Capture
    WIFI_CAP_SSID_CONFUSION_CAP,     ///< SSID Confusion Assisted Capture
    WIFI_CAP_EVIL_TWIN_CAPTIVE       ///< Evil Twin com Captive Portal
};

/**
 * @brief Estrutura de handshake capturado
 */
struct __attribute__((packed)) CapturedHandshake {
    uint8_t bssid[6];
    uint8_t client_mac[6];
    char ssid[33];
    uint8_t anonce[32];
    uint8_t snonce[32];
    uint8_t mic[16];
    uint8_t eapol_frame[128]; // Reduzido de 256 para economizar memória (max WPA2 ~120 bytes)
    uint16_t eapol_len;
    bool is_complete;
    bool has_pmkid;
    uint8_t pmkid[16];
};

/**
 * @brief Estrutura de PMKID capturado
 */
struct __attribute__((packed)) CapturedPMKID {
    uint8_t bssid[6];
    uint8_t client_mac[6];
    char ssid[33];
    uint8_t pmkid[16];
    unsigned long timestamp;
};

// ============================================================================
// FUNÇÕES PRINCIPAIS
// ============================================================================

/**
 * @brief Inicializa o módulo de captura
 */
void wifi_cap_init();

/**
 * @brief Obtém handshakes capturados
 * @return Ponteiro para array de CapturedHandshake
 */
CapturedHandshake* wifi_cap_get_handshakes();

/**
 * @brief Obtém número de handshakes capturados
 * @return Quantidade
 */
int wifi_cap_get_handshake_count();

/**
 * @brief Obtém PMKIDs capturados
 * @return Ponteiro para array de CapturedPMKID
 */
CapturedPMKID* wifi_cap_get_pmkids();

/**
 * @brief Obtém número de PMKIDs capturados
 * @return Quantidade
 */
int wifi_cap_get_pmkid_count();

/**
 * @brief Salva capturas no SD Card
 * @param filename Nome do arquivo
 * @return true se salvou
 */
bool wifi_cap_save_to_sd(const char* filename);

// ============================================================================
// PMKID ATTACKS
// ============================================================================

/**
 * @brief PMKID Clientless Capture (2018-2025)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note Alta sucesso em APs com roaming ativado
 */
bool wifi_cap_pmkid_clientless(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Rogue AP PMKID Solicitation
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_rogue_ap_pmkid(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Targeted Association Request PMKID
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_assoc_req_pmkid(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Fast Roaming PMKID Harvest (802.11r)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note Explora 802.11r para capturar sem deauth
 */
bool wifi_cap_fast_roaming_pmkid(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Multi-Target PMKID Batch Capture
 * @return true se iniciou
 * @note Coleta PMKIDs em massa de múltiplos APs
 */
bool wifi_cap_multi_target_pmkid();

// ============================================================================
// HANDSHAKE CAPTURE
// ============================================================================

/**
 * @brief WPA2 4-Way Handshake Capture Passivo
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note 100% em redes ativas com clientes
 */
bool wifi_cap_passive_handshake(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Deauth + Handshake Capture
 * @param target_bssid BSSID do AP
 * @param target_client MAC do cliente (ou broadcast)
 * @param channel Canal
 * @return true se iniciou
 * @note Clássico e efetivo
 */
bool wifi_cap_deauth_handshake(uint8_t* target_bssid, uint8_t* target_client, uint8_t channel);

/**
 * @brief Deauth Storm + Multi-Handshake Capture
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_deauth_storm_multi(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Channel Hopping Handshake Sniff
 * @return true se iniciou
 * @note Varre canais capturando handshakes dual-band
 */
bool wifi_cap_channel_hop_sniff();

/**
 * @brief EAPOL Logoff + Reconnect Capture
 * @param target_bssid BSSID do AP
 * @param target_client MAC do cliente
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_eapol_logoff(uint8_t* target_bssid, uint8_t* target_client, uint8_t channel);

/**
 * @brief Beacon Spam + Handshake Force
 * @param target_ssid SSID alvo
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_beacon_spam_force(const char* target_ssid, uint8_t channel);

/**
 * @brief Karma-like Probe Response Handshake
 * @return true se iniciou
 * @note Responde probes induzindo associação
 */
bool wifi_cap_karma_probe_resp();

// ============================================================================
// DOWNGRADE ATTACKS
// ============================================================================

/**
 * @brief Evil Twin WPA2 Downgrade
 * @param target_ssid SSID alvo
 * @param channel Canal
 * @return true se iniciou
 * @note Força clientes WPA3 transition a usar WPA2
 */
bool wifi_cap_evil_twin_downgrade(const char* target_ssid, uint8_t channel);

/**
 * @brief Transition Mode Exploit (Dragonblood 2024)
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_transition_exploit(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Mixed Mode Downgrade SAE to PSK (2025)
 * @param target_ssid SSID alvo
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_sae_to_psk(const char* target_ssid, uint8_t channel);

/**
 * @brief WPA3 SAE Partial Handshake Capture
 * @param target_bssid BSSID do AP
 * @param channel Canal
 * @return true se iniciou
 * @note Captura commits SAE para side-channel
 */
bool wifi_cap_wpa3_sae_partial(uint8_t* target_bssid, uint8_t channel);

/**
 * @brief Evil Twin com Captive Portal Handshake
 * @param target_ssid SSID alvo
 * @param channel Canal
 * @return true se iniciou
 */
bool wifi_cap_evil_twin_captive(const char* target_ssid, uint8_t channel);

// ============================================================================
// CONTROLE
// ============================================================================

/**
 * @brief Para captura
 */
void wifi_cap_stop();

/**
 * @brief Limpa capturas
 */
void wifi_cap_clear();

/**
 * @brief Atualiza captura (chamar no loop)
 * @return Número de novas capturas
 */
int wifi_cap_update();

/**
 * @brief Verifica se tem handshake completo
 * @return true se capturou handshake válido
 */
bool wifi_cap_has_complete_handshake();

#endif // __WIFI_HANDSHAKE_ATTACKS_H__
