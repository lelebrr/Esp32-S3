/**
 * @file gps_menu.h
 * GPS Menu UI for MorphNode
 *
 * Displays GPS status, coordinates, satellite info, and speed.
 *
 * Updated: 2025-12-19
 */

#ifndef GPS_MENU_H
#define GPS_MENU_H

#include "lvgl.h"
#include <Arduino.h>


// Initialize GPS Menu
void create_gps_menu(lv_obj_t *parent);

// Update UI elements (call periodically)
void update_gps_menu();

#endif // GPS_MENU_H
