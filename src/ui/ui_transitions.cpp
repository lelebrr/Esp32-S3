/**
 * @file ui_transitions.cpp
 * @brief Implementação do sistema de transições animadas
 */

#include "ui_transitions.h"
#include "ui_themes.h"

UITransitions uiTransitions;

UITransitions::UITransitions()
    : _currentScreen(nullptr), _nextScreen(nullptr), _transitioning(false),
      _onComplete(nullptr) {
  _config.type = TRANSITION_FADE;
  _config.duration_ms = 300;
  _config.enableOnAllScreens = true;
}

void UITransitions::begin() {
  // Inicialização se necessário
}

void UITransitions::setDefaultTransition(TransitionType type,
                                         uint16_t duration_ms) {
  _config.type = type;
  _config.duration_ms = duration_ms;
}

void UITransitions::transitionOut(lv_obj_t *screen, void (*onComplete)()) {
  if (!screen || _transitioning)
    return;

  _currentScreen = screen;
  _onComplete = onComplete;
  _transitioning = true;

  switch (_config.type) {
  case TRANSITION_FADE:
    applyFadeOut();
    break;
  case TRANSITION_SLIDE_LEFT:
    applySlideOut(-1);
    break;
  case TRANSITION_SLIDE_RIGHT:
    applySlideOut(1);
    break;
  case TRANSITION_SLIDE_UP:
    applySlideOut(-2);
    break;
  case TRANSITION_SLIDE_DOWN:
    applySlideOut(2);
    break;
  case TRANSITION_ZOOM_OUT:
    applyZoomOut();
    break;
  case TRANSITION_GLITCH:
    applyGlitch();
    break;
  default:
    _transitioning = false;
    if (onComplete)
      onComplete();
    break;
  }
}

void UITransitions::transitionIn(lv_obj_t *screen) {
  if (!screen)
    return;

  _nextScreen = screen;
  _transitioning = true;

  switch (_config.type) {
  case TRANSITION_FADE:
    applyFadeIn();
    break;
  case TRANSITION_SLIDE_LEFT:
    applySlideIn(-1);
    break;
  case TRANSITION_SLIDE_RIGHT:
    applySlideIn(1);
    break;
  case TRANSITION_SLIDE_UP:
    applySlideIn(-2);
    break;
  case TRANSITION_SLIDE_DOWN:
    applySlideIn(2);
    break;
  case TRANSITION_ZOOM_IN:
    applyZoomIn();
    break;
  case TRANSITION_GLITCH:
    applyGlitch();
    break;
  default:
    _transitioning = false;
    break;
  }
}

void UITransitions::switchScreen(lv_obj_t *newScreen, TransitionType type,
                                 uint16_t duration_ms) {
  if (_transitioning || !newScreen)
    return;

  // Usa parâmetros ou valores padrão
  TransitionType useType = (type != TRANSITION_NONE) ? type : _config.type;
  uint16_t useDuration = (duration_ms > 0) ? duration_ms : _config.duration_ms;

  // Salva configuração temporária
  TransitionType oldType = _config.type;
  uint16_t oldDuration = _config.duration_ms;
  _config.type = useType;
  _config.duration_ms = useDuration;

  _nextScreen = newScreen;

  // Se não há tela atual, só faz entrada
  lv_obj_t *current = lv_scr_act();
  if (!current || useType == TRANSITION_NONE) {
    lv_scr_load(newScreen);
    return;
  }

  // Armazena próxima tela em variável estática para callback não-capturante
  static lv_obj_t *s_pending_screen = nullptr;
  s_pending_screen = newScreen;

  // Executa saída e depois entrada
  transitionOut(current, []() {
    if (s_pending_screen) {
      lv_scr_load(s_pending_screen);
      uiTransitions.transitionIn(s_pending_screen);
      s_pending_screen = nullptr;
    }
  });

  // Restaura config
  _config.type = oldType;
  _config.duration_ms = oldDuration;
}

// ═══════════════════════════════════════════════════════════════════════════
// FADE TRANSITIONS
// ═══════════════════════════════════════════════════════════════════════════

void UITransitions::fadeOutCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_style_opa(obj, (lv_opa_t)value, 0);
}

void UITransitions::fadeInCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_style_opa(obj, (lv_opa_t)value, 0);
}

void UITransitions::applyFadeOut() {
  if (!_currentScreen)
    return;

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _currentScreen);
  lv_anim_set_values(&a, 255, 0);
  lv_anim_set_time(&a, _config.duration_ms / 2);
  lv_anim_set_exec_cb(&a, fadeOutCb);
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
    uiTransitions._transitioning = false;
    if (uiTransitions._onComplete)
      uiTransitions._onComplete();
  });
  lv_anim_start(&a);
}

void UITransitions::applyFadeIn() {
  if (!_nextScreen)
    return;

  // Começa invisível
  lv_obj_set_style_opa(_nextScreen, 0, 0);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _nextScreen);
  lv_anim_set_values(&a, 0, 255);
  lv_anim_set_time(&a, _config.duration_ms / 2);
  lv_anim_set_exec_cb(&a, fadeInCb);
  lv_anim_set_ready_cb(
      &a, [](lv_anim_t *anim) { uiTransitions._transitioning = false; });
  lv_anim_start(&a);
}

// ═══════════════════════════════════════════════════════════════════════════
// SLIDE TRANSITIONS
// ═══════════════════════════════════════════════════════════════════════════

void UITransitions::slideOutCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_x(obj, value);
}

void UITransitions::slideInCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_x(obj, value);
}

void UITransitions::applySlideOut(int8_t direction) {
  if (!_currentScreen)
    return;

  int16_t endX = 0;
  int16_t endY = 0;

  // direction: -1=left, 1=right, -2=up, 2=down
  if (direction == -1)
    endX = -lv_obj_get_width(_currentScreen);
  else if (direction == 1)
    endX = lv_obj_get_width(_currentScreen);
  else if (direction == -2)
    endY = -lv_obj_get_height(_currentScreen);
  else if (direction == 2)
    endY = lv_obj_get_height(_currentScreen);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _currentScreen);
  lv_anim_set_values(&a, 0, (direction == -2 || direction == 2) ? endY : endX);
  lv_anim_set_time(&a, _config.duration_ms);
  lv_anim_set_exec_cb(
      &a,
      (direction == -2 || direction == 2)
          ? [](void *var, int32_t val) { lv_obj_set_y((lv_obj_t *)var, val); }
          : slideOutCb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
    uiTransitions._transitioning = false;
    if (uiTransitions._onComplete)
      uiTransitions._onComplete();
  });
  lv_anim_start(&a);
}

void UITransitions::applySlideIn(int8_t direction) {
  if (!_nextScreen)
    return;

  int16_t startX = 0;
  int16_t startY = 0;

  // direction: -1=left (vem da direita), 1=right (vem da esquerda)
  if (direction == -1)
    startX = lv_obj_get_width(_nextScreen);
  else if (direction == 1)
    startX = -lv_obj_get_width(_nextScreen);
  else if (direction == -2)
    startY = lv_obj_get_height(_nextScreen);
  else if (direction == 2)
    startY = -lv_obj_get_height(_nextScreen);

  // Posiciona fora da tela
  if (direction == -2 || direction == 2) {
    lv_obj_set_y(_nextScreen, startY);
  } else {
    lv_obj_set_x(_nextScreen, startX);
  }

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _nextScreen);
  lv_anim_set_values(&a, (direction == -2 || direction == 2) ? startY : startX,
                     0);
  lv_anim_set_time(&a, _config.duration_ms);
  lv_anim_set_exec_cb(
      &a,
      (direction == -2 || direction == 2)
          ? [](void *var, int32_t val) { lv_obj_set_y((lv_obj_t *)var, val); }
          : slideInCb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
  lv_anim_set_ready_cb(
      &a, [](lv_anim_t *anim) { uiTransitions._transitioning = false; });
  lv_anim_start(&a);
}

// ═══════════════════════════════════════════════════════════════════════════
// ZOOM TRANSITIONS
// ═══════════════════════════════════════════════════════════════════════════

void UITransitions::zoomOutCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_style_transform_zoom(obj, value, 0);
  lv_obj_set_style_opa(obj, (lv_opa_t)(value * 255 / 256), 0);
}

void UITransitions::zoomInCb(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_style_transform_zoom(obj, value, 0);
  lv_obj_set_style_opa(obj, (lv_opa_t)(value * 255 / 256), 0);
}

void UITransitions::applyZoomOut() {
  if (!_currentScreen)
    return;

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _currentScreen);
  lv_anim_set_values(&a, 256, 0); // 256 = 100% zoom
  lv_anim_set_time(&a, _config.duration_ms);
  lv_anim_set_exec_cb(&a, zoomOutCb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
    uiTransitions._transitioning = false;
    if (uiTransitions._onComplete)
      uiTransitions._onComplete();
  });
  lv_anim_start(&a);
}

void UITransitions::applyZoomIn() {
  if (!_nextScreen)
    return;

  lv_obj_set_style_transform_zoom(_nextScreen, 0, 0);
  lv_obj_set_style_opa(_nextScreen, 0, 0);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _nextScreen);
  lv_anim_set_values(&a, 0, 256);
  lv_anim_set_time(&a, _config.duration_ms);
  lv_anim_set_exec_cb(&a, zoomInCb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
    lv_obj_set_style_transform_zoom((lv_obj_t *)anim->var, 256, 0);
    uiTransitions._transitioning = false;
  });
  lv_anim_start(&a);
}

// ═══════════════════════════════════════════════════════════════════════════
// GLITCH EFFECT (para tema Hacker Hollywood)
// ═══════════════════════════════════════════════════════════════════════════

void UITransitions::applyGlitch() {
  if (!_currentScreen && !_nextScreen)
    return;

  lv_obj_t *target = _currentScreen ? _currentScreen : _nextScreen;

  // Efeito glitch: pisca rápido + offset aleatório
  for (int i = 0; i < 5; i++) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, target);
    lv_anim_set_delay(&a, i * 50);
    lv_anim_set_values(&a, 0, (i % 2 == 0) ? 255 : 128);
    lv_anim_set_time(&a, 30);
    lv_anim_set_exec_cb(&a, fadeInCb);
    if (i == 4) {
      lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) {
        lv_obj_set_style_opa((lv_obj_t *)anim->var, 255, 0);
        uiTransitions._transitioning = false;
        if (uiTransitions._onComplete)
          uiTransitions._onComplete();
      });
    }
    lv_anim_start(&a);
  }
}
