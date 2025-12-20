/**
 * @file lvgl_menu.cpp
 * @brief LVGL Graphical User Interface
 * 
 * Implements the touch-screen interface using LVGL.
 * Manages menus, gesture inputs, and screen navigation for all modules.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "lvgl_menu.h"
#include "pin_config.h"
#include "attacks_manager.h"
#include "gesture_sensor.h"
#include "manual_menu.h" // Added manual_menu.h
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config_manager.h"

// TFT_eSPI instance (includes touch support)
TFT_eSPI tft = TFT_eSPI();

// Screen dimensions
#define SCR_W TFT_WIDTH   // 240
#define SCR_H TFT_HEIGHT  // 320

// LVGL Buffers
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

// LVGL Input Device for Gestures
static lv_indev_t *gesture_indev = NULL;
static lv_group_t *main_group = NULL;
static volatile uint32_t last_gesture_key = 0;

// Screen stack for navigation
static lv_obj_t *main_screen = NULL;
static lv_obj_t *current_screen = NULL;

// Touch calibration for MSP2402
#define TOUCH_X_MIN 200
#define TOUCH_X_MAX 3900
#define TOUCH_Y_MIN 200
#define TOUCH_Y_MAX 3900

// ========================================
// Color Definitions
// ========================================
#define COLOR_BG        lv_color_hex(0x1a1a2e)
#define COLOR_PANEL     lv_color_hex(0x16213e)
#define COLOR_TITLE     lv_color_hex(0xe94560)
#define COLOR_BLE       lv_color_hex(0x0077b6)
#define COLOR_WIFI      lv_color_hex(0x00b4d8)
#define COLOR_RF        lv_color_hex(0xf77f00)
#define COLOR_NFC       lv_color_hex(0x9d4edd)
#define COLOR_IR        lv_color_hex(0xffbe0b)
#define COLOR_USB       lv_color_hex(0x38b000)
#define COLOR_DANGER    lv_color_hex(0xe94560)
#define COLOR_SUCCESS   lv_color_hex(0x00ff00)

// ========================================
// LVGL Display Flush Callback
// ========================================
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)color_p, w * h, true);
    tft.endWrite();
    
    lv_disp_flush_ready(disp);
}

// ========================================
// LVGL Touch Read Callback
// ========================================
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t tx, ty;
    
    if(tft.getTouch(&tx, &ty)) {
        data->point.x = tx;
        data->point.y = ty;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ========================================
// LVGL Gesture Keypad Read Callback
// ========================================
void gesture_keypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (last_gesture_key != 0) {
        data->key = last_gesture_key;
        data->state = LV_INDEV_STATE_PR;
        last_gesture_key = 0;
    } else {
        data->state = LV_INDEV_STATE_REL;
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
        case ACTION_SPECIAL_1:  return LV_KEY_NEXT;
        case ACTION_SPECIAL_2:  return LV_KEY_PREV;
        default:                return 0;
    }
}

// ========================================
// Forward Declarations
// ========================================
// ========================================
// Forward Declarations
// ========================================
/** @brief Show Main Menu Screen */
void show_main_menu();
/** @brief Show BLE Attacks Menu */
void show_ble_menu();
/** @brief Show WiFi Attacks Menu */
void show_wifi_menu();
/** @brief Show RF SubGHz Menu */
void show_rf_menu();
/** @brief Show RF Jammer Menu */
void show_rf_jammer_menu();
/** @brief Show RF Tools Menu */
void show_rf_tools_menu();
/** @brief Show NFC Attacks Menu */
void show_nfc_menu();
/** @brief Show IR Attacks Menu */
void show_ir_menu();
/** @brief Show USB Attacks Menu */
void show_usb_menu();
/** @brief Show NRF24 Attacks Menu */
void show_nrf_menu();
/** @brief Show Settings Menu */
void show_settings_menu();
/** @brief Show Help/Manual Screen */
void show_help_menu();

// ========================================
// Attack Event Handlers
// ========================================
static void attack_event_cb(lv_event_t *e) {
    AttackType type = (AttackType)(intptr_t)lv_event_get_user_data(e);
    attacks_start(type);
    Serial.printf("[UI] Attack started: %s\n", attacks_get_name(type));
}

static void stop_event_cb(lv_event_t *e) {
    attacks_stop();
    Serial.println("[UI] All attacks stopped");
}

static void back_event_cb(lv_event_t *e) {
    show_main_menu();
}

// Category navigation callbacks
static void nav_ble_cb(lv_event_t *e)  { show_ble_menu(); }
static void nav_wifi_cb(lv_event_t *e) { show_wifi_menu(); }
static void nav_rf_cb(lv_event_t *e)   { show_rf_menu(); }
static void nav_nfc_cb(lv_event_t *e)  { show_nfc_menu(); }
static void nav_ir_cb(lv_event_t *e)   { show_ir_menu(); }
static void nav_usb_cb(lv_event_t *e)  { show_usb_menu(); }
static void nav_nrf_cb(lv_event_t *e)  { show_nrf_menu(); }
static void nav_rf_jammer_cb(lv_event_t *e) { show_rf_jammer_menu(); }
static void nav_rf_jammer_cb(lv_event_t *e) { show_rf_jammer_menu(); }
static void nav_rf_tools_cb(lv_event_t *e)  { show_rf_tools_menu(); }
static void nav_rf_tools_cb(lv_event_t *e)  { show_rf_tools_menu(); }
static void nav_settings_cb(lv_event_t *e)  { show_settings_menu(); }
static void nav_help_cb(lv_event_t *e)      { show_help_menu(); }

// ========================================
// UI Helper Functions
// ========================================
lv_obj_t* create_screen_base(const char* title, lv_color_t title_color) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    
    // Title
    lv_obj_t *lbl = lv_label_create(scr);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_text_color(lbl, title_color, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 8);
    
    return scr;
}

/**
 * @brief Displays the Help/Manual Screen
 */
// ========================================
// HELP MENU
// ========================================
/**
 * @brief Displays the Help/Manual Screen
 * Validates documentation and displays quick reference guide
 */
void show_help_menu() {
    lv_obj_t *scr = create_screen_base("❓ MANUAL / HELP", lv_color_hex(0x222222));
    
    // Create a text area for scrolling content
    lv_obj_t *ta = lv_textarea_create(scr);
    lv_obj_set_size(ta, 220, 220);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_readonly(ta, true);
    lv_obj_set_style_text_font(ta, &lv_font_montserrat_10, 0);
    lv_obj_set_style_bg_color(ta,  lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_color(ta, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_border_width(ta, 1, 0);
    
    const char* help_text = 
        "=== BLE ===\n"
        "Spam BR: Brazillian names\n"
        "Sour/Swift/Fast: Pairing spam\n\n"
        "=== WiFi ===\n"
        "Deauth: Kick clients\n"
        "Beacon: Fake networks\n"
        "Evil Twin: Clone AP\n"
        "PMKID: Capture hash\n\n"
        "=== RF SubGHz ===\n"
        "Jammer: Block remote control\n"
        "Rolljam: Jam+Capture\n"
        "Replay: Send recorded signal\n\n"
        "=== NFC ===\n"
        "Clone: Copy Cards\n"
        "Fault: Glitch Readers\n\n"
        "=== USB/NRF ===\n"
        "BadUSB: Keyboard Inject\n"
        "WAX: NTLM Hash Grab\n"
        "MouseJack: NRF Inject\n\n"
        "=== HW ===\n"
        "Glitcher: Voltage Fault\n";

    lv_textarea_set_text(ta, help_text);
    
    create_back_btn(scr);
    switch_screen(scr);
}

/**
 * @brief Create a standardized menu button
 * @param parent Parent object
 * @param text Button label
 * @param x X Position
 * @param y Y Position
 * @param w Width
 * @param h Height
 * @param color Button background color
 * @param cb Event callback
 * @param user_data User data pointer
 * @return lv_obj_t* Pointer to the button object
 */
lv_obj_t* create_menu_btn(lv_obj_t *parent, const char *text, lv_coord_t x, lv_coord_t y, 
                          lv_coord_t w, lv_coord_t h, lv_color_t color, 
                          lv_event_cb_t cb, void* user_data) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    
    if (cb) {
        lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, user_data);
    }
    
    // Focus style for gesture navigation
    lv_obj_set_style_outline_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btn, COLOR_SUCCESS, LV_STATE_FOCUSED);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_center(label);
    
    if (main_group) {
        lv_group_add_obj(main_group, btn);
    }
    
    return btn;
}

/**
 * @brief Create an attack trigger button
 * @param parent Parent object
 * @param text Button label
 * @param x X Position
 * @param y Y Position
 * @param w Width
 * @param h Height
 * @param color Button color
 * @param attack AttackType enum to trigger
 * @return lv_obj_t* Pointer to the button object
 */
lv_obj_t* create_attack_btn(lv_obj_t *parent, const char *text, lv_coord_t x, lv_coord_t y,
                            lv_coord_t w, lv_coord_t h, lv_color_t color, AttackType attack) {
    return create_menu_btn(parent, text, x, y, w, h, color, attack_event_cb, (void*)(intptr_t)attack);
}

lv_obj_t* create_back_btn(lv_obj_t *parent) {
    return create_menu_btn(parent, "< VOLTAR", 15, 275, 210, 35, COLOR_PANEL, back_event_cb, NULL);
}

lv_obj_t* create_stop_btn(lv_obj_t *parent, lv_coord_t y) {
    lv_obj_t *btn = create_menu_btn(parent, "STOP ALL", 15, y, 210, 40, COLOR_DANGER, stop_event_cb, NULL);
    return btn;
}

void switch_screen(lv_obj_t *new_scr) {
    if (current_screen && current_screen != main_screen) {
        lv_obj_del(current_screen);
    }
    current_screen = new_scr;
    lv_scr_load(new_scr);
    
    // Reset focus group
    lv_group_remove_all_objs(main_group);
}

// ========================================
// MAIN MENU - Categories
// ========================================
// ========================================
// MAIN MENU - Categories
// ========================================
/**
 * @brief Show the main dashboard menu
 */
void show_main_menu() {
    lv_obj_t *scr = create_screen_base("MONSTER S3", COLOR_TITLE);
    
    // Subtitle
    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "Pentest Toolkit v2.0");
    lv_obj_set_style_text_color(sub, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_10, 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 30);
    
    // Category Buttons (2 columns)
    int y = 50;
    int h = 38;
    int gap = 5;
    
    create_menu_btn(scr, "📶 BLE",      15,  y, 100, h, COLOR_BLE,  nav_ble_cb, NULL);
    create_menu_btn(scr, "📡 WiFi",    125,  y, 100, h, COLOR_WIFI, nav_wifi_cb, NULL);
    y += h + gap;
    
    create_menu_btn(scr, "📻 RF/SubGHz", 15, y, 100, h, COLOR_RF,   nav_rf_cb, NULL);
    create_menu_btn(scr, "💳 NFC",     125,  y, 100, h, COLOR_NFC,  nav_nfc_cb, NULL);
    y += h + gap;
    
    create_menu_btn(scr, "📺 IR",       15,  y, 100, h, COLOR_IR,   nav_ir_cb, NULL);
    create_menu_btn(scr, "💻 USB",     125,  y, 100, h, COLOR_USB,  nav_usb_cb, NULL);
    y += h + gap;

    create_menu_btn(scr, "🖱️ NRF24",    15,  y, 210, h, COLOR_RF,   nav_nrf_cb, NULL);
    y += h + gap;
    
    create_menu_btn(scr, "⚙️ Config",   15,  y, 100, h, lv_color_hex(0x666666),  nav_settings_cb, NULL);
    create_menu_btn(scr, "❓ Help",     125, y, 100, h, lv_color_hex(0x444444),  nav_help_cb, NULL);
    y += h + gap;
    
    // Status area
    lv_obj_t *status = lv_label_create(scr);
    lv_label_set_text(status, "Swipe/Touch para navegar");
    lv_obj_set_style_text_color(status, lv_color_hex(0x444444), 0);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_10, 0);
    lv_obj_align(status, LV_ALIGN_BOTTOM_MID, 0, -50);
    
    // Stop All button
    create_stop_btn(scr, 270);
    
    switch_screen(scr);
    main_screen = scr;
}

// ========================================
// BLE MENU - 4 Attacks
// ========================================
// ========================================
// BLE MENU - 4 Attacks
// ========================================
/**
 * @brief Show BLE Attacks Menu
 */
void show_ble_menu() {
    lv_obj_t *scr = create_screen_base("📶 BLE ATTACKS", COLOR_BLE);
    
    int y = 45;
    int h = 42;
    int gap = 5;
    
    create_attack_btn(scr, "Spam BR (Nomes)", 15, y, 210, h, COLOR_BLE, ATTACK_BLE_SPAM);
    y += h + gap;
    
    create_attack_btn(scr, "🍎 Sour Apple (iOS)", 15, y, 210, h, COLOR_BLE, ATTACK_BLE_SOUR_APPLE);
    y += h + gap;
    
    create_attack_btn(scr, "🪟 Swift Pair (Win)", 15, y, 210, h, COLOR_BLE, ATTACK_BLE_SWIFT_PAIR);
    y += h + gap;
    
    create_attack_btn(scr, "🤖 Fast Pair (Android)", 15, y, 210, h, COLOR_BLE, ATTACK_BLE_FAST_PAIR);
    y += h + gap + 10;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// WIFI MENU - 5 Attacks
// ========================================
// ========================================
// WIFI MENU - 5 Attacks
// ========================================
/**
 * @brief Show WiFi Attacks Menu
 */
void show_wifi_menu() {
    lv_obj_t *scr = create_screen_base("📡 WiFi ATTACKS", COLOR_WIFI);
    
    int y = 45;
    int h = 38;
    int gap = 4;
    
    create_attack_btn(scr, "⚡ Deauth All", 15, y, 210, h, COLOR_WIFI, ATTACK_WIFI_DEAUTH);
    y += h + gap;
    
    create_attack_btn(scr, "🌊 Beacon Spam", 15, y, 210, h, COLOR_WIFI, ATTACK_WIFI_BEACON_SPAM);
    y += h + gap;
    
    create_attack_btn(scr, "👯 Evil Twin", 15, y, 210, h, COLOR_WIFI, ATTACK_WIFI_EVIL_TWIN);
    y += h + gap;
    
    create_attack_btn(scr, "🔐 PMKID Capture", 15, y, 210, h, COLOR_WIFI, ATTACK_WIFI_PMKID);
    y += h + gap;
    
    create_attack_btn(scr, "🗺️ Wardriving GPS", 15, y, 210, h, COLOR_WIFI, ATTACK_WIFI_WARDRIVE);
    y += h + gap + 5;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// RF MENU - Main Categories
// ========================================
// ========================================
// RF MENU - Main Categories
// ========================================
/**
 * @brief Show RF SubGHz Main Menu
 */
void show_rf_menu() {
    lv_obj_t *scr = create_screen_base("📻 RF SubGHz", COLOR_RF);
    
    int y = 50;
    int h = 50;
    int gap = 8;
    
    create_menu_btn(scr, "📢 JAMMERS", 15, y, 210, h, COLOR_RF, nav_rf_jammer_cb, NULL);
    y += h + gap;
    
    create_menu_btn(scr, "🛠️ TOOLS", 15, y, 210, h, COLOR_RF, nav_rf_tools_cb, NULL);
    y += h + gap;
    
    // Quick actions
    lv_obj_t *lbl = lv_label_create(scr);
    lv_label_set_text(lbl, "Quick Actions:");
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x888888), 0);
    lv_obj_set_pos(lbl, 15, y);
    y += 20;
    
    create_attack_btn(scr, "📡 Capture", 15, y, 100, 35, COLOR_RF, ATTACK_RF_CAPTURE);
    create_attack_btn(scr, "🔁 Replay",  125, y, 100, 35, COLOR_RF, ATTACK_RF_REPLAY);
    
    create_stop_btn(scr, 225);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// RF JAMMER SUBMENU - 5 Jammers
// ========================================
// ========================================
// RF JAMMER SUBMENU - 5 Jammers
// ========================================
/**
 * @brief Show RF Jammer Selection Menu
 */
void show_rf_jammer_menu() {
    lv_obj_t *scr = create_screen_base("📢 RF JAMMERS", COLOR_RF);
    
    int y = 45;
    int h = 36;
    int gap = 4;
    
    create_attack_btn(scr, "433 MHz", 15, y, 100, h, COLOR_RF, ATTACK_RF_JAMMER_433);
    create_attack_btn(scr, "315 MHz", 125, y, 100, h, COLOR_RF, ATTACK_RF_JAMMER_315);
    y += h + gap;
    
    create_attack_btn(scr, "868 MHz", 15, y, 100, h, COLOR_RF, ATTACK_RF_JAMMER_868);
    create_attack_btn(scr, "Burst", 125, y, 100, h, COLOR_RF, ATTACK_RF_JAMMER_BURST);
    y += h + gap;
    
    create_attack_btn(scr, "🧠 Smart Jammer", 15, y, 210, h, COLOR_RF, ATTACK_RF_JAMMER_SMART);
    y += h + gap + 10;
    
    create_stop_btn(scr, y);
    
    // Back to RF menu
    create_menu_btn(scr, "< RF Menu", 15, 275, 210, 35, COLOR_PANEL, nav_rf_cb, NULL);
    
    switch_screen(scr);
}

// ========================================
// RF TOOLS SUBMENU - Capture/Replay/Brute
// ========================================
// ========================================
// RF TOOLS SUBMENU - Capture/Replay/Brute
// ========================================
/**
 * @brief Show RF Tools Menu
 */
void show_rf_tools_menu() {
    lv_obj_t *scr = create_screen_base("🛠️ RF TOOLS", COLOR_RF);
    
    int y = 45;
    int h = 34;
    int gap = 4;
    
    create_attack_btn(scr, "📡 Scan/Capture", 15, y, 210, h, COLOR_RF, ATTACK_RF_CAPTURE);
    y += h + gap;
    
    create_attack_btn(scr, "🔁 Replay Last", 15, y, 210, h, COLOR_RF, ATTACK_RF_REPLAY);
    y += h + gap;
    
    create_attack_btn(scr, "👻 Ghost Replay", 15, y, 210, h, COLOR_RF, ATTACK_RF_GHOST_REPLAY);
    y += h + gap;
    
    create_attack_btn(scr, "🔓 Brute Force", 15, y, 100, h, COLOR_RF, ATTACK_RF_BRUTE_FORCE);
    create_attack_btn(scr, "De Bruijn", 125, y, 100, h, COLOR_RF, ATTACK_RF_DEBRUIJN);
    y += h + gap;
    
    create_attack_btn(scr, "📊 Spectrum Scan", 15, y, 210, h, COLOR_RF, ATTACK_RF_SPECTRUM);
    y += h + gap;

    // Advanced Car Attacks
    create_attack_btn(scr, "🚗 Rolljam", 15, y, 100, h, COLOR_RF, ATTACK_RF_ROLLJAM);
    create_attack_btn(scr, "🔙 Rollback", 125, y, 100, h, COLOR_RF, ATTACK_RF_ROLLBACK);
    y += h + gap + 5;
    
    create_stop_btn(scr, y);
    create_menu_btn(scr, "< RF Menu", 15, 275, 210, 35, COLOR_PANEL, nav_rf_cb, NULL);
    
    switch_screen(scr);
}

// ========================================
// NFC MENU - 3 Attacks
// ========================================
// ========================================
// NFC MENU - 3 Attacks
// ========================================
/**
 * @brief Show NFC Attacks Menu
 */
void show_nfc_menu() {
    lv_obj_t *scr = create_screen_base("💳 NFC ATTACKS", COLOR_NFC);
    
    int y = 55;
    int h = 50;
    int gap = 8;
    
    create_attack_btn(scr, "📋 Clone Card", 15, y, 210, h, COLOR_NFC, ATTACK_NFC_CLONE);
    y += h + gap;
    
    create_attack_btn(scr, "⚡ Fault Injection", 15, y, 210, h, COLOR_NFC, ATTACK_NFC_FAULT);
    y += h + gap;
    
    create_attack_btn(scr, "🎣 Phishing URL", 15, y, 210, h, COLOR_NFC, ATTACK_NFC_PHISHING);
    y += h + gap + 5;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// IR MENU - 3 Attacks
// ========================================
// ========================================
// IR MENU - 3 Attacks
// ========================================
/**
 * @brief Show IR Attacks Menu
 */
void show_ir_menu() {
    lv_obj_t *scr = create_screen_base("📺 IR ATTACKS", COLOR_IR);
    
    int y = 55;
    int h = 50;
    int gap = 8;
    
    create_attack_btn(scr, "📺 TV-B-Gone", 15, y, 210, h, COLOR_IR, ATTACK_IR_TV_OFF);
    y += h + gap;
    
    create_attack_btn(scr, "🔓 Brute Force", 15, y, 210, h, COLOR_IR, ATTACK_IR_BRUTE);
    y += h + gap;
    
    create_attack_btn(scr, "📖 Clone Remote", 15, y, 210, h, COLOR_IR, ATTACK_IR_CLONE);
    y += h + gap + 5;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// USB MENU - 2 Attacks
// ========================================
// ========================================
// USB MENU - 2 Attacks
// ========================================
/**
 * @brief Show USB Attacks Menu
 */
void show_usb_menu() {
    lv_obj_t *scr = create_screen_base("💻 USB ATTACKS", COLOR_USB);
    
    int y = 70;
    int h = 55;
    int gap = 12;
    
    create_attack_btn(scr, "⌨️ BadUSB HID", 15, y, 210, h, COLOR_USB, ATTACK_USB_BADUSB);
    y += h + gap;
    
    create_attack_btn(scr, "📤 Data Exfil", 15, y, 210, h, COLOR_USB, ATTACK_USB_EXFIL);
    y += h + gap;

    create_attack_btn(scr, "🕯️ WAX Payload", 15, y, 210, h, COLOR_USB, ATTACK_USB_WAX);
    y += h + gap;

    create_attack_btn(scr, "🐍 PoshC2 Payload", 15, y, 210, h, COLOR_USB, ATTACK_POSHC2);
    y += h + gap + 10;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// NRF24 MENU - 2 Attacks
// ========================================
// ========================================
// NRF24 MENU - 2 Attacks
// ========================================
/**
 * @brief Show NRF24 Attacks Menu
 */
void show_nrf_menu() {
    lv_obj_t *scr = create_screen_base("🖱️ NRF24 ATTACKS", COLOR_RF);
    
    int y = 70;
    int h = 55;
    int gap = 12;
    
    create_attack_btn(scr, "🖱️ Mousejack", 15, y, 210, h, COLOR_RF, ATTACK_NRF_MOUSEJACK);
    y += h + gap;
    
    create_attack_btn(scr, "⌨️ Key Sniffer", 15, y, 210, h, COLOR_RF, ATTACK_NRF_KEYBOARD);
    y += h + gap + 10;
    
    create_stop_btn(scr, y);
    create_back_btn(scr);
    
    switch_screen(scr);
}

// ========================================
// SETTINGS MENU
// ========================================

static void config_save_event_cb(lv_event_t *e) {
    if (config_save()) {
        Serial.println("[UI] Settings saved");
    }
}

static void config_reset_event_cb(lv_event_t *e) {
    config_reset_defaults();
    show_settings_menu(); // Reload UI
    Serial.println("[UI] Settings reset to defaults");
}

static void reboot_event_cb(lv_event_t *e) {
    esp_restart();
}

static void toggle_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool *param = (bool*)lv_event_get_user_data(e);
    if (param) {
        *param = lv_obj_has_state(sw, LV_STATE_CHECKED);
        config_apply(); // Apply immediately (e.g. brightness)
        config_set_dirty();
    }
}

static void slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int *param = (int*)lv_event_get_user_data(e);
    if (param) {
        *param = (int)lv_slider_get_value(slider);
        config_apply();
        config_set_dirty();
    }
}

static lv_obj_t* create_switch(lv_obj_t *parent, const char *label, bool *val_ptr) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 210, 40);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 5, 0);
    
    lv_obj_t *lbl = lv_label_create(cont);
    lv_label_set_text(lbl, label);
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    
    lv_obj_t *sw = lv_switch_create(cont);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -5, 0);
    if (*val_ptr) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw, toggle_event_cb, LV_EVENT_VALUE_CHANGED, val_ptr);
    
    // Add to focus group
    if (main_group) lv_group_add_obj(main_group, sw);
    
    return cont;
}

static lv_obj_t* create_slider_row(lv_obj_t *parent, const char *label, int min, int max, int *val_ptr) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 210, 60);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 5, 0);
    
    lv_obj_t *lbl = lv_label_create(cont);
    lv_label_set_text(lbl, label);
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    
    lv_obj_t *slider = lv_slider_create(cont);
    lv_obj_set_width(slider, 190);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, *val_ptr, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, val_ptr);
    
    if (main_group) lv_group_add_obj(main_group, slider);
    
    return cont;
}

/**
 * @brief Show System Settings Menu
 * Configure brightness, timeout, toggles, etc.
 */
void show_settings_menu() {
    lv_obj_t *scr = create_screen_base("⚙️ SETTINGS", lv_color_hex(0x666666));
    
    // Scrollable container
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 240, 230);
    lv_obj_set_pos(cont, 0, 40);
    lv_obj_set_style_bg_color(cont, COLOR_BG, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Get config handle
    DeviceConfig *cfg = config_get_mutable();
    
    // Toggles
    create_switch(cont, "WiFi Enabled", &cfg->wifi_enabled);
    create_switch(cont, "BLE Enabled", &cfg->ble_enabled);
    create_switch(cont, "Auto Sleep", &cfg->auto_sleep_enabled);
    
    // Sliders
    create_slider_row(cont, "Display Brightness", 10, 255, &cfg->display_brightness);
    create_slider_row(cont, "Standby Time (s)", 30, 600, &cfg->sleep_timeout_sec);
    
    // Actions
    lv_obj_t *btn_save = lv_btn_create(cont);
    lv_obj_set_size(btn_save, 210, 35);
    lv_obj_set_style_bg_color(btn_save, COLOR_SUCCESS, 0);
    lv_obj_add_event_cb(btn_save, config_save_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "💾 SAVE SETTINGS");
    lv_obj_center(lbl_save);
    if (main_group) lv_group_add_obj(main_group, btn_save);
    
    lv_obj_t *btn_reset = lv_btn_create(cont);
    lv_obj_set_size(btn_reset, 210, 35);
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0xFF8800), 0);
    lv_obj_add_event_cb(btn_reset, config_reset_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "🔄 RESET DEFAULTS");
    lv_obj_center(lbl_reset);
    if (main_group) lv_group_add_obj(main_group, btn_reset);
    
    lv_obj_t *btn_reboot = lv_btn_create(cont);
    lv_obj_set_size(btn_reboot, 210, 35);
    lv_obj_set_style_bg_color(btn_reboot, COLOR_DANGER, 0);
    lv_obj_add_event_cb(btn_reboot, reboot_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl_reboot = lv_label_create(btn_reboot);
    lv_label_set_text(lbl_reboot, "🔴 REBOOT DEVICE");
    lv_obj_center(lbl_reboot);
    if (main_group) lv_group_add_obj(main_group, btn_reboot);
    
    // System Info
    lv_obj_t *info = lv_label_create(cont);
    String info_txt = "RAM: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
    info_txt += "Freq: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
    info_txt += "MAC: " + WiFi.macAddress();
    lv_label_set_text(info, info_txt.c_str());
    lv_obj_set_style_text_color(info, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_10, 0);
    
    switch_screen(scr);
}

// ========================================
// Public API
// ========================================
// ========================================
// Public API
// ========================================
/**
 * @brief Initialize LVGL, Display, and Touch Driver
 * Sets up the GUI environment and loads the main menu
 */
void setup_lvgl_menu() {
    Serial.println("[DISPLAY] Initializing TFT_eSPI...");
    
    // Initialize TFT
    tft.init();
    tft.setRotation(0); // Portrait
    tft.fillScreen(TFT_BLACK);
    
    // Backlight ON
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
    
    Serial.println("[DISPLAY] ILI9341 initialized (240x320)");
    
    // Touch calibration
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
    Serial.println("[TOUCH] XPT2046 calibrated");
    
    // Initialize LVGL
    lv_init();
    
    // Create default focus group for gesture navigation
    main_group = lv_group_create();
    lv_group_set_default(main_group);
    
    // Allocate buffer in PSRAM (40 lines)
    size_t buf_size = SCR_W * 40;
    buf = (lv_color_t*) ps_malloc(buf_size * sizeof(lv_color_t));
    if(!buf) {
        Serial.println("[LVGL] PSRAM failed, using heap");
        buf = (lv_color_t*) malloc(buf_size * sizeof(lv_color_t));
    }
    
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, buf_size);
    
    // Register display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Register touch driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);
    
    // Register gesture keypad driver
    static lv_indev_drv_t gesture_drv;
    lv_indev_drv_init(&gesture_drv);
    gesture_drv.type = LV_INDEV_TYPE_KEYPAD;
    gesture_drv.read_cb = gesture_keypad_read;
    gesture_indev = lv_indev_drv_register(&gesture_drv);
    
    // Assign gesture input to the focus group
    lv_indev_set_group(gesture_indev, main_group);
    Serial.println("[GESTURE] LVGL keypad input registered");
    
    // Create Main Menu UI
    show_main_menu();
    
    Serial.println("[LVGL] Complete Menu System Ready - 29 Attacks Available!");
}

void lvgl_loop() {
    lv_task_handler();
}

lv_group_t* lvgl_get_main_group() {
    return main_group;
}
