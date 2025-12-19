#pragma once

/**
 * @file ui_focus_mode.h
 * @brief Modo Focus/Minimal - Interface simplificada sem distrações
 *
 * Mostra apenas informações essenciais: hora, status do ataque atual
 */

#include <lvgl.h>

/**
 * @brief Configuração do modo Focus
 */
struct FocusModeConfig {
  bool showClock;         // Mostrar relógio
  bool showEmoji;         // Mostrar emoji
  bool showBattery;       // Mostrar bateria
  bool showAttackStatus;  // Mostrar status de ataque
  bool hideNotifications; // Esconder notificações
  bool reducedAnimations; // Reduzir animações
  bool autoDimScreen;     // Diminuir brilho automaticamente
  uint8_t dimBrightness;  // Brilho quando escurecido (%)
};

/**
 * @brief Modo Focus - Interface minimalista
 */
class FocusMode {
public:
  FocusMode();

  /**
   * @brief Inicializa o modo focus
   */
  void begin();

  /**
   * @brief Ativa o modo focus
   */
  void enable();

  /**
   * @brief Desativa o modo focus
   */
  void disable();

  /**
   * @brief Alterna on/off
   */
  void toggle();

  /**
   * @brief Verifica se está ativo
   */
  bool isEnabled() const { return _enabled; }

  /**
   * @brief Atualiza (chamar no loop)
   */
  void update();

  /**
   * @brief Cria a UI minimalista
   */
  void createUI();

  /**
   * @brief Define status de ataque para exibição
   */
  void setAttackStatus(const char *status, bool active);

  /**
   * @brief Configuração
   */
  void setConfig(const FocusModeConfig &config);
  FocusModeConfig getConfig() const { return _config; }

private:
  FocusModeConfig _config;
  lv_obj_t *_screen;
  lv_obj_t *_clockLabel;
  lv_obj_t *_emojiLabel;
  lv_obj_t *_batteryLabel;
  lv_obj_t *_attackLabel;
  lv_obj_t *_attackIndicator;

  bool _enabled;
  uint8_t _originalBrightness;
  uint32_t _lastUpdate;

  void updateClock();
  void updateBattery();
  void applyDimming();
  void restoreBrightness();
};

extern FocusMode focusMode;

// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Modo Minimal - Modo Stealth extremo
 *
 * Apenas mostra o mínimo necessário em preto e vermelho escuro
 */
class MinimalMode {
public:
  MinimalMode();

  void begin();
  void enable();
  void disable();
  bool isEnabled() const { return _enabled; }

  /**
   * @brief Define mensagem a mostrar
   */
  void setMessage(const char *msg);

  /**
   * @brief Mostra indicador de atividade (pulso vermelho)
   */
  void showActivity(bool active);

  void update();

private:
  lv_obj_t *_screen;
  lv_obj_t *_messageLabel;
  lv_obj_t *_activityDot;

  bool _enabled;
  uint32_t _lastPulse;
};

extern MinimalMode minimalMode;
