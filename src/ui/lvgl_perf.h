#pragma once

/**
 * @file lvgl_perf.h
 * @brief Otimizações de performance para LVGL no ESP32-S3
 *
 * Inclui cache de sprites, partial update, gerenciamento de cor, etc.
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Configuração de performance
 */
struct LVGLPerfConfig {
  bool partialUpdate;    // Atualização parcial (economiza CPU)
  bool use16BitColor;    // 16-bit ao invés de 32-bit (economiza RAM)
  bool disableAntiAlias; // Desativa AA (mais rápido)
  bool usePSRAMBuffers;  // Buffers em PSRAM
  uint8_t maxFPS;        // Limitar FPS (10-60)
  bool enableGPU;        // Usar aceleração DMA2D (se disponível)
};

/**
 * @brief Cache de sprites/imagens
 */
class SpriteCache {
public:
  static const int MAX_CACHED_SPRITES = 20;

  SpriteCache();

  /**
   * @brief Inicializa o cache
   * @param maxMemory Memória máxima em bytes (0 = automático)
   */
  void begin(size_t maxMemory = 0);

  /**
   * @brief Carrega sprite em cache
   * @param path Caminho do arquivo
   * @return Ponteiro para imagem LVGL ou nullptr
   */
  const lv_img_dsc_t *load(const char *path);

  /**
   * @brief Remove sprite do cache
   */
  void unload(const char *path);

  /**
   * @brief Limpa todo o cache
   */
  void clear();

  /**
   * @brief Memória usada pelo cache
   */
  size_t getUsedMemory() const { return _usedMemory; }

  /**
   * @brief Número de sprites em cache
   */
  int getCachedCount() const { return _cachedCount; }

private:
  struct CachedSprite {
    char path[64];
    lv_img_dsc_t *image;
    size_t size;
    uint32_t lastAccess;
  };

  CachedSprite _cache[MAX_CACHED_SPRITES];
  int _cachedCount;
  size_t _usedMemory;
  size_t _maxMemory;

  int findInCache(const char *path);
  void evictOldest();
};

/**
 * @brief Gerenciador de performance LVGL
 */
class LVGLPerformance {
public:
  LVGLPerformance();

  /**
   * @brief Inicializa otimizações
   */
  void begin();

  /**
   * @brief Aplica configurações
   */
  void applyConfig(const LVGLPerfConfig &config);

  /**
   * @brief Obtém configuração atual
   */
  LVGLPerfConfig getConfig() const { return _config; }

  /**
   * @brief Ativa modo de economia (reduz FPS e qualidade)
   */
  void enablePowerSaving(bool enable);

  /**
   * @brief Ativa modo de alta performance
   */
  void enableHighPerformance(bool enable);

  /**
   * @brief FPS atual
   */
  uint8_t getCurrentFPS() const { return _currentFPS; }

  /**
   * @brief Tempo médio de renderização (ms)
   */
  uint16_t getAverageRenderTime() const { return _avgRenderTime; }

  /**
   * @brief Cache de sprites
   */
  SpriteCache *getSpriteCache() { return &_spriteCache; }

  /**
   * @brief Força garbage collection do LVGL
   */
  void forceGC();

  /**
   * @brief Limita área de refresh (para economizar)
   */
  void setRefreshArea(int16_t x, int16_t y, int16_t w, int16_t h);

  /**
   * @brief Restaura refresh de tela inteira
   */
  void resetRefreshArea();

private:
  LVGLPerfConfig _config;
  SpriteCache _spriteCache;
  uint8_t _currentFPS;
  uint16_t _avgRenderTime;
  uint32_t _frameCount;
  uint32_t _lastFPSUpdate;
  uint32_t _renderTimeSum;

  void updateMetrics();
};

extern LVGLPerformance lvglPerf;
