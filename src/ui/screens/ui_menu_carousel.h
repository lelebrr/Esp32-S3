#pragma once

/**
 * @file ui_menu_carousel.h
 * @brief Menu principal estilo Lele - Uma categoria por tela
 * 
 * Design:
 * - Ícone grande (120px) centralizado
 * - Nome da categoria abaixo
 * - Navegação: Swipe horizontal ou botões PREV/NEXT
 * - Tap no centro entra na categoria
 */

#include <Arduino.h>
#include <lvgl.h>
#include <vector>

// Cores tema
#define COLOR_BG_DARK      0x0D0D0D
#define COLOR_CYAN         0x00FFF5
#define COLOR_GREEN        0x00FF41
#define COLOR_RED          0xFF0055
#define COLOR_PURPLE       0x9D00FF
#define COLOR_ORANGE       0xFF6600
#define COLOR_WHITE        0xFFFFFF
#define COLOR_GRAY         0x666666

/**
 * @brief Item do menu carousel
 */
struct CarouselItem {
  const char *icon;      // Emoji 120px
  const char *name;      // Nome da categoria
  uint32_t color;        // Cor de destaque
  void (*onClick)();     // Callback ao selecionar
  bool enabled;
};

/**
 * @brief Menu Carousel - Uma função por tela
 */
class MenuCarousel {
public:
  MenuCarousel();

  /** Cria o carousel no container pai */
  void create(lv_obj_t *parent);

  /** Navegação */
  void next();
  void prev();
  void select();

  /** Vai para índice específico */
  void goTo(int index);
  
  /** Getters */
  int getCurrentIndex() const { return _currentIndex; }
  int getItemCount() const { return _items.size(); }

  /** Mostra/oculta */
  void show();
  void hide();
  bool isVisible() const { return _visible; }

  /** Update */
  void update();

private:
  lv_obj_t *_container;
  lv_obj_t *_iconLabel;
  lv_obj_t *_nameLabel;
  lv_obj_t *_indicatorContainer;
  lv_obj_t *_prevBtn;
  lv_obj_t *_nextBtn;
  lv_obj_t *_selectBtn;
  
  std::vector<CarouselItem> _items;
  int _currentIndex;
  bool _visible;
  
  // Swipe detection
  lv_point_t _touchStart;
  bool _swiping;

  void updateDisplay();
  void createIndicators();
  void updateIndicators();
  void animateTransition(bool toRight);
  
  static void onPrevClick(lv_event_t *e);
  static void onNextClick(lv_event_t *e);
  static void onSelectClick(lv_event_t *e);
  static void onSwipeEvent(lv_event_t *e);
};

// Callbacks de categoria
void category_wifi_click();
void category_ble_click();
void category_ir_click();
void category_usb_click();
void category_files_click();
void category_settings_click();
void category_about_click();

extern MenuCarousel menuCarousel;
