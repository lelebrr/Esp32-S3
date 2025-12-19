#pragma once

/**
 * @file ui_particles.h
 * @brief Sistema de partículas para efeitos visuais futurísticos
 */

#include <Arduino.h>
#include <lvgl.h>

// Configuração de partículas (Otimizado)
#define MAX_PARTICLES 30  // Reduzido de 50 para economizar DRAM
#define PARTICLE_MIN_SIZE 2
#define PARTICLE_MAX_SIZE 6

/**
 * @brief Tipos de efeito de partícula
 */
enum ParticleEffect {
  PARTICLE_FLOAT,   // Flutuando suavemente
  PARTICLE_RISE,    // Subindo
  PARTICLE_FALL,    // Caindo (matrix rain)
  PARTICLE_EXPLODE, // Explosão radial
  PARTICLE_ORBIT,   // Orbitando ponto central
  PARTICLE_SPARKLE  // Brilho estático
};

/**
 * @brief Estrutura de uma partícula
 */
struct Particle {
  float x, y;       // Posição
  float vx, vy;     // Velocidade
  float size;       // Tamanho
  uint32_t color;   // Cor (hex)
  uint8_t alpha;    // Transparência (0-255)
  uint16_t life;    // Vida restante (frames)
  uint16_t maxLife; // Vida máxima
  bool active;      // Se está ativa
};

/**
 * @brief Sistema de partículas
 */
class ParticleSystem {
public:
  ParticleSystem();

  /**
   * @brief Inicializa o sistema com um container LVGL
   */
  void init(lv_obj_t *parent);

  /**
   * @brief Atualiza todas as partículas (chamar no loop)
   */
  void update();

  /**
   * @brief Define o efeito atual
   */
  void setEffect(ParticleEffect effect);

  /**
   * @brief Emite novas partículas
   * @param count Número de partículas
   * @param x Centro X (opcional, usa centro da tela)
   * @param y Centro Y (opcional, usa centro da tela)
   */
  void emit(int count, int x = -1, int y = -1);

  /**
   * @brief Liga/desliga emissão contínua
   */
  void setContinuous(bool enable, int rate = 2);

  /**
   * @brief Define paleta de cores
   */
  void setColors(const uint32_t *colors, int count);

  /**
   * @brief Limpa todas as partículas
   */
  void clear();

  /**
   * @brief Mostra/oculta o sistema
   */
  void show();
  void hide();

private:
  lv_obj_t *_container;
  lv_obj_t *_particleObjs[MAX_PARTICLES];
  Particle _particles[MAX_PARTICLES];

  ParticleEffect _effect;
  bool _continuous;
  int _emitRate;
  int _frameCount;

  uint32_t _colors[8];
  int _colorCount;

  int _screenWidth;
  int _screenHeight;

  void spawnParticle(int index, int x, int y);
  void updateParticle(int index);
  uint32_t getRandomColor();
};

extern ParticleSystem particles;
