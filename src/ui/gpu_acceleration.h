#pragma once

/**
 * @file gpu_acceleration.h
 * @brief Aceleração de hardware usando DMA2D do ESP32-S3
 *
 * Acelera operações de cópia, preenchimento e blending de pixels
 */

#include <Arduino.h>
#include <esp_heap_caps.h>

/**
 * @brief Configuração da aceleração GPU
 */
struct GPUAccelConfig {
  bool enabled;
  bool useForFill;  // Usar para preenchimento de retângulos
  bool useForCopy;  // Usar para cópia de memória
  bool useForBlend; // Usar para alpha blending
  bool asyncMode;   // Modo assíncrono (não bloqueia CPU)
  uint8_t priority; // Prioridade DMA (0-3)
};

/**
 * @brief Acelerador de hardware GPU/DMA2D
 */
class GPUAccelerator {
public:
  GPUAccelerator();

  /**
   * @brief Inicializa o acelerador
   * @return true se GPU disponível
   */
  bool begin();

  /**
   * @brief Verifica se GPU está disponível
   */
  bool isAvailable() const { return _available; }

  /**
   * @brief Configuração
   */
  void setConfig(const GPUAccelConfig &config);
  GPUAccelConfig getConfig() const { return _config; }

  /**
   * @brief Preenche área com cor (acelerado)
   * @param dest Buffer de destino
   * @param width Largura em pixels
   * @param height Altura em pixels
   * @param color Cor RGB565
   */
  void fillRect(uint16_t *dest, uint16_t width, uint16_t height,
                uint16_t color);

  /**
   * @brief Copia bloco de memória (acelerado)
   * @param dest Destino
   * @param src Origem
   * @param size Tamanho em bytes
   */
  void copyMemory(void *dest, const void *src, size_t size);

  /**
   * @brief Copia retângulo de imagem
   * @param dest Buffer destino
   * @param src Buffer origem
   * @param width Largura
   * @param height Altura
   * @param destStride Stride do destino (bytes por linha)
   * @param srcStride Stride da origem
   */
  void copyRect(uint16_t *dest, const uint16_t *src, uint16_t width,
                uint16_t height, uint16_t destStride, uint16_t srcStride);

  /**
   * @brief Alpha blend de duas imagens
   * @param dest Buffer destino (também é o background)
   * @param src Buffer origem (foreground com alpha)
   * @param width Largura
   * @param height Altura
   * @param alpha Alpha global (0-255)
   */
  void blendRect(uint16_t *dest, const uint16_t *src, uint16_t width,
                 uint16_t height, uint8_t alpha);

  /**
   * @brief Converte ARGB8888 para RGB565
   */
  void convertARGB8888toRGB565(uint16_t *dest, const uint32_t *src,
                               size_t pixelCount);

  /**
   * @brief Aguarda operação assíncrona terminar
   */
  void waitComplete();

  /**
   * @brief Estatísticas de uso
   */
  uint32_t getOpsCount() const { return _opsCount; }
  uint32_t getBytesCopied() const { return _bytesCopied; }

private:
  GPUAccelConfig _config;
  bool _available;
  bool _busy;
  uint32_t _opsCount;
  uint32_t _bytesCopied;

  // DMA channel handle (ESP-IDF)
  void *_dmaChannel;

  bool initDMA();
  void deinitDMA();
};

extern GPUAccelerator gpuAccel;
