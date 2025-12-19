#pragma once

/**
 * @file ui_animated_wallpaper.h
 * @brief Sistema de wallpapers animados usando partículas e efeitos
 */

#include "ui_particles.h"
#include <lvgl.h>


/**
 * @brief Tipos de wallpaper animado
 */
enum AnimatedWallpaperType {
  ANIM_WP_NONE = 0,
  ANIM_WP_MATRIX_RAIN, // Chuva de caracteres Matrix
  ANIM_WP_STARFIELD,   // Campo de estrelas
  ANIM_WP_PARTICLES,   // Partículas flutuantes
  ANIM_WP_PLASMA,      // Efeito plasma (suave)
  ANIM_WP_FIREFLIES,   // Vagalumes
  ANIM_WP_SNOW,        // Neve caindo
  ANIM_WP_BUBBLES,     // Bolhas subindo
  ANIM_WP_COUNT
};

/**
 * @brief Configuração do wallpaper animado
 */
struct AnimatedWallpaperConfig {
  AnimatedWallpaperType type;
  uint8_t speed;      // 1-10
  uint8_t density;    // 1-10
  uint32_t color1;    // Cor primária
  uint32_t color2;    // Cor secundária
  bool syncWithTheme; // Usa cores do tema atual
};

/**
 * @brief Gerenciador de wallpapers animados
 */
class AnimatedWallpaper {
public:
  AnimatedWallpaper();

  /**
   * @brief Inicializa o sistema
   * @param parent Container pai (geralmente a tela de fundo)
   */
  void init(lv_obj_t *parent);

  /**
   * @brief Atualiza a animação (chamar no loop, ~30fps)
   */
  void update();

  /**
   * @brief Define o tipo de wallpaper
   */
  void setType(AnimatedWallpaperType type);
  AnimatedWallpaperType getType() const { return _config.type; }

  /**
   * @brief Liga/desliga
   */
  void setEnabled(bool enable);
  bool isEnabled() const { return _enabled; }

  /**
   * @brief Configuração
   */
  void setConfig(const AnimatedWallpaperConfig &config);
  AnimatedWallpaperConfig getConfig() const { return _config; }

  /**
   * @brief Define cores
   */
  void setColors(uint32_t color1, uint32_t color2);

  /**
   * @brief Sincroniza cores com tema atual
   */
  void syncWithTheme();

private:
  AnimatedWallpaperConfig _config;
  lv_obj_t *_container;
  ParticleSystem _particles;
  bool _enabled;
  uint32_t _lastUpdate;
  int _frameCount;

  // Elementos específicos
  lv_obj_t *_matrixChars[40];
  lv_obj_t *_stars[30];

  void setupMatrixRain();
  void updateMatrixRain();

  void setupStarfield();
  void updateStarfield();

  void setupFireflies();
  void updateFireflies();

  void setupSnow();
  void updateSnow();

  void setupBubbles();
  void updateBubbles();

  void clearElements();
};

extern AnimatedWallpaper animatedWallpaper;
