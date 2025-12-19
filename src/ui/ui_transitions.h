#pragma once

/**
 * @file ui_transitions.h
 * @brief Sistema de transições animadas entre telas LVGL
 *
 * Suporta fade, slide, zoom e mais efeitos visuais
 */

#include <lvgl.h>

/**
 * @brief Tipos de transição disponíveis
 */
enum TransitionType {
  TRANSITION_NONE = 0,    // Sem transição (instantâneo)
  TRANSITION_FADE,        // Fade in/out
  TRANSITION_SLIDE_LEFT,  // Desliza para esquerda
  TRANSITION_SLIDE_RIGHT, // Desliza para direita
  TRANSITION_SLIDE_UP,    // Desliza para cima
  TRANSITION_SLIDE_DOWN,  // Desliza para baixo
  TRANSITION_ZOOM_IN,     // Zoom de dentro para fora
  TRANSITION_ZOOM_OUT,    // Zoom de fora para dentro
  TRANSITION_GLITCH,      // Efeito glitch (para tema Hacker Hollywood)
  TRANSITION_COUNT
};

/**
 * @brief Configuração de transição
 */
struct TransitionConfig {
  TransitionType type;
  uint16_t duration_ms;    // Duração em ms (padrão 300)
  bool enableOnAllScreens; // Aplicar a todas as transições
};

/**
 * @brief Gerenciador de transições
 */
class UITransitions {
public:
  UITransitions();

  /**
   * @brief Inicializa o sistema de transições
   */
  void begin();

  /**
   * @brief Define o tipo de transição padrão
   */
  void setDefaultTransition(TransitionType type, uint16_t duration_ms = 300);

  /**
   * @brief Executa transição de saída na tela atual
   * @param screen Tela que está saindo
   * @param onComplete Callback quando a transição terminar
   */
  void transitionOut(lv_obj_t *screen, void (*onComplete)() = nullptr);

  /**
   * @brief Executa transição de entrada na nova tela
   * @param screen Nova tela entrando
   */
  void transitionIn(lv_obj_t *screen);

  /**
   * @brief Troca de tela com transição completa
   * @param newScreen Nova tela para carregar
   * @param type Tipo de transição (usa padrão se NONE)
   * @param duration_ms Duração (usa padrão se 0)
   */
  void switchScreen(lv_obj_t *newScreen, TransitionType type = TRANSITION_NONE,
                    uint16_t duration_ms = 0);

  /**
   * @brief Verifica se está em transição
   */
  bool isTransitioning() const { return _transitioning; }

  /**
   * @brief Obtém configuração atual
   */
  TransitionConfig getConfig() const { return _config; }

private:
  TransitionConfig _config;
  lv_obj_t *_currentScreen;
  lv_obj_t *_nextScreen;
  bool _transitioning;
  void (*_onComplete)();

  // Callbacks de animação
  static void fadeOutCb(void *var, int32_t value);
  static void fadeInCb(void *var, int32_t value);
  static void slideOutCb(void *var, int32_t value);
  static void slideInCb(void *var, int32_t value);
  static void zoomOutCb(void *var, int32_t value);
  static void zoomInCb(void *var, int32_t value);

  void applyFadeOut();
  void applyFadeIn();
  void applySlideOut(int8_t direction);
  void applySlideIn(int8_t direction);
  void applyZoomOut();
  void applyZoomIn();
  void applyGlitch();
};

extern UITransitions uiTransitions;
