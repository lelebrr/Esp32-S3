/**
 * @file ui_settings_ai.cpp
 * @brief AI Settings (DISABLED - Missing config_manager)
 */
#include "ui_settings_ai.h"
#include "globals.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include <debug_log.h>

static lv_obj_t *scr = nullptr;
static bool is_active = false;

static void on_back_click(lv_event_t *e) {
    ui_settings_ai_hide();
    ui_settings_show();
}

static void create_screen() {
    scr = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr, THEME_BG, 0);
    
    lv_obj_t *header = lv_obj_create(scr);
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
    lv_label_set_text(title, "AI Settings");
    lv_obj_set_style_text_color(title, THEME_PRIMARY, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t *msg = lv_label_create(scr);
    lv_label_set_text(msg, "AI settings disabled\n(config_manager not defined)");
    lv_obj_set_style_text_color(msg, THEME_TEXT_DIM, 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
}

void ui_settings_ai_show() {
    if (!scr) { create_screen(); }
    is_active = true;
    lv_scr_load(scr);
}

void ui_settings_ai_hide() {
    is_active = false;
    if (scr) { lv_obj_del(scr); scr = nullptr; }
}

bool ui_settings_ai_is_active() { return is_active; }
