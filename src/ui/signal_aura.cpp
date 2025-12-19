/**
 * @file signal_aura.cpp
 * @brief Implementação da aura de força de sinal
 */

#include "signal_aura.h"
#include "ui_themes.h"
#include <math.h>

SignalAura signalAura;

SignalAura::SignalAura()
    : _container(nullptr), _strength(0), _color(0x00FF88), _visible(false),
      _pulsing(true), _frameCount(0) {
  for (int i = 0; i < 3; i++) {
    _auraRings[i] = nullptr;
  }
}

void SignalAura::init(lv_obj_t *parent) {
  if (!parent)
    return;

  // Container (transparente)
  _container = lv_obj_create(parent);
  lv_obj_set_size(_container, 120, 120);
  lv_obj_center(_container);
  lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_container, 0, 0);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

  // 3 anéis concêntricos (do maior ao menor)
  int sizes[] = {120, 100, 80};
  for (int i = 0; i < 3; i++) {
    _auraRings[i] = lv_obj_create(_container);
    lv_obj_set_size(_auraRings[i], sizes[i], sizes[i]);
    lv_obj_center(_auraRings[i]);
    lv_obj_set_style_radius(_auraRings[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(_auraRings[i], LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_auraRings[i], 2, 0);
    lv_obj_set_style_border_color(_auraRings[i], lv_color_hex(_color), 0);
    lv_obj_set_style_border_opa(_auraRings[i], LV_OPA_0, 0);
    lv_obj_clear_flag(_auraRings[i],
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
}

void SignalAura::update() {
  if (!_visible || !_container)
    return;

  _frameCount++;

  if (_pulsing) {
    // Efeito pulsante
    float pulse = (sin(_frameCount * 0.1) + 1) / 2; // 0 a 1

    for (int i = 0; i < 3; i++) {
      if (_auraRings[i]) {
        uint8_t baseOpa = (_strength * (3 - i) / 3);
        uint8_t finalOpa = baseOpa + (uint8_t)(pulse * 30);
        if (finalOpa > 255)
          finalOpa = 255;
        lv_obj_set_style_border_opa(_auraRings[i], finalOpa, 0);
      }
    }
  }
}

void SignalAura::setSignalStrength(uint8_t strength) {
  _strength = (strength > 100) ? 100 : strength;
  _color = getColorForStrength(_strength);
  updateRings();
}

void SignalAura::setRSSI(int8_t rssi) {
  // Converte RSSI para porcentagem
  // -30 dBm = 100%, -90 dBm = 0%
  int strength = 0;
  if (rssi >= -30) {
    strength = 100;
  } else if (rssi <= -90) {
    strength = 0;
  } else {
    strength = (rssi + 90) * 100 / 60;
  }
  setSignalStrength((uint8_t)strength);
}

void SignalAura::show() {
  _visible = true;
  if (_container) {
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
  }
  updateRings();
}

void SignalAura::hide() {
  _visible = false;
  if (_container) {
    lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
  }
}

void SignalAura::setColor(uint32_t color) {
  _color = color;
  updateRings();
}

void SignalAura::setPulsing(bool enable) { _pulsing = enable; }

void SignalAura::updateRings() {
  if (!_container)
    return;

  lv_color_t col = lv_color_hex(_color);

  for (int i = 0; i < 3; i++) {
    if (!_auraRings[i])
      continue;

    lv_obj_set_style_border_color(_auraRings[i], col, 0);

    // Opacidade baseada na força do sinal
    // Anel mais interno = mais visível, externo = menos
    uint8_t ringThreshold = (2 - i) * 33; // 66, 33, 0
    uint8_t opa = 0;

    if (_strength > ringThreshold) {
      opa = (_strength - ringThreshold) * 255 / 33;
      if (opa > 180)
        opa = 180; // Cap para não ficar muito intenso
    }

    lv_obj_set_style_border_opa(_auraRings[i], opa, 0);
  }
}

uint32_t SignalAura::getColorForStrength(uint8_t strength) {
  // Verde (forte) -> Amarelo (médio) -> Vermelho (fraco)
  if (strength >= 70) {
    return 0x00FF88; // Verde
  } else if (strength >= 40) {
    return 0xFFAA00; // Amarelo/Laranja
  } else if (strength >= 20) {
    return 0xFF6600; // Laranja
  } else {
    return 0xFF0044; // Vermelho
  }
}
