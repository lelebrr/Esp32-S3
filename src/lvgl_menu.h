#ifndef LVGL_MENU_H
#define LVGL_MENU_H

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include "gesture_sensor.h"

void setup_lvgl_menu();
void lvgl_loop();

// Gesture integration
void lvgl_send_gesture_key(uint32_t key);
uint32_t gesture_action_to_lvgl_key(GestureAction action);
lv_group_t* lvgl_get_main_group(); // Call this in loop if handle is separate

#endif
