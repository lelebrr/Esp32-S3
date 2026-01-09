/**
 * @file lvgl_menu.h
 * @brief MorphNode - Professional LVGL Menu System
 *
 * 8 Main Categories with deep submenus, cyberpunk neon styling,
 * LED synchronization, and TTS voice announcements.
 *
 * @author MorphNode Team
 * @date 2025-12-28
 */

#ifndef LVGL_MENU_H
#define LVGL_MENU_H

#include "gesture_sensor.h"
#include <Arduino.h>
#include <lvgl.h>

// ============================================================================
// SCREEN TYPES - All menu screens
// ============================================================================
typedef enum {
    SCREEN_MAIN = 0,
    SCREEN_PASSPORT,

    // Main 8 categories
    SCREEN_QUICK_ATTACK,
    SCREEN_WIFI,
    SCREEN_BLE,
    SCREEN_NFC,
    SCREEN_SUBGHZ,
    SCREEN_IR,
    SCREEN_HARDWARE,
    SCREEN_AI_VOICE,
    
    // New Apps
    SCREEN_GPIO,
    SCREEN_USB,

    // WiFi submenus
    SCREEN_WIFI_EVIL_TWIN,

    // Hardware submenus
    SCREEN_HW_MODULES,
    SCREEN_HW_LEDS,
    SCREEN_HW_ENERGY,
    SCREEN_HW_RESET,

    // Generic
    SCREEN_STATUS,
    SCREEN_SETTINGS

} ScreenType;

// ============================================================================
// PUBLIC API
// ============================================================================

void show_passport_screen(); // NEW: Idle screen
void show_main_menu();       // App Grid
void show_gpio_menu();
void show_usb_menu();
void show_settings_menu(); // NEW Phase 13

/**
 * @brief Initialize TFT display, LVGL, and show main menu
 */
void setup_lvgl_menu();

/**
 * @brief Call in main loop to handle LVGL tasks
 */
void lvgl_loop();

/**
 * @brief Get current screen type
 */
ScreenType lvgl_get_current_screen();

/**
 * @brief Navigate to specific screen
 */
void lvgl_show_screen(ScreenType screen);

/**
 * @brief Get LVGL group for gesture navigation
 */
lv_group_t *lvgl_get_main_group();

// ============================================================================
// GESTURE INTEGRATION
// ============================================================================

/**
 * @brief Send gesture key to LVGL
 */
void lvgl_send_gesture_key(uint32_t key);

/**
 * @brief Convert gesture action to LVGL key code
 */
uint32_t gesture_action_to_lvgl_key(GestureAction action);

// ============================================================================
// LED & TTS SYNC
// ============================================================================

/**
 * @brief Sync LED animation with current menu state
 */
void lvgl_sync_leds();

/**
 * @brief Announce current menu via TTS
 */
void lvgl_announce_menu(const char *menu_name);

// ============================================================================
// HARDWARE MODULE CONTROL
// ============================================================================

/**
 * @brief Toggle hardware module on/off
 * @param module_name Name of module (wifi, ble, nfc, etc)
 * @param enabled New state
 */
void lvgl_toggle_module(const char *module_name, bool enabled);

/**
 * @brief Get current module state
 */
bool lvgl_get_module_state(const char *module_name);

// ============================================================================
// UI HELPERS & COLORS
// ============================================================================

// Colors (Neon Cyber-Glass palette)
#define COL_BG          0x050510 // Deep Navy/Black
#define COL_PANEL       0x151525 // Semi-transparent panel
#define COL_BTN         0x1e1e30 // Default Button
#define COL_BTN_PRESSED 0x3a3a50
#define COL_ACCENT_CYAN 0x00f3ff // Cyber Cyan
#define COL_ACCENT_MAG  0xff00ff // Neon Magenta
#define COL_RED         0xff2a2a // Alert Red
#define COL_GREEN       0x00ff9d // Matrix Green
#define COL_YELLOW      0xf9ed69
#define COL_TEXT        0xffffff
#define COL_TEXT_SEC    0x8888aa

/**
 * @brief Create a modern Icon Button (Vertical layout)
 */
lv_obj_t *create_icon_btn(
    lv_obj_t *parent, const char *icon, const char *text, 
    int w, int h, lv_event_cb_t event_cb, bool enabled = true
);

/**
 * @brief Create a standard menu button
 */
lv_obj_t *create_menu_btn(
    lv_obj_t *parent, const char *text, int x, int y, int w, int h, uint32_t color, lv_event_cb_t event_cb,
    bool enabled = true
);

/**
 * @brief Create a standard screen header with back button
 */
void create_header(lv_obj_t *scr, const char *title);

/**
 * @brief Show AI Voice Menu (used as parent for some submenus)
 */
void show_ai_voice_menu();
void show_main_menu(); // Ensure this is also available

#endif // LVGL_MENU_H
