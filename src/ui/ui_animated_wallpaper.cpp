/**
 * @file ui_animated_wallpaper.cpp
 * @brief Implementação de wallpapers animados
 */

#include "ui_animated_wallpaper.h"
#include "ui_themes.h"

AnimatedWallpaper animatedWallpaper;

AnimatedWallpaper::AnimatedWallpaper()
    : _container(nullptr), _enabled(false), _lastUpdate(0), _frameCount(0) {

  _config.type = ANIM_WP_NONE;
  _config.speed = 5;
  _config.density = 5;
  _config.color1 = 0x00FF00;
  _config.color2 = 0x004400;
  _config.syncWithTheme = true;

  for (int i = 0; i < 40; i++)
    _matrixChars[i] = nullptr;
  for (int i = 0; i < 30; i++)
    _stars[i] = nullptr;
}

void AnimatedWallpaper::init(lv_obj_t *parent) {
  if (!parent)
    return;

  _container = lv_obj_create(parent);
  lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_container, 0, 0);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_to_index(_container, 0); // Atrás de tudo

  _particles.init(_container);
}

void AnimatedWallpaper::update() {
  if (!_enabled || !_container)
    return;

  uint32_t now = millis();
  uint32_t interval = 1000 / (15 + _config.speed * 3); // 15-45 FPS

  if (now - _lastUpdate < interval)
    return;

  _lastUpdate = now;
  _frameCount++;

  switch (_config.type) {
  case ANIM_WP_MATRIX_RAIN:
    updateMatrixRain();
    break;
  case ANIM_WP_STARFIELD:
    updateStarfield();
    break;
  case ANIM_WP_PARTICLES:
    _particles.update();
    break;
  case ANIM_WP_FIREFLIES:
    updateFireflies();
    break;
  case ANIM_WP_SNOW:
    updateSnow();
    break;
  case ANIM_WP_BUBBLES:
    updateBubbles();
    break;
  default:
    break;
  }
}

void AnimatedWallpaper::setType(AnimatedWallpaperType type) {
  if (type == _config.type)
    return;

  clearElements();
  _config.type = type;

  if (!_enabled)
    return;

  switch (type) {
  case ANIM_WP_MATRIX_RAIN:
    setupMatrixRain();
    break;
  case ANIM_WP_STARFIELD:
    setupStarfield();
    break;
  case ANIM_WP_PARTICLES:
    _particles.setEffect(PARTICLE_FLOAT);
    {
      uint32_t colors[] = {_config.color1};
      _particles.setColors(colors, 1);
    }
    _particles.emit(20);
    break;
  case ANIM_WP_FIREFLIES:
    setupFireflies();
    break;
  case ANIM_WP_SNOW:
    setupSnow();
    break;
  case ANIM_WP_BUBBLES:
    setupBubbles();
    break;
  default:
    break;
  }
}

void AnimatedWallpaper::setEnabled(bool enable) {
  _enabled = enable;
  if (enable) {
    setType(_config.type);
  } else {
    clearElements();
  }
}

void AnimatedWallpaper::setConfig(const AnimatedWallpaperConfig &config) {
  _config = config;
  if (_config.syncWithTheme) {
    syncWithTheme();
  }
}

void AnimatedWallpaper::setColors(uint32_t color1, uint32_t color2) {
  _config.color1 = color1;
  _config.color2 = color2;
}

void AnimatedWallpaper::syncWithTheme() {
  // Usa cores do tema atual
  _config.color1 = lv_color_to32(THEME_PRIMARY);
  _config.color2 = lv_color_to32(THEME_SECONDARY);
}

void AnimatedWallpaper::clearElements() {
  for (int i = 0; i < 40; i++) {
    if (_matrixChars[i]) {
      lv_obj_del(_matrixChars[i]);
      _matrixChars[i] = nullptr;
    }
  }
  for (int i = 0; i < 30; i++) {
    if (_stars[i]) {
      lv_obj_del(_stars[i]);
      _stars[i] = nullptr;
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// MATRIX RAIN
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedWallpaper::setupMatrixRain() {
  if (!_container)
    return;

  int count = 10 + _config.density * 3; // 10-40 colunas
  if (count > 40)
    count = 40;

  for (int i = 0; i < count; i++) {
    _matrixChars[i] = lv_label_create(_container);
    lv_obj_set_style_text_font(_matrixChars[i], &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_matrixChars[i], lv_color_hex(_config.color1),
                                0);
    lv_label_set_text(_matrixChars[i], "0");
    lv_obj_set_pos(_matrixChars[i], random(368), random(-200, 0));
  }
}

void AnimatedWallpaper::updateMatrixRain() {
  static const char *chars = "01アイウエオカキクケコ";
  int charLen = strlen(chars) / 3; // UTF-8

  for (int i = 0; i < 40 && _matrixChars[i]; i++) {
    int16_t y = lv_obj_get_y(_matrixChars[i]);
    y += 3 + _config.speed;

    if (y > 448) {
      y = random(-100, -20);
      lv_obj_set_x(_matrixChars[i], random(368));
    }

    lv_obj_set_y(_matrixChars[i], y);

    // Muda caractere aleatoriamente
    if (random(10) < 3) {
      char c[2] = {(char)('0' + random(10)), '\0'};
      lv_label_set_text(_matrixChars[i], c);
    }

    // Fade baseado na posição
    uint8_t opa = 255 - (y * 255 / 448);
    lv_obj_set_style_opa(_matrixChars[i], opa, 0);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// STARFIELD
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedWallpaper::setupStarfield() {
  if (!_container)
    return;

  int count = 10 + _config.density * 2;
  if (count > 30)
    count = 30;

  for (int i = 0; i < count; i++) {
    _stars[i] = lv_obj_create(_container);
    int size = random(2, 5);
    lv_obj_set_size(_stars[i], size, size);
    lv_obj_set_style_radius(_stars[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_stars[i], lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(_stars[i], 0, 0);
    lv_obj_set_pos(_stars[i], random(368), random(448));
    lv_obj_clear_flag(_stars[i],
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
}

void AnimatedWallpaper::updateStarfield() {
  for (int i = 0; i < 30 && _stars[i]; i++) {
    int16_t x = lv_obj_get_x(_stars[i]);
    int16_t y = lv_obj_get_y(_stars[i]);

    // Move do centro para fora
    int16_t cx = 184, cy = 224;
    float dx = x - cx;
    float dy = y - cy;
    float speed = 1 + _config.speed * 0.5 + lv_obj_get_width(_stars[i]) * 0.5;

    x += dx * speed / 50;
    y += dy * speed / 50;

    // Reset quando sai da tela
    if (x < 0 || x > 368 || y < 0 || y > 448) {
      x = cx + random(-20, 20);
      y = cy + random(-20, 20);
      lv_obj_set_size(_stars[i], random(2, 5), random(2, 5));
    }

    lv_obj_set_pos(_stars[i], x, y);

    // Pulsação
    uint8_t opa = 150 + (sin(_frameCount * 0.1 + i) + 1) * 50;
    lv_obj_set_style_opa(_stars[i], opa, 0);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// FIREFLIES
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedWallpaper::setupFireflies() {
  if (!_container)
    return;

  int count = 5 + _config.density;
  if (count > 15)
    count = 15;

  for (int i = 0; i < count; i++) {
    _stars[i] = lv_obj_create(_container);
    lv_obj_set_size(_stars[i], 8, 8);
    lv_obj_set_style_radius(_stars[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_stars[i], lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_border_width(_stars[i], 0, 0);
    lv_obj_set_style_shadow_width(_stars[i], 15, 0);
    lv_obj_set_style_shadow_color(_stars[i], lv_color_hex(0xFFFF00), 0);
    lv_obj_set_pos(_stars[i], random(368), random(448));
    lv_obj_clear_flag(_stars[i],
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
}

void AnimatedWallpaper::updateFireflies() {
  for (int i = 0; i < 15 && _stars[i]; i++) {
    int16_t x = lv_obj_get_x(_stars[i]);
    int16_t y = lv_obj_get_y(_stars[i]);

    // Movimento aleatório suave
    x += random(-3, 4);
    y += random(-3, 4);

    // Mantém na tela
    x = constrain(x, 10, 358);
    y = constrain(y, 10, 438);

    lv_obj_set_pos(_stars[i], x, y);

    // Pisca
    uint8_t opa = (sin(_frameCount * 0.2 + i * 1.5) + 1) * 127;
    lv_obj_set_style_opa(_stars[i], opa, 0);
    lv_obj_set_style_shadow_opa(_stars[i], opa, 0);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// SNOW
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedWallpaper::setupSnow() {
  if (!_container)
    return;

  int count = 15 + _config.density * 3;
  if (count > 30)
    count = 30;

  for (int i = 0; i < count; i++) {
    _stars[i] = lv_obj_create(_container);
    int size = random(3, 8);
    lv_obj_set_size(_stars[i], size, size);
    lv_obj_set_style_radius(_stars[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_stars[i], lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(_stars[i], 200, 0);
    lv_obj_set_style_border_width(_stars[i], 0, 0);
    lv_obj_set_pos(_stars[i], random(368), random(-50, 448));
    lv_obj_clear_flag(_stars[i],
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
}

void AnimatedWallpaper::updateSnow() {
  for (int i = 0; i < 30 && _stars[i]; i++) {
    int16_t x = lv_obj_get_x(_stars[i]);
    int16_t y = lv_obj_get_y(_stars[i]);
    int size = lv_obj_get_width(_stars[i]);

    // Cai + drift lateral
    y += 1 + size / 3 + _config.speed / 3;
    x += sin(_frameCount * 0.05 + i) * 2;

    // Reset quando sai
    if (y > 450) {
      y = random(-30, -5);
      x = random(368);
    }

    lv_obj_set_pos(_stars[i], x, y);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// BUBBLES
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedWallpaper::setupBubbles() {
  if (!_container)
    return;

  int count = 10 + _config.density * 2;
  if (count > 25)
    count = 25;

  for (int i = 0; i < count; i++) {
    _stars[i] = lv_obj_create(_container);
    int size = random(10, 25);
    lv_obj_set_size(_stars[i], size, size);
    lv_obj_set_style_radius(_stars[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(_stars[i], LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_stars[i], 2, 0);
    lv_obj_set_style_border_color(_stars[i], lv_color_hex(_config.color1), 0);
    lv_obj_set_style_border_opa(_stars[i], 150, 0);
    lv_obj_set_pos(_stars[i], random(368), random(448, 500));
    lv_obj_clear_flag(_stars[i],
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
}

void AnimatedWallpaper::updateBubbles() {
  for (int i = 0; i < 25 && _stars[i]; i++) {
    int16_t x = lv_obj_get_x(_stars[i]);
    int16_t y = lv_obj_get_y(_stars[i]);
    int size = lv_obj_get_width(_stars[i]);

    // Sobe + wobble
    y -= 1 + (25 - size) / 10 + _config.speed / 5;
    x += sin(_frameCount * 0.03 + i * 0.5) * 2;

    // Reset
    if (y < -30) {
      y = random(450, 480);
      x = random(368);
      size = random(10, 25);
      lv_obj_set_size(_stars[i], size, size);
    }

    lv_obj_set_pos(_stars[i], x, y);
  }
}
