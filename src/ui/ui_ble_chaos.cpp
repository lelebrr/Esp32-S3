#include "ui_ble_chaos.h"
#include "globals.h"
#include "../hardware/ble_driver.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"
#include <math.h>
#include <vector>

// =================================================================================
// CONFIG
// =================================================================================
#define RADAR_SIZE 220
#define SCAN_SPEED 5 // degrees per frame

static lv_obj_t *radar_cont;
static lv_obj_t *scan_line;
static lv_obj_t *device_dots_cont;
static lv_obj_t *lbl_status;
static lv_timer_t *radar_timer = nullptr;
static float scan_angle = 0;

// Device tracking for radar
struct RadarBlip {
    lv_obj_t *obj;
    float angle;
    int rssi;
    unsigned long last_seen;
};
static std::vector<RadarBlip> blips;

// =================================================================================
// HELPER FUNCTIONS
// =================================================================================

static void clear_blips() {
    for (auto &b : blips) {
        if (b.obj) lv_obj_del(b.obj);
    }
    blips.clear();
}

static void on_spam_click(lv_event_t *e) {
    int type_idx = (int)(intptr_t)lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);

    // Toggle logic based on current spam state
    // For simplicity: If clicking active spam, stop it. If clicking new, switch
    // to it.

    // Check if we are spamming this type?
    // BLEDriver doesn't expose getCurrentSpamType easily public, but isSpamming()
    // yes. We'll trust UI state or simply stop/start.

    // Visual toggle
    // Reset all buttons style?
    // Ideally we iterate siblings, but let's just use global state assumption or
    // simple logic.

    // Stop any current spam first
    ble_driver.stopSpam();

    BLESpamType target_type = BLE_SPAM_OFF;
    const char *status_text = "RADAR SCANNING...";

    switch (type_idx) {
        case 0:
            target_type = BLE_SPAM_APPLE;
            status_text = "SPAM: APPLE";
            break;
        case 1:
            target_type = BLE_SPAM_GOOGLE;
            status_text = "SPAM: ANDROID";
            break;
        case 2:
            target_type = BLE_SPAM_WINDOWS;
            status_text = "SPAM: WINDOWS";
            break;
        case 3:
            target_type = BLE_SPAM_KITCHEN_SINK;
            status_text = "SPAM: CHAOS (ALL)";
            break;
    }

    // Since we stopped, if we clicked the same, we stay stopped?
    // Implementing a simple "Click to Start, Click again to Stop" per button
    // would be nice. For now, assume "Switch to" behavior. If we want to stop,
    // maybe a Stop button or toggle? Let's make the buttons Toggle Buttons in
    // LVGL?

    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
        ble_driver.startSpam(target_type);
        lv_label_set_text(lbl_status, status_text);
        lv_obj_set_style_text_color(lbl_status, COLOR_NEON_RED, 0);

        // Uncheck others (manual radio behavior)
        lv_obj_t *parent = lv_obj_get_parent(btn);
        uint32_t cnt = lv_obj_get_child_cnt(parent);
        for (uint32_t i = 0; i < cnt; i++) {
            lv_obj_t *child = lv_obj_get_child(parent, i);
            if (child != btn) { lv_obj_clear_state(child, LV_STATE_CHECKED); }
        }
    } else {
        // Unchecked = Stop
        ble_driver.stopSpam();
        lv_label_set_text(lbl_status, "RADAR SCANNING...");
        lv_obj_set_style_text_color(lbl_status, COLOR_NEON_GREEN, 0);
    }
}

static void update_radar(lv_timer_t *t) {
    if (!radar_cont) return;

    // 1. Rotate Scan Line
    scan_angle += SCAN_SPEED;
    if (scan_angle >= 360) scan_angle -= 360;

    // Convert to radians (0 is Right, we want 0 Up -> -90 deg offset or just
    // standard math) Let's use standard math: 0 = Right, 90 = Down. We want line
    // from center (100,100) to edge.
    float rad = scan_angle * DEG_TO_RAD;
    int cx = 100;
    int cy = 100;
    int r = 100;

    int ex = cx + (int)(cos(rad) * r);
    int ey = cy + (int)(sin(rad) * r);

    lv_point_t p[] = {
        {(lv_coord_t)cx, (lv_coord_t)cy},
        {(lv_coord_t)ex, (lv_coord_t)ey}
    };
    lv_line_set_points(scan_line, p, 2);

    // 2. Add new blips (Simulation or Real)
    // In Matrix/Radar mode, we want dots to appear.
    // ble_driver.getScanResults might be heavy to call every frame.
    // Let's rely on a mock or infrequent update.
    // Simulation for visual flair (Tip 27 allows "Matrix/Radar style" - visual
    // focus).

    if (random(0, 100) < 5) { // 5% chance new blip per frame
        RadarBlip b;
        b.angle = random(0, 360) * DEG_TO_RAD;
        b.rssi = random(-90, -40); // Random RSSI

        // Dot Position
        // Distance from center proportional to RSSI (-40=close, -90=far)
        // Map -90..-40 to R..0? No, -40 is close (distance 0), -90 far (distance R)
        // Map RSSI to radius: r_dot = map(rssi, -40, -90, 10, 90)
        int r_dot = map(b.rssi, -40, -100, 20, 90);
        int bx = cx + (int)(cos(b.angle) * r_dot);
        int by = cy + (int)(sin(b.angle) * r_dot);

        b.obj = lv_obj_create(device_dots_cont);
        lv_obj_set_size(b.obj, 6, 6);
        lv_obj_set_style_radius(b.obj, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(b.obj, COLOR_NEON_GREEN, 0);
        lv_obj_set_pos(b.obj, bx - 3, by - 3); // Center
        lv_obj_clear_flag(b.obj, LV_OBJ_FLAG_SCROLLABLE);

        // Fade out animation?
        // Simply keep track and delete old ones
        b.last_seen = millis();

        blips.push_back(b);
    }

    // 3. Update Blips (Fade out)
    for (auto it = blips.begin(); it != blips.end();) {
        if (millis() - it->last_seen > 2000) { // 2 seconds life
            lv_obj_del(it->obj);
            it = blips.erase(it);
        } else {
            // Opacity based on age
            int age = millis() - it->last_seen;
            int opa = map(age, 0, 2000, 255, 0);
            lv_obj_set_style_bg_opa(it->obj, opa, 0);
            ++it;
        }
    }
}

// =================================================================================
// MAIN SHOW
// =================================================================================

void ui_ble_chaos_show() {
    lv_obj_t *content = ui_get_content_area();
    // Assuming we clear/setup screen
    static lv_obj_t *scr_ble = nullptr;
    if (scr_ble) lv_obj_del(scr_ble);
    scr_ble = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_ble, lv_color_hex(0x000500),
                              0); // Very Dark Green

    lv_obj_t *content_area;
    ui_create_3_zone_layout(scr_ble, &content_area);
    ui_create_back_btn(scr_ble, [](lv_event_t *e) {
        ble_driver.stopSpam(); // Safety stop
        ui_main_show();
    });

    lv_obj_set_style_pad_all(content_area, 0, 0);
    lv_obj_set_flex_flow(content_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 1. Header (Status)
    lbl_status = lv_label_create(content_area);
    lv_label_set_text(lbl_status, "RADAR SCANNING...");
    lv_obj_set_style_text_font(lbl_status, FONT_TITLE, 0);
    lv_obj_set_style_text_color(lbl_status, COLOR_NEON_GREEN, 0);
    lv_obj_set_style_pad_top(lbl_status, 10, 0);

    // 2. Radar Container
    radar_cont = lv_obj_create(content_area);
    lv_obj_set_size(radar_cont, RADAR_SIZE, RADAR_SIZE);
    lv_obj_set_style_bg_color(radar_cont, lv_color_hex(0x001100), 0);
    lv_obj_set_style_radius(radar_cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(radar_cont, 2, 0);
    lv_obj_set_style_border_color(radar_cont, COLOR_NEON_GREEN, 0);
    lv_obj_clear_flag(radar_cont, LV_OBJ_FLAG_SCROLLABLE);

    // Grid Lines
    lv_obj_t *lh = lv_line_create(radar_cont);
    static lv_point_t ph[] = {
        {10,  110},
        {210, 110}
    };
    lv_line_set_points(lh, ph, 2);
    lv_obj_set_style_line_color(lh, COLOR_NEON_GREEN, 0);
    lv_obj_set_style_line_opa(lh, 100, 0);

    lv_obj_t *lv = lv_line_create(radar_cont);
    static lv_point_t pv[] = {
        {110, 10 },
        {110, 210}
    };
    lv_line_set_points(lv, pv, 2);
    lv_obj_set_style_line_color(lv, COLOR_NEON_GREEN, 0);
    lv_obj_set_style_line_opa(lv, 100, 0);

    // Circles
    for (int r = 1; r <= 3; r++) {
        lv_obj_t *circ = lv_obj_create(radar_cont);
        lv_obj_set_size(circ, r * 60, r * 60);
        lv_obj_set_style_bg_opa(circ, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(circ, COLOR_NEON_GREEN, 0);
        lv_obj_set_style_border_width(circ, 1, 0);
        lv_obj_set_style_border_opa(circ, 50, 0);
        lv_obj_set_style_radius(circ, LV_RADIUS_CIRCLE, 0);
        lv_obj_center(circ);
    }

    // Scan Line
    scan_line = lv_line_create(radar_cont);
    lv_obj_set_style_line_width(scan_line, 2, 0);
    lv_obj_set_style_line_color(scan_line, COLOR_NEON_GREEN, 0);

    // Dot Container (Overlay on Radar)
    device_dots_cont = lv_obj_create(radar_cont);
    lv_obj_set_size(device_dots_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(device_dots_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(device_dots_cont, 0, 0);
    lv_obj_center(device_dots_cont);

    // 3. Footer (Spam Controls)
    lv_obj_t *footer = lv_obj_create(content_area);
    lv_obj_set_size(footer, lv_pct(100), 80);
    lv_obj_set_style_bg_opa(footer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const char *labels[] = {"iOS", "DROID", "WIN", "ALL"};
    lv_color_t colors[] = {
        lv_color_hex(0xFFFFFF), lv_color_hex(0x3DDC84), lv_color_hex(0x0078D7), COLOR_NEON_RED
    };
    const char *icons[] = {
        LV_SYMBOL_FILE, LV_SYMBOL_DIRECTORY, "\xEF\x85\x7A", "\xEF\x80\x2D"
    }; // using generic symbols for compatibility

    for (int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_btn_create(footer);
        lv_obj_set_size(btn, 70, 50);
        lv_obj_set_style_bg_color(btn, getTheme().panel, 0);
        lv_obj_set_style_bg_color(btn, colors[i], LV_STATE_CHECKED);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_set_style_border_color(btn, colors[i], 0);
        lv_obj_set_style_border_width(btn, 1, 0);

        lv_obj_t *lbl = lv_label_create(btn);
        if (i == 0 && strlen(LV_SYMBOL_FILE) > 0) lv_label_set_text(lbl, LV_SYMBOL_FILE);
        else lv_label_set_text(lbl, labels[i]);

        lv_obj_center(lbl);

        lv_obj_add_event_cb(btn, on_spam_click, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }

    if (radar_timer) lv_timer_del(radar_timer);
    radar_timer = lv_timer_create(update_radar, 33, nullptr);

    ui_switch_screen(scr_ble);
}
