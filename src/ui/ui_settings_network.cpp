/**
 * @file ui_settings_network.cpp
 * @brief Submenu de configurações de Rede & Wireless (DISABLED - Missing GlobalState members)
 */

#include "ui_settings_network.h"
#include "globals.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include <debug_log.h>

static lv_obj_t *scr_network = nullptr;
static bool is_active = false;

static void on_back_click(lv_event_t *e) {
    ui_settings_network_hide();
    ui_settings_show();
}

// Stub implementation - full screen disabled due to missing GlobalState members
static void create_screen() {
    scr_network = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr_network, THEME_BG, 0);
    
    // Minimal header with back button
    lv_obj_t *header = lv_obj_create(scr_network);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, THEME_PANEL, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t *btn_back = lv_btn_create(header);
    lv_obj_set_size(btn_back, 40, 35);
    lv_obj_align(btn_back, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_opa(btn_back, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btn_back, on_back_click, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *arrow = lv_label_create(btn_back);
    lv_label_set_text(arrow, "<-");
    lv_obj_center(arrow);
    
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Rede & Wireless");
    lv_obj_set_style_text_color(title, THEME_PRIMARY, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);
    
    // Placeholder message
    lv_obj_t *msg = lv_label_create(scr_network);
    lv_label_set_text(msg, "Network settings disabled\n(GlobalState members not defined)");
    lv_obj_set_style_text_color(msg, THEME_TEXT_DIM, 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
}

void ui_settings_network_show() {
    if (!scr_network) { create_screen(); }
    is_active = true;
    lv_scr_load(scr_network);
}

void ui_settings_network_hide() {
    is_active = false;
    if (scr_network) {
        lv_obj_del(scr_network);
        scr_network = nullptr;
    }
}

bool ui_settings_network_is_active() { return is_active; }
