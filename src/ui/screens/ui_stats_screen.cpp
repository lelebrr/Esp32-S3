/**
 * @file ui_stats_screen.cpp
 * @brief Implementação da tela de estatísticas
 */

#include "ui_stats_screen.h"
#include "../ui_main.h"

StatsScreen statsScreen;

StatsScreen::StatsScreen() : _screen(nullptr) {}

void StatsScreen::create(lv_obj_t *parent) {
    _screen = lv_obj_create(parent);
    lv_obj_set_size(_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_border_width(_screen, 0, 0);
    lv_obj_set_style_pad_all(_screen, 10, 0);
    lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);

    // Título
    lv_obj_t *title = lv_label_create(_screen);
    lv_label_set_text(title, "📊 Estatísticas");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff88), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Container para stats
    lv_obj_t *cont = lv_obj_create(_screen);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(70));
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 8, 0);

    // Stats de sessão
    _lblSession = lv_label_create(cont);
    lv_label_set_text(_lblSession, "Sessão: 00:00:00");
    lv_obj_set_style_text_color(_lblSession, lv_color_hex(0xcccccc), 0);

    _lblNetworks = lv_label_create(cont);
    lv_label_set_text(_lblNetworks, "Redes: 0");

    _lblHandshakes = lv_label_create(cont);
    lv_label_set_text(_lblHandshakes, "Handshakes: 0");
    lv_obj_set_style_text_color(_lblHandshakes, lv_color_hex(0x00ff88), 0);

    _lblDeauths = lv_label_create(cont);
    lv_label_set_text(_lblDeauths, "Deauths: 0");

    // Sistema
    _lblMemory = lv_label_create(cont);
    lv_label_set_text(_lblMemory, "RAM: 0KB | Temp: 0C");
    lv_obj_set_style_text_color(_lblMemory, lv_color_hex(0x888888), 0);

    _lblBattery = lv_label_create(cont);
    lv_label_set_text(_lblBattery, "Bateria: 100%");

    // Botão voltar
    _btnBack = lv_btn_create(_screen);
    lv_obj_set_size(_btnBack, 80, 35);
    lv_obj_align(_btnBack, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(_btnBack, lv_color_hex(0x333355), 0);
    lv_obj_add_event_cb(_btnBack, onBackClick, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *lblBack = lv_label_create(_btnBack);
    lv_label_set_text(lblBack, "← Voltar");
    lv_obj_center(lblBack);
}

void StatsScreen::show() {
    if (_screen) {
        lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
        update();
    }
}

void StatsScreen::hide() {
    if (_screen) lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void StatsScreen::update() {
    if (!_screen) return;

    lv_label_set_text(_lblSession, "Sessão: 00:00:00");
    lv_label_set_text(_lblNetworks, "Redes: 0");
    lv_label_set_text(_lblHandshakes, "Handshakes: 0");
    lv_label_set_text(_lblDeauths, "Deauths: 0");
    lv_label_set_text(_lblMemory, "RAM: N/A | Temp: N/A");
    lv_label_set_text(_lblBattery, "Bateria: N/A");
}

void StatsScreen::onBackClick(lv_event_t *e) {
    statsScreen.hide();
    ui_main_show();
}
