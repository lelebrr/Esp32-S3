#include "ui_wifi_chaos.h"
#include "../hardware/wifi_driver.h"
#include "../wifi/wifi_attacks.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_particles.h" // For effects
#include "ui_themes.h"
#include <debug_log.h>
#include <globals.h>

// =================================================================================
// CONSTANTS & CONFIG
// =================================================================================
#define MAX_BUBBLES 12
#define BUBBLE_BASE_SIZE 50
#define CONTAINER_WIDTH (320)
#define CONTAINER_HEIGHT (240) // Approx

struct Bubble {
    lv_obj_t *obj;
    lv_obj_t *label_ssid;
    float x, y;
    float vx, vy;
    int rssi;
    int index; // Network index
    bool active;
};

static Bubble bubbles[MAX_BUBBLES];
static int active_bubble_count = 0;
static lv_timer_t *physics_timer = nullptr;
static lv_obj_t *scr_chaos = nullptr;
static lv_obj_t *radial_menu = nullptr;
static int selected_bubble_idx = -1;

// Styles
static lv_style_t style_bubble;
static bool styles_inited = false;

// =================================================================================
// HELPER FUNCTIONS
// =================================================================================

static void close_radial_menu() {
    if (radial_menu) {
        lv_obj_del(radial_menu);
        radial_menu = nullptr;
        selected_bubble_idx = -1;
    }
}

static void on_attack_select(lv_event_t *e) {
    int attack_type = (int)(intptr_t)lv_event_get_user_data(e);

    if (selected_bubble_idx >= 0 && selected_bubble_idx < active_bubble_count) {
        WiFiNetwork net = wifi_driver.getNetwork(bubbles[selected_bubble_idx].index);
        LOG_WIFI("Chaos Attack %d on %s", attack_type, net.ssid);

        switch (attack_type) {
            case 0: // Deauth
                wifi_attacks.setTarget(net.bssid, net.ssid, net.channel);
                wifi_attacks.startDeauth(50);
                break;
            case 1: // Capture
                wifi_attacks.setTarget(net.bssid, net.ssid, net.channel);
                wifi_attacks.startHandshakeCapture();
                break;
            case 2: // Clone
                wifi_attacks.startEvilTwin(net.ssid, 0);
                break;
        }
    }
    close_radial_menu();
}

static void create_radial_menu(float x, float y) {
    close_radial_menu();

    if (!scr_chaos) return;

    // Create a transparent overlay for the menu
    radial_menu = lv_obj_create(scr_chaos);
    lv_obj_set_size(radial_menu, 160, 160);

    // Center menu on the bubble position
    int mx = (int)x - 80 + (BUBBLE_BASE_SIZE / 2);
    int my = (int)y - 80 + (BUBBLE_BASE_SIZE / 2);

    // Clamp to screen (approx 320x240)
    if (mx < 0) mx = 0;
    if (mx > 320 - 160) mx = 320 - 160;
    if (my < 0) my = 0;
    if (my > 240 - 160) my = 240 - 160;

    lv_obj_set_pos(radial_menu, mx, my);
    lv_obj_set_style_bg_opa(radial_menu, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(radial_menu, 0, 0);
    lv_obj_clear_flag(radial_menu, LV_OBJ_FLAG_SCROLLABLE);

    // Create 3 buttons in a triangle/circle
    const char *icons[] = {LV_SYMBOL_CLOSE, LV_SYMBOL_SAVE, LV_SYMBOL_COPY}; // Deauth, Capture, Clone
    lv_color_t colors[] = {COLOR_NEON_RED, COLOR_NEON_BLUE, getTheme().warning};

    // Positions relative to 160x160 center (80,80)
    struct Point {
        int x;
        int y;
    };
    Point points[] = {
        {80,  20 },
        {20,  120},
        {140, 120}
    };

    for (int i = 0; i < 3; i++) {
        lv_obj_t *btn = lv_btn_create(radial_menu);
        lv_obj_set_size(btn, 45, 45);
        lv_obj_set_pos(btn, points[i].x - 22, points[i].y - 22);
        lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(btn, colors[i], 0);
        lv_obj_set_style_shadow_width(btn, 10, 0);
        lv_obj_set_style_shadow_color(btn, colors[i], 0);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, icons[i]);
        lv_obj_center(lbl);

        lv_obj_add_event_cb(btn, on_attack_select, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

static void on_bubble_click(lv_event_t *e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);

    if (selected_bubble_idx == idx && radial_menu) {
        close_radial_menu();
        return;
    }
    selected_bubble_idx = idx;
    create_radial_menu(bubbles[idx].x, bubbles[idx].y);
}

static void physics_timer_cb(lv_timer_t *t) {
    if (!scr_chaos) return;

    for (int i = 0; i < active_bubble_count; i++) {
        Bubble *b = &bubbles[i];

        // Update Position
        b->x += b->vx;
        b->y += b->vy;

        // Wall collisions (screen boundaries)
        if (b->x <= 0) {
            b->x = 0;
            b->vx = -b->vx;
        }
        if (b->x >= CONTAINER_WIDTH - BUBBLE_BASE_SIZE) {
            b->x = CONTAINER_WIDTH - BUBBLE_BASE_SIZE;
            b->vx = -b->vx;
        }
        if (b->y <= 0) {
            b->y = 0;
            b->vy = -b->vy;
        }
        if (b->y >= CONTAINER_HEIGHT - BUBBLE_BASE_SIZE) {
            b->y = CONTAINER_HEIGHT - BUBBLE_BASE_SIZE;
            b->vy = -b->vy;
        }

        // Random motion
        if (random(0, 100) < 5) {
            b->vx += ((float)random(-10, 11)) / 10.0f;
            b->vy += ((float)random(-10, 11)) / 10.0f;
        }

        lv_obj_set_pos(b->obj, (lv_coord_t)b->x, (lv_coord_t)b->y);

        // Pulse effect
        if (i == selected_bubble_idx) {
            lv_obj_set_style_shadow_width(b->obj, 20 + (millis() % 500) / 20, 0);
            lv_obj_set_style_border_color(b->obj, COLOR_NEON_GREEN, 0);
            // Freeze logic for menu?
            b->vx *= 0.9;
            b->vy *= 0.9;
        } else {
            lv_obj_set_style_shadow_width(b->obj, 10, 0);
            lv_obj_set_style_border_color(b->obj, COLOR_NEON_BLUE, 0);
        }
    }
}

// =================================================================================
// MAIN SHOW FUNCTION
// =================================================================================

void ui_wifi_chaos_show(void) {
    // Use a dedicated screen object to avoid issues with content area helpers for
    // now as this is a full screen effect.

    if (scr_chaos) {
        lv_obj_del(scr_chaos);
        scr_chaos = nullptr;
    }

    scr_chaos = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_chaos, lv_color_black(), 0);

    // Back Button
    ui_create_back_btn(scr_chaos, [](lv_event_t *e) {
        if (physics_timer) {
            lv_timer_del(physics_timer);
            physics_timer = nullptr;
        }
        ui_main_show();
    });

    // Init Styles
    if (!styles_inited) {
        lv_style_init(&style_bubble);
        lv_style_set_radius(&style_bubble, LV_RADIUS_CIRCLE);
        lv_style_set_bg_opa(&style_bubble, LV_OPA_50);
        lv_style_set_bg_color(&style_bubble, lv_color_hex(0x202020));
        lv_style_set_border_width(&style_bubble, 2);
        lv_style_set_border_color(&style_bubble, COLOR_NEON_BLUE);
        lv_style_set_text_color(&style_bubble, lv_color_white());
        lv_style_set_shadow_width(&style_bubble, 10);
        lv_style_set_shadow_color(&style_bubble, COLOR_NEON_BLUE);
        styles_inited = true;
    }

    // Header Info
    lv_obj_t *header = lv_label_create(scr_chaos);
    lv_label_set_text(header, "CHAOS SCANNER");
    lv_obj_set_style_text_color(header, COLOR_NEON_RED, 0);
    lv_obj_set_style_text_font(header, &lv_font_montserrat_14, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 10);

    // Initial Bubbles
    active_bubble_count = 0;
    int net_count = wifi_driver.getNetworkCount();
    if (net_count > MAX_BUBBLES) net_count = MAX_BUBBLES;

    for (int i = 0; i < net_count; i++) {
        WiFiNetwork net = wifi_driver.getNetwork(i);
        Bubble *b = &bubbles[i];
        b->index = i;
        b->active = true;

        // Random Pos
        b->x = random(30, CONTAINER_WIDTH - 30);
        b->y = random(50, CONTAINER_HEIGHT - 50);
        b->vx = ((float)random(-20, 21)) / 10.0f;
        b->vy = ((float)random(-20, 21)) / 10.0f;
        if (b->vx == 0) b->vx = 1;

        b->obj = lv_obj_create(scr_chaos);
        lv_obj_set_size(b->obj, BUBBLE_BASE_SIZE, BUBBLE_BASE_SIZE);
        lv_obj_add_style(b->obj, &style_bubble, 0);
        lv_obj_set_pos(b->obj, (lv_coord_t)b->x, (lv_coord_t)b->y);
        lv_obj_clear_flag(b->obj, LV_OBJ_FLAG_SCROLLABLE);

        // Label
        b->label_ssid = lv_label_create(b->obj);
        lv_label_set_text(b->label_ssid, net.ssid);
        lv_label_set_long_mode(b->label_ssid, LV_LABEL_LONG_DOT);
        lv_obj_set_width(b->label_ssid, 45);
        lv_obj_set_style_text_font(b->label_ssid, &lv_font_montserrat_10, 0);
        lv_obj_center(b->label_ssid);

        // Logic for signal color etc
        if (net.rssi > -60) lv_obj_set_style_border_color(b->obj, COLOR_NEON_GREEN, 0);

        lv_obj_add_event_cb(b->obj, on_bubble_click, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        active_bubble_count++;
    }

    if (active_bubble_count == 0) {
        lv_obj_t *lbl = lv_label_create(scr_chaos);
        lv_label_set_text(lbl, "No Networks Found\nTry Scanning...");
        lv_obj_center(lbl);
    }

    if (physics_timer) lv_timer_del(physics_timer);
    physics_timer = lv_timer_create(physics_timer_cb, 30, nullptr);

    ui_switch_screen(scr_chaos);
}
