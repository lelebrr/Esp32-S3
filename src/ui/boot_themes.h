#pragma once

/**
 * @file boot_themes.h
 * @brief 10 temas de boot screen futurísticos selecionáveis
 *
 * Cada tema pode ser escolhido nas configurações e salvo em NVS.
 * Otimizado para tela AMOLED 368x448 com cores neon cyberpunk.
 */

#include <Arduino.h>
#include <Preferences.h>
#include <lvgl.h>


// Número máximo de partículas por tema
#define MAX_BOOT_PARTICLES 30

/**
 * @brief Tipos de tema de boot
 */
enum BootTheme {
  BOOT_THEME_CYBER_DRAGON = 0, // 1. Cyber-Dragon Pulse (padrão)
  BOOT_THEME_MATRIX_RAIN,      // 2. Matrix Rain + Dragon Egg
  BOOT_THEME_HOLOGRAPHIC,      // 3. Holographic Core Reactor
  BOOT_THEME_NEURAL_BOOT,      // 4. Neural Boot Sequence (NEURA9)
  BOOT_THEME_GLITCH_CORE,      // 5. Glitch Core 2077
  BOOT_THEME_VOID_PORTAL,      // 6. Void Portal
  BOOT_THEME_TRON_LEGACY,      // 7. Tron Legacy Style
  BOOT_THEME_QUANTUM,          // 8. Quantum Boot
  BOOT_THEME_BLADE_RUNNER,     // 9. Blade Runner Rain
  BOOT_THEME_AMOLED_NEON,      // 10. Pure AMOLED Neon Strike
  BOOT_THEME_COUNT
};

/**
 * @brief Paleta de cores oficial LeleWatch
 */
#define BOOT_COLOR_CYAN lv_color_hex(0x00FFFF)    // Primária
#define BOOT_COLOR_MAGENTA lv_color_hex(0xFF00FF) // Secundária
#define BOOT_COLOR_PURPLE lv_color_hex(0x8B00FF)  // Acento roxo
#define BOOT_COLOR_WHITE lv_color_hex(0xFFFFFF)   // Destaque
#define BOOT_COLOR_BLACK lv_color_hex(0x000000)   // Fundo AMOLED
#define BOOT_COLOR_MATRIX lv_color_hex(0x00FF41)  // Verde Matrix
#define BOOT_COLOR_ORANGE lv_color_hex(0xFF6600)  // Blade Runner
#define BOOT_COLOR_BLUE lv_color_hex(0x0088FF)    // Azul elétrico
#define BOOT_COLOR_ERROR lv_color_hex(0xFF0033)   // Erro/Glitch

/**
 * @brief Estrutura de partícula
 */
struct BootParticle {
  float x, y;
  float vx, vy;
  uint8_t alpha;
  uint8_t size;
  bool active;
};

/**
 * @brief Nome e descrição de cada tema
 */
struct ThemeInfo {
  const char *name;
  const char *description;
};

static const ThemeInfo THEME_NAMES[BOOT_THEME_COUNT] = {
    {"Cyber-Dragon", "Dragao neon com pulso circular"},
    {"Matrix Rain", "Chuva Matrix + ovo de dragao"},
    {"Holographic", "Reator holografico girando"},
    {"Neural Boot", "Rede neural NEURA9"},
    {"Glitch Core", "Efeito glitch cyberpunk"},
    {"Void Portal", "Portal dimensional roxo"},
    {"Tron Legacy", "Grid 3D neon azul"},
    {"Quantum", "Atomos e particulas quanticas"},
    {"Blade Runner", "Chuva neon com reflexo"},
    {"AMOLED Neon", "Minimalista preto + raio neon"}};

/**
 * @brief Gerenciador de temas de boot
 */
class BootThemeManager {
public:
  BootThemeManager();

  /**
   * @brief Inicializa o gerenciador e carrega tema salvo do NVS
   */
  void begin();

  /**
   * @brief Inicia a animação de boot com o tema atual
   */
  void startAnimation();

  /**
   * @brief Atualiza a animação (chamar no loop)
   * @return true quando animação terminou
   */
  bool update();

  /**
   * @brief Define o tema de boot
   * @param theme Tema a usar
   * @param save Se true, salva em NVS
   */
  void setTheme(BootTheme theme, bool save = true);

  /**
   * @brief Obtém tema atual
   */
  BootTheme getTheme() const { return _currentTheme; }

  /**
   * @brief Obtém informações do tema
   */
  const ThemeInfo &getThemeInfo(BootTheme theme) const {
    return THEME_NAMES[theme];
  }

  /**
   * @brief Próximo tema (para preview nas configurações)
   */
  void nextTheme();
  void prevTheme();

  /**
   * @brief Pula a animação
   */
  void skip() { _complete = true; }

  /**
   * @brief Verifica se terminou
   */
  bool isComplete() const { return _complete; }

  /**
   * @brief Define callback ao completar
   */
  void onComplete(void (*callback)()) { _onComplete = callback; }

private:
  BootTheme _currentTheme;
  lv_obj_t *_screen;
  lv_obj_t *_logoLabel;
  lv_obj_t *_subtitleLabel;
  lv_obj_t *_progressBar;
  lv_obj_t *_statusLabel;
  lv_obj_t *_centerCircle;
  lv_obj_t *_dragonLabel;

  BootParticle _particles[MAX_BOOT_PARTICLES];
  lv_obj_t *_particleObjs[MAX_BOOT_PARTICLES];

  unsigned long _startTime;
  unsigned long _lastFrame;
  int _frameCount;
  int _progress;
  bool _complete;
  bool _initialized;

  Preferences _prefs;
  void (*_onComplete)();

  // Funções de tema específicas
  void initCyberDragon();
  void updateCyberDragon();

  void initMatrixRain();
  void updateMatrixRain();

  void initHolographic();
  void updateHolographic();

  void initNeuralBoot();
  void updateNeuralBoot();

  void initGlitchCore();
  void updateGlitchCore();

  void initVoidPortal();
  void updateVoidPortal();

  void initTronLegacy();
  void updateTronLegacy();

  void initQuantum();
  void updateQuantum();

  void initBladeRunner();
  void updateBladeRunner();

  void initAmoledNeon();
  void updateAmoledNeon();

  // Utilitários
  void createBaseScreen();
  void spawnParticle(int index, float x, float y);
  void updateParticles();
  void updateProgress();
  void saveTheme();
  void loadTheme();
};

extern BootThemeManager bootThemes;
