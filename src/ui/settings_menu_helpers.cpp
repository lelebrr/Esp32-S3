#include "globals.h"
#include "settings_menu_v2.h"

// Helper: Create slider row
lv_obj_t *
SettingsMenuV2::createSlider(lv_obj_t *p, const char *lbl, int min, int max, int val, lv_event_cb_t cb) {
    lv_obj_t *row = lv_obj_create(p);
    lv_obj_set_size(row, LV_PCT(100), 50);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 5, 0);

    lv_obj_t *label = lv_label_create(row);
    lv_label_set_text(label, lbl);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xA0A0B0), 0);

    lv_obj_t *slider = lv_slider_create(row);
    lv_obj_set_width(slider, LV_PCT(70));
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x1A1A24), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x00FFF5), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x00FFF5), LV_PART_KNOB);

    lv_obj_t *valLbl = lv_label_create(row);
    lv_label_set_text_fmt(valLbl, "%d", val);
    lv_obj_align(valLbl, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_text_color(valLbl, lv_color_hex(0x00FFF5), 0);

    if (cb) lv_obj_add_event_cb(slider, cb, LV_EVENT_VALUE_CHANGED, valLbl);
    return row;
}

// Helper: Create switch row
lv_obj_t *SettingsMenuV2::createSwitch(lv_obj_t *p, const char *lbl, bool val, lv_event_cb_t cb) {
    lv_obj_t *row = lv_obj_create(p);
    lv_obj_set_size(row, LV_PCT(100), 40);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(row);
    lv_label_set_text(label, lbl);
    lv_obj_set_style_text_color(label, lv_color_hex(0xA0A0B0), 0);

    lv_obj_t *sw = lv_switch_create(row);
    if (val) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x1A1A24), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x00FFF5), LV_PART_INDICATOR | LV_STATE_CHECKED);

    if (cb) lv_obj_add_event_cb(sw, cb, LV_EVENT_VALUE_CHANGED, nullptr);
    return row;
}

// Helper: Create dropdown
lv_obj_t *SettingsMenuV2::createDropdown(
    lv_obj_t *p, const char *lbl, const char *opts, uint8_t sel, lv_event_cb_t cb
) {
    lv_obj_t *row = lv_obj_create(p);
    lv_obj_set_size(row, LV_PCT(100), 60);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    lv_obj_t *label = lv_label_create(row);
    lv_label_set_text(label, lbl);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xA0A0B0), 0);

    lv_obj_t *dd = lv_dropdown_create(row);
    lv_dropdown_set_options(dd, opts);
    lv_dropdown_set_selected(dd, sel);
    lv_obj_set_width(dd, LV_PCT(100));
    lv_obj_align(dd, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(dd, lv_color_hex(0x1A1A24), 0);
    lv_obj_set_style_text_color(dd, lv_color_hex(0x00FFF5), 0);

    if (cb) lv_obj_add_event_cb(dd, cb, LV_EVENT_VALUE_CHANGED, nullptr);
    return row;
}

// Helper: Create button
lv_obj_t *SettingsMenuV2::createButton(lv_obj_t *p, const char *lbl, const char *btn, lv_event_cb_t cb) {
    lv_obj_t *row = lv_obj_create(p);
    lv_obj_set_size(row, LV_PCT(100), 45);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(row);
    lv_label_set_text(label, lbl);
    lv_obj_set_style_text_color(label, lv_color_hex(0xA0A0B0), 0);

    lv_obj_t *button = lv_btn_create(row);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x00FFF5), 0);
    lv_obj_t *btnLbl = lv_label_create(button);
    lv_label_set_text(btnLbl, btn);
    lv_obj_set_style_text_color(btnLbl, lv_color_hex(0x000000), 0);

    if (cb) lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, nullptr);
    return row;
}
