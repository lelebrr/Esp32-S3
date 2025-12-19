/**
 * @file gps_menu.cpp
 * GPS Menu UI Implementation
 * Monster S3 Firmware
 * 
 * Updated: 2025-12-19
 */

#include "gps_menu.h"
#include "gps_driver.h"
#include <stdio.h>

static lv_obj_t* label_lat;
static lv_obj_t* label_lng;
static lv_obj_t* label_alt;
static lv_obj_t* label_speed;
static lv_obj_t* label_sats;
static lv_obj_t* label_time;
static lv_obj_t* label_status;

// Helper to create styled labels
static lv_obj_t* create_label(lv_obj_t* parent, const char* text, int x, int y) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    return lbl;
}

static lv_obj_t* create_value_label(lv_obj_t* parent, int x, int y) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, "--");
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x00FF00), 0); // Green
    return lbl;
}

void create_gps_menu(lv_obj_t* parent) {
    // Title
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "GPS - GY-NEO6MV2");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFDD00), 0); // Gold

    // Status
    label_status = lv_label_create(parent);
    lv_label_set_text(label_status, "Status: Searching...");
    lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_text_color(label_status, lv_color_hex(0xFF0000), 0); // Red initially

    // Layout (Labels + Values)
    int left_col = 20;
    int right_col = 140;
    int start_y = 80;
    int row_h = 35;

    // Row 1: Latitude / Longitude
    create_label(parent, "Latitude:", left_col, start_y);
    label_lat = create_value_label(parent, left_col, start_y + 20);

    create_label(parent, "Longitude:", right_col, start_y);
    label_lng = create_value_label(parent, right_col, start_y + 20);

    // Row 2: Altitude / Speed
    create_label(parent, "Altitude:", left_col, start_y + row_h*2);
    label_alt = create_value_label(parent, left_col, start_y + row_h*2 + 20);

    create_label(parent, "Speed:", right_col, start_y + row_h*2);
    label_speed = create_value_label(parent, right_col, start_y + row_h*2 + 20);

    // Row 3: Satellites / Time
    create_label(parent, "Satellites:", left_col, start_y + row_h*4);
    label_sats = create_value_label(parent, left_col, start_y + row_h*4 + 20);

    create_label(parent, "UTC Time:", right_col, start_y + row_h*4);
    label_time = create_value_label(parent, right_col, start_y + row_h*4 + 20);
}

void update_gps_menu() {
    if (!label_lat) return; // Menu not created

    // GPSData data = GPSDriver::getData(); // REMOVED - Deprecated
    bool isValid = GPSDriver::isValid(); // Check validity directly
    char buf[32];

    // Status Update
    if (isValid) {
        lv_label_set_text(label_status, "Status: 3D FIX LOCKED");
        lv_obj_set_style_text_color(label_status, lv_color_hex(0x00FF00), 0); // Green
    } else {
        lv_label_set_text(label_status, "Status: Acquiring Satellites...");
        lv_obj_set_style_text_color(label_status, lv_color_hex(0xFF0000), 0); // Red
    }

    // Values Update
    if (isValid) {
        snprintf(buf, sizeof(buf), "%.6f", GPSDriver::getLatitude());
        lv_label_set_text(label_lat, buf);

        snprintf(buf, sizeof(buf), "%.6f", GPSDriver::getLongitude());
        lv_label_set_text(label_lng, buf);

        snprintf(buf, sizeof(buf), "%.1f m", GPSDriver::getAltitude());
        lv_label_set_text(label_alt, buf);

        snprintf(buf, sizeof(buf), "%.1f km/h", GPSDriver::getSpeedKmh());
        lv_label_set_text(label_speed, buf);

        snprintf(buf, sizeof(buf), "%d sats (%.1f)", GPSDriver::getSatellites(), GPSDriver::getHDOP());
        lv_label_set_text(label_sats, buf);

        lv_label_set_text(label_time, GPSDriver::getTimeString().c_str());
    } else {
        // Show partial data if available (e.g. satellites without fix)
        snprintf(buf, sizeof(buf), "%d visible", GPSDriver::getSatellites());
        lv_label_set_text(label_sats, buf);
        
        // Keep "..." for others if no valid data
        if (GPSDriver::getSatellites() == 0) {
           // reset calls if needed
        }
    }
}
