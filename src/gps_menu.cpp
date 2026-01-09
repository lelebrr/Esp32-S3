#include "gps_menu.h"
#include "gps_driver.h"
#include "lvgl_menu.h" // For Colors and Helpers
#include <stdio.h>

static lv_obj_t *label_lat;
static lv_obj_t *label_lng;
static lv_obj_t *label_alt;
static lv_obj_t *label_speed;
static lv_obj_t *label_sats;
static lv_obj_t *label_time;
static lv_obj_t *label_status;
static lv_obj_t *status_led; // Visual indicator

#ifndef SCR_W
#define SCR_W 320
#define SCR_H 240
#endif

// Helper for Glass Label items
static void create_glass_item(lv_obj_t *parent, const char *title, lv_obj_t **value_label) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 140, 60);
    lv_obj_set_style_bg_color(cont, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_60, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(COL_ACCENT_CYAN), 0);
    lv_obj_set_style_border_width(cont, 1, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_title = lv_label_create(cont);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0); // Use safe font

    *value_label = lv_label_create(cont);
    lv_label_set_text(*value_label, "--");
    lv_obj_set_style_text_color(*value_label, lv_color_hex(COL_TEXT), 0);
    lv_obj_set_style_text_font(*value_label, &lv_font_montserrat_16, 0);
}

void create_gps_menu(lv_obj_t *parent) {
    lv_obj_clean(parent);
    lv_obj_set_style_bg_color(parent, lv_color_hex(COL_BG), 0);
    create_header(parent, "GPS Module");

    // Main Grid
    lv_obj_t *grid = lv_obj_create(parent);
    lv_obj_set_size(grid, SCR_W - 10, SCR_H - 90);
    lv_obj_set_pos(grid, 5, 85);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(grid, 10, 10);

    // Status Bar Panel
    lv_obj_t *status_panel = lv_obj_create(parent);
    lv_obj_set_size(status_panel, SCR_W - 20, 30);
    lv_obj_set_pos(status_panel, 10, 50);
    lv_obj_set_style_bg_color(status_panel, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_border_width(status_panel, 0, 0);
    lv_obj_set_flex_flow(status_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(status_panel, 2, 0);
    lv_obj_set_style_pad_gap(status_panel, 10, 0);

    status_led = lv_obj_create(status_panel);
    lv_obj_set_size(status_led, 8, 8);
    lv_obj_set_style_radius(status_led, 4, 0);
    lv_obj_set_style_bg_color(status_led, lv_color_hex(COL_RED), 0);

    label_status = lv_label_create(status_panel);
    lv_label_set_text(label_status, "SEARCHING...");
    lv_obj_set_style_text_color(label_status, lv_color_hex(COL_RED), 0);

    // Data Items
    create_glass_item(grid, "LATITUDE", &label_lat);
    create_glass_item(grid, "LONGITUDE", &label_lng);
    create_glass_item(grid, "ALTITUDE", &label_alt);
    create_glass_item(grid, "SPEED", &label_speed);
    create_glass_item(grid, "SATELLITES", &label_sats);
    create_glass_item(grid, "UTC TIME", &label_time);
}

void update_gps_menu() {
    if (!label_lat) return;

    bool isValid = GPSDriver::isValid();
    char buf[32];

    // Status Update
    if (isValid) {
        lv_label_set_text(label_status, "3D FIX LOCKED");
        lv_obj_set_style_text_color(label_status, lv_color_hex(COL_GREEN), 0);
        lv_obj_set_style_bg_color(status_led, lv_color_hex(COL_GREEN), 0);
    } else {
        lv_label_set_text(label_status, "SEARCHING...");
        lv_obj_set_style_text_color(label_status, lv_color_hex(COL_RED), 0);
        lv_obj_set_style_bg_color(status_led, lv_color_hex(COL_RED), 0);
    }

    // Values Update
    if (isValid) {
        snprintf(buf, sizeof(buf), "%.5f", GPSDriver::getLatitude());
        lv_label_set_text(label_lat, buf);

        snprintf(buf, sizeof(buf), "%.5f", GPSDriver::getLongitude());
        lv_label_set_text(label_lng, buf);

        snprintf(buf, sizeof(buf), "%.0fm", GPSDriver::getAltitude());
        lv_label_set_text(label_alt, buf);

        snprintf(buf, sizeof(buf), "%.0f km/h", GPSDriver::getSpeedKmh());
        lv_label_set_text(label_speed, buf);

        snprintf(buf, sizeof(buf), "%d (%.1f)", GPSDriver::getSatellites(), GPSDriver::getHDOP());
        lv_label_set_text(label_sats, buf);

        lv_label_set_text(label_time, GPSDriver::getTimeString().c_str());
    } else {
        snprintf(buf, sizeof(buf), "%d visible", GPSDriver::getSatellites());
        lv_label_set_text(label_sats, buf);
    }
}
