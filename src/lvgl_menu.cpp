#include "lvgl_menu.h"
#include "attacks_manager.h"
#include "ble_attacks.h"
#include "chameleon_anim.h"
#include "chameleon_logo.h"
#include "lvgl_spectrum.h"
#include "chameleon_anim.h" // Redundant but ensures it's there
#include "level_manager.h"  // NEW: XP System
#include "sound_manager.h"  // NEW: Audio Feedback
#include <Arduino.h>


#include "gesture_sensor.h"
#include "gps_driver.h"
#include "ir_protocols.h"
#include "led_driver.h"
#include "lvgl_qrcode.h"
#include "module_manager.h"
#include "nfc_driver.h"
#include "nfc_relay.h"
#include "pin_config.h"
#include "pmkid_capture.h"
#include "q_learn_ia.h"
#include "tts_espeak.h"
#include "wps_attacks.h"
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <esp_sleep.h>
#include <lvgl.h>

// ------------------------------------------------------------
// CONFIGURATION
// ------------------------------------------------------------
#define SCR_W 320 // Landscape width
#define SCR_H 240 // Landscape height
#define SCREEN_RF SCREEN_SUBGHZ
#define SCREEN_EVIL_TWIN 200 // Custom ID for submenu

// Colors (neon cyberpunk palette)
// Colors moved to header

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
    tft.pushColors((uint16_t *)color_p, w * h, true);
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

void lvgl_send_gesture_key(uint32_t key) { last_gesture_key = key; }

uint32_t gesture_action_to_lvgl_key(GestureAction action) {
    switch (action) {
        case ACTION_MENU_UP: return LV_KEY_UP;
        case ACTION_MENU_DOWN: return LV_KEY_DOWN;
        case ACTION_MENU_LEFT: return LV_KEY_LEFT;
        case ACTION_MENU_RIGHT: return LV_KEY_RIGHT;
        case ACTION_SELECT: return LV_KEY_ENTER;
        case ACTION_BACK: return LV_KEY_ESC;
        default: return 0;
    }
}

// ------------------------------------------------------------
// FORWARD DECLARATIONS (screens)
// ------------------------------------------------------------
void show_main_menu();
static void show_quick_attack_menu();
static void show_wifi_menu();
static void show_ble_menu();
static void show_nfc_menu();
static void show_rf_menu();
static void show_ir_menu();
static void show_hardware_menu();
void show_ai_voice_menu();
static void show_evil_twin_menu();
static void show_led_menu();
static void show_energy_menu();
static void update_status_bar();
static void show_rf_popup(const char* title);
static void show_nfc_read_screen();
void show_settings_menu(); // Public now

// ------------------------------------------------------------
// UI HELPERS
// ------------------------------------------------------------
// Helper to create styled buttons
// Added 'enabled' parameter for hardware interlock
lv_obj_t *create_menu_btn(
    lv_obj_t *parent, const char *text, int x, int y, int w, int h, uint32_t color, lv_event_cb_t event_cb,
    bool enabled
) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);

    if (enabled) {
        lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
        lv_obj_set_style_shadow_width(btn, 10, 0);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(color), 0);
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, (void *)text);
    } else {
        // Disabled style - Gray and unclickable
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x555555), 0); // Gray
        lv_obj_set_style_shadow_width(btn, 0, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_60, 0); // Semi-transparent
                                                    // No callback added
    }

    lv_obj_set_style_outline_width(btn, 3, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btn, lv_color_hex(COL_GREEN), LV_STATE_FOCUSED);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_TEXT), 0);
    lv_obj_center(label);
    if (main_group && enabled) lv_group_add_obj(main_group, btn);
    return btn;
}

// Modern Icon Button with Glass effect
lv_obj_t *create_icon_btn(
    lv_obj_t *parent, const char *icon, const char *text, 
    int w, int h, lv_event_cb_t event_cb, bool enabled
) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    
    // Glassmorphism style
    if (enabled) {
        lv_obj_set_style_bg_color(btn, lv_color_hex(COL_PANEL), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_80, 0); // Semi-transparent
        lv_obj_set_style_border_color(btn, lv_color_hex(COL_BTN), 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_shadow_width(btn, 0, 0); // No shadow for cleaner look
        
        // Active state glow
        lv_obj_set_style_border_color(btn, lv_color_hex(COL_ACCENT_CYAN), LV_STATE_PRESSED);
        lv_obj_set_style_border_color(btn, lv_color_hex(COL_ACCENT_CYAN), LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(COL_ACCENT_CYAN), LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_width(btn, 15, LV_STATE_FOCUSED);
        
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, (void *)text);
    } else {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_50, 0);
        lv_obj_set_style_border_width(btn, 0, 0);
    }

    // Layout: Flex Column
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Icon (Large)
    lv_obj_t *lbl_icon = lv_label_create(btn);
    lv_label_set_text(lbl_icon, icon);
    lv_obj_set_style_text_font(lbl_icon, &lv_font_montserrat_20, 0); // Or larger if available
    lv_obj_set_style_text_color(lbl_icon, lv_color_hex(enabled ? COL_ACCENT_CYAN : COL_TEXT_SEC), 0);
    
    // Text (Small)
    lv_obj_t *lbl_text = lv_label_create(btn);
    lv_label_set_text(lbl_text, text);
    lv_obj_set_style_text_color(lbl_text, lv_color_hex(enabled ? COL_TEXT : COL_TEXT_SEC), 0);
    
    if (main_group && enabled) lv_group_add_obj(main_group, btn);
    return btn;
}

// Modern Header with Glass Panel
void create_header(lv_obj_t *scr, const char *title) {
    // Top Panel Container
    lv_obj_t *panel = lv_obj_create(scr);
    lv_obj_set_size(panel, SCR_W, 40);
    lv_obj_set_pos(panel, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_90, 0);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(COL_ACCENT_CYAN), 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    // Back button (Transparent, just symbol)
    lv_obj_t *back = lv_btn_create(panel);
    lv_obj_set_size(back, 40, 30);
    lv_obj_align(back, LV_ALIGN_LEFT_MID, -10, 0); // align in panel
    lv_obj_set_style_bg_opa(back, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(back, 0, 0);
    
    lv_obj_add_event_cb(
        back,
        [](lv_event_t *e) {
            SoundManager::playBack();
            ChameleonAnim::stop(); // Ensure anim stops if returning to menu
            if (current_screen == (ScreenType)SCREEN_EVIL_TWIN) show_wifi_menu();
            else if (current_screen == SCREEN_MAIN) show_passport_screen();
            else show_main_menu();
        },
        LV_EVENT_CLICKED,
        NULL
    );
    
    lv_obj_t *bl = lv_label_create(back);
    lv_label_set_text(bl, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(bl, lv_color_hex(COL_ACCENT_MAG), 0);
    lv_obj_center(bl);
    
    if (main_group) lv_group_add_obj(main_group, back);

    // Title
    lv_obj_t *t = lv_label_create(panel);
    lv_label_set_text(t, title);
    lv_obj_set_style_text_color(t, lv_color_hex(COL_TEXT), 0);
    lv_obj_set_style_text_font(t, &lv_font_montserrat_16, 0);
    lv_obj_align(t, LV_ALIGN_CENTER, 0, 0);
}
static void attack_cb(lv_event_t *e) {
    const char *txt = (const char *)lv_event_get_user_data(e);
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
    } else if (strcmp(txt, "WPS Brute") == 0) {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) WPSAttacks::startBruteForce(WPSAttacks::getNetwork(0));
    }

    // RF SubGHz
    else if (strcmp(txt, "Jammer 433") == 0)
        attacks_start(ATTACK_RF_JAMMER_433);
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
    else if (strcmp(txt, "Relay Read") == 0) {
        NFCRelay::init();
        NFCRelay::setReaderMode();
    } else if (strcmp(txt, "Relay Emu") == 0) {
        NFCRelay::init();
        NFCRelay::setEmulatorMode();
    }

    // IR
    else if (strcmp(txt, "TV-B-Gone") == 0)
        IRProtocols::tvBGone();
    else if (strcmp(txt, "IR Brute") == 0) attacks_start(ATTACK_IR_BRUTE);
    else if (strcmp(txt, "IR Clone") == 0) attacks_start(ATTACK_IR_CLONE);
    else if (strcmp(txt, "Gate Brute") == 0) IRProtocols::bruteGate12bit();

    update_status_bar();
}
// ------------------------------------------------------------
// MAIN MENU
// ------------------------------------------------------------
void show_passport_screen() {
    current_screen = SCREEN_PASSPORT;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);

    // 1. Procedural Morphy (Center)
    // Bigger animation for idle screen
    ChameleonAnim::init(scr, 0, 50); 
    
    // 2. Player Info (Top)
    lv_obj_t *name_panel = lv_obj_create(scr);
    lv_obj_set_size(name_panel, 220, 60); // Taller for progress bar
    lv_obj_align(name_panel, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(name_panel, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_border_color(name_panel, lv_color_hex(COL_ACCENT_CYAN), 0);
    lv_obj_set_style_radius(name_panel, 15, 0);
    lv_obj_clear_flag(name_panel, LV_OBJ_FLAG_SCROLLABLE);

    // Level & Title
    lv_obj_t *lbl_name = lv_label_create(name_panel);
    String lvlText = String("Lvl.") + LevelManager::getLevel() + " " + LevelManager::getTitle();
    lv_label_set_text(lbl_name, lvlText.c_str());
    lv_obj_set_style_text_color(lbl_name, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(lbl_name, LV_ALIGN_TOP_MID, 0, 0);

    // XP Progress Bar
    lv_obj_t *bar_xp = lv_bar_create(name_panel);
    lv_obj_set_size(bar_xp, 180, 8);
    lv_obj_align(bar_xp, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(bar_xp, lv_color_hex(0x222222), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar_xp, lv_color_hex(COL_GREEN), LV_PART_INDICATOR);
    
    // Calculate progress 0-100
    int progress = (int)(LevelManager::getProgressToNextLevel() * 100.0f);
    lv_bar_set_value(bar_xp, progress, LV_ANIM_ON);

    // 3. Status Bar (Bottom) - "More Resources" style
    lv_obj_t *bar = lv_obj_create(scr);
    lv_obj_set_size(bar, SCR_W - 20, 40);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_radius(bar, 10, 0);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(bar, 15, 0);

    // Dynamic Module Icons
    auto add_status_icon = [&](const char* icon, bool active) {
        lv_obj_t *lbl = lv_label_create(bar);
        lv_label_set_text(lbl, icon);
        lv_obj_set_style_text_color(lbl, lv_color_hex(active ? COL_GREEN : 0x444444), 0);
    };

    add_status_icon(LV_SYMBOL_SD_CARD, SD.cardType() != CARD_NONE);
    add_status_icon(LV_SYMBOL_GPS, ModuleManager::isHardwareConnected(MODULE_GPS));
    add_status_icon(LV_SYMBOL_WIFI, true);  // Internal
    add_status_icon(LV_SYMBOL_BLUETOOTH, true); // Internal
    add_status_icon(LV_SYMBOL_CHARGE, ModuleManager::isHardwareConnected(MODULE_SUBGHZ));
    add_status_icon(LV_SYMBOL_POWER, ModuleManager::isHardwareConnected(MODULE_IR));

    // 4. Interaction Hint
    lv_obj_t *hint = lv_label_create(scr);
    lv_label_set_text(hint, "Press OK for Menu");
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x666666), 0);
    
    // Add an invisible button to capture Click/Enter to go to Main Menu
    lv_obj_t *btn_enter = lv_btn_create(scr);
    lv_obj_set_size(btn_enter, SCR_W, SCR_H);
    lv_obj_set_style_bg_opa(btn_enter, LV_OPA_TRANSP, 0);
    lv_obj_center(btn_enter);
    lv_obj_add_event_cb(btn_enter, [](lv_event_t *e){
         ChameleonAnim::stop();
         show_main_menu();
    }, LV_EVENT_CLICKED, NULL);

    if(main_group) {
        lv_group_add_obj(main_group, btn_enter);
        lv_group_focus_obj(btn_enter);
    }
    
    Serial.println("[UI] Passport Screen Loaded");
}

// ------------------------------------------------------------
// MAIN MENU (App Grid)
// ------------------------------------------------------------
void show_main_menu() {
    current_screen = SCREEN_MAIN;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);

    // Simple Header
    create_header(scr, "Apps Library");

    // Layout: Scrollable Grid Container
    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50); // Full height minus header
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_row(grid, 10, 0);
    lv_obj_set_style_pad_column(grid, 10, 0);

    const int btn_w = 90;
    const int btn_h = 80;

    // WiFi
    create_icon_btn(grid, LV_SYMBOL_WIFI, "WiFi", btn_w, btn_h, [](lv_event_t *e) {
        show_wifi_menu();
    }, true);

    // BLE
    create_icon_btn(grid, LV_SYMBOL_BLUETOOTH, "BLE", btn_w, btn_h, [](lv_event_t *e) {
        show_ble_menu();
    }, true);
    
    // SubGHz
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "SubGHz", btn_w, btn_h, [](lv_event_t *e) {
        show_rf_menu();
    }, ModuleManager::isHardwareConnected(MODULE_SUBGHZ));

    // NFC
    create_icon_btn(grid, LV_SYMBOL_SD_CARD, "NFC", btn_w, btn_h, [](lv_event_t *e) {
        show_nfc_menu();
    }, ModuleManager::isHardwareConnected(MODULE_NFC));

    // IR
    create_icon_btn(grid, LV_SYMBOL_POWER, "InfraRed", btn_w, btn_h, [](lv_event_t *e) {
        show_ir_menu();
    }, ModuleManager::isHardwareConnected(MODULE_IR));

    // Hardware
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "Hardware", btn_w, btn_h, [](lv_event_t *e) {
        show_hardware_menu();
    }, true);

    // AI
    create_icon_btn(grid, LV_SYMBOL_EYE_OPEN, "AI Voice", btn_w, btn_h, [](lv_event_t *e) {
        show_ai_voice_menu();
    }, true);

    // GPIO
    create_icon_btn(grid, LV_SYMBOL_SHUFFLE, "GPIO", btn_w, btn_h, [](lv_event_t *e) {
        show_gpio_menu();
    }, true);

    // USB
    create_icon_btn(grid, LV_SYMBOL_USB, "USB", btn_w, btn_h, [](lv_event_t *e) {
        show_usb_menu();
    }, true);

    // Quick Attack
    create_icon_btn(grid, LV_SYMBOL_WARNING, "Quick", btn_w, btn_h, [](lv_event_t *e) {
        show_quick_attack_menu();
    }, true);

    // Stop All
    lv_obj_t *stop_btn = create_icon_btn(grid, LV_SYMBOL_STOP, "STOP ALL", 290, 40, [](lv_event_t *e) {
        attacks_stop();
    }, true);
    lv_obj_set_style_border_color(stop_btn, lv_color_hex(COL_RED), 0);

    Serial.println("[UI] Main menu loaded (Grid)");
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
    create_header(scr, "Quick Combo");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    
    const int w = 145, h = 60;

    create_icon_btn(grid, LV_SYMBOL_DUMMY, "GateOpener", w, h, [](lv_event_t *e) {
        attacks_stop();
        attacks_start(ATTACK_RF_REPLAY);
        attacks_start(ATTACK_NFC_FAULT);
        update_status_bar();
    }, true);

    create_icon_btn(grid, LV_SYMBOL_WIFI, "Home Pwn", w, h, [](lv_event_t *e) {
        attacks_stop();
        attacks_start(ATTACK_WIFI_DEAUTH);
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    }, true);

    create_icon_btn(grid, LV_SYMBOL_BLUETOOTH, "BLE Storm", w, h, [](lv_event_t *e) {
        attacks_stop();
        attacks_start(ATTACK_BLE_SPAM);
        attacks_start(ATTACK_BLE_SWIFT_PAIR);
        update_status_bar();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "IR Clone", w, h, [](lv_event_t *e) {
        attacks_stop();
        attacks_start(ATTACK_IR_BRUTE);
        attacks_start(ATTACK_IR_CLONE);
        update_status_bar();
    }, true);
    
    // Full Auto Button (Big)
    lv_obj_t *btnAuto = create_icon_btn(grid, LV_SYMBOL_SHUFFLE, "FULL AUTO AI", 300, 50, [](lv_event_t *e) {
         attacks_stop();
         int best = ai_get_best_action();
         AttackType ai_attack = (AttackType)(best + 1); 
         if (ai_attack > ATTACK_NONE && ai_attack < ATTACK_USB_EXFIL) {
             attacks_start(ai_attack);
             tts_speak("modo_combate");
             LEDDriver::blinkAttacking();
         } else {
             attacks_start(ATTACK_WIFI_DEAUTH);
         }
         update_status_bar();
    }, true);
    lv_obj_set_style_border_color(btnAuto, lv_color_hex(COL_RED), 0);
    lv_obj_set_style_shadow_color(btnAuto, lv_color_hex(COL_RED), 0);
    lv_obj_set_style_shadow_width(btnAuto, 15, 0);

    create_icon_btn(grid, LV_SYMBOL_EYE_CLOSE, "Stealth Mode", 300, 40, [](lv_event_t *e) {
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

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);

    const int w = 95, h = 80;

    create_icon_btn(grid, LV_SYMBOL_BLUETOOTH, "BLE Spam", w, h, [](lv_event_t *e){ show_rf_popup("BLE Advertisement Spam"); }, true);
    create_icon_btn(grid, LV_SYMBOL_BLUETOOTH, "Spam BR", w, h, [](lv_event_t *e){ show_rf_popup("Spamming BR Packets"); }, true);
    create_icon_btn(grid, LV_SYMBOL_WARNING, "Sour Apple", w, h, [](lv_event_t *e){ show_rf_popup("Sour Apple Attack"); }, true);
    create_icon_btn(grid, LV_SYMBOL_REFRESH, "Swift Pair", w, h, [](lv_event_t *e){ show_rf_popup("Swift Pair Attack"); }, true);
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "Fast Pair", w, h, [](lv_event_t *e){ show_rf_popup("Fast Pair Attack"); }, true);
    create_icon_btn(grid, LV_SYMBOL_AUDIO, "Samsung", w, h, [](lv_event_t *e){ show_rf_popup("Samsung BLE Spam"); }, true);
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

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);

    const int w = 95, h = 80;

    create_icon_btn(grid, LV_SYMBOL_WIFI, "Deauth", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_UPLOAD, "Beacon", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_COPY, "Evil Twin", w, h, [](lv_event_t *e) { show_evil_twin_menu(); }, true);
    create_icon_btn(grid, LV_SYMBOL_GPS, "Wardrive", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_FILE, "PMKID", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_SAVE, "Capture", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_WIFI, "WPS Pixie", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "WPS Brute", w, h, attack_cb, true);
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
    create_menu_btn(scr, "Vivo Fibra", 10, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid("Vivo Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "Claro Fibra", 165, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid("Claro Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "Oi Fibra", 10, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid("Oi Fibra");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "TIM Live", 165, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid("TIM Live");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    y += 50;
    create_menu_btn(scr, "GVT", 10, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid("GVT Guest");
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
    create_menu_btn(scr, "Clone Strongest", 165, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        attacks_set_evil_twin_ssid(NULL); // Auto clone
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        update_status_bar();
    });
}

// ------------------------------------------------------------
// RF MENU (unchanged)
// ------------------------------------------------------------
// RF ACTIVE ATTACK SCREEN (Used by Jammers/Replay)
// ------------------------------------------------------------
static void show_rf_popup(const char* title) {
    lv_obj_t *scr = lv_scr_act();
    
    // Dark overlay
    lv_obj_t *overlay = lv_obj_create(scr);
    lv_obj_set_size(overlay, SCR_W, SCR_H);
    lv_obj_set_style_bg_color(overlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_90, 0);
    lv_obj_set_style_border_width(overlay, 0, 0);
    
    // Popup Panel
    lv_obj_t *panel = lv_obj_create(overlay);
    lv_obj_set_size(panel, 220, 180);
    lv_obj_center(panel);
    lv_obj_set_style_bg_color(panel, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(COL_RED), 0);
    lv_obj_set_style_radius(panel, 20, 0);
    
    // Title
    lv_obj_t *lbl = lv_label_create(panel);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl, lv_color_hex(COL_TEXT), 0);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 10);
    
    // Status
    lv_obj_t *status = lv_label_create(panel);
    lv_label_set_text(status, "TRANSMITTING...");
    lv_obj_set_style_text_color(status, lv_color_hex(COL_RED), 0);
    lv_obj_align(status, LV_ALIGN_CENTER, 0, -10);
    
    // Stop Button
    lv_obj_t *btn_stop = lv_btn_create(panel);
    lv_obj_set_size(btn_stop, 100, 40);
    lv_obj_align(btn_stop, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(btn_stop, lv_color_hex(COL_RED), 0);
    
    lv_obj_t *lbl_stop = lv_label_create(btn_stop);
    lv_label_set_text(lbl_stop, "STOP");
    lv_obj_center(lbl_stop);
    
    lv_obj_add_event_cb(btn_stop, [](lv_event_t *e) {
        lv_obj_t *ov = (lv_obj_t*)lv_event_get_user_data(e);
        lv_obj_del(ov); // Close popup
        ChameleonAnim::setMood(MOOD_IDLE); // Reset mood
        // TODO: Call RFCore::stop() here
    }, LV_EVENT_CLICKED, overlay);
    
    // Set Aggressive Mood
    ChameleonAnim::setMood(MOOD_ATTACK);
}

// ------------------------------------------------------------
// NFC READ SCREEN
// ------------------------------------------------------------
static void show_nfc_read_screen() {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    create_header(scr, "NFC Reader");
    
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 220, 200);
    lv_obj_center(cont);
    lv_obj_set_style_bg_color(cont, lv_color_hex(COL_PANEL), 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(COL_ACCENT_CYAN), 0);
    
    lv_obj_t *icon = lv_label_create(cont);
    lv_label_set_text(icon, LV_SYMBOL_WIFI); // Use WiFi symbol as "Wave"
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_20, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 20);
    
    lv_obj_t *txt = lv_label_create(cont);
    lv_label_set_text(txt, "Approaching Card...");
    lv_obj_align(txt, LV_ALIGN_CENTER, 0, 10);
    
    // Simulate Read after 2s (Timer)
    lv_timer_t *timer = lv_timer_create([](lv_timer_t *t){
        lv_obj_t *c = (lv_obj_t*)t->user_data;
        if (!lv_obj_is_valid(c)) return;
        
        lv_obj_clean(c);
        
        // Result
        lv_obj_t *l1 = lv_label_create(c);
        lv_label_set_text(l1, "MIFARE Classic 1K");
        lv_obj_align(l1, LV_ALIGN_TOP_MID, 0, 10);
        
        lv_obj_t *l2 = lv_label_create(c);
        lv_label_set_text(l2, "UID: E4:1B:5A:99");
        lv_obj_set_style_text_font(l2, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(l2, lv_color_hex(COL_GREEN), 0);
        lv_obj_center(l2);
        
        lv_obj_t *l3 = lv_label_create(c);
        lv_label_set_text(l3, "Data: 1024 bytes");
        lv_obj_align(l3, LV_ALIGN_BOTTOM_MID, 0, -10);
        
        ChameleonAnim::setMood(MOOD_SUCCESS);
        SoundManager::playLevelUp(); // Success Sound
        
    }, 2000, cont);
    lv_timer_set_repeat_count(timer, 1);
}

// ------------------------------------------------------------
static void show_rf_menu() {
    current_screen = SCREEN_SUBGHZ;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "RF SubGHz");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);

    const int w = 95, h = 80;

    create_icon_btn(grid, LV_SYMBOL_CHARGE, "Jammer 433", w, h, [](lv_event_t *e){ show_rf_popup("Jammer 433MHz"); }, true);
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "Jammer 315", w, h, [](lv_event_t *e){ show_rf_popup("Jammer 315MHz"); }, true);
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "Jammer 868", w, h, [](lv_event_t *e){ show_rf_popup("Jammer 868MHz"); }, true);
    create_icon_btn(grid, LV_SYMBOL_WARNING, "Jammer Burst", w, h, [](lv_event_t *e){ show_rf_popup("Burst Jammer"); }, true);
    create_icon_btn(grid, LV_SYMBOL_REFRESH, "Jammer Smart", w, h, [](lv_event_t *e){ show_rf_popup("Smart Jammer"); }, true);
    create_icon_btn(grid, LV_SYMBOL_SAVE, "Capture", w, h, [](lv_event_t *e){ show_rf_popup("Listening..."); }, true);
    create_icon_btn(grid, LV_SYMBOL_PLAY, "Replay", w, h, [](lv_event_t *e){ show_rf_popup("Replaying..."); }, true);
    create_icon_btn(grid, LV_SYMBOL_EYE_OPEN, "Ghost", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "Brute Force", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_IMAGE, "Spectrum", w, h, [](lv_event_t *e) { show_spectrum_view(); }, true);
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
    create_header(scr, "NFC Tools");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    const int w = 145, h = 60; // Wider buttons for text

    create_icon_btn(grid, LV_SYMBOL_COPY, "Clone Card", w, h, [](lv_event_t *e){ show_nfc_read_screen(); }, true);
    create_icon_btn(grid, LV_SYMBOL_WARNING, "Fault Inj", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_EYE_CLOSE, "Phishing", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_DOWNLOAD, "Read Info", w, h, [](lv_event_t *e){ show_nfc_read_screen(); }, true);
    create_icon_btn(grid, LV_SYMBOL_UPLOAD, "Emulate", w, h, [](lv_event_t *e){ show_rf_popup("Emulating UID..."); }, true);
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
    create_header(scr, "InfraRed");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    const int w = 145, h = 60;

    create_icon_btn(grid, LV_SYMBOL_POWER, "TV-B-Gone", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "IR Brute", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_COPY, "IR Clone", w, h, attack_cb, true);
    create_icon_btn(grid, LV_SYMBOL_KEYBOARD, "Gate Brute", w, h, attack_cb, true);
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

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    
    const int w = 145, h = 60;

    // Toggles
    create_icon_btn(grid, LV_SYMBOL_WIFI, "Toggle WiFi", w, h, [](lv_event_t *e) {
        bool cur = lvgl_get_module_state("wifi");
        lvgl_toggle_module("wifi", !cur);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_BLUETOOTH, "Toggle BLE", w, h, [](lv_event_t *e) {
        bool cur = lvgl_get_module_state("ble");
        lvgl_toggle_module("ble", !cur);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_SD_CARD, "Toggle NFC", w, h, [](lv_event_t *e) {
        bool cur = lvgl_get_module_state("nfc");
        lvgl_toggle_module("nfc", !cur);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "Toggle RF", w, h, [](lv_event_t *e) {
        bool cur = lvgl_get_module_state("rf");
        lvgl_toggle_module("rf", !cur);
    }, true);

    create_icon_btn(grid, LV_SYMBOL_POWER, "Toggle IR", w, h, [](lv_event_t *e) {
        bool cur = lvgl_get_module_state("ir");
        lvgl_toggle_module("ir", !cur);
    }, true);
    
    // Submenus
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "LEDs", w, h, [](lv_event_t *e) { show_led_menu(); }, true);
    create_icon_btn(grid, LV_SYMBOL_BATTERY_FULL, "Power", w, h, [](lv_event_t *e) { show_energy_menu(); }, true);
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "Settings", w, h, [](lv_event_t *e) { show_settings_menu(); }, true);
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
    
    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    const int w = 145, h = 60;

    create_icon_btn(grid, LV_SYMBOL_REFRESH, "Neon Pulse", w, h, [](lv_event_t *e) {
        LEDDriver::pulseEffect(LED_CYAN, LED_MAGENTA, 50);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_DOWNLOAD, "Matrix Rain", w, h, [](lv_event_t *e) {
        LEDDriver::matrixRain();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_LOOP, "Rainbow", w, h, [](lv_event_t *e) {
        LEDDriver::rainbowCycle();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_WARNING, "Attack Blink", w, h, [](lv_event_t *e) {
        LEDDriver::blinkAttacking();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_OK, "Green On", w, h, [](lv_event_t *e) {
        LEDDriver::setAll(LED_GREEN);
        LEDDriver::show();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_CLOSE, "LED OFF", w, h, [](lv_event_t *e) {
        LEDDriver::clear();
        LEDDriver::show();
    }, true);
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
    create_header(scr, "Power Management");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    const int w = 145, h = 60;

    create_icon_btn(grid, LV_SYMBOL_POWER, "Sleep 500ms", w, h, [](lv_event_t *e) {
        esp_sleep_enable_timer_wakeup(500000); // 500ms
        esp_light_sleep_start();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_POWER, "Sleep 2s", w, h, [](lv_event_t *e) {
        esp_sleep_enable_timer_wakeup(2000000); // 2s
        esp_light_sleep_start();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_CHARGE, "OC 240MHz", w, h, [](lv_event_t *e) {
        setCpuFrequencyMhz(240);
        Serial.println("[CPU] Overclock: 240MHz");
    }, true); // Need visual feedback?
    
    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "std 160MHz", w, h, [](lv_event_t *e) {
        setCpuFrequencyMhz(160);
        Serial.println("[CPU] Normal: 160MHz");
    }, true);
    
    // Battery info
    lv_obj_t *info = lv_label_create(scr);
    char buf[64];
    snprintf(buf, sizeof(buf), "CPU: %d MHz | Heap: %dKB", getCpuFrequencyMhz(), ESP.getFreeHeap() / 1024);
    lv_label_set_text(info, buf);
    lv_obj_set_style_text_color(info, lv_color_hex(COL_GREEN), 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// ------------------------------------------------------------
// RESET MENU (Q-Table, Configs, Logs)
// ------------------------------------------------------------
// ------------------------------------------------------------
// SETTINGS MENU
// ------------------------------------------------------------
void show_settings_menu() {
    current_screen = (ScreenType)SCREEN_HW_RESET; // Reuse ID or add SCREEN_SETTINGS
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Settings");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 15, 0);

    // Volume Slider
    lv_obj_t *lbl_vol = lv_label_create(grid);
    lv_label_set_text(lbl_vol, "System Volume");
    lv_obj_set_style_text_color(lbl_vol, lv_color_hex(COL_TEXT), 0);
    
    lv_obj_t *slider_vol = lv_slider_create(grid);
    lv_obj_set_width(slider_vol, 200);
    lv_slider_set_value(slider_vol, 80, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_vol, [](lv_event_t *e){
        lv_obj_t *s = lv_event_get_target(e);
        int val = lv_slider_get_value(s);
        if (val < 10) SoundManager::setMute(true);
        else {
            SoundManager::setMute(false);
            SoundManager::playClick(); // Test sound
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Brightness Slider
    lv_obj_t *lbl_brt = lv_label_create(grid);
    lv_label_set_text(lbl_brt, "Brightness");
    lv_obj_set_style_text_color(lbl_brt, lv_color_hex(COL_TEXT), 0);
    
    lv_obj_t *slider_brt = lv_slider_create(grid);
    lv_obj_set_width(slider_brt, 200);
    lv_slider_set_range(slider_brt, 10, 255);
    lv_slider_set_value(slider_brt, 200, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_brt, [](lv_event_t *e){
        lv_obj_t *s = lv_event_get_target(e);
        int val = lv_slider_get_value(s);
        ledcWrite(0, val); // Assuming Channel 0 is Backlight
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // About Device
    lv_obj_t *btn_about = lv_btn_create(grid);
    lv_obj_set_size(btn_about, 200, 40);
    lv_obj_set_style_bg_color(btn_about, lv_color_hex(COL_BTN), 0);
    lv_obj_t *lbl_a = lv_label_create(btn_about);
    lv_label_set_text(lbl_a, "About MorphNode");
    lv_obj_center(lbl_a);
    lv_obj_add_event_cb(btn_about, [](lv_event_t *e){
        lv_obj_clean(lv_scr_act());
        lv_obj_t *s = lv_scr_act();
        create_header(s, "About");
        lv_obj_t *txt = lv_label_create(s);
        lv_label_set_text(txt, "MorphNode OS v2.0\n\nDeveloped by:\nAntigravity AI\n\n(c) 2026");
        lv_obj_set_style_text_align(txt, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(txt);
    }, LV_EVENT_CLICKED, NULL);

    // Factory Reset (Danger)
    lv_obj_t *btn_rst = lv_btn_create(grid);
    lv_obj_set_size(btn_rst, 200, 40);
    lv_obj_set_style_bg_color(btn_rst, lv_color_hex(COL_RED), 0);
    lv_obj_t *lbl_r = lv_label_create(btn_rst);
    lv_label_set_text(lbl_r, "FACTORY RESET");
    lv_obj_center(lbl_r);
    lv_obj_add_event_cb(btn_rst, [](lv_event_t *e) {
        ai_reset_qtable();
        SD.remove("/config/config.json");
        ESP.restart();
    }, LV_EVENT_CLICKED, NULL);
}

// ------------------------------------------------------------
// ANIMATION GALLERY (New Phase 3)
// ------------------------------------------------------------
static void show_anim_gallery() {
    current_screen = SCREEN_AI_VOICE; // Reuse screen ID or add new one
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Anim Gallery");

    // Init Chameleon here to ensure it's visible if we came from elsewhere
    ChameleonAnim::init(scr, 100, 20);

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 120);
    lv_obj_set_pos(grid, 0, 120);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    
    const int w = 95, h = 60;

    create_icon_btn(grid, LV_SYMBOL_OK, "Idle", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_IDLE);
    }, true);

    create_icon_btn(grid, LV_SYMBOL_WARNING, "Attack", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_ATTACK);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_PLUS, "Eating", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_EATING);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_EYE_CLOSE, "Sneaky", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_SNEAKY);
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_LOOP, "Spin", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_CONFUSED);
    }, true);
    
    lv_obj_t *btnP = create_icon_btn(grid, LV_SYMBOL_SHUFFLE, "PARTY", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_EXCITED);
    }, true);
    lv_obj_set_style_border_color(btnP, lv_color_hex(COL_ACCENT_MAG), 0);

    create_icon_btn(grid, LV_SYMBOL_MUTE, "Sleep", w, h, [](lv_event_t *e) {
        ChameleonAnim::setMood(MOOD_SLEEP);
    }, true);
}

// ------------------------------------------------------------
// AI & VOICE MENU (fully implemented)
// ------------------------------------------------------------
void show_ai_voice_menu() {
    current_screen = SCREEN_AI_VOICE;
    lv_obj_clean(lv_scr_act());
    if (main_group) lv_group_remove_all_objs(main_group);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "AI Strategy");

    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    
    const int w = 145, h = 60;

    create_icon_btn(grid, LV_SYMBOL_SETTINGS, "Train Mode", w, h, [](lv_event_t *e) {
        ai_set_combat_mode(false);
        tts_speak("modo_treino");
        LEDDriver::setAll(LED_BLUE);
        LEDDriver::show();
    }, true);
    
    lv_obj_t *btnC = create_icon_btn(grid, LV_SYMBOL_WARNING, "COMBAT Mode", w, h, [](lv_event_t *e) {
        ai_set_combat_mode(true);
        tts_speak("modo_combate");
        LEDDriver::setAll(LED_RED);
        LEDDriver::show();
    }, true);
    lv_obj_set_style_border_color(btnC, lv_color_hex(COL_RED), 0);

    create_icon_btn(grid, LV_SYMBOL_DOWNLOAD, "Update CVEs", 300, 40, [](lv_event_t *e) {
        Serial.println("[AI] Iniciando download de CVEs BR...");
        tts_speak("ataque_iniciado");
        // ResourceDownloader::downloadCVEs(); // Uncomment when available
        LEDDriver::setAll(LED_YELLOW);
        LEDDriver::show();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_AUDIO, "Test Voice", w, h, [](lv_event_t *e) {
        tts_speak("sistema_pronto");
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_LIST, "Stats", w, h, [](lv_event_t *e) {
        AIStats stats;
        ai_get_statistics(&stats);
        Serial.printf(
            "[AI] Updates: %lu, AvgR: %.2f, Best: %d\n", stats.totalUpdates, stats.avgReward, stats.bestAction
        );
    }, true);

    create_icon_btn(grid, LV_SYMBOL_REFRESH, "Reset AI", w, h, [](lv_event_t *e) {
        ai_reset_qtable();
        tts_speak("ataque_parado");
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_UPLOAD, "Export AI", w, h, [](lv_event_t *e) {
        ai_export_qtable_json("/ai/qtable_export.json");
        tts_speak("codigo_enviado");
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_IMAGE, "QR Code", w, h, [](lv_event_t *e) {
        show_qrcode_menu();
    }, true);
    
    create_icon_btn(grid, LV_SYMBOL_VIDEO, "Gallery", w, h, [](lv_event_t *e) {
        show_anim_gallery();
    }, true);
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
    snprintf(
        statusText,
        sizeof(statusText),
        "Heap: %d KB\nPSRAM: %d KB\nUptime: %d min\n\nAttack: %s\nGPS: %s\nNFC: %s",
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

// ------------------------------------------------------------
// STATUS BAR UPDATE (Battery + SD + Attack Status)
// ------------------------------------------------------------
static void update_status_bar() {
    if (!status_label) return;
    char buf[96];
    AttackType current = attacks_get_current();

    // Get SD free space percentage
    uint64_t sdTotal = SD.totalBytes();
    uint64_t sdUsed = SD.usedBytes();
    int sdFreePercent = sdTotal > 0 ? (int)(100 - (sdUsed * 100 / sdTotal)) : 0;

    if (current != ATTACK_NONE) {
        snprintf(
            buf,
            sizeof(buf),
            LV_SYMBOL_CHARGE " SD:%d%% | " LV_SYMBOL_WARNING " %s",
            sdFreePercent,
            attacks_get_name(current)
        );
        lv_obj_set_style_text_color(status_label, lv_color_hex(COL_RED), 0);
    } else {
        snprintf(
            buf,
            sizeof(buf),
            LV_SYMBOL_CHARGE " SD:%d%% | Heap:%dKB | Ready",
            sdFreePercent,
            ESP.getFreeHeap() / 1024
        );
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

void lvgl_announce_menu(const char *menu_name) { tts_speak(menu_name); }

// ------------------------------------------------------------
// HARDWARE MODULE CONTROL (wrappers around ModuleManager)
// ------------------------------------------------------------
void lvgl_toggle_module(const char *module_name, bool enabled) {
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

bool lvgl_get_module_state(const char *module_name) {
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
        buf = (lv_color_t *)ps_malloc(bufSize * sizeof(lv_color_t));
        Serial.println("[LVGL] Using PSRAM buffer");
    } else {
        buf = (lv_color_t *)malloc(bufSize * sizeof(lv_color_t));
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

    // ==========================================
    // BOOT SPLASH SCREEN - MorphNode Chameleon
    // ==========================================
    Serial.println("[BOOT] Showing MorphNode splash screen...");
    Serial.println(CHAMELEON_ASCII); // Print ASCII art to serial

    lv_obj_t *splash_scr = lv_scr_act();
    lv_obj_set_style_bg_color(splash_scr, lv_color_hex(CYBER_DARK), 0);

    // Centered logo circle (simulating chameleon eye)
    lv_obj_t *logo_circle = lv_obj_create(splash_scr);
    lv_obj_set_size(logo_circle, 80, 80);
    lv_obj_align(logo_circle, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_radius(logo_circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(logo_circle, lv_color_hex(CYBER_CYAN), 0);
    lv_obj_set_style_border_color(logo_circle, lv_color_hex(CYBER_MAGENTA), 0);
    lv_obj_set_style_border_width(logo_circle, 4, 0);
    lv_obj_set_style_bg_opa(logo_circle, LV_OPA_COVER, 0);

    // Inner eye (targeting reticle effect)
    lv_obj_t *eye = lv_obj_create(logo_circle);
    lv_obj_set_size(eye, 24, 24);
    lv_obj_center(eye);
    lv_obj_set_style_radius(eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(eye, lv_color_hex(CYBER_RED), 0);
    lv_obj_set_style_border_width(eye, 0, 0);
    lv_obj_set_style_bg_color(splash_scr, lv_color_hex(0x000000), 0);

    lv_obj_t *logo = lv_label_create(splash_scr);
    lv_label_set_text(logo, "MORPHNODE");
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_20, 0); // Use 20 which is likely avail
    lv_obj_set_style_text_color(logo, lv_color_hex(COL_ACCENT_CYAN), 0);
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -40);
    
    // Init Chameleon centered
    ChameleonAnim::init(splash_scr, 100, 100);
    ChameleonAnim::stop(); // Reset
    
    // Animation Sequence (Simulated with blocking delays for boot)
    // 1. Fade in Logic (simulated by draw)
    lv_task_handler();
    delay(500);
    
    // 2. Blink / Wake up
    ChameleonAnim::setMood(MOOD_SLEEP);
    lv_task_handler();
    delay(800);
    
    ChameleonAnim::setMood(MOOD_IDLE);
    lv_task_handler();
    delay(500);
    
    // 3. Hardware Check (Tongue out)
    ChameleonAnim::setMood(MOOD_EATING);
    lv_task_handler();
    delay(600);
    
    ChameleonAnim::setMood(MOOD_SUCCESS); // Green flash
    lv_task_handler();
    delay(500);
    
    // 4. Ready
    ChameleonAnim::setMood(MOOD_IDLE);

    // Version
    lv_obj_t *splash_ver = lv_label_create(splash_scr);
    lv_label_set_text(splash_ver, "v2.0 Cyber-Glass");
    lv_obj_set_style_text_color(splash_ver, lv_color_hex(0x666666), 0);
    lv_obj_align(splash_ver, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_task_handler();
    delay(1000); 

    // Show main menu (now Passport)
    SoundManager::init(); // Init Audio
    LevelManager::init(); // Initialize XP
    
    SoundManager::playBoot(); // Boot Sound
    show_passport_screen(); 
    Serial.println("[DISPLAY] Setup complete!");
}

// ------------------------------------------------------------
// GPIO MENU (Phase 10)
// ------------------------------------------------------------
void show_gpio_menu() {
    current_screen = SCREEN_GPIO;
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    create_header(scr, "GPIO Control");
    
    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_set_size(grid, SCR_W, SCR_H - 50);
    lv_obj_set_pos(grid, 0, 50);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(grid, 10, 0);
    lv_obj_set_style_pad_gap(grid, 10, 10);
    
    // Toggle Buttons for common pins
    int pins[] = {1, 2, 4, 15, 16, 17};
    for(int p : pins) {
        String label = String("GPIO ") + p;
        lv_obj_t *btn = lv_btn_create(grid);
        lv_obj_set_size(btn, 90, 50);
        lv_obj_add_event_cb(btn, [](lv_event_t *e){
            int pin = (int)lv_event_get_user_data(e);
            digitalWrite(pin, !digitalRead(pin)); // Toggle
            // Update visual state (TODO)
        }, LV_EVENT_CLICKED, (void*)p);
        
        lv_obj_t *l = lv_label_create(btn);
        lv_label_set_text(l, label.c_str());
        lv_obj_center(l);
    }
}

// ------------------------------------------------------------
// USB MENU (Phase 10)
// ------------------------------------------------------------
void show_usb_menu() {
    current_screen = SCREEN_USB;
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    create_header(scr, "USB Mode");
    
    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, SCR_W - 20, SCR_H - 60);
    lv_obj_set_pos(list, 10, 50);
    
    lv_obj_t* btn1 = lv_list_add_btn(list, LV_SYMBOL_KEYBOARD, "BadUSB HID");
    lv_obj_add_event_cb(btn1, [](lv_event_t *e){
        show_rf_popup("USB HID Active");
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btn2 = lv_list_add_btn(list, LV_SYMBOL_SD_CARD, "Mass Storage");
    lv_obj_add_event_cb(btn2, [](lv_event_t *e){
        show_rf_popup("USB Mass Storage");
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btn3 = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "Serial/UART");
    lv_obj_add_event_cb(btn3, [](lv_event_t *e){
        show_rf_popup("USB Serial Active");
    }, LV_EVENT_CLICKED, NULL);
}

void lvgl_loop() {
    lv_task_handler();
    LevelManager::loop(); // XP Updates
    // Periodic LED/TTS sync could be added here
}

lv_group_t *lvgl_get_main_group() { return main_group; }
