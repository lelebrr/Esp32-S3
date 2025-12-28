/**
 * @file lvgl_menu.h
 * @brief Monster S3 - Professional LVGL Menu System
 * 
 * 8 Main Categories with deep submenus, cyberpunk neon styling,
 * LED synchronization, and TTS voice announcements.
 * 
 * @author Monster S3 Team
 * @date 2025-12-28
 */

#ifndef LVGL_MENU_H
#define LVGL_MENU_H

#include <Arduino.h>
#include <lvgl.h>
#include "gesture_sensor.h"

// ============================================================================
// SCREEN TYPES - All menu screens
// ============================================================================
typedef enum {
    SCREEN_MAIN = 0,
    
    // Main 8 categories
    SCREEN_QUICK_ATTACK,
    SCREEN_WIFI,
    SCREEN_BLE,
    SCREEN_NFC,
    SCREEN_SUBGHZ,
    SCREEN_IR,
    SCREEN_HARDWARE,
    SCREEN_AI_VOICE,
    
    // WiFi submenus
    SCREEN_WIFI_EVIL_TWIN,
    
    // Hardware submenus
    SCREEN_HW_MODULES,
    SCREEN_HW_LEDS,
    SCREEN_HW_ENERGY,
    SCREEN_HW_RESET,
    
    // AI & Voz submenus
    SCREEN_AI_MODE,
    SCREEN_AI_VOICE_SETTINGS,
    
    // Generic
    SCREEN_STATUS,
    SCREEN_SETTINGS

} ScreenType;

// ============================================================================
// PUBLIC API
// ============================================================================

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
lv_group_t* lvgl_get_main_group();

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
void lvgl_announce_menu(const char* menu_name);

// ============================================================================
// HARDWARE MODULE CONTROL
// ============================================================================

/**
 * @brief Toggle hardware module on/off
 * @param module_name Name of module (wifi, ble, nfc, etc)
 * @param enabled New state
 */
void lvgl_toggle_module(const char* module_name, bool enabled);

/**
 * @brief Get current module state
 */
bool lvgl_get_module_state(const char* module_name);

#endif // LVGL_MENU_H
