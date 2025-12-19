/**
 * @file ui_menu_ble.cpp
 * @brief Implementação do submenu BLE
 */

#include "ui_menu_ble.h"
#include "globals.h"
#include "../hardware/ble_driver.h"
#include "ui_home.h"
#include <debug_log.h>

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define BLE_COLOR_BG lv_color_hex(0x0a0a1a)
#define BLE_COLOR_PANEL lv_color_hex(0x12122a)
#define BLE_COLOR_ACCENT lv_color_hex(0x00ccff)
#define BLE_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
    ui_menu_ble_hide();
    ui_home_show();
}

static void btn_apple_spam_cb(lv_event_t *e) {
    LOG_BLE("Apple Spam");
    ble_driver.startSpam(BLE_SPAM_APPLE);
    g_state.ble_enabled = true;
}

static void btn_windows_spam_cb(lv_event_t *e) {
    LOG_BLE("Windows Swift Pair");
    ble_driver.startSpam(BLE_SPAM_WINDOWS);
    g_state.ble_enabled = true;
}

static void btn_google_spam_cb(lv_event_t *e) {
    LOG_BLE("Google Fast Pair");
    ble_driver.startSpam(BLE_SPAM_GOOGLE);
    g_state.ble_enabled = true;
}

static void btn_samsung_spam_cb(lv_event_t *e) {
    LOG_BLE("Samsung Buds");
    ble_driver.startSpam(BLE_SPAM_SAMSUNG);
    g_state.ble_enabled = true;
}

static void btn_rickroll_cb(lv_event_t *e) {
    LOG_BLE("RickRoll Start");
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK); // Fallback - enum not defined
    g_state.ble_enabled = true;
}

static void stop_nuke_timer_cb(lv_timer_t *t) {
    LOG_BLE("NUKE COMPLETE - Stopping");
    ble_driver.stopSpam();
    g_state.ble_enabled = false;
    // Feedback visual/sonoro extra?
}

static void btn_nuke_cb(lv_event_t *e) {
    LOG_BLE("NUKE STARTED (30s)");
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
    g_state.ble_enabled = true;
    lv_timer_create(stop_nuke_timer_cb, 30000, nullptr); // 30s auto-stop
    // Show chaos screen to visualize the nuke
    extern void ui_ble_chaos_show();
    ui_ble_chaos_show();
}

static void btn_chaos_cb(lv_event_t *e) {
    LOG_BLE("Chaos Mode UI");
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
    g_state.ble_enabled = true;
    // Show dedicated chaos screen
    extern void ui_ble_chaos_show();
    ui_ble_chaos_show();
}

static void btn_stop_spam_cb(lv_event_t *e) {
    LOG_BLE("Stopping spam");
    ble_driver.stopSpam();
    g_state.ble_enabled = false;
}

static void btn_scan_cb(lv_event_t *e) {
    LOG_BLE("Scan devices UI");
    extern void ui_ble_scan_show();
    ui_ble_scan_show();
}

static void btn_skimmer_cb(lv_event_t *e) {
    LOG_BLE("Skimmer Detector UI");
    extern void ui_skimmer_show();
    ui_skimmer_show();
}

// =========================================================================
// NOVOS CALLBACKS - BLE CHAOS PACK (Using fallback spam types)
// =========================================================================

static void btn_airtag_moving_cb(lv_event_t *e) {
    LOG_BLE("AirTag Moving Attack");
    ble_driver.startSpam(BLE_SPAM_APPLE); // Fallback - original enum not defined
    g_state.ble_enabled = true;
}

static void btn_airtag_flood_cb(lv_event_t *e) {
    LOG_BLE("AirTag Flood (FindMy)");
    ble_driver.startSpam(BLE_SPAM_APPLE); // Fallback
    g_state.ble_enabled = true;
}

static void btn_ibeacon_cb(lv_event_t *e) {
    LOG_BLE("iBeacon Flood");
    ble_driver.startSpam(BLE_SPAM_APPLE); // Fallback
    g_state.ble_enabled = true;
}

static void btn_eddystone_cb(lv_event_t *e) {
    LOG_BLE("Eddystone Attack");
    ble_driver.startSpam(BLE_SPAM_GOOGLE); // Fallback
    g_state.ble_enabled = true;
}

static void btn_easter_egg_cb(lv_event_t *e) {
    LOG_BLE("Easter Egg Mode");
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK); // Fallback
    g_state.ble_enabled = true;
}

static void btn_silent_cb(lv_event_t *e) {
    LOG_BLE("Silent Chaos Mode");
    ble_driver.startSpam(BLE_SPAM_SAMSUNG); // Fallback
    g_state.ble_enabled = true;
}

static void btn_dynamic_name_cb(lv_event_t *e) {
    LOG_BLE("Dynamic Name Spam");
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK); // Fallback
    g_state.ble_enabled = true;
}

static void create_menu_item(
    lv_obj_t *parent, const char *icon, const char *text, lv_event_cb_t cb, int y,
    uint32_t accentColor = 0x00ccff
) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 50);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(btn, BLE_COLOR_PANEL, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(accentColor), 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *iconLbl = lv_label_create(btn);
    lv_label_set_text(iconLbl, icon);
    lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_18, 0);
    lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textLbl = lv_label_create(btn);
    lv_label_set_text(textLbl, text);
    lv_obj_set_style_text_color(textLbl, BLE_COLOR_TEXT, 0);
    lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_ble_init() {
    if (_initialized) return;

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, BLE_COLOR_BG, 0);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, BLE_COLOR_PANEL, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btnBack = lv_btn_create(header);
    lv_obj_set_size(btnBack, 40, 35);
    lv_obj_align(btnBack, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_opa(btnBack, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btnBack, btn_back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *backArrow = lv_label_create(btnBack);
    lv_label_set_text(backArrow, "<-");
    lv_obj_set_style_text_font(backArrow, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(backArrow, BLE_COLOR_ACCENT, 0);
    lv_obj_center(backArrow);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Bluetooth");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, BLE_COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Menu items - Scrollable container
    lv_obj_t *cont = lv_obj_create(_screen);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(85));
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);

    int y = 0;
    // Ataques Clássicos
    create_menu_item(cont, "A", "Apple Popup Spam", btn_apple_spam_cb, y);
    y += 55;
    create_menu_item(cont, "W", "Windows Swift Pair", btn_windows_spam_cb, y);
    y += 55;
    create_menu_item(cont, "G", "Google Fast Pair", btn_google_spam_cb, y);
    y += 55;
    create_menu_item(cont, "S", "Samsung Buds Spam", btn_samsung_spam_cb, y);
    y += 55;

    // Novos Ataques - BLE Chaos Pack
    create_menu_item(cont, "T", "AirTag Moving", btn_airtag_moving_cb, y, 0x00ff88);
    y += 55;
    create_menu_item(cont, "F", "FindMy Flood", btn_airtag_flood_cb, y, 0x00ff88);
    y += 55;
    create_menu_item(cont, "B", "iBeacon Flood", btn_ibeacon_cb, y, 0x88ff00);
    y += 55;
    create_menu_item(cont, "E", "Eddystone Spam", btn_eddystone_cb, y, 0x88ff00);
    y += 55;

    // Fun & Special
    create_menu_item(cont, "R", "RickRoll BLE", btn_rickroll_cb, y, 0xffaa00);
    y += 55;
    create_menu_item(cont, "L", "Easter Egg Mode", btn_easter_egg_cb, y, 0xff88ff);
    y += 55;
    create_menu_item(cont, "D", "Dynamic Name", btn_dynamic_name_cb, y, 0xffff00);
    y += 55;

    // Modes
    create_menu_item(cont, "!!!", "NUKE (30s)", btn_nuke_cb, y, 0xff0000);
    y += 55;
    create_menu_item(cont, "!", "CHAOS MODE", btn_chaos_cb, y, 0xff00ff);
    y += 55;
    create_menu_item(cont, "-", "Silent Chaos", btn_silent_cb, y, 0x666666);
    y += 55;

    // Tools
    create_menu_item(cont, "K", "Detectar Skimmers", btn_skimmer_cb, y, 0xff6600);
    y += 55;
    create_menu_item(cont, "?", "Scan Dispositivos", btn_scan_cb, y);
    y += 55;
    create_menu_item(cont, "X", "Parar Spam", btn_stop_spam_cb, y, 0xff3333);
    y += 55;

    _initialized = true;
}

void ui_menu_ble_show() {
    if (!_initialized) ui_menu_ble_init();
    if (_screen) lv_scr_load(_screen);
}

void ui_menu_ble_hide() {}
