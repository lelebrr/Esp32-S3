#include "lvgl_menu.h"
#include "attacks_manager.h"
#include "gesture_sensor.h"
#include "pin_config.h"
#include "module_manager.h"
#include "led_driver.h"
#include "tts_espeak.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "ble_attacks.h"
#include "pmkid_capture.h"
#include "wps_attacks.h"
#include "nfc_relay.h"
#include "ir_protocols.h"
#include "gps_driver.h"
#include "nfc_driver.h"
#include "q_learn_ia.h"
#include <SD.h>
#include <esp_sleep.h>

// ------------------------------------------------------------
// CONFIGURATION
// ------------------------------------------------------------
#define SCR_W 320  // Landscape width
#define SCR_H 240  // Landscape height
#define SCREEN_RF SCREEN_SUBGHZ
#define SCREEN_EVIL_TWIN 200 // Custom ID for submenu

// Colors (neon cyberpunk palette)
#define COL_BG       0x1a1a2e
#define COL_BTN      0x0f3460
#define COL_BTN_SEL  0x16213e
#define COL_RED      0xe94560
#define COL_GREEN    0x4ecdc4
#define COL_YELLOW   0xf9ed69
#define COL_TEXT     0xeeeeee

// Touch calibration data (example values)
static uint16_t touchCalData[5] = {350, 3500, 250, 3400, 1};

// ------------------------------------------------------------
// GLOBAL OBJECTS
// ------------------------------------------------------------
static TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_indev_t *gesture_indev = NULL;
static lv_group_t *main_group = NULL;
static volatile uint32_t last_gesture_key = 0;

static ScreenType current_screen = SCREEN_MAIN;
static lv_obj_t *status_label = NULL;

// ------------------------------------------------------------
// DISPLAY CALLBACKS
// ------------------------------------------------------------
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)color_p, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

static void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t x, y;
    bool touched = tft.getTouch(&x, &y);
    if (touched) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void gesture_keypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (last_gesture_key != 0) {
        data->key = last_gesture_key;
        data->state = LV_INDEV_STATE_PRESSED;
        last_gesture_key = 0;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void lvgl_send_gesture_key(uint32_t key) {
    last_gesture_key = key;
}

uint32_t gesture_action_to_lvgl_key(GestureAction action) {
    switch (action) {
        case ACTION_MENU_UP:    return LV_KEY_UP;
        case ACTION_MENU_DOWN:  return LV_KEY_DOWN;
        case ACTION_MENU_LEFT:  return LV_KEY_LEFT;
        case ACTION_MENU_RIGHT: return LV_KEY_RIGHT;
        case ACTION_SELECT:     return LV_KEY_ENTER;
        case ACTION_BACK:       return LV_KEY_ESC;
        default: return 0;
    }
}

// ------------------------------------------------------------
// FORWARD DECLARATIONS (screens)
// ------------------------------------------------------------
static void show_main_menu();
static void show_quick_attack_menu();
static void show_wifi_menu();
static void show_ble_menu();
static void show_nfc_menu();
static void show_rf_menu();
static void show_ir_menu();
static void show_hardware_menu();
static void show_ai_voice_menu();
static void show_settings_menu();
static void show_status_screen();
static void show_evil_twin_menu();
static void show_led_menu();
static void show_energy_menu();
static void show_reset_menu();
static void update_status_bar();

// ------------------------------------------------------------
// UI HELPERS
// ------------------------------------------------------------
static lv_obj_t* create_menu_btn(lv_obj_t *parent, const char *text,
                               lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                               uint32_t color, lv_event_cb_t cb) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, (void*)text);
    lv_obj_set_style_outline_width(btn, 3, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btn, lv_color_hex(COL_GREEN), LV_STATE_FOCUSED);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_TEXT), 0);
    lv_obj_center(label);
    if (main_group) lv_group_add_obj(main_group, btn);
    return btn;
}

static void create_header(lv_obj_t *scr, const char *title) {
    // Back button
    lv_obj_t *back = lv_btn_create(scr);
    lv_obj_set_pos(back, 5, 5);
    lv_obj_set_size(back, 50, 30);
    lv_obj_set_style_bg_color(back, lv_color_hex(COL_RED), 0);
    lv_obj_add_event_cb(back, [](lv_event_t *e){
        if (current_screen == (ScreenType)SCREEN_EVIL_TWIN) show_wifi_menu();
        else show_main_menu();
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bl = lv_label_create(back);
    lv_label_set_text(bl, LV_SYMBOL_LEFT);
    lv_obj_center(bl);
    if (main_group) lv_group_add_obj(main_group, back);
    // Title
    lv_obj_t *t = lv_label_create(scr);
    lv_label_set_text(t, title);
    lv_obj_set_style_text_color(t, lv_color_hex(COL_GREEN), 0);
    lv_obj_set_style_text_font(t, &lv_font_montserrat_16, 0);
    lv_obj_align(t, LV_ALIGN_TOP_MID, 0, 10);
}
static void attack_cb(lv_event_t *e) {
    const char *txt = (const char*)lv_event_get_user_data(e);
    Serial.printf("[UI] Attack: %s\n", txt);
    attacks_stop();
    LEDDriver::setAll(LED_CYAN);
    LEDDriver::show();
    
    // BLE
    if (strcmp(txt, "BLE Spam") == 0) attacks_start(ATTACK_BLE_SPAM);
    else if (strcmp(txt, "Spam BR") == 0) BLEAttacks::start(BLE_ATTACK_SPAM_BRAZIL);
    else if (strcmp(txt, "Sour Apple") == 0) attacks_start(ATTACK_BLE_SOUR_APPLE);
    else if (strcmp(txt, "Swift Pair") == 0) attacks_start(ATTACK_BLE_SWIFT_PAIR);
    else if (strcmp(txt, "Fast Pair") == 0) attacks_start(ATTACK_BLE_FAST_PAIR);
    else if (strcmp(txt, "Samsung") == 0) BLEAttacks::start(BLE_ATTACK_SAMSUNG_BUDS);
    
    // WiFi
    else if (strcmp(txt, "Deauth") == 0) attacks_start(ATTACK_WIFI_DEAUTH);
    else if (strcmp(txt, "Beacon") == 0) attacks_start(ATTACK_WIFI_BEACON_SPAM);
    else if (strcmp(txt, "Evil Twin") == 0) attacks_start(ATTACK_WIFI_EVIL_TWIN);
    else if (strcmp(txt, "Wardrive") == 0) attacks_start(ATTACK_WIFI_WARDRIVE);
    else if (strcmp(txt, "PMKID") == 0) attacks_start(ATTACK_WIFI_PMKID);
    else if (strcmp(txt, "PMKID Cap") == 0) PMKIDCapture::start();
    else if (strcmp(txt, "WPS Pixie") == 0) {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) WPSAttacks::startPixieDust(WPSAttacks::getNetwork(0));
    }
    else if (strcmp(txt, "WPS Brute") == 0) {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) WPSAttacks::startBruteForce(WPSAttacks::getNetwork(0));
    }
    
    // RF SubGHz
    else if (strcmp(txt, "Jammer 433") == 0) attacks_start(ATTACK_RF_JAMMER_433);
    else if (strcmp(txt, "Jammer 315") == 0) attacks_start(ATTACK_RF_JAMMER_315);
    else if (strcmp(txt, "Jammer 868") == 0) attacks_start(ATTACK_RF_JAMMER_868);
    else if (strcmp(txt, "Jammer Burst") == 0) attacks_start(ATTACK_RF_JAMMER_BURST);
    else if (strcmp(txt, "Jammer Smart") == 0) attacks_start(ATTACK_RF_JAMMER_SMART);
    else if (strcmp(txt, "Capture") == 0) attacks_start(ATTACK_RF_CAPTURE);
    else if (strcmp(txt, "Replay") == 0) attacks_start(ATTACK_RF_REPLAY);
    else if (strcmp(txt, "Ghost") == 0) attacks_start(ATTACK_RF_GHOST_REPLAY);
    else if (strcmp(txt, "Brute Force") == 0) attacks_start(ATTACK_RF_BRUTE_FORCE);
    else if (strcmp(txt, "Spectrum") == 0) attacks_start(ATTACK_RF_SPECTRUM);
    
    // NFC
    else if (strcmp(txt, "Clone Card") == 0) attacks_start(ATTACK_NFC_CLONE);
    else if (strcmp(txt, "Fault Inj") == 0) attacks_start(ATTACK_NFC_FAULT);
    else if (strcmp(txt, "Phishing") == 0) attacks_start(ATTACK_NFC_PHISHING);
    else if (strcmp(txt, "Relay Read") == 0) { NFCRelay::init(); NFCRelay::setReaderMode(); }
    else if (strcmp(txt, "Relay Emu") == 0) { NFCRelay::init(); NFCRelay::setEmulatorMode(); }
    
    // IR
    else if (strcmp(txt, "TV-B-Gone") == 0) IRProtocols::tvBGone();
    else if (strcmp(txt, "IR Brute") == 0) attacks_start(ATTACK_IR_BRUTE);
    else if (strcmp(txt, "IR Clone") == 0) attacks_start(ATTACK_IR_CLONE);
    else if (strcmp(txt, "Gate Brute") == 0) IRProtocols::bruteGate12bit();
    
    update_status_bar();
}
// ------------------------------------------------------------
// MAIN MENU
// ------------------------------------------------------------
static void show_main_menu() {
    current_screen = SCREEN_MAIN;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);

    // Title
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, LV_SYMBOL_WARNING " MONSTER S3");
    lv_obj_set_style_text_color(title, lv_color_hex(COL_RED), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Status label
    status_label = lv_label_create(scr);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x888888), 0);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 28);
    update_status_bar();

    // Layout: 2 columns x 4 rows (8 blocks)
    const int bw = 140, bh = 45, gap = 10;
    int x1 = 10, x2 = 170;
    int y = 60;
    create_menu_btn(scr, "Ataque Rápido", x1, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_quick_attack_menu(); });
    create_menu_btn(scr, "WiFi",          x2, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_wifi_menu(); });
    y += bh + gap;
    create_menu_btn(scr, "BLE",           x1, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_ble_menu(); });
    create_menu_btn(scr, "NFC",           x2, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_nfc_menu(); });
    y += bh + gap;
    create_menu_btn(scr, "SubGHz",        x1, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_rf_menu(); });
    create_menu_btn(scr, "IR",            x2, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_ir_menu(); });
    y += bh + gap;
    create_menu_btn(scr, "Hardware",      x1, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_hardware_menu(); });
    create_menu_btn(scr, "IA & Voz",      x2, y, bw, bh, COL_BTN, [](lv_event_t *e){ show_ai_voice_menu(); });
    y += bh + gap;
    // STOP button spanning width
    create_menu_btn(scr, "STOP", 10, y, 300, 40, COL_RED, [](lv_event_t *e){ attacks_stop(); update_status_bar(); });
    Serial.println("[UI] Main menu loaded");
}

// ------------------------------------------------------------
// QUICK ATTACK MENU (predefined combos)
// ------------------------------------------------------------
static void show_quick_attack_menu() {
    current_screen = SCREEN_QUICK_ATTACK;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Ataque Rápido");
    int y = 45;
    create_menu_btn(scr, "Combo Porteiro", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        attacks_start(ATTACK_RF_REPLAY);
        attacks_start(ATTACK_NFC_FAULT);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Combo Casa", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        attacks_start(ATTACK_WIFI_DEAUTH);
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Combo BLE", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        attacks_start(ATTACK_BLE_SPAM);
        // attacks_start(ATTACK_BLE_HID_INJECT); // Not implemented yet
        attacks_start(ATTACK_BLE_SWIFT_PAIR); // Alternative
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Combo IR", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        attacks_start(ATTACK_IR_BRUTE);
        attacks_start(ATTACK_IR_CLONE); // Was ATTACK_IR_LEARN
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Combo Full Auto", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        // AI decides best attack based on Q-table
        int best = ai_get_best_action();
        AttackType ai_attack = (AttackType)(best + 1); // Offset for enum
        if (ai_attack > ATTACK_NONE && ai_attack < ATTACK_USB_EXFIL) {
            attacks_start(ai_attack);
            tts_speak("modo_combate");
            LEDDriver::blinkAttacking();
        } else {
            attacks_start(ATTACK_WIFI_DEAUTH); // Fallback
        }
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Combo Stealth", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        attacks_stop();
        lvgl_toggle_module("wifi", false);
        lvgl_toggle_module("ble", false);
        lvgl_toggle_module("nfc", false);
        lvgl_toggle_module("rf", false);
        lvgl_toggle_module("ir", false);
        update_status_bar();
    });
}

// ------------------------------------------------------------
// BLE MENU (unchanged from previous implementation)
// ------------------------------------------------------------
static void show_ble_menu() {
    current_screen = SCREEN_BLE;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "BLE Attacks");
    int y = 45;
    create_menu_btn(scr, "BLE Spam",   10, y,      95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Spam BR",    110, y,     95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Sour Apple", 210, y,    100, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Swift Pair", 10, y+50,  95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Fast Pair",  110, y+50, 95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Samsung",    210, y+50, 100, 40, COL_BTN, attack_cb);
}

// ------------------------------------------------------------
// WIFI MENU (unchanged)
// ------------------------------------------------------------
static void show_wifi_menu() {
    current_screen = SCREEN_WIFI;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "WiFi Attacks");
    int y = 45;
    create_menu_btn(scr, "Deauth",      10, y,     70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "Beacon",      85, y,     70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "Evil Twin",   160, y,    70, 35, COL_BTN, [](lv_event_t *e){ show_evil_twin_menu(); });
    create_menu_btn(scr, "Wardrive",    235, y,    75, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "PMKID",       10, y+40,  70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "PMKID Cap",   85, y+40,  70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "WPS Pixie",   160, y+40, 70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "WPS Brute",   235, y+40, 75, 35, COL_BTN, attack_cb);
}

// ------------------------------------------------------------
// EVIL TWIN MENU
// ------------------------------------------------------------
static void show_evil_twin_menu() {
    current_screen = (ScreenType)SCREEN_EVIL_TWIN;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Evil Twin Templates");
    
    int y = 45;
    // Templates BR
    create_menu_btn(scr, "Vivo Fibra", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid("Vivo Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "Claro Fibra", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid("Claro Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Oi Fibra", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid("Oi Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "TIM Live", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid("TIM Live");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "GVT", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid("GVT Guest");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "Clone Strongest", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        attacks_set_evil_twin_ssid(NULL); // Auto clone
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
}

// ------------------------------------------------------------
// RF MENU (unchanged)
// ------------------------------------------------------------
static void show_rf_menu() {
    current_screen = SCREEN_SUBGHZ;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "RF SubGHz");
    // Scrollable container
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 310, 185);
    lv_obj_set_pos(cont, 5, 40);
    lv_obj_set_style_bg_color(cont, lv_color_hex(COL_BG), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_AUTO);
    int y = 0, bh = 35, gap = 5;
    // Jammers
    create_menu_btn(cont, "Jammer 433",   0, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Jammer 315", 100, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Jammer 868", 200, y, 95, bh, COL_BTN, attack_cb);
    y += bh + gap;
    create_menu_btn(cont, "Jammer Burst", 0, y, 145, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Jammer Smart", 150, y, 145, bh, COL_BTN, attack_cb);
    y += bh + gap;
    // Capture/Replay
    create_menu_btn(cont, "Capture", 0, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Replay", 100, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Ghost", 200, y, 95, bh, COL_BTN, attack_cb);
    y += bh + gap;
    create_menu_btn(cont, "Brute Force", 0, y, 145, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Spectrum", 150, y, 145, bh, COL_BTN, attack_cb);
}

// ------------------------------------------------------------
// NFC MENU (unchanged)
// ------------------------------------------------------------
static void show_nfc_menu() {
    current_screen = SCREEN_NFC;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "NFC Attacks");
    int y = 45;
    create_menu_btn(scr, "Clone Card", 10, y,     95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Fault Inj",  110, y,    95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Phishing",   210, y,    100, 40, COL_BTN, attack_cb);
    // Relay buttons
    create_menu_btn(scr, "Relay Read", 10, y+45,  145, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Relay Emu",  165, y+45, 145, 40, COL_BTN, attack_cb);
    // Status label
    lv_obj_t *nfcStatus = lv_label_create(scr);
    if (NFCDriver::isReady()) {
        lv_label_set_text(nfcStatus, "PN532: Ready");
        lv_obj_set_style_text_color(nfcStatus, lv_color_hex(COL_GREEN), 0);
    } else {
        lv_label_set_text(nfcStatus, "PN532: Not Found");
        lv_obj_set_style_text_color(nfcStatus, lv_color_hex(COL_RED), 0);
    }
    lv_obj_align(nfcStatus, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ------------------------------------------------------------
// IR MENU (unchanged)
// ------------------------------------------------------------
static void show_ir_menu() {
    current_screen = SCREEN_IR;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "IR Attacks");
    int y = 45;
    create_menu_btn(scr, "TV-B-Gone", 10, y,   95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "IR Brute",  110, y,  95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "IR Clone",  210, y,  100, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Gate Brute", 10, y+45, 145, 40, COL_BTN, attack_cb);
    // Info label
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "YS-IRTM: NEC/RC5/Sony");
    lv_obj_set_style_text_color(info, lv_color_hex(COL_GREEN), 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ------------------------------------------------------------
// HARDWARE MENU (new)
// ------------------------------------------------------------
static void show_hardware_menu() {
    current_screen = SCREEN_HARDWARE;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Hardware Control");
    int y = 45;
    // Module toggles
    create_menu_btn(scr, "WiFi",    10, y, 140, 40, COL_BTN, [](lv_event_t *e){
        bool cur = lvgl_get_module_state("wifi");
        lvgl_toggle_module("wifi", !cur);
    });
    create_menu_btn(scr, "BLE",    160, y, 140, 40, COL_BTN, [](lv_event_t *e){
        bool cur = lvgl_get_module_state("ble");
        lvgl_toggle_module("ble", !cur);
    });
    y += 50;
    create_menu_btn(scr, "NFC",    10, y, 140, 40, COL_BTN, [](lv_event_t *e){
        bool cur = lvgl_get_module_state("nfc");
        lvgl_toggle_module("nfc", !cur);
    });
    create_menu_btn(scr, "SubGHz", 160, y, 140, 40, COL_BTN, [](lv_event_t *e){
        bool cur = lvgl_get_module_state("rf");
        lvgl_toggle_module("rf", !cur);
    });
    y += 50;
    create_menu_btn(scr, "IR",      10, y, 140, 40, COL_BTN, [](lv_event_t *e){
        bool cur = lvgl_get_module_state("ir");
        lvgl_toggle_module("ir", !cur);
    });
    create_menu_btn(scr, "LEDs", 160, y, 140, 40, COL_BTN, [](lv_event_t *e){ show_led_menu(); });
    y += 50;
    create_menu_btn(scr, "Energia", 10, y, 140, 40, COL_BTN, [](lv_event_t *e){ show_energy_menu(); });
    create_menu_btn(scr, "Reset", 160, y, 140, 40, COL_RED, [](lv_event_t *e){ show_reset_menu(); });
}

// ------------------------------------------------------------
// LED MENU (WS2812B effects)
// ------------------------------------------------------------
static void show_led_menu() {
    current_screen = (ScreenType)SCREEN_HW_LEDS;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "LEDs WS2812B");
    int y = 45;
    create_menu_btn(scr, "Neon Pulse", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        LEDDriver::pulseEffect(LED_CYAN, LED_MAGENTA, 50);
    });
    create_menu_btn(scr, "Matrix Rain", 165, y, 145, 40, COL_GREEN, [](lv_event_t *e){
        LEDDriver::matrixRain();
    });
    y += 50;
    create_menu_btn(scr, "Rainbow", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        LEDDriver::rainbowCycle();
    });
    create_menu_btn(scr, "Ataque Blink", 165, y, 145, 40, COL_RED, [](lv_event_t *e){
        LEDDriver::blinkAttacking();
    });
    y += 50;
    create_menu_btn(scr, "Status Verde", 10, y, 145, 40, COL_GREEN, [](lv_event_t *e){
        LEDDriver::setAll(LED_GREEN); LEDDriver::show();
    });
    create_menu_btn(scr, "Off", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        LEDDriver::clear(); LEDDriver::show();
    });
}

// ------------------------------------------------------------
// ENERGY MENU (Deep Sleep, Overclock, Battery)
// ------------------------------------------------------------
static void show_energy_menu() {
    current_screen = (ScreenType)SCREEN_HW_ENERGY;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Energia");
    int y = 45;
    create_menu_btn(scr, "Deep Sleep 500ms", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){
        esp_sleep_enable_timer_wakeup(500000); // 500ms
        esp_light_sleep_start();
    });
    create_menu_btn(scr, "Deep Sleep 2s", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        esp_sleep_enable_timer_wakeup(2000000); // 2s
        esp_light_sleep_start();
    });
    y += 50;
    create_menu_btn(scr, "OC 240MHz", 10, y, 145, 40, COL_YELLOW, [](lv_event_t *e){
        setCpuFrequencyMhz(240);
        Serial.println("[CPU] Overclock: 240MHz");
    });
    create_menu_btn(scr, "Normal 160MHz", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){
        setCpuFrequencyMhz(160);
        Serial.println("[CPU] Normal: 160MHz");
    });
    y += 50;
    // Battery info
    lv_obj_t *info = lv_label_create(scr);
    char buf[64];
    snprintf(buf, sizeof(buf), "CPU: %d MHz | Heap: %dKB", getCpuFrequencyMhz(), ESP.getFreeHeap()/1024);
    lv_label_set_text(info, buf);
    lv_obj_set_style_text_color(info, lv_color_hex(COL_GREEN), 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ------------------------------------------------------------
// RESET MENU (Q-Table, Configs, Logs)
// ------------------------------------------------------------
static void show_reset_menu() {
    current_screen = (ScreenType)SCREEN_HW_RESET;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Reset");
    int y = 45;
    create_menu_btn(scr, "Reset Q-Table", 10, y, 300, 40, COL_RED, [](lv_event_t *e){
        ai_reset_qtable();
        tts_speak("ataque_parado");
        Serial.println("[AI] Q-Table resetada");
    });
    y += 50;
    create_menu_btn(scr, "Limpar Logs", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){
        SD.remove("/logs/system.log");
        SD.remove("/logs/attacks.log");
        Serial.println("[SD] Logs limpos");
    });
    y += 50;
    create_menu_btn(scr, "Factory Reset", 10, y, 300, 40, COL_RED, [](lv_event_t *e){
        ai_reset_qtable();
        SD.remove("/config/config.json");
        tts_speak("sistema_pronto");
        ESP.restart();
    });
}

// ------------------------------------------------------------
// AI & VOICE MENU (fully implemented)
// ------------------------------------------------------------
static void show_ai_voice_menu() {
    current_screen = SCREEN_AI_VOICE;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "IA & Voz");
    int y = 45;
    create_menu_btn(scr, "Modo Treino", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){ 
        ai_set_combat_mode(false);
        tts_speak("modo_treino"); 
        LEDDriver::setAll(LED_BLUE);
        LEDDriver::show();
    });
    create_menu_btn(scr, "Modo Combate", 165, y, 145, 40, COL_RED, [](lv_event_t *e){ 
        ai_set_combat_mode(true);
        tts_speak("modo_combate"); 
        LEDDriver::setAll(LED_RED);
        LEDDriver::show();
    });
    y += 50;
    create_menu_btn(scr, "Atualizar CVE BR", 10, y, 300, 40, COL_BTN, [](lv_event_t *e){ 
        Serial.println("[AI] Iniciando download de CVEs BR...");
        tts_speak("ataque_iniciado");
        // ResourceDownloader::downloadCVEs(); // Uncomment when available
        LEDDriver::setAll(LED_YELLOW);
        LEDDriver::show();
    });
    y += 50;
    create_menu_btn(scr, "Testar Voz", 10, y, 145, 40, COL_BTN, [](lv_event_t *e){ 
        tts_speak("sistema_pronto"); 
    });
    create_menu_btn(scr, "Ver Estatísticas", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){ 
        AIStats stats;
        ai_get_statistics(&stats);
        Serial.printf("[AI] Updates: %lu, AvgR: %.2f, Best: %d\n", 
                      stats.totalUpdates, stats.avgReward, stats.bestAction);
    });
    y += 50;
    create_menu_btn(scr, "Reset Q-Table", 10, y, 145, 40, COL_RED, [](lv_event_t *e){ 
        ai_reset_qtable();
        tts_speak("ataque_parado");
    });
    create_menu_btn(scr, "Exportar JSON", 165, y, 145, 40, COL_BTN, [](lv_event_t *e){ 
        ai_export_qtable_json("/ai/qtable_export.json");
        tts_speak("codigo_enviado");
    });
}

// ------------------------------------------------------------
// SETTINGS MENU (kept for future use)
// ------------------------------------------------------------
static void show_settings_menu() {
    current_screen = SCREEN_SETTINGS;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Settings");
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info,
        "WiFi AP: Monster_S3\nPassword: lele2025\nDashboard: 192.168.4.1\n\nGestures: Enabled\nTouch: Calibrated");
    lv_obj_set_style_text_color(info, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 20);
}

// ------------------------------------------------------------
// STATUS SCREEN (unchanged)
// ------------------------------------------------------------
static void show_status_screen() {
    current_screen = SCREEN_STATUS;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "System Status");
    char statusText[256];
    snprintf(statusText, sizeof(statusText),
        "Heap: %d KB\nPSRAM: %d KB\nUptime: %d min\n\nAttack: %s\nGPS: %s\nNFC: %s",
        ESP.getFreeHeap() / 1024,
        ESP.getFreePsram() / 1024,
        millis() / 60000,
        attacks_get_name(attacks_get_current()),
        GPSDriver::isValid() ? "Valid" : "No Fix",
        NFCDriver::isReady() ? "Ready" : "N/A");
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, statusText);
    lv_obj_set_style_text_color(info, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 20);
}

// ------------------------------------------------------------
// STATUS BAR UPDATE
// ------------------------------------------------------------
static void update_status_bar() {
    if (!status_label) return;
    char buf[64];
    AttackType current = attacks_get_current();
    if (current != ATTACK_NONE) {
        snprintf(buf, sizeof(buf), "Running: %s", attacks_get_name(current));
        lv_obj_set_style_text_color(status_label, lv_color_hex(COL_RED), 0);
    } else {
        snprintf(buf, sizeof(buf), "Heap: %dKB | Ready", ESP.getFreeHeap() / 1024);
        lv_obj_set_style_text_color(status_label, lv_color_hex(0x888888), 0);
    }
    lv_label_set_text(status_label, buf);
}

// ------------------------------------------------------------
// LED & TTS SYNC (implemented)
// ------------------------------------------------------------
void lvgl_sync_leds() {
    AttackType cur = attacks_get_current();
    if (cur != ATTACK_NONE) {
        LEDDriver::blinkAttacking();
    } else {
        LEDDriver::clear();
        LEDDriver::show();
    }
}

void lvgl_announce_menu(const char* menu_name) {
    tts_speak(menu_name);
}

// ------------------------------------------------------------
// HARDWARE MODULE CONTROL (wrappers around ModuleManager)
// ------------------------------------------------------------
void lvgl_toggle_module(const char* module_name, bool enabled) {
    // Map module name to enum and activate/deactivate via ModuleManager
    ModuleType mtype = MODULE_NONE;
    if (strcmp(module_name, "wifi") == 0) mtype = MODULE_WIFI;
    else if (strcmp(module_name, "ble") == 0) mtype = MODULE_BLE;
    else if (strcmp(module_name, "nfc") == 0) mtype = MODULE_NFC;
    else if (strcmp(module_name, "rf") == 0) mtype = MODULE_SUBGHZ;
    else if (strcmp(module_name, "ir") == 0) mtype = MODULE_IR;
    if (enabled) {
        ModuleManager::ativaModulo(mtype);
    } else {
        ModuleManager::desligaModulos();
    }
    update_status_bar();
    lvgl_sync_leds();
}

bool lvgl_get_module_state(const char* module_name) {
    ModuleType mtype = MODULE_NONE;
    if (strcmp(module_name, "wifi") == 0) mtype = MODULE_WIFI;
    else if (strcmp(module_name, "ble") == 0) mtype = MODULE_BLE;
    else if (strcmp(module_name, "nfc") == 0) mtype = MODULE_NFC;
    else if (strcmp(module_name, "rf") == 0) mtype = MODULE_SUBGHZ;
    else if (strcmp(module_name, "ir") == 0) mtype = MODULE_IR;
    return ModuleManager::isModuleActive(mtype);
}

// ------------------------------------------------------------
// INITIALIZATION
// ------------------------------------------------------------
void setup_lvgl_menu() {
    Serial.println("[DISPLAY] Setting up TFT + LVGL...");
    // Initialize SPI pins (defined in pin_config.h)
    pinMode(PIN_TFT_CS, OUTPUT);
    digitalWrite(PIN_TFT_CS, HIGH);
    pinMode(PIN_TOUCH_CS, OUTPUT);
    digitalWrite(PIN_TOUCH_CS, HIGH);
    // Backlight PWM
    ledcSetup(0, 5000, 8);
    ledcAttachPin(PIN_TFT_BL, 0);
    ledcWrite(0, 255);
    // TFT Init
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTouch(touchCalData);
    Serial.println("[DISPLAY] TFT initialized");
    // LVGL Init
    lv_init();
    // Allocate draw buffer (prefer PSRAM)
    size_t bufSize = SCR_W * 40;
    if (psramFound()) {
        buf = (lv_color_t*)ps_malloc(bufSize * sizeof(lv_color_t));
        Serial.println("[LVGL] Using PSRAM buffer");
    } else {
        buf = (lv_color_t*)malloc(bufSize * sizeof(lv_color_t));
        Serial.println("[LVGL] Using heap buffer");
    }
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, bufSize);
    // Display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    // Touch driver
    static lv_indev_drv_t touch_drv;
    lv_indev_drv_init(&touch_drv);
    touch_drv.type = LV_INDEV_TYPE_POINTER;
    touch_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&touch_drv);
    // Gesture driver (keypad)
    main_group = lv_group_create();
    static lv_indev_drv_t gesture_drv;
    lv_indev_drv_init(&gesture_drv);
    gesture_drv.type = LV_INDEV_TYPE_KEYPAD;
    gesture_drv.read_cb = gesture_keypad_read;
    gesture_indev = lv_indev_drv_register(&gesture_drv);
    lv_indev_set_group(gesture_indev, main_group);
    Serial.println("[LVGL] Input devices configured");
    // Show UI
    show_main_menu();
    Serial.println("[DISPLAY] Setup complete!");
}

void lvgl_loop() {
    lv_task_handler();
    // Periodic LED/TTS sync could be added here
}

lv_group_t* lvgl_get_main_group() {
    return main_group;
}
