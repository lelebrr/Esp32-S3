/**
 * @file ui_menu_ai.cpp
 * @brief Implementação do submenu de IA
 */

#include "ui_menu_ai.h"
// #include "../ai/neura9_inference.h" // File not found - disabled
#include "globals.h"
#include "ui_home.h"
#include <debug_log.h>

static lv_obj_t *_screen = nullptr;
static lv_obj_t *_statusLabel = nullptr;
static lv_obj_t *_modeLabel = nullptr;
static bool _initialized = false;

#define AI_COLOR_BG lv_color_hex(0x0a0a1a)
#define AI_COLOR_PANEL lv_color_hex(0x12122a)
#define AI_COLOR_ACCENT lv_color_hex(0xff00ff)
#define AI_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
    ui_menu_ai_hide();
    ui_home_show();
}

static void btn_mode_toggle_cb(lv_event_t *e) {
    static bool autoMode = true;
    autoMode = !autoMode;
    if (_modeLabel) { lv_label_set_text(_modeLabel, autoMode ? "Modo: AUTO" : "Modo: MANUAL"); }
}

static void btn_history_cb(lv_event_t *e) { LOG_UI("Show detection history"); }

static void btn_info_cb(lv_event_t *e) { LOG_UI("Show model info"); }

static void create_menu_item(lv_obj_t *parent, const char *icon, const char *text, lv_event_cb_t cb, int y) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 50);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(btn, AI_COLOR_PANEL, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, AI_COLOR_ACCENT, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *iconLbl = lv_label_create(btn);
    lv_label_set_text(iconLbl, icon);
    lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_18, 0);
    lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textLbl = lv_label_create(btn);
    lv_label_set_text(textLbl, text);
    lv_obj_set_style_text_color(textLbl, AI_COLOR_TEXT, 0);
    lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_ai_init() {
    if (_initialized) return;

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, AI_COLOR_BG, 0);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, AI_COLOR_PANEL, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btnBack = lv_btn_create(header);
    lv_obj_set_size(btnBack, 40, 35);
    lv_obj_align(btnBack, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_opa(btnBack, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btnBack, btn_back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *backArrow = lv_label_create(btnBack);
    lv_label_set_text(backArrow, "<-");
    lv_obj_set_style_text_font(backArrow, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(backArrow, AI_COLOR_ACCENT, 0);
    lv_obj_center(backArrow);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "NEURA9 AI");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, AI_COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Status box
    lv_obj_t *statusBox = lv_obj_create(_screen);
    lv_obj_set_size(statusBox, lv_pct(90), 80);
    lv_obj_align(statusBox, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_bg_color(statusBox, AI_COLOR_PANEL, 0);
    lv_obj_set_style_radius(statusBox, 12, 0);
    lv_obj_set_style_border_width(statusBox, 2, 0);
    lv_obj_set_style_border_color(statusBox, AI_COLOR_ACCENT, 0);
    lv_obj_clear_flag(statusBox, LV_OBJ_FLAG_SCROLLABLE);

    _statusLabel = lv_label_create(statusBox);
    lv_label_set_text(_statusLabel, "IA Carregada");
    lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0x00ff88), 0);
    lv_obj_align(_statusLabel, LV_ALIGN_TOP_MID, 0, 10);

    _modeLabel = lv_label_create(statusBox);
    lv_label_set_text(_modeLabel, "Modo: AUTO");
    lv_obj_set_style_text_color(_modeLabel, AI_COLOR_TEXT, 0);
    lv_obj_align(_modeLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

    // Menu items
    create_menu_item(_screen, "M", "Alternar AUTO/MANUAL", btn_mode_toggle_cb, 155);
    create_menu_item(_screen, "H", "Historico de Deteccoes", btn_history_cb, 215);
    create_menu_item(_screen, "I", "Info do Modelo", btn_info_cb, 275);

    _initialized = true;
}

void ui_menu_ai_show() {
    if (!_initialized) ui_menu_ai_init();
    if (_screen) lv_scr_load(_screen);
}

void ui_menu_ai_hide() {}
