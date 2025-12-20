/**
 * @file manual_menu.h
 * @brief Manual/Help Menu Class Definition
 * 
 * Defines the ManualMenu class for managing help screens
 * and navigation within the manual system.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#ifndef MANUAL_MENU_H
#define MANUAL_MENU_H

#include <lvgl.h>

/**
 * @brief Manual/Help Menu Module
 * Displays documentation for implemented attacks
 */
class ManualMenu {
public:
    static void init(lv_obj_t* parent);
    static void show_manual(const char* title, const char* text);
    
private:
    static void create_category_btn(lv_obj_t* parent, const char* label, lv_event_cb_t event_cb);
    static void event_handler_rf(lv_event_t* e);
    static void event_handler_wifi(lv_event_t* e);
    static void event_handler_ble(lv_event_t* e);
    static void event_handler_nrf(lv_event_t* e);
    static void event_handler_close(lv_event_t* e);
    
    static lv_obj_t* manual_scr;
    static lv_obj_t* text_area;
};

#endif // MANUAL_MENU_H
