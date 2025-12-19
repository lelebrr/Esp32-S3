#pragma once
/**
 * @file boot_animation.h
 * @brief Animação de boot futurista para LeleWatch
 *
 * Exibe uma animação cyberpunk/matrix durante a inicialização
 */

#include <Arduino.h>
#include <lvgl.h>

// Constantes da animação
#define BOOT_ANIM_DURATION_MS 4000
#define BOOT_ANIM_STEPS 5

// Estados da animação
enum BootAnimState {
  BOOT_ANIM_LOGO,
  BOOT_ANIM_MATRIX,
  BOOT_ANIM_SCAN_LINES,
  BOOT_ANIM_LOADING,
  BOOT_ANIM_COMPLETE
};

// Mensagens de status durante boot (PROGMEM - economiza DRAM)
static const char* const boot_messages[] PROGMEM = {
    "Initializing hardware...", "Loading NEURA9 AI...",
    "Starting WiFi engine...", "Activating BLE module...", "LeleWatch Ready!"};

class BootAnimation {
public:
  BootAnimation();

  // Inicia a animação de boot
  void start();

  // Atualiza frame da animação (chamar em loop)
  // Retorna true quando completa
  bool update();

  // Verifica se animação terminou
  bool isComplete() const { return _complete; }

  // Define callback para quando terminar
  void onComplete(void (*callback)()) { _onComplete = callback; }

  // Pula animação
  void skip() { _complete = true; }

private:
  void drawLogo();
  void drawMatrixRain();
  void drawScanLines();
  void drawLoadingBar(int progress);
  void drawStatusText(const char *text);
  void drawGlitchEffect();

  // LVGL objects
  lv_obj_t *_screen;
  lv_obj_t *_logoLabel;
  lv_obj_t *_progressBar;
  lv_obj_t *_statusLabel;
  lv_obj_t *_versionLabel;
  lv_obj_t *_scanLine;       // Scanline effect

  // Animation state
  BootAnimState _state;
  unsigned long _startTime;
  unsigned long _lastFrame;
  int _frameIndex;
  int _progress;
  bool _complete;

  // Matrix rain effect
  int _rainDrops[20]; // Y positions
  
  // Typing effect
  int _typingIndex;           // Current character index
  const char* _fullLogoText;  // Full text to type
  unsigned long _lastTyping;  // Last typing update time

  // Callback
  void (*_onComplete)();
};

// Instância global
extern BootAnimation bootAnimation;
