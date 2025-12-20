/**
 * @file manual_menu.cpp
 * @brief Manual/Help Menu Implementation
 * 
 * Provides the content and logic for displaying on-screen
 * manual pages for various attack modules.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "manual_menu.h"
#include <Arduino.h>

lv_obj_t* ManualMenu::manual_scr = nullptr;
lv_obj_t* ManualMenu::text_area = nullptr;

// Manual Texts
const char* TEXT_RF = 
"RF TOOLS MANUAL\n\n"
"Freqs: 315/433/868 MHz\n\n"
"1. Jammer: Blocks RF signals.\n"
"   - Continuous: Constant noise.\n"
"   - Burst: Pulses (save battery).\n"
"   - Smart: Jams only when signal detected.\n\n"
"2. Capture/Replay: Record and playback signals.\n"
"   - Supports: Princeton, Nice, CAME, etc.\n"
"   - Raw: Captures unknown protocols.\n\n"
"3. Rolljam: Attacks rolling codes.\n"
"   - Jams signal while capturing it.\n"
"   - Victim presses twice -> 1st blocked, 2nd captured.\n"
"   - Replay 1st to unlock, keep 2nd for attacker.\n\n"
"4. Rollback: Exploit resync vulnerability.\n"
"   - Captures multiple codes.\n"
"   - Replays in sequence to 'rewind' counter.\n";

const char* TEXT_WIFI = 
"WIFI ATTACKS MANUAL\n\n"
"1. Deauth: Disconnects clients from AP.\n"
"   - Uses 802.11 deauthentication frames.\n"
"   - Effect: Denial of Service.\n\n"
"2. Beacon Spam: Creates fake APs.\n"
"   - Annoyance/Confusion attack.\n\n"
"3. Evil Twin: Clones target AP.\n"
"   - Creates open AP with same name.\n"
"   - Captures clients automatically.\n\n"
"4. PMKID: WPA2 Crack helper.\n"
"   - Captures RSNA pairwise master key ID.\n"
"   - Craccable offline with Hashcat.\n";

const char* TEXT_BLE = 
"BLE ATTACKS MANUAL\n\n"
"1. Spam BR: Brazilian device names.\n"
"   - Floods scan lists.\n\n"
"2. Sour Apple: iOS Crash/Popup.\n"
"   - Mimics AirPods/AppleTV nearby.\n\n"
"3. Swift Pair: Windows Spam.\n"
"4. Fast Pair: Android Spam.\n";

const char* TEXT_NRF = 
"NRF24 (2.4GHz) MANUAL\n\n"
"Hardware: NRF24L01+ on SPI.\n\n"
"1. Mousejack: Hijack wireless mice.\n"
"   - Scans channels for vulnerable dongles.\n"
"   - Injects keystrokes via mouse endpoint.\n\n"
"2. Sniffer: Listen for keystrokes.\n"
"   - Microsoft/Logitech wireless keyboards.\n"
"   - Decrypts known vulnerability patterns.\n";

void ManualMenu::init(lv_obj_t* parent) {
    // Create a tab or a specific screen for manuals
    // For now, we assume this is called to populate a "Manual" tab
    
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    
    create_category_btn(cont, "RF Tools Manual", event_handler_rf);
    create_category_btn(cont, "WiFi Attacks Manual", event_handler_wifi);
    create_category_btn(cont, "BLE Attacks Manual", event_handler_ble);
    create_category_btn(cont, "NRF24 Attacks Manual", event_handler_nrf);
}

void ManualMenu::create_category_btn(lv_obj_t* parent, const char* label, lv_event_cb_t event_cb) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_width(btn, lv_pct(90));
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_center(lbl);
}

void ManualMenu::show_manual(const char* title, const char* text) {
    manual_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(manual_scr, lv_color_hex(0x000000), 0);
    
    // Title
    lv_obj_t* lbl_title = lv_label_create(manual_scr);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x00FF00), 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Text Area
    text_area = lv_textarea_create(manual_scr);
    lv_obj_set_size(text_area, lv_pct(95), lv_pct(70));
    lv_obj_align(text_area, LV_ALIGN_CENTER, 0, 10);
    lv_textarea_set_text(text_area, text);
    lv_obj_set_style_text_font(text_area, &lv_font_montserrat_10, 0);
    lv_obj_set_style_bg_color(text_area, lv_color_hex(0x101010), 0);
    lv_obj_set_style_text_color(text_area, lv_color_hex(0xFFFFFF), 0);
    lv_textarea_set_cursor_click_pos(text_area, false);
    
    // Close Button
    lv_obj_t* btn_close = lv_btn_create(manual_scr);
    lv_obj_set_size(btn_close, 80, 40);
    lv_obj_align(btn_close, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn_close, event_handler_close, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* lbl_close = lv_label_create(btn_close);
    lv_label_set_text(lbl_close, "BACK");
    lv_obj_center(lbl_close);
    
    lv_scr_load_anim(manual_scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
}

void ManualMenu::event_handler_rf(lv_event_t* e) {
    show_manual("RF Tools", TEXT_RF);
}

void ManualMenu::event_handler_wifi(lv_event_t* e) {
    show_manual("WiFi Attacks", TEXT_WIFI);
}

void ManualMenu::event_handler_ble(lv_event_t* e) {
    show_manual("BLE Attacks", TEXT_BLE);
}

void ManualMenu::event_handler_nrf(lv_event_t* e) {
    show_manual("NRF24 Attacks", TEXT_NRF);
}

void ManualMenu::event_handler_close(lv_event_t* e) {
    // Return to main menu (assuming it's loaded implicitly by deleting current)
    // Actually we need to explicitly load the previous screen or let LVGL handle history (not built-in)
    // For now, we will assume a "main_scr" exists or we just delete this and reload main
    // Better: Helper in LVGL Menu to restore
    
    // Simplified: Just delete this scr, which might leave us in void if no prev_scr is active.
    // Safe way: Assuming lvgl_menu handles this.
    // We will call a function from lvgl_menu if possible, or just load default.
    
    extern lv_obj_t* ui_Screen1; // Assuming global reference from lvgl_menu
    if (ui_Screen1) {
        lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
    }
}
