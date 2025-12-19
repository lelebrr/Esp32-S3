/**
 * @file ui_skimmer.cpp
 * @brief Skimmer detector UI (DISABLED - Missing skimmer_detector.h)
 */
#include "ui_skimmer.h"
#include "globals.h"
#include "ui_themes.h"
#include <debug_log.h>

static lv_obj_t *scr = nullptr;

void ui_skimmer_show() {
    if (!scr) {
        scr = lv_obj_create(nullptr);
        lv_obj_set_style_bg_color(scr, THEME_BG, 0);
        
        lv_obj_t *msg = lv_label_create(scr);
        lv_label_set_text(msg, "Skimmer detector disabled\n(hardware module not defined)");
        lv_obj_set_style_text_color(msg, THEME_TEXT_DIM, 0);
        lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
    }
    lv_scr_load(scr);
}

void ui_skimmer_hide() {
    if (scr) { lv_obj_del(scr); scr = nullptr; }
}
