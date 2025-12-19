#pragma once

/**
 * @file ui_attacks.h
 * @brief Header da tela de ataques WiFi
 */

#include "../core/pin_definitions.h"
#include <Arduino.h>
#include <lvgl.h>

lv_obj_t *ui_attacks_create();
void ui_attacks_show();
void ui_attacks_refresh();
bool ui_attacks_is_active();
