#pragma once

/**
 * @file ui_menu_grid.h
 * @brief Menu principal com grid de ícones
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Item de menu com ícone e ação
 */
struct MenuItem {
  const char *icon;  // Emoji ou símbolo
  const char *label; // Texto abaixo do ícone
  void (*onClick)(); // Callback ao clicar
  bool enabled;      // Se está habilitado
};

/**
 * @brief Menu principal em grid
 */
class MenuGrid {
public:
  MenuGrid();

  /**
   * @brief Cria o menu no container pai
   */
  void create(lv_obj_t *parent);

  /**
   * @brief Mostra/oculta o menu
   */
  void show();
  void hide();
  bool isVisible() const { return _visible; }

  /**
   * @brief Atualiza a UI do menu
   */
  void update();

private:
  lv_obj_t *_container;
  lv_obj_t *_grid;
  lv_obj_t *_buttons[8];
  lv_obj_t *_labels[8];
  bool _visible;

  static void onButtonClick(lv_event_t *e);
  void createButton(int index, MenuItem *item, int row, int col);
};

// Declarações de funções de callback para os menus
void menu_scan_click();
void menu_networks_click();
void menu_attacks_click();
void menu_stats_click();
void menu_files_click();
void menu_ble_click();
void menu_settings_click();
void menu_about_click();

extern MenuGrid menuGrid;
