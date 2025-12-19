/*
 * 🖥️ UI MAIN MANAGER
 *
 * Gerencia as telas e o LVGL.
 */

#include "ui_main.h"
#include "globals.h"
#include "gesture_handler.h"

#include "screens/ui_files_screen.h"
#include "screens/ui_stats_screen.h"
#include "status_bar.h"
#include "ui_attacks.h"
#include "ui_ble_chaos.h"
#include "ui_captures.h"
#include "ui_helpers.h"
#include "ui_home.h"
#include "ui_lock_screen.h"
#include "ui_notifications.h"
#include "ui_settings.h"
#include <debug_log.h>

// Objetos LVGL
static lv_obj_t *scr_main = nullptr;
static lv_obj_t *lbl_mood = nullptr;

GlobalState g_state;

// Estado da UI
static UIScreen current_screen = UI_SCREEN_MAIN;
static bool ui_initialized = false;
static lv_timer_t *update_timer = nullptr;
static lv_scr_load_anim_t current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN;

#ifndef UI_COLOR_BG
#define UI_COLOR_BG lv_color_hex(0x0D0D0D)
#endif

void ui_set_screen(UIScreen screen);

static void ui_update_timer_cb(lv_timer_t *timer);
static void init_styles();

static lv_obj_t *content_area = nullptr;
lv_obj_t *ui_get_content_area() { return content_area; }
void ui_clear_content_area() {
    if (content_area) lv_obj_clean(content_area);
}

static void ui_update_timer_cb(lv_timer_t *timer) {
    if (!ui_initialized) return;
}

static void init_styles() { ui_init_global_styles(); }

bool ui_main_init() {
    if (ui_initialized) return true;

    LOG_UI("Iniciando interface principal...");
    init_styles();

    scr_main = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr_main, UI_COLOR_BG, 0);
    lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

    statusBar.create(scr_main);
    ui_create_3_zone_layout(scr_main, &content_area);
    ui_home_show();

    lv_scr_load(scr_main);
    update_timer = lv_timer_create(ui_update_timer_cb, 500, nullptr);

    gesture_handler.begin();
    gesture_handler.setCallback([](GestureType gesture, int16_t x, int16_t y) {
        switch (gesture) {
            case GESTURE_SWIPE_UP:
                LOG_UI("Swipe Up -> Notifications");
                ui_notification_center_show();
                break;
            case GESTURE_SWIPE_DOWN:
                LOG_UI("Swipe Down -> Settings");
                ui_settings_show();
                break;
            case GESTURE_SWIPE_LEFT:
                if (ui_get_current_screen() == UI_SCREEN_MAIN) {
                    ui_set_screen(UI_SCREEN_WIFI_CHAOS);
                } else if (ui_get_current_screen() == UI_SCREEN_WIFI_CHAOS) {
                    ui_set_screen(UI_SCREEN_BLE_CHAOS);
                }
                break;
            case GESTURE_SWIPE_RIGHT:
                if (ui_get_current_screen() == UI_SCREEN_BLE_CHAOS) {
                    ui_set_screen(UI_SCREEN_WIFI_CHAOS);
                } else if (ui_get_current_screen() == UI_SCREEN_WIFI_CHAOS) {
                    ui_set_screen(UI_SCREEN_MAIN);
                }
                break;
            default: break;
        }
    });

    lv_timer_create([](lv_timer_t *t) { gesture_handler.update(); }, 20, nullptr);

    ui_initialized = true;
    current_screen = UI_SCREEN_MAIN;
    LOG_UI("Interface pronta!");

    lv_timer_create(
        [](lv_timer_t *t) {
            if (g_state.battery_percent == 0 && !g_state.is_charging) {
                ui_notification_push("Aviso", "Bateria nao detectada", NOTIFY_WARNING);
            }
            if (!g_state.wifi_enabled) { ui_notification_push("Info", "WiFi desativado", NOTIFY_INFO); }
            lv_timer_del(t);
        },
        2000, nullptr
    );

    return true;
}

void ui_set_mood_text(const char *text) {
    if (lbl_mood) lv_label_set_text(lbl_mood, text);
}

UIScreen ui_get_current_screen() { return current_screen; }

void ui_set_screen(UIScreen screen) {
    current_screen = screen;
    lv_obj_t *target = nullptr;

    switch (screen) {
        case UI_SCREEN_MAIN: target = scr_main; break;
        case UI_SCREEN_ATTACKS:
        case UI_SCREEN_WIFI_CHAOS: ui_attacks_show(); return;
        case UI_SCREEN_BLE_CHAOS: ui_ble_chaos_show(); return;
        case UI_SCREEN_SETTINGS: ui_settings_show(); return;
        case UI_SCREEN_LOCK: lockScreen.show(); return;
        case UI_SCREEN_CAPTURES: ui_captures_show(); return;
        case UI_SCREEN_STATS: statsScreen.show(); return;
        case UI_SCREEN_FILES: filesScreen.show(); return;
        case UI_SCREEN_PLUGINS: return;
        case UI_SCREEN_IOT_PWN: return;
        default: target = scr_main; break;
    }

    if (target) { ui_switch_screen(target, current_transition_anim); }
}

void ui_set_transition_animation(uint8_t anim_type) {
    switch (anim_type) {
        case 0: current_transition_anim = LV_SCR_LOAD_ANIM_NONE; break;
        case 1: current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN; break;
        case 2: current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_LEFT; break;
        case 3: current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_RIGHT; break;
        case 4: current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_TOP; break;
        case 5: current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_BOTTOM; break;
        case 6: current_transition_anim = LV_SCR_LOAD_ANIM_OVER_LEFT; break;
        case 7: current_transition_anim = LV_SCR_LOAD_ANIM_OVER_RIGHT; break;
        default: current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN; break;
    }
}

void ui_main_show() {
    if (scr_main) {
        ui_switch_screen(scr_main, current_transition_anim);
        current_screen = UI_SCREEN_MAIN;
        ui_home_show();
    }
}
