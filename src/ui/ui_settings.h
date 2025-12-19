#pragma once

/**
 * @file ui_settings.h
 * @brief Header da tela de configurações (menu principal com 5 categorias)
 */

#include "../core/pin_definitions.h"
#include <Arduino.h>
#include <lvgl.h>

// Menu principal de configurações
lv_obj_t *ui_settings_create();
void ui_settings_show();
void ui_settings_update();
bool ui_settings_is_active();
