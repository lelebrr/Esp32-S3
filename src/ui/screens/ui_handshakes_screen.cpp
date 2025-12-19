/**
 * @file ui_handshakes_screen.cpp
 * @brief Implementação da tela de handshakes
 */

#include "ui_handshakes_screen.h"
#include "../ui_main.h"
#include <SD_MMC.h>
#include <debug_log.h>


HandshakesScreen handshakesScreen;

HandshakesScreen::HandshakesScreen() : _screen(nullptr), _count(0) {}

void HandshakesScreen::create(lv_obj_t *parent) {
    _screen = lv_obj_create(parent);
    lv_obj_set_size(_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_border_width(_screen, 0, 0);
    lv_obj_set_style_pad_all(_screen, 5, 0);
    lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);

    // Header
    lv_obj_t *title = lv_label_create(_screen);
    lv_label_set_text(title, "🔐 Handshakes");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff88), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 5, 5);

    _lblCount = lv_label_create(_screen);
    lv_label_set_text(_lblCount, "(0)");
    lv_obj_set_style_text_color(_lblCount, lv_color_hex(0x888888), 0);
    lv_obj_align(_lblCount, LV_ALIGN_TOP_MID, 0, 8);

    // Botões de ação
    lv_obj_t *btnBar = lv_obj_create(_screen);
    lv_obj_set_size(btnBar, lv_pct(100), 35);
    lv_obj_align(btnBar, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_bg_opa(btnBar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btnBar, 0, 0);
    lv_obj_set_flex_flow(btnBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(btnBar, 10, 0);
    lv_obj_clear_flag(btnBar, LV_OBJ_FLAG_SCROLLABLE);

    _btnExport = lv_btn_create(btnBar);
    lv_obj_set_size(_btnExport, 90, 30);
    lv_obj_set_style_bg_color(_btnExport, lv_color_hex(0x00aa66), 0);
    lv_obj_add_event_cb(_btnExport, onExportClick, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lblExp = lv_label_create(_btnExport);
    lv_label_set_text(lblExp, "💾 Export");
    lv_obj_center(lblExp);

    _btnClear = lv_btn_create(btnBar);
    lv_obj_set_size(_btnClear, 80, 30);
    lv_obj_set_style_bg_color(_btnClear, lv_color_hex(0x884444), 0);
    lv_obj_add_event_cb(_btnClear, onClearClick, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lblClr = lv_label_create(_btnClear);
    lv_label_set_text(lblClr, "🗑️ Limpar");
    lv_obj_center(lblClr);

    // Lista de handshakes
    _list = lv_list_create(_screen);
    lv_obj_set_size(_list, lv_pct(100), lv_pct(58));
    lv_obj_align(_list, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(_list, lv_color_hex(0x0f0f23), 0);
    lv_obj_set_style_border_width(_list, 0, 0);
    lv_obj_set_style_radius(_list, 8, 0);

    // Botão voltar
    _btnBack = lv_btn_create(_screen);
    lv_obj_set_size(_btnBack, 80, 30);
    lv_obj_align(_btnBack, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(_btnBack, lv_color_hex(0x333355), 0);
    lv_obj_add_event_cb(_btnBack, onBackClick, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *lblBack = lv_label_create(_btnBack);
    lv_label_set_text(lblBack, "← Voltar");
    lv_obj_center(lblBack);
}

void HandshakesScreen::show() {
    if (_screen) {
        lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
        update();
    }
}

void HandshakesScreen::hide() {
    if (_screen) lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void HandshakesScreen::update() {
    char buf[16];
    snprintf(buf, sizeof(buf), "(%d)", _count);
    lv_label_set_text(_lblCount, buf);
}

void HandshakesScreen::addHandshake(const HandshakeData &hs) {
    if (_count < 100) {
        memcpy(&_handshakes[_count], &hs, sizeof(HandshakeData));
        _count++;
        populateList();
        update();
    }
}

void HandshakesScreen::clearAll() {
    _count = 0;
    lv_obj_clean(_list);
    update();
}

void HandshakesScreen::populateList() {
    if (!_list) return;

    lv_obj_clean(_list);

    for (int i = 0; i < _count; i++) {
        char buf[64];
        char macStr[18];
        snprintf(
            macStr,
            sizeof(macStr),
            "%02X:%02X:%02X:%02X:%02X:%02X",
            _handshakes[i].bssid[0],
            _handshakes[i].bssid[1],
            _handshakes[i].bssid[2],
            _handshakes[i].bssid[3],
            _handshakes[i].bssid[4],
            _handshakes[i].bssid[5]
        );

        snprintf(
            buf,
            sizeof(buf),
            "%s %s %s",
            _handshakes[i].pmkid ? "🔑" : "🤝",
            _handshakes[i].ssid,
            _handshakes[i].pmkid ? "[PMKID]" : ""
        );

        lv_obj_t *btn = lv_list_add_btn(_list, nullptr, buf);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x1a1a3e), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0x00ff88), 0);
    }
}

void HandshakesScreen::onExportClick(lv_event_t *e) {
    LOG_UI("Exportando para SD...");

    // Tenta salvar no SD
    if (!SD_MMC.begin("/sdcard", true)) {
        LOG_ERROR("SD não disponível");
        return;
    }

    // Cria pasta se não existe
    SD_MMC.mkdir("/wavepwn");
    SD_MMC.mkdir("/wavepwn/handshakes");

    // Escreve lista em arquivo texto
    File f = SD_MMC.open("/wavepwn/handshakes/list.txt", FILE_WRITE);
    if (f) {
        for (int i = 0; i < handshakesScreen._count; i++) {
            f.printf(
                "%s|%02X:%02X:%02X:%02X:%02X:%02X|%s\n",
                handshakesScreen._handshakes[i].ssid,
                handshakesScreen._handshakes[i].bssid[0],
                handshakesScreen._handshakes[i].bssid[1],
                handshakesScreen._handshakes[i].bssid[2],
                handshakesScreen._handshakes[i].bssid[3],
                handshakesScreen._handshakes[i].bssid[4],
                handshakesScreen._handshakes[i].bssid[5],
                handshakesScreen._handshakes[i].pmkid ? "PMKID" : "4WAY"
            );
        }
        f.close();
        LOG_UI("Lista exportada!");
    }
}

void HandshakesScreen::onClearClick(lv_event_t *e) { handshakesScreen.clearAll(); }

void HandshakesScreen::onBackClick(lv_event_t *e) {
    handshakesScreen.hide();
    ui_main_show();
}
