/**
 * @file ui_networks_screen.cpp
 * @brief Implementação da tela de redes WiFi
 */

#include "ui_networks_screen.h"
#include "../ui_main.h"
#include <debug_log.h>

NetworksScreen networksScreen;

NetworksScreen::NetworksScreen() : _screen(nullptr), _list(nullptr), _networkCount(0), _onSelect(nullptr) {}

void NetworksScreen::create(lv_obj_t *parent) {
    _screen = lv_obj_create(parent);
    lv_obj_set_size(_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_border_width(_screen, 0, 0);
    lv_obj_set_style_pad_all(_screen, 5, 0);
    lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 40);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "📶 Redes WiFi");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff88), 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 5, 0);

    _lblCount = lv_label_create(header);
    lv_label_set_text(_lblCount, "(0)");
    lv_obj_set_style_text_color(_lblCount, lv_color_hex(0x888888), 0);
    lv_obj_align(_lblCount, LV_ALIGN_RIGHT_MID, -50, 0);

    // Botão Scan
    _btnScan = lv_btn_create(header);
    lv_obj_set_size(_btnScan, 45, 30);
    lv_obj_align(_btnScan, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(_btnScan, lv_color_hex(0x00aa66), 0);
    lv_obj_add_event_cb(_btnScan, onScanClick, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *lblScan = lv_label_create(_btnScan);
    lv_label_set_text(lblScan, "🔄");
    lv_obj_center(lblScan);

    // Lista de redes
    _list = lv_list_create(_screen);
    lv_obj_set_size(_list, lv_pct(100), lv_pct(75));
    lv_obj_align(_list, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(_list, lv_color_hex(0x0f0f23), 0);
    lv_obj_set_style_border_width(_list, 0, 0);
    lv_obj_set_style_radius(_list, 8, 0);

    // Footer com botão voltar
    _btnBack = lv_btn_create(_screen);
    lv_obj_set_size(_btnBack, 80, 30);
    lv_obj_align(_btnBack, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(_btnBack, lv_color_hex(0x333355), 0);
    lv_obj_add_event_cb(_btnBack, onBackClick, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *lblBack = lv_label_create(_btnBack);
    lv_label_set_text(lblBack, "← Voltar");
    lv_obj_center(lblBack);
}

void NetworksScreen::show() {
    if (_screen) {
        lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
        update();
    }
}

void NetworksScreen::hide() {
    if (_screen) lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void NetworksScreen::update() {
    char buf[16];
    snprintf(buf, sizeof(buf), "(%d)", _networkCount);
    lv_label_set_text(_lblCount, buf);
}

void NetworksScreen::setNetworks(const PwnNetwork *networks, int count) {
    _networkCount = count > 50 ? 50 : count;
    memcpy(_networks, networks, sizeof(PwnNetwork) * _networkCount);
    populateList();
    update();
}

void NetworksScreen::populateList() {
    if (!_list) return;

    // Limpa lista existente
    lv_obj_clean(_list);

    for (int i = 0; i < _networkCount; i++) {
        char buf[64];
        snprintf(
            buf,
            sizeof(buf),
            "%s %s CH%d %ddBm",
            getSecurityIcon(_networks[i].encryption),
            _networks[i].ssid,
            _networks[i].channel,
            _networks[i].rssi
        );

        lv_obj_t *btn = lv_list_add_btn(_list, nullptr, buf);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x1a1a3e), 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
        lv_obj_set_style_text_color(btn, getRSSIColor(_networks[i].rssi), 0);
        lv_obj_add_event_cb(btn, onItemClick, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

const char *NetworksScreen::getSecurityIcon(uint8_t enc) {
    switch (enc) {
        case 0: return "🔓"; // Open
        case 1: return "🔐"; // WEP
        case 2: return "🔒"; // WPA
        case 3: return "🔒"; // WPA2
        case 4: return "🛡️";  // WPA3
        default: return "❓";
    }
}

lv_color_t NetworksScreen::getRSSIColor(int8_t rssi) {
    if (rssi > -50) return lv_color_hex(0x00ff88); // Excelente
    if (rssi > -70) return lv_color_hex(0xffff00); // Bom
    if (rssi > -80) return lv_color_hex(0xff8800); // Médio
    return lv_color_hex(0xff4444);                 // Fraco
}

void NetworksScreen::onItemClick(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    LOG_UI("Selecionou rede %d: %s", index, networksScreen._networks[index].ssid);

    if (networksScreen._onSelect) { networksScreen._onSelect(&networksScreen._networks[index]); }

    // TODO: Mostrar opções (ataque, info, etc.)
}

void NetworksScreen::onScanClick(lv_event_t *e) {
    LOG_UI("Iniciando scan...");
    extern void ui_start_wifi_scan();
    ui_start_wifi_scan();
}

void NetworksScreen::onBackClick(lv_event_t *e) {
    networksScreen.hide();
    ui_main_show();
}
