/**
 * @file ui_menu_wifi.cpp
 * @brief Implementação do submenu WiFi
 */

#include "ui_menu_wifi.h"
#include "globals.h"
#include "../hardware/wifi_driver.h"
#include "ui_attacks.h"
#include "ui_home.h"
#include <debug_log.h>

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define WIFI_COLOR_BG lv_color_hex(0x0a0a1a)
#define WIFI_COLOR_PANEL lv_color_hex(0x12122a)
#define WIFI_COLOR_ACCENT lv_color_hex(0x00ff88)
#define WIFI_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
    ui_menu_wifi_hide();
    ui_home_show();
}

static void btn_scan_cb(lv_event_t *e) {
    LOG_WIFI("Iniciando scan...");
    // wifi_driver.scanNetworks(true); // Method not defined - disabled
}

static void btn_attacks_cb(lv_event_t *e) { ui_attacks_show(); }

static void btn_deauth_cb(lv_event_t *e) { LOG_WIFI("Deauth clicked"); }

static void btn_beacon_cb(lv_event_t *e) { LOG_WIFI("Beacon Flood clicked"); }

static void btn_handshake_cb(lv_event_t *e) { LOG_WIFI("Handshake capture clicked"); }

static void btn_eviltwin_cb(lv_event_t *e) { LOG_WIFI("Evil Twin clicked"); }

static void create_menu_item(lv_obj_t *parent, const char *icon, const char *text, lv_event_cb_t cb, int y) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 50);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(btn, WIFI_COLOR_PANEL, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, WIFI_COLOR_ACCENT, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *iconLbl = lv_label_create(btn);
    lv_label_set_text(iconLbl, icon);
    lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_18, 0);
    lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textLbl = lv_label_create(btn);
    lv_label_set_text(textLbl, text);
    lv_obj_set_style_text_color(textLbl, WIFI_COLOR_TEXT, 0);
    lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);

    lv_obj_t *arrow = lv_label_create(btn);
    lv_label_set_text(arrow, ">");
    lv_obj_set_style_text_color(arrow, WIFI_COLOR_ACCENT, 0);
    lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -10, 0);
}

void ui_menu_wifi_init() {
    if (_initialized) return;

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, WIFI_COLOR_BG, 0);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, WIFI_COLOR_PANEL, 0);
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
    lv_obj_set_style_text_color(backArrow, WIFI_COLOR_ACCENT, 0);
    lv_obj_center(backArrow);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "WiFi");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, WIFI_COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Menu items
    create_menu_item(_screen, "S", "Scan Redes", btn_scan_cb, 60);
    create_menu_item(_screen, "N", "Redes Encontradas", btn_attacks_cb, 120);
    create_menu_item(_screen, "D", "Deauth Attack", btn_deauth_cb, 180);
    create_menu_item(_screen, "B", "Beacon Flood", btn_beacon_cb, 240);
    create_menu_item(_screen, "H", "Handshake Capture", btn_handshake_cb, 300);
    create_menu_item(_screen, "E", "Evil Twin", btn_eviltwin_cb, 360);

    _initialized = true;
}

void ui_menu_wifi_show() {
    if (!_initialized) ui_menu_wifi_init();
    if (_screen) lv_scr_load(_screen);
}

void ui_menu_wifi_hide() {}
