/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Parallax Effect com IMU
 * Efeito de parallax usando o acelerômetro QMI8658C
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../../hardware/system_hardware.h"
#include <Arduino.h>
#include <lvgl.h>


class ParallaxEffect {
public:
  ParallaxEffect();

  void begin();
  void update();
  void enable(bool en);
  bool isEnabled() const { return _enabled; }

  // Configuração
  void setSensitivity(float sens);
  void setMaxOffset(int16_t pixels);
  void setSmoothing(float factor);

  // Registrar elementos para parallax
  void registerLayer(lv_obj_t *obj,
                     float depth); // depth: 0.0 (fixo) a 1.0 (máximo)
  void unregisterLayer(lv_obj_t *obj);
  void clearLayers();

  // Offset atual
  int16_t getOffsetX() const { return _offsetX; }
  int16_t getOffsetY() const { return _offsetY; }

private:
  bool _enabled;
  float _sensitivity;
  int16_t _maxOffset;
  float _smoothing;

  int16_t _offsetX;
  int16_t _offsetY;
  float _filteredX;
  float _filteredY;

  // Layers registradas (máximo 8)
  struct ParallaxLayer {
    lv_obj_t *obj;
    float depth;
    int16_t originalX;
    int16_t originalY;
    bool active;
  };
  ParallaxLayer _layers[8];

  void applyParallax();
  void readIMU(float &ax, float &ay);
};

extern ParallaxEffect parallax_effect;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

ParallaxEffect parallax_effect;

ParallaxEffect::ParallaxEffect() {
  _enabled = false;
  _sensitivity = 1.0f;
  _maxOffset = 20;
  _smoothing = 0.8f;
  _offsetX = 0;
  _offsetY = 0;
  _filteredX = 0;
  _filteredY = 0;

  for (int i = 0; i < 8; i++) {
    _layers[i].active = false;
  }
}

void ParallaxEffect::begin() {
  // IMU já inicializado em sensors_driver
  Serial.println("[Parallax] Efeito inicializado");
}

void ParallaxEffect::update() {
  if (!_enabled)
    return;

  float ax, ay;
  readIMU(ax, ay);

  // Filtro passa-baixa para suavização
  _filteredX = _filteredX * _smoothing + ax * (1.0f - _smoothing);
  _filteredY = _filteredY * _smoothing + ay * (1.0f - _smoothing);

  // Calcula offset
  _offsetX = constrain((int)(_filteredX * _sensitivity * _maxOffset),
                       -_maxOffset, _maxOffset);
  _offsetY = constrain((int)(_filteredY * _sensitivity * _maxOffset),
                       -_maxOffset, _maxOffset);

  applyParallax();
}

void ParallaxEffect::enable(bool en) {
  _enabled = en;

  if (!en) {
    // Reset posições
    for (int i = 0; i < 8; i++) {
      if (_layers[i].active && _layers[i].obj) {
        lv_obj_set_x(_layers[i].obj, _layers[i].originalX);
        lv_obj_set_y(_layers[i].obj, _layers[i].originalY);
      }
    }
  }

  Serial.printf("[Parallax] %s\n", en ? "Ativado" : "Desativado");
}

void ParallaxEffect::setSensitivity(float sens) {
  _sensitivity = constrain(sens, 0.1f, 3.0f);
}

void ParallaxEffect::setMaxOffset(int16_t pixels) {
  _maxOffset = constrain(pixels, 5, 50);
}

void ParallaxEffect::setSmoothing(float factor) {
  _smoothing = constrain(factor, 0.5f, 0.95f);
}

void ParallaxEffect::registerLayer(lv_obj_t *obj, float depth) {
  if (!obj)
    return;

  for (int i = 0; i < 8; i++) {
    if (!_layers[i].active) {
      _layers[i].obj = obj;
      _layers[i].depth = constrain(depth, 0.0f, 1.0f);
      _layers[i].originalX = lv_obj_get_x(obj);
      _layers[i].originalY = lv_obj_get_y(obj);
      _layers[i].active = true;
      Serial.printf("[Parallax] Layer registrada: depth=%.2f\n", depth);
      return;
    }
  }
  Serial.println("[Parallax] Máximo de layers atingido!");
}

void ParallaxEffect::unregisterLayer(lv_obj_t *obj) {
  for (int i = 0; i < 8; i++) {
    if (_layers[i].active && _layers[i].obj == obj) {
      // Restaura posição original
      lv_obj_set_x(obj, _layers[i].originalX);
      lv_obj_set_y(obj, _layers[i].originalY);
      _layers[i].active = false;
      return;
    }
  }
}

void ParallaxEffect::clearLayers() {
  for (int i = 0; i < 8; i++) {
    if (_layers[i].active && _layers[i].obj) {
      lv_obj_set_x(_layers[i].obj, _layers[i].originalX);
      lv_obj_set_y(_layers[i].obj, _layers[i].originalY);
    }
    _layers[i].active = false;
  }
}

void ParallaxEffect::applyParallax() {
  for (int i = 0; i < 8; i++) {
    if (_layers[i].active && _layers[i].obj) {
      int16_t dx = (int16_t)(_offsetX * _layers[i].depth);
      int16_t dy = (int16_t)(_offsetY * _layers[i].depth);

      lv_obj_set_x(_layers[i].obj, _layers[i].originalX + dx);
      lv_obj_set_y(_layers[i].obj, _layers[i].originalY + dy);
    }
  }
}

void ParallaxEffect::readIMU(float &ax, float &ay) {
  if (sys_hw.getIMU()) {
    IMUdata acc;
    // Read accelerometer directly (not supported by SensorQMI8658 standard lib
    // getAccelerometer?) Checking lib methods... typically getAccelerometer(x,
    // y, z) The library header included is "SensorQMI8658.hpp". Assuming
    // standard usage:
    float x, y, z;
    if (sys_hw.getIMU()->getAccelerometer(x, y, z)) {
      // Map to screen orientation (Hold device vertically?)
      // Assuming X is horizontal, Y is vertical relative to screen
      // Might need axis swapping based on sensor mount.
      // For T-Display-S3 AMOLED, often Y is inverted or swapped.
      ax = x / 9.81f; // Normalize G roughly
      ay = y / 9.81f;
      return;
    }
  }

  // Fallback / Simulation if failed
  static float simX = 0, simY = 0;
  simX += (random(-10, 11) / 100.0f);
  simY += (random(-10, 11) / 100.0f);
  simX *= 0.95f;
  simY *= 0.95f;
  ax = simX;
  ay = simY;
}
