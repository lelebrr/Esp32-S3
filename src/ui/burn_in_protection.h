#pragma once

/**
 * @file burn_in_protection.h
 * @brief Proteção contra burn-in para tela AMOLED
 *
 * Move elementos estáticos 1-2 pixels periodicamente para
 * evitar marcas permanentes no display.
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Configuração de proteção contra burn-in
 */
struct BurnInConfig {
  bool enabled;
  uint16_t interval_s; // Intervalo entre shifts (padrão 30s)
  int8_t max_offset;   // Offset máximo em pixels (padrão 2)
  bool apply_to_all;   // Aplicar a todos os elementos
};

/**
 * @brief Sistema de proteção contra burn-in
 */
class BurnInProtection {
public:
  BurnInProtection();

  /**
   * @brief Inicializa o sistema
   */
  void begin();

  /**
   * @brief Atualiza (chamar no loop principal)
   */
  void update();

  /**
   * @brief Liga/desliga proteção
   */
  void setEnabled(bool enable);
  bool isEnabled() const { return _config.enabled; }

  /**
   * @brief Define intervalo de shift
   * @param seconds Segundos entre cada shift (10-120)
   */
  void setInterval(uint16_t seconds);

  /**
   * @brief Define offset máximo
   * @param pixels Pixels de deslocamento (1-5)
   */
  void setMaxOffset(int8_t pixels);

  /**
   * @brief Registra objeto para proteção
   * @param obj Objeto LVGL a proteger
   */
  void registerObject(lv_obj_t *obj);

  /**
   * @brief Remove objeto da proteção
   */
  void unregisterObject(lv_obj_t *obj);

  /**
   * @brief Limpa todos os objetos registrados
   */
  void clearObjects();

  /**
   * @brief Obtém offset X atual
   */
  int8_t getCurrentOffsetX() const { return _currentOffsetX; }

  /**
   * @brief Obtém offset Y atual
   */
  int8_t getCurrentOffsetY() const { return _currentOffsetY; }

  /**
   * @brief Configuração
   */
  void setConfig(const BurnInConfig &config);
  BurnInConfig getConfig() const { return _config; }

private:
  BurnInConfig _config;
  uint32_t _lastShiftTime;
  int8_t _currentOffsetX;
  int8_t _currentOffsetY;
  int8_t _directionX;
  int8_t _directionY;

  static const int MAX_PROTECTED_OBJECTS = 20;
  lv_obj_t *_protectedObjects[MAX_PROTECTED_OBJECTS];
  int16_t _originalX[MAX_PROTECTED_OBJECTS];
  int16_t _originalY[MAX_PROTECTED_OBJECTS];
  int _objectCount;

  void applyShift();
  void resetPositions();
};

extern BurnInProtection burnInProtection;
