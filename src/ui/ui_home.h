#pragma once

/**
 * @file ui_home.h
 * @brief Nova tela principal futurística com menus organizados
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Categorias do menu principal
 */
enum HomeMenuCategory {
  MENU_WIFI = 0,
  MENU_BLE,
  MENU_VOICE,
  MENU_AI,
  MENU_TOOLS,
  MENU_SETTINGS,
  MENU_COUNT
};

/**
 * @brief Inicializa a tela home
 */
void ui_home_init();

/**
 * @brief Mostra a tela home
 */
void ui_home_show();

/**
 * @brief Atualiza elementos na home
 */
void ui_home_update();

/**
 * @brief Define texto do assistente
 */
void ui_home_set_assistant_text(const char *text);

/**
 * @brief Abre um menu específico
 */
void ui_home_open_menu(HomeMenuCategory category);
