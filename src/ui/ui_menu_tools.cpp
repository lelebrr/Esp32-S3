/**
 * @file ui_menu_tools.cpp
 * @brief Implementação do submenu de ferramentas
 */

#include "ui_menu_tools.h"
// #include "../plugins/age_tracker.h" // File not found - disabled
// #include "../plugins/exp_system.h" // File not found - disabled
// #include "../plugins/memtemp.h" // File not found - disabled
#include "screens/ui_handshakes_screen.h"
#include "screens/ui_stats_screen.h"
#include "ui_home.h"
#include <debug_log.h>

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define TOOLS_COLOR_BG lv_color_hex(0x0a0a1a)
#define TOOLS_COLOR_PANEL lv_color_hex(0x12122a)
#define TOOLS_COLOR_ACCENT lv_color_hex(0xff8800)
#define TOOLS_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
    ui_menu_tools_hide();
    ui_home_show();
}

static void btn_handshakes_cb(lv_event_t *e) { handshakesScreen.show(); }

static void btn_stats_cb(lv_event_t *e) { statsScreen.show(); }

static void btn_sdcard_cb(lv_event_t *e) { LOG_UI("SD Card manager"); }

static void btn_sysinfo_cb(lv_event_t *e) {
    LOG_UI("System info");
    // TODO: Implementar exibição de informações do sistema
}

static void create_menu_item(lv_obj_t *parent, const char *icon, const char *text, lv_event_cb_t cb, int y) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 50);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(btn, TOOLS_COLOR_PANEL, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, TOOLS_COLOR_ACCENT, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *iconLbl = lv_label_create(btn);
    lv_label_set_text(iconLbl, icon);
    lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_18, 0);
    lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textLbl = lv_label_create(btn);
    lv_label_set_text(textLbl, text);
    lv_obj_set_style_text_color(textLbl, TOOLS_COLOR_TEXT, 0);
    lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_tools_init() {
    if (_initialized) return;

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, TOOLS_COLOR_BG, 0);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, TOOLS_COLOR_PANEL, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btnBack = lv_btn_create(header);
    lv_obj_set_size(btnBack, 40, 35);
    lv_obj_align(btnBack, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_opa(btnBack, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btnBack, btn_back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *backArrow = lv_label_create(btnBack);
    lv_label_set_text(backArrow, "←");
    lv_obj_set_style_text_font(backArrow, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(backArrow, TOOLS_COLOR_ACCENT, 0);
    lv_obj_center(backArrow);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "🛠 Ferramentas");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, TOOLS_COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Menu items
    create_menu_item(_screen, "🔐", "Handshakes Capturados", btn_handshakes_cb, 60);
    create_menu_item(_screen, "📊", "Estatísticas", btn_stats_cb, 120);
    create_menu_item(_screen, "💾", "SD Card", btn_sdcard_cb, 180);
    create_menu_item(_screen, "ℹ", "Info do Sistema", btn_sysinfo_cb, 240);

    _initialized = true;
}

void ui_menu_tools_show() {
    if (!_initialized) ui_menu_tools_init();
    if (_screen) lv_scr_load(_screen);
}

void ui_menu_tools_hide() {}
