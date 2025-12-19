/**
 * @file ui_radial_menu.cpp
 * @brief Implementação do menu radial
 */

#include "ui_radial_menu.h"
#include "ui_themes.h"
#include <math.h>

RadialMenu radialMenu;

// Itens padrão do menu radial
static void actionWifiScan() { /* Implementar */ }
static void actionBleSpam() { /* Implementar */ }
static void actionDeauth() { /* Implementar */ }
static void actionSettings() { /* Implementar */ }
static void actionStealth() { /* Implementar */ }
static void actionPower() { /* Implementar */ }
static void actionCapture() { /* Implementar */ }
static void actionHome() { /* Implementar */ }

static const RadialMenuItem defaultItems[] = {
    {"📡", "WiFi", actionWifiScan, 0x00FF88},
    {"📶", "BLE", actionBleSpam, 0x0088FF},
    {"💀", "Deauth", actionDeauth, 0xFF0044},
    {"⚙️", "Config", actionSettings, 0x888888},
    {"🌙", "Stealth", actionStealth, 0x440000},
    {"🔋", "Power", actionPower, 0xFFAA00},
    {"📸", "Capture", actionCapture, 0x00FFFF},
    {"🏠", "Home", actionHome, 0xFFFFFF}};

RadialMenu::RadialMenu()
    : _container(nullptr), _background(nullptr), _centerCircle(nullptr),
      _itemCount(RADIAL_MENU_ITEMS), _selectedIndex(-1), _visible(false),
      _animating(false), _centerX(0), _centerY(0), _onItemSelected(nullptr) {

  for (int i = 0; i < RADIAL_MENU_ITEMS; i++) {
    _itemButtons[i] = nullptr;
    _itemLabels[i] = nullptr;
    _items[i] = defaultItems[i];
  }
}

void RadialMenu::begin() {
  // O UI será criado quando show() for chamado
}

void RadialMenu::createUI() {
  if (_container)
    return;

  // Container principal (fullscreen overlay)
  _container = lv_obj_create(lv_layer_top());
  lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(_container, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(_container, LV_OPA_50, 0);
  lv_obj_set_style_border_width(_container, 0, 0);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);

  // Background circle (blur effect simulation)
  _background = lv_obj_create(_container);
  lv_obj_set_size(_background, RADIAL_MENU_RADIUS * 2 + 60,
                  RADIAL_MENU_RADIUS * 2 + 60);
  lv_obj_set_style_radius(_background, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(_background, THEME_PANEL, 0);
  lv_obj_set_style_bg_opa(_background, LV_OPA_80, 0);
  lv_obj_set_style_border_width(_background, 2, 0);
  lv_obj_set_style_border_color(_background, THEME_PRIMARY, 0);
  lv_obj_set_style_shadow_width(_background, 30, 0);
  lv_obj_set_style_shadow_color(_background, THEME_PRIMARY, 0);
  lv_obj_set_style_shadow_opa(_background, LV_OPA_30, 0);
  lv_obj_clear_flag(_background, LV_OBJ_FLAG_SCROLLABLE);

  // Centro (circle indicator)
  _centerCircle = lv_obj_create(_background);
  lv_obj_set_size(_centerCircle, 50, 50);
  lv_obj_center(_centerCircle);
  lv_obj_set_style_radius(_centerCircle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(_centerCircle, THEME_ACCENT, 0);
  lv_obj_set_style_border_width(_centerCircle, 0, 0);
  lv_obj_clear_flag(_centerCircle, LV_OBJ_FLAG_SCROLLABLE);

  // Cria botões para cada item
  for (int i = 0; i < _itemCount; i++) {
    _itemButtons[i] = lv_obj_create(_background);
    lv_obj_set_size(_itemButtons[i], RADIAL_MENU_ICON_SIZE,
                    RADIAL_MENU_ICON_SIZE);
    lv_obj_set_style_radius(_itemButtons[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_itemButtons[i], lv_color_hex(_items[i].color),
                              0);
    lv_obj_set_style_bg_opa(_itemButtons[i], LV_OPA_80, 0);
    lv_obj_set_style_border_width(_itemButtons[i], 2, 0);
    lv_obj_set_style_border_color(_itemButtons[i], lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(_itemButtons[i], LV_OPA_0, 0);
    lv_obj_clear_flag(_itemButtons[i], LV_OBJ_FLAG_SCROLLABLE);

    // Label com ícone
    _itemLabels[i] = lv_label_create(_itemButtons[i]);
    lv_label_set_text(_itemLabels[i], _items[i].icon);
    lv_obj_center(_itemLabels[i]);
  }

  positionItems();
}

void RadialMenu::positionItems() {
  if (!_background)
    return;

  // Posiciona itens em círculo
  float angleStep = (2 * M_PI) / _itemCount;
  float startAngle = -M_PI / 2; // Começa no topo

  for (int i = 0; i < _itemCount; i++) {
    float angle = startAngle + (i * angleStep);
    int16_t x = (int16_t)(cos(angle) * RADIAL_MENU_RADIUS);
    int16_t y = (int16_t)(sin(angle) * RADIAL_MENU_RADIUS);

    lv_obj_align(_itemButtons[i], LV_ALIGN_CENTER, x, y);
  }
}

void RadialMenu::show(int16_t centerX, int16_t centerY) {
  if (_visible || _animating)
    return;

  createUI();

  _centerX = centerX;
  _centerY = centerY;
  _selectedIndex = -1;

  // Posiciona o background no centro do toque
  lv_obj_align(_background, LV_ALIGN_TOP_LEFT,
               centerX - (RADIAL_MENU_RADIUS + 30),
               centerY - (RADIAL_MENU_RADIUS + 30));

  lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
  _visible = true;

  animateShow();
}

void RadialMenu::hide() {
  if (!_visible)
    return;

  animateHide();
  _visible = false;
}

void RadialMenu::update() {
  // Animações são gerenciadas pelo LVGL
}

int RadialMenu::selectByPosition(int16_t touchX, int16_t touchY) {
  if (!_visible)
    return -1;

  // Calcula distância do centro
  float dx = touchX - _centerX;
  float dy = touchY - _centerY;
  float distance = sqrt(dx * dx + dy * dy);

  // Se está muito perto do centro, não seleciona nada
  if (distance < 30) {
    clearHighlight();
    _selectedIndex = -1;
    return -1;
  }

  // Calcula ângulo e determina qual item
  float angle = atan2(dy, dx);
  float normalizedAngle = angle + M_PI / 2; // Ajusta para começar do topo
  if (normalizedAngle < 0)
    normalizedAngle += 2 * M_PI;

  int index = (int)(normalizedAngle / (2 * M_PI / _itemCount)) % _itemCount;

  if (index != _selectedIndex) {
    clearHighlight();
    _selectedIndex = index;
    highlightItem(index);
  }

  return index;
}

void RadialMenu::executeAction(int index) {
  if (index >= 0 && index < _itemCount && _items[index].callback) {
    hide();
    _items[index].callback();

    if (_onItemSelected) {
      _onItemSelected(index);
    }
  }
}

void RadialMenu::setItems(const RadialMenuItem items[], int count) {
  _itemCount = (count > RADIAL_MENU_ITEMS) ? RADIAL_MENU_ITEMS : count;
  for (int i = 0; i < _itemCount; i++) {
    _items[i] = items[i];
  }
}

void RadialMenu::onItemSelected(void (*callback)(int index)) {
  _onItemSelected = callback;
}

void RadialMenu::highlightItem(int index) {
  if (index >= 0 && index < _itemCount && _itemButtons[index]) {
    lv_obj_set_style_border_opa(_itemButtons[index], LV_OPA_COVER, 0);
    lv_obj_set_style_transform_zoom(_itemButtons[index], 280, 0); // 110% zoom
    lv_obj_set_style_bg_opa(_itemButtons[index], LV_OPA_COVER, 0);
  }
}

void RadialMenu::clearHighlight() {
  for (int i = 0; i < _itemCount; i++) {
    if (_itemButtons[i]) {
      lv_obj_set_style_border_opa(_itemButtons[i], LV_OPA_0, 0);
      lv_obj_set_style_transform_zoom(_itemButtons[i], 256, 0); // 100%
      lv_obj_set_style_bg_opa(_itemButtons[i], LV_OPA_80, 0);
    }
  }
}

void RadialMenu::animateShow() {
  _animating = true;

  // Anima de escala 0 para 100%
  lv_obj_set_style_transform_zoom(_background, 0, 0);
  lv_obj_set_style_opa(_background, 0, 0);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _background);
  lv_anim_set_values(&a, 0, 256);
  lv_anim_set_time(&a, 200);
  lv_anim_set_exec_cb(&a, [](void *var, int32_t val) {
    lv_obj_set_style_transform_zoom((lv_obj_t *)var, val, 0);
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)(val * 255 / 256), 0);
  });
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
  lv_anim_set_ready_cb(&a,
                       [](lv_anim_t *anim) { radialMenu._animating = false; });
  lv_anim_start(&a);
}

void RadialMenu::animateHide() {
  _animating = true;

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _background);
  lv_anim_set_values(&a, 256, 0);
  lv_anim_set_time(&a, 150);
  lv_anim_set_exec_cb(&a, [](void *var, int32_t val) {
    lv_obj_set_style_transform_zoom((lv_obj_t *)var, val, 0);
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)(val * 255 / 256), 0);
  });
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
    lv_obj_add_flag(radialMenu._container, LV_OBJ_FLAG_HIDDEN);
    radialMenu._animating = false;
  });
  lv_anim_start(&a);
}
