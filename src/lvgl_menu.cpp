/**
 * @file lvgl_menu.cpp
 * @brief Monster S3 Firmware - Complete LVGL Menu System
 * 
 * Display: MSP2402 - 2.4" ILI9341 240x320 SPI with XPT2046 Touch
 * 
 * Features:
 * - Main menu with category buttons
 * - Submenus for each attack category
 * - All 27 attacks accessible
 * - Status display
 * - Settings menu
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "lvgl_menu.h"
#include "attacks_manager.h"
#include "gesture_sensor.h"
#include "pin_config.h"
#include "nfc_driver.h"
#include "gps_driver.h"
#include "module_manager.h"
#include "led_driver.h"
#include "ble_attacks.h"
#include "ir_protocols.h"
#include "wps_attacks.h"
#include "pmkid_capture.h"
#include "nfc_relay.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
#define SCR_W 320  // Landscape width
#define SCR_H 240  // Landscape height

// Colors
#define COL_BG       0x1a1a2e
#define COL_BTN      0x0f3460
#define COL_BTN_SEL  0x16213e
#define COL_RED      0xe94560
#define COL_GREEN    0x4ecdc4
#define COL_YELLOW   0xf9ed69
#define COL_TEXT     0xeeeeee

// Touch Calibration
static uint16_t touchCalData[5] = {350, 3500, 250, 3400, 1};

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
static TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_indev_t *gesture_indev = NULL;
static lv_group_t *main_group = NULL;
static volatile uint32_t last_gesture_key = 0;

// Current screen/menu state
typedef enum {
    SCREEN_MAIN = 0,
    SCREEN_BLE,
    SCREEN_WIFI,
    SCREEN_RF,
    SCREEN_NFC,
    SCREEN_IR,
    SCREEN_USB,
    SCREEN_SETTINGS,
    SCREEN_STATUS
} ScreenType;

static ScreenType current_screen = SCREEN_MAIN;
static lv_obj_t *status_label = NULL;

// ============================================================================
// DISPLAY CALLBACKS
// ============================================================================
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)&color_p->full, w * h, true);
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

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
static void show_main_menu();
static void show_ble_menu();
static void show_wifi_menu();
static void show_rf_menu();
static void show_nfc_menu();
static void show_ir_menu();
static void show_usb_menu();
static void show_settings_menu();
static void show_status_screen();
static void update_status_bar();

// ============================================================================
// BUTTON HELPER
// ============================================================================
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

// ============================================================================
// MAIN MENU EVENT HANDLER
// ============================================================================
static void main_menu_cb(lv_event_t *e) {
    const char *txt = (const char*)lv_event_get_user_data(e);
    Serial.printf("[UI] Menu: %s\n", txt);
    
    if (strcmp(txt, "BLE") == 0) show_ble_menu();
    else if (strcmp(txt, "WiFi") == 0) show_wifi_menu();
    else if (strcmp(txt, "RF") == 0) show_rf_menu();
    else if (strcmp(txt, "NFC") == 0) show_nfc_menu();
    else if (strcmp(txt, "IR") == 0) show_ir_menu();
    else if (strcmp(txt, "USB") == 0) show_usb_menu();
    else if (strcmp(txt, "Settings") == 0) show_settings_menu();
    else if (strcmp(txt, "Status") == 0) show_status_screen();
    else if (strcmp(txt, "STOP") == 0) {
        attacks_stop();
        update_status_bar();
    }
}

// ============================================================================
// ATTACK MENU EVENT HANDLER
// ============================================================================
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
    else if (strcmp(txt, "Beacon") == 0 || strcmp(txt, "Beacon Spam") == 0) attacks_start(ATTACK_WIFI_BEACON_SPAM);
    else if (strcmp(txt, "Evil Twin") == 0) attacks_start(ATTACK_WIFI_EVIL_TWIN);
    else if (strcmp(txt, "PMKID") == 0) attacks_start(ATTACK_WIFI_PMKID);
    else if (strcmp(txt, "PMKID Cap") == 0) PMKIDCapture::start();
    else if (strcmp(txt, "WPS Pixie") == 0) {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) {
            WPSAttacks::startPixieDust(WPSAttacks::getNetwork(0));
        }
    }
    else if (strcmp(txt, "WPS Brute") == 0) {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) {
            WPSAttacks::startBruteForce(WPSAttacks::getNetwork(0));
        }
    }
    else if (strcmp(txt, "Wardrive") == 0) attacks_start(ATTACK_WIFI_WARDRIVE);
    // RF
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
    // USB
    else if (strcmp(txt, "BadUSB") == 0) attacks_start(ATTACK_USB_BADUSB);
    else if (strcmp(txt, "WiFi Exfil") == 0) attacks_start(ATTACK_USB_EXFIL);
    
    update_status_bar();
}

// ============================================================================
// BACK BUTTON
// ============================================================================
static void back_cb(lv_event_t *e) {
    show_main_menu();
}

// ============================================================================
// CREATE HEADER WITH BACK BUTTON
// ============================================================================
static void create_header(lv_obj_t *scr, const char *title) {
    // Back button
    lv_obj_t *back = lv_btn_create(scr);
    lv_obj_set_pos(back, 5, 5);
    lv_obj_set_size(back, 50, 30);
    lv_obj_set_style_bg_color(back, lv_color_hex(COL_RED), 0);
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, NULL);
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

// ============================================================================
// SHOW MAIN MENU
// ============================================================================
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
    
    // Category buttons (2 columns x 3 rows + bottom row)
    int bw = 100, bh = 40, gap = 10;
    int x1 = 10, x2 = 115, x3 = 220;
    int y1 = 50, y2 = 95, y3 = 140, y4 = 195;
    
    // Row 1: BLE, WiFi, RF
    create_menu_btn(scr, "BLE",  x1, y1, bw, bh, COL_BTN, main_menu_cb);
    create_menu_btn(scr, "WiFi", x2, y1, bw, bh, COL_BTN, main_menu_cb);
    create_menu_btn(scr, "RF",   x3, y1, 90, bh, COL_BTN, main_menu_cb);
    
    // Row 2: NFC, IR, USB
    create_menu_btn(scr, "NFC", x1, y2, bw, bh, COL_BTN, main_menu_cb);
    create_menu_btn(scr, "IR",  x2, y2, bw, bh, COL_BTN, main_menu_cb);
    create_menu_btn(scr, "USB", x3, y2, 90, bh, COL_BTN, main_menu_cb);
    
    // Row 3: Settings, Status
    create_menu_btn(scr, "Settings", x1, y3, 145, bh, COL_BTN_SEL, main_menu_cb);
    create_menu_btn(scr, "Status",   165, y3, 145, bh, COL_BTN_SEL, main_menu_cb);
    
    // Row 4: STOP ALL
    create_menu_btn(scr, "STOP", 10, y4, 300, 40, COL_RED, main_menu_cb);
    
    Serial.println("[UI] Main menu loaded");
}

// ============================================================================
// BLE SUBMENU
// ============================================================================
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
    create_menu_btn(scr, "Sour Apple", 210, y,     100, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Swift Pair", 10, y+50,   95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Fast Pair",  110, y+50,  95, 40, COL_BTN, attack_cb);
    create_menu_btn(scr, "Samsung",    210, y+50,  100, 40, COL_BTN, attack_cb);
}

// ============================================================================
// WIFI SUBMENU
// ============================================================================
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
    create_menu_btn(scr, "Evil Twin",   160, y,    70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "Wardrive",    235, y,    75, 35, COL_BTN, attack_cb);
    
    create_menu_btn(scr, "PMKID",       10, y+40,  70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "PMKID Cap",   85, y+40,  70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "WPS Pixie",   160, y+40, 70, 35, COL_BTN, attack_cb);
    create_menu_btn(scr, "WPS Brute",   235, y+40, 75, 35, COL_BTN, attack_cb);
}

// ============================================================================
// RF SUBMENU (Scrollable)
// ============================================================================
static void show_rf_menu() {
    current_screen = SCREEN_RF;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "RF SubGHz");
    
    // Create scrollable container
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
    create_menu_btn(cont, "Jammer Smart",  150, y, 145, bh, COL_BTN, attack_cb);
    y += bh + gap;
    
    // Capture/Replay
    create_menu_btn(cont, "Capture", 0, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Replay", 100, y, 95, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Ghost", 200, y, 95, bh, COL_BTN, attack_cb);
    y += bh + gap;
    
    create_menu_btn(cont, "Brute Force", 0, y, 145, bh, COL_BTN, attack_cb);
    create_menu_btn(cont, "Spectrum", 150, y, 145, bh, COL_BTN, attack_cb);
}

// ============================================================================
// NFC SUBMENU
// ============================================================================
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
    
    // NFC Status
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

// ============================================================================
// IR SUBMENU
// ============================================================================
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
    
    // Gate brute
    create_menu_btn(scr, "Gate Brute", 10, y+45, 145, 40, COL_BTN, attack_cb);
    
    // Info
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "YS-IRTM: NEC/RC5/Sony");
    lv_obj_set_style_text_color(info, lv_color_hex(COL_GREEN), 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ============================================================================
// USB SUBMENU
// ============================================================================
static void show_usb_menu() {
    current_screen = SCREEN_USB;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "USB Attacks");
    
    int y = 60;
    create_menu_btn(scr, "BadUSB",     30, y,   120, 50, COL_BTN, attack_cb);
    create_menu_btn(scr, "WiFi Exfil", 170, y,  120, 50, COL_BTN, attack_cb);
    
    lv_obj_t *note = lv_label_create(scr);
    lv_label_set_text(note, "Connect USB to target PC");
    lv_obj_set_style_text_color(note, lv_color_hex(COL_YELLOW), 0);
    lv_obj_align(note, LV_ALIGN_BOTTOM_MID, 0, -20);
}

// ============================================================================
// SETTINGS MENU
// ============================================================================
static void show_settings_menu() {
    current_screen = SCREEN_SETTINGS;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Settings");
    
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, 
        "WiFi AP: Monster_S3\n"
        "Password: lele2025\n"
        "Dashboard: 192.168.4.1\n\n"
        "Gestures: Enabled\n"
        "Touch: Calibrated");
    lv_obj_set_style_text_color(info, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 20);
}

// ============================================================================
// STATUS SCREEN
// ============================================================================
static void show_status_screen() {
    current_screen = SCREEN_STATUS;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "System Status");
    
    char statusText[256];
    snprintf(statusText, sizeof(statusText),
        "Heap: %d KB\n"
        "PSRAM: %d KB\n"
        "Uptime: %d min\n\n"
        "Attack: %s\n"
        "GPS: %s\n"
        "NFC: %s",
        ESP.getFreeHeap() / 1024,
        ESP.getFreePsram() / 1024,
        millis() / 60000,
        attacks_get_name(attacks_get_current()),
        GPSDriver::isValid() ? "Valid" : "No Fix",
        NFCDriver::isReady() ? "Ready" : "N/A"
    );
    
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, statusText);
    lv_obj_set_style_text_color(info, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 20);
}

// ============================================================================
// UPDATE STATUS BAR
// ============================================================================
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

// ============================================================================
// INITIALIZATION
// ============================================================================
void setup_lvgl_menu() {
    Serial.println("[DISPLAY] Setting up TFT + LVGL...");
    
    // Initialize SPI for display
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
    tft.setRotation(1);  // Landscape
    tft.fillScreen(TFT_BLACK);
    tft.setTouch(touchCalData);
    
    Serial.println("[DISPLAY] TFT initialized");
    
    // LVGL Init
    lv_init();
    
    // Allocate buffer in PSRAM
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
    
    // Gesture driver
    main_group = lv_group_create();
    static lv_indev_drv_t gesture_drv;
    lv_indev_drv_init(&gesture_drv);
    gesture_drv.type = LV_INDEV_TYPE_KEYPAD;
    gesture_drv.read_cb = gesture_keypad_read;
    gesture_indev = lv_indev_drv_register(&gesture_drv);
    lv_indev_set_group(gesture_indev, main_group);
    
    Serial.println("[LVGL] Input devices configured");
    
    // Create UI
    show_main_menu();
    
    Serial.println("[DISPLAY] Setup complete!");
}

void lvgl_loop() {
    lv_task_handler();
}

lv_group_t* lvgl_get_main_group() {
    return main_group;
}
