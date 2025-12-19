#pragma once

/**
 * @file ui_lock_screen.h
 * @brief Tela de bloqueio com relógio e senha opcional
 */

#include <lvgl.h>

/**
 * @brief Tipos de desbloqueio
 */
enum LockType {
  LOCK_NONE = 0, // Sem bloqueio (swipe para desbloquear)
  LOCK_PATTERN,  // Padrão de toque (3x3 grid)
  LOCK_PIN,      // PIN numérico (4 dígitos)
  LOCK_COUNT
};

/**
 * @brief Configuração da tela de bloqueio
 */
struct LockScreenConfig {
  bool enabled;
  LockType lockType;
  char pin[5];              // PIN de 4 dígitos
  uint8_t pattern[9];       // Padrão de pontos (sequência)
  uint8_t patternLength;    // Tamanho do padrão atual
  bool showEmoji;          // Mostrar emoji na tela de bloqueio
  bool showStats;           // Mostrar estatísticas (handshakes, etc)
  uint16_t autoLockSeconds; // Tempo para auto-bloqueio (0=nunca)
};

/**
 * @brief Tela de bloqueio
 */
class LockScreen {
public:
  LockScreen();

  /**
   * @brief Inicializa a tela de bloqueio
   */
  void begin();

  /**
   * @brief Mostra a tela de bloqueio
   */
  void show();

  /**
   * @brief Tenta desbloquear
   * @return true se desbloqueou com sucesso
   */
  bool tryUnlock();

  /**
   * @brief Esconde a tela de bloqueio
   */
  void hide();

  /**
   * @brief Verifica se está bloqueado
   */
  bool isLocked() const { return _locked; }

  /**
   * @brief Verifica se está visível
   */
  bool isVisible() const { return _visible; }

  /**
   * @brief Atualiza relógio e animações
   */
  void update();

  /**
   * @brief Configuração
   */
  void setConfig(const LockScreenConfig &config);
  LockScreenConfig getConfig() const { return _config; }

  /**
   * @brief Define PIN
   */
  void setPin(const char *pin);

  /**
   * @brief Define padrão
   */
  void setPattern(const uint8_t *pattern, uint8_t length);

  /**
   * @brief Callback quando desbloqueado
   */
  void onUnlock(void (*callback)());

private:
  LockScreenConfig _config;
  lv_obj_t *_screen;
  lv_obj_t *_clockLabel;
  lv_obj_t *_dateLabel;
  lv_obj_t *_emojiLabel;
  lv_obj_t *_statusLabel;
  lv_obj_t *_unlockHint;
  lv_obj_t *_patternGrid[9];
  lv_obj_t *_pinDisplay;

  bool _locked;
  bool _visible;
  uint8_t _inputPattern[9];
  uint8_t _inputPatternLength;
  char _inputPin[5];
  uint8_t _inputPinLength;

  uint32_t _lastActivityTime;
  void (*_onUnlock)();

  void createUI();
  void updateClock();
  void handleSwipe();
  void handlePatternInput(int pointIndex);
  void handlePinInput(char digit);
  bool validatePattern();
  bool validatePin();
  void showError();
  void animateUnlock();
};

extern LockScreen lockScreen;
