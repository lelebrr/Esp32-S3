#pragma once

/**
 * @file image_compression.h
 * @brief Sistema de compressão e otimização de imagens
 *
 * Suporta compressão PNG, conversão para LVGL .bin, e cache
 */

#include <Arduino.h>
#include <FS.h>

/**
 * @brief Formato de imagem suportado
 */
enum ImageFormat {
  IMG_FORMAT_PNG,
  IMG_FORMAT_JPEG,
  IMG_FORMAT_BMP,
  IMG_FORMAT_LVGL_BIN, // Formato binário LVGL otimizado
  IMG_FORMAT_RLE,      // Run-Length Encoding
  IMG_FORMAT_UNKNOWN
};

/**
 * @brief Informações de uma imagem
 */
struct ImageInfo {
  char filename[64];
  uint16_t width;
  uint16_t height;
  uint8_t bitDepth; // 8, 16, 24, 32
  uint32_t fileSize;
  uint32_t rawSize; // Tamanho descomprimido
  float compressionRatio;
  ImageFormat format;
  bool hasAlpha;
};

/**
 * @brief Configuração de compressão
 */
struct CompressionConfig {
  uint8_t quality;     // 1-100 para JPEG
  bool dither;         // Dithering ao reduzir cores
  bool quantize16bit;  // Converter para 16-bit (RGB565)
  bool removeAlpha;    // Remover canal alpha
  bool generateMipmap; // Gerar versões menores
};

/**
 * @brief Sistema de compressão de imagens
 */
class ImageCompressor {
public:
  ImageCompressor();

  /**
   * @brief Inicializa o compressor
   */
  void begin();

  /**
   * @brief Analisa uma imagem
   * @param path Caminho do arquivo
   * @param info Ponteiro para receber informações
   * @return true se sucesso
   */
  bool analyzeImage(const char *path, ImageInfo *info);

  /**
   * @brief Converte PNG para formato LVGL .bin
   * @param srcPath Arquivo PNG de origem
   * @param destPath Arquivo .bin de destino
   * @return true se sucesso
   */
  bool convertToLVGLBin(const char *srcPath, const char *destPath);

  /**
   * @brief Comprime imagem usando RLE
   * @param src Dados de origem
   * @param srcSize Tamanho original
   * @param dest Buffer de destino
   * @param maxDestSize Tamanho máximo de destino
   * @return Tamanho comprimido ou 0 se falhou
   */
  size_t compressRLE(const uint8_t *src, size_t srcSize, uint8_t *dest,
                     size_t maxDestSize);

  /**
   * @brief Descomprime dados RLE
   * @param src Dados comprimidos
   * @param srcSize Tamanho comprimido
   * @param dest Buffer de destino
   * @param maxDestSize Tamanho máximo
   * @return Tamanho descomprimido
   */
  size_t decompressRLE(const uint8_t *src, size_t srcSize, uint8_t *dest,
                       size_t maxDestSize);

  /**
   * @brief Reduz profundidade de cor (32-bit → 16-bit)
   * @param src Buffer ARGB8888
   * @param dest Buffer RGB565
   * @param pixelCount Número de pixels
   * @param dither Aplicar dithering
   */
  void quantize32to16(const uint32_t *src, uint16_t *dest, size_t pixelCount,
                      bool dither = false);

  /**
   * @brief Redimensiona imagem
   * @param src Buffer origem
   * @param srcW, srcH Dimensões origem
   * @param dest Buffer destino
   * @param destW, destH Dimensões destino
   */
  void resize(const uint16_t *src, uint16_t srcW, uint16_t srcH, uint16_t *dest,
              uint16_t destW, uint16_t destH);

  /**
   * @brief Detecta formato de arquivo
   */
  ImageFormat detectFormat(const char *path);

  /**
   * @brief Configuração
   */
  void setConfig(const CompressionConfig &config);
  CompressionConfig getConfig() const { return _config; }

  /**
   * @brief Estatísticas
   */
  uint32_t getTotalSaved() const { return _totalSaved; }
  uint32_t getFilesProcessed() const { return _filesProcessed; }

private:
  CompressionConfig _config;
  uint32_t _totalSaved;
  uint32_t _filesProcessed;

  bool readPNGHeader(File &file, ImageInfo *info);
  bool readJPEGHeader(File &file, ImageInfo *info);
  bool readBMPHeader(File &file, ImageInfo *info);
};

extern ImageCompressor imageCompressor;
