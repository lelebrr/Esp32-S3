/**
 * @file image_compression.cpp
 * @brief Implementação do sistema de compressão de imagens
 */

#include "image_compression.h"
#include <LittleFS.h>
#include <SD.h>
#include <debug_log.h>

ImageCompressor imageCompressor;

ImageCompressor::ImageCompressor() : _totalSaved(0), _filesProcessed(0) {

    _config.quality = 85;
    _config.dither = true;
    _config.quantize16bit = true;
    _config.removeAlpha = false;
    _config.generateMipmap = false;
}

void ImageCompressor::begin() { LOG_DEBUG("IMG_COMP: Image compressor initialized"); }

bool ImageCompressor::analyzeImage(const char *path, ImageInfo *info) {
    if (!info) return false;

    memset(info, 0, sizeof(ImageInfo));
    strncpy(info->filename, path, sizeof(info->filename) - 1);

    // Tenta abrir do SD ou LittleFS
    File file;
    if (SD.exists(path)) {
        file = SD.open(path, "r");
    } else if (LittleFS.exists(path)) {
        file = LittleFS.open(path, "r");
    }

    if (!file) {
        LOG_ERROR("IMG_COMP: Failed to open: %s", path);
        return false;
    }

    info->fileSize = file.size();
    info->format = detectFormat(path);

    bool success = false;
    switch (info->format) {
        case IMG_FORMAT_PNG: success = readPNGHeader(file, info); break;
        case IMG_FORMAT_JPEG: success = readJPEGHeader(file, info); break;
        case IMG_FORMAT_BMP: success = readBMPHeader(file, info); break;
        default: break;
    }

    file.close();

    if (success) {
        // Calcula tamanho raw e compressão
        info->rawSize = info->width * info->height * (info->bitDepth / 8);
        if (info->rawSize > 0) { info->compressionRatio = (float)info->rawSize / (float)info->fileSize; }
        _filesProcessed++;
    }

    return success;
}

ImageFormat ImageCompressor::detectFormat(const char *path) {
    if (!path) return IMG_FORMAT_UNKNOWN;

    String p = String(path);
    p.toLowerCase();

    if (p.endsWith(".png")) return IMG_FORMAT_PNG;
    if (p.endsWith(".jpg") || p.endsWith(".jpeg")) return IMG_FORMAT_JPEG;
    if (p.endsWith(".bmp")) return IMG_FORMAT_BMP;
    if (p.endsWith(".bin")) return IMG_FORMAT_LVGL_BIN;

    return IMG_FORMAT_UNKNOWN;
}

bool ImageCompressor::readPNGHeader(File &file, ImageInfo *info) {
    // PNG signature: 89 50 4E 47 0D 0A 1A 0A
    uint8_t sig[8];
    if (file.read(sig, 8) != 8) return false;

    if (sig[0] != 0x89 || sig[1] != 'P' || sig[2] != 'N' || sig[3] != 'G') return false;

    // IHDR chunk: length(4) + type(4) + width(4) + height(4) + bitdepth(1) +
    // colortype(1)
    uint8_t ihdr[25];
    if (file.read(ihdr, 25) != 25) return false;

    // Width and height are big-endian
    info->width = (ihdr[8] << 24) | (ihdr[9] << 16) | (ihdr[10] << 8) | ihdr[11];
    info->height = (ihdr[12] << 24) | (ihdr[13] << 16) | (ihdr[14] << 8) | ihdr[15];
    info->bitDepth = ihdr[16];

    uint8_t colorType = ihdr[17];
    info->hasAlpha = (colorType == 4 || colorType == 6);

    // Ajusta bit depth para bytes por pixel
    switch (colorType) {
        case 0: info->bitDepth = 8; break;  // Grayscale
        case 2: info->bitDepth = 24; break; // RGB
        case 3: info->bitDepth = 8; break;  // Indexed
        case 4: info->bitDepth = 16; break; // Grayscale + Alpha
        case 6: info->bitDepth = 32; break; // RGBA
    }

    return true;
}

bool ImageCompressor::readJPEGHeader(File &file, ImageInfo *info) {
    // JPEG SOI marker: FF D8
    uint8_t sig[2];
    if (file.read(sig, 2) != 2) return false;

    if (sig[0] != 0xFF || sig[1] != 0xD8) return false;

    // Procura SOF0 marker (FF C0) para obter dimensões
    while (file.available()) {
        uint8_t marker[2];
        if (file.read(marker, 2) != 2) break;

        if (marker[0] != 0xFF) continue;

        if (marker[1] == 0xC0 || marker[1] == 0xC2) {
            // SOF0 ou SOF2
            uint8_t sof[7];
            if (file.read(sof, 7) != 7) break;

            info->height = (sof[3] << 8) | sof[4];
            info->width = (sof[5] << 8) | sof[6];
            info->bitDepth = sof[2] * 8; // components * 8
            info->hasAlpha = false;
            return true;
        }

        // Skip other segments
        if (marker[1] >= 0xD0 && marker[1] <= 0xD9) continue;
        if (marker[1] == 0x01) continue;

        uint8_t len[2];
        if (file.read(len, 2) != 2) break;
        uint16_t segLen = (len[0] << 8) | len[1];
        file.seek(file.position() + segLen - 2);
    }

    return false;
}

bool ImageCompressor::readBMPHeader(File &file, ImageInfo *info) {
    // BMP signature: 'BM'
    uint8_t sig[2];
    if (file.read(sig, 2) != 2) return false;

    if (sig[0] != 'B' || sig[1] != 'M') return false;

    // Skip to offset 18 for width/height
    file.seek(18);

    uint8_t dims[8];
    if (file.read(dims, 8) != 8) return false;

    // Width and height are little-endian
    info->width = dims[0] | (dims[1] << 8) | (dims[2] << 16) | (dims[3] << 24);
    info->height = dims[4] | (dims[5] << 8) | (dims[6] << 16) | (dims[7] << 24);

    // Read bit depth at offset 28
    file.seek(28);
    uint8_t bd[2];
    if (file.read(bd, 2) != 2) return false;

    info->bitDepth = bd[0] | (bd[1] << 8);
    info->hasAlpha = (info->bitDepth == 32);

    return true;
}

bool ImageCompressor::convertToLVGLBin(const char *srcPath, const char *destPath) {
    ImageInfo info;
    if (!analyzeImage(srcPath, &info)) { return false; }

    // Por agora, apenas copia e adiciona header LVGL
    // Implementação completa requer decodificação PNG

    LOG_DEBUG("IMG_COMP: Convert %s -> %s (%dx%d)", srcPath, destPath, info.width, info.height);

    LOG_WARN("IMG_COMP: LVGL binary conversion not yet implemented");
    return false;
}

size_t ImageCompressor::compressRLE(const uint8_t *src, size_t srcSize, uint8_t *dest, size_t maxDestSize) {
    if (!src || !dest || srcSize == 0) return 0;

    size_t destIdx = 0;
    size_t i = 0;

    while (i < srcSize && destIdx < maxDestSize - 2) {
        uint8_t current = src[i];
        uint8_t count = 1;

        // Conta pixels iguais consecutivos (máx 255)
        while (i + count < srcSize && count < 255 && src[i + count] == current) { count++; }

        if (count >= 3) {
            // Run: escape(0xFF) + count + value
            if (destIdx + 3 > maxDestSize) break;
            dest[destIdx++] = 0xFF;
            dest[destIdx++] = count;
            dest[destIdx++] = current;
            i += count;
        } else {
            // Literal
            if (current == 0xFF) {
                // Escape literal 0xFF
                if (destIdx + 3 > maxDestSize) break;
                dest[destIdx++] = 0xFF;
                dest[destIdx++] = 1;
                dest[destIdx++] = 0xFF;
            } else {
                dest[destIdx++] = current;
            }
            i++;
        }
    }

    uint32_t saved = srcSize - destIdx;
    if (saved > 0) {
        _totalSaved += saved;
        LOG_DEBUG("IMG_COMP: RLE: %d -> %d bytes (%.1f%%)", srcSize, destIdx, 100.0 * destIdx / srcSize);
    }

    return destIdx;
}

size_t ImageCompressor::decompressRLE(const uint8_t *src, size_t srcSize, uint8_t *dest, size_t maxDestSize) {
    if (!src || !dest || srcSize == 0) return 0;

    size_t destIdx = 0;
    size_t i = 0;

    while (i < srcSize && destIdx < maxDestSize) {
        uint8_t b = src[i++];

        if (b == 0xFF && i + 1 < srcSize) {
            uint8_t count = src[i++];
            uint8_t value = src[i++];
            while (count-- > 0 && destIdx < maxDestSize) { dest[destIdx++] = value; }
        } else {
            dest[destIdx++] = b;
        }
    }

    return destIdx;
}

void ImageCompressor::quantize32to16(const uint32_t *src, uint16_t *dest, size_t pixelCount, bool dither) {
    if (!src || !dest) return;

    for (size_t i = 0; i < pixelCount; i++) {
        uint32_t px = src[i];
        uint8_t r = (px >> 16) & 0xFF;
        uint8_t g = (px >> 8) & 0xFF;
        uint8_t b = px & 0xFF;

        if (dither && i > 0) {
            // Floyd-Steinberg simplificado
            int16_t errR = (r & 0x07);
            int16_t errG = (g & 0x03);
            int16_t errB = (b & 0x07);

            r = min(255, r + errR / 2);
            g = min(255, g + errG / 2);
            b = min(255, b + errB / 2);
        }

        // RGB888 -> RGB565
        dest[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
}

void ImageCompressor::resize(
    const uint16_t *src, uint16_t srcW, uint16_t srcH, uint16_t *dest, uint16_t destW, uint16_t destH
) {
    if (!src || !dest) return;

    // Bilinear interpolation simplificada
    float xRatio = (float)srcW / destW;
    float yRatio = (float)srcH / destH;

    for (uint16_t y = 0; y < destH; y++) {
        for (uint16_t x = 0; x < destW; x++) {
            uint16_t srcX = (uint16_t)(x * xRatio);
            uint16_t srcY = (uint16_t)(y * yRatio);

            if (srcX >= srcW) srcX = srcW - 1;
            if (srcY >= srcH) srcY = srcH - 1;

            dest[y * destW + x] = src[srcY * srcW + srcX];
        }
    }
}

void ImageCompressor::setConfig(const CompressionConfig &config) { _config = config; }
