#pragma once

/**
 * @file ui_radial_menu.h
 * @brief Menu radial ativado por long press
 *
 * Círculo com 8 ações rápidas que aparece ao segurar o dedo na tela
 */

#include <lvgl.h>

#define RADIAL_MENU_ITEMS 8
#define RADIAL_MENU_RADIUS 100
#define RADIAL_MENU_ICON_SIZE 40

/**
 * @brief Estrutura de item do menu radial
 */
struct RadialMenuItem {
  const char *icon;   // Ícone (emoji ou símbolo)
  const char *label;  // Label curto
  void (*callback)(); // Função a executar
  uint32_t color;     // Cor do item
};

/**
 * @brief Menu Radial com ações rápidas
 */
class RadialMenu {
public:
  RadialMenu();

  /**
   * @brief Inicializa o menu radial
   */
  void begin();

  /**
   * @brief Mostra o menu na posição especificada
   * @param centerX Centro X do menu
   * @param centerY Centro Y do menu
   */
  void show(int16_t centerX, int16_t centerY);

  /**
   * @brief Esconde o menu
   */
  void hide();

  /**
   * @brief Verifica se está visível
   */
  bool isVisible() const { return _visible; }

  /**
   * @brief Atualiza animação (chamar no loop)
   */
  void update();

  /**
   * @brief Seleciona item baseado na posição do dedo
   * @param touchX Posição X do toque
   * @param touchY Posição Y do toque
   * @return Índice do item selecionado (-1 se nenhum)
   */
  int selectByPosition(int16_t touchX, int16_t touchY);

  /**
   * @brief Executa a ação do item selecionado
   * @param index Índice do item
   */
  void executeAction(int index);

  /**
   * @brief Define os itens do menu
   */
  void setItems(const RadialMenuItem items[], int count);

  /**
   * @brief Define callback para quando um item é selecionado
   */
  void onItemSelected(void (*callback)(int index));

private:
  lv_obj_t *_container;
  lv_obj_t *_background;
  lv_obj_t *_centerCircle;
  lv_obj_t *_itemButtons[RADIAL_MENU_ITEMS];
  lv_obj_t *_itemLabels[RADIAL_MENU_ITEMS];

  RadialMenuItem _items[RADIAL_MENU_ITEMS];
  int _itemCount;
  int _selectedIndex;
  bool _visible;
  bool _animating;

  int16_t _centerX;
  int16_t _centerY;

  void (*_onItemSelected)(int index);

  void createUI();
  void positionItems();
  void animateShow();
  void animateHide();
  void highlightItem(int index);
  void clearHighlight();
};

extern RadialMenu radialMenu;
