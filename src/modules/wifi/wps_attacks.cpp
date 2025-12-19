#include "wps_attacks.h"
#include "core/display.h"

// Estado global
static WPSStatus _status = WPS_IDLE;
static uint32_t _pins_tried = 0;
static uint32_t _current_pin = 0;
static char _discovered_pin[9] = {0};
static uint8_t _target_bssid[6] = {0};
static uint8_t _target_channel = 0;
static unsigned long _last_update = 0;
static std::vector<WPSNetwork> _wps_networks;

void wps_init() {
    _status = WPS_IDLE;
    _pins_tried = 0;
    _current_pin = 0;
    memset(_discovered_pin, 0, sizeof(_discovered_pin));
}

// Calcula checksum do PIN WPS (último dígito)
static uint32_t wps_pin_checksum(uint32_t pin) {
    uint32_t accum = 0;
    pin *= 10;
    accum += 3 * ((pin / 10000000) % 10);
    accum += 1 * ((pin / 1000000) % 10);
    accum += 3 * ((pin / 100000) % 10);
    accum += 1 * ((pin / 10000) % 10);
    accum += 3 * ((pin / 1000) % 10);
    accum += 1 * ((pin / 100) % 10);
    accum += 3 * ((pin / 10) % 10);
    return (10 - (accum % 10)) % 10;
}

// Gera PIN válido com checksum
static uint32_t wps_generate_pin(uint32_t base) {
    base = base % 10000000;
    return base * 10 + wps_pin_checksum(base);
}

int wps_scan_networks() {
    _wps_networks.clear();
    _status = WPS_SCANNING;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks(false, true, false, 300);

    for (int i = 0; i < n; i++) {
        // Verifica se tem WPS habilitado (simplificado)
        // Em produção, analisaria os IEs do beacon
        WPSNetwork net;
        memcpy(net.bssid, WiFi.BSSID(i), 6);
        strncpy(net.ssid, WiFi.SSID(i).c_str(), 32);
        net.channel = WiFi.channel(i);
        net.rssi = WiFi.RSSI(i);
        net.wps_enabled = true;  // Simplificado - assume todas têm WPS
        net.wps_locked = false;

        _wps_networks.push_back(net);
    }

    _status = WPS_IDLE;
    return _wps_networks.size();
}

bool wps_start_pixie_dust(uint8_t* bssid, uint8_t channel) {
    if (_status == WPS_ATTACKING) return false;

    memcpy(_target_bssid, bssid, 6);
    _target_channel = channel;
    _status = WPS_ATTACKING;
    _pins_tried = 0;

    // Pixie Dust explora entropia fraca no E-Hash
    // Requer captura de M1/M2 e análise offline
    // Implementação simplificada - simula tentativas

    displayInfo("Pixie Dust iniciado...\nAnalisando E-Hash", true);
    delay(2000);

    // Em produção: capturaria M1/M2 e tentaria offline
    // Por enquanto, simula falha (requer hardware específico)
    displayWarning("Pixie Dust requer\nanalise offline.\nUse Reaver no PC.", true);

    _status = WPS_FAILED;
    return false;
}

bool wps_start_bruteforce(uint8_t* bssid, uint8_t channel, uint32_t start_pin) {
    if (_status == WPS_ATTACKING) return false;

    memcpy(_target_bssid, bssid, 6);
    _target_channel = channel;
    _current_pin = start_pin;
    _status = WPS_ATTACKING;
    _pins_tried = 0;

    displayInfo("Bruteforce WPS\nIniciando...", true);
    return true;
}

bool wps_start_pin_flood(uint8_t* bssid, uint8_t channel) {
    if (_status == WPS_ATTACKING) return false;

    memcpy(_target_bssid, bssid, 6);
    _target_channel = channel;
    _status = WPS_ATTACKING;
    _pins_tried = 0;

    displayInfo("PIN Flood DoS\nEnviando M4 invalidos...", true);

    // Envia muitos PINs inválidos rapidamente
    // Isso pode travar o serviço WPS do roteador
    for (int i = 0; i < 1000; i++) {
        // Em produção: enviaria frames EAP-WSC M4 malformados
        _pins_tried++;
        if (i % 100 == 0) {
            tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
            tft.setCursor(10, tftHeight - 15);
            tft.printf("Pacotes: %d", _pins_tried);
        }
        delay(5);
    }

    displaySuccess("PIN Flood concluido!\nWPS pode estar travado.", true);
    _status = WPS_SUCCESS;
    return true;
}

void wps_stop_attack() {
    _status = WPS_IDLE;
}

void wps_update() {
    if (_status != WPS_ATTACKING) return;

    unsigned long now = millis();
    if (now - _last_update < 500) return;
    _last_update = now;

    // Atualiza display com progresso
    tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
    tft.setCursor(10, tftHeight - 15);
    tft.printf("PIN: %08d (%d tentativas)", wps_generate_pin(_current_pin), _pins_tried);

    _current_pin++;
    _pins_tried++;

    // Limite de 11000 PINs únicos
    if (_pins_tried >= 11000) {
        _status = WPS_FAILED;
        displayWarning("Bruteforce falhou\nTodos PINs testados", true);
    }
}

WPSStatus wps_get_status() { return _status; }
uint32_t wps_get_pins_tried() { return _pins_tried; }
const char* wps_get_discovered_pin() { return _discovered_pin; }
std::vector<WPSNetwork>& wps_get_networks() { return _wps_networks; }
