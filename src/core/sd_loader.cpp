/**
 * @file sd_loader.cpp
 * @brief SD Card Asset Loader Implementation
 */

#include "sd_loader.h"
#include <debug_log.h>

static bool _sdLoaderReady = false;

bool sd_loader_init() {
    if (!SD_MMC.begin()) {
        LOG_ERROR("SD card not available for asset loading");
        return false;
    }
    
    // Check if assets directory exists
    if (!SD_MMC.exists(SD_ASSETS_PATH)) {
        LOG_WARN("Assets directory not found, creating...");
        sd_create_asset_dirs();
    }
    
    _sdLoaderReady = true;
    LOG_UI("SD loader initialized");
    return true;
}

bool sd_loader_is_ready() {
    return _sdLoaderReady;
}

bool sd_asset_exists(const char* path) {
    if (!_sdLoaderReady) return false;
    
    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SD_ASSETS_PATH, path);
    return SD_MMC.exists(fullPath);
}

uint8_t* sd_load_file(const char* path, size_t* outSize) {
    if (!_sdLoaderReady || !outSize) return nullptr;
    
    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SD_ASSETS_PATH, path);
    
    File file = SD_MMC.open(fullPath, FILE_READ);
    if (!file) {
        LOG_ERROR("Failed to open asset: %s", fullPath);
        return nullptr;
    }
    
    size_t fileSize = file.size();
    if (fileSize == 0) {
        file.close();
        return nullptr;
    }
    
    // Allocate buffer - prefer PSRAM if available
    uint8_t* buffer = nullptr;
    
#if SD_LOADER_USE_PSRAM
    buffer = (uint8_t*)heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#endif
    
    if (!buffer) {
        // Fall back to regular heap
        buffer = (uint8_t*)malloc(fileSize);
    }
    
    if (!buffer) {
        LOG_ERROR("Failed to allocate %u bytes for asset", fileSize);
        file.close();
        return nullptr;
    }
    
    size_t bytesRead = file.read(buffer, fileSize);
    file.close();
    
    if (bytesRead != fileSize) {
        LOG_WARN("Partial read: %u of %u bytes", bytesRead, fileSize);
    }
    
    *outSize = bytesRead;
    return buffer;
}

String sd_load_text(const char* path) {
    size_t size = 0;
    uint8_t* data = sd_load_file(path, &size);
    
    if (!data || size == 0) {
        return String();
    }
    
    String result;
    result.reserve(size);
    
    for (size_t i = 0; i < size; i++) {
        result += (char)data[i];
    }
    
    sd_free_buffer(data);
    return result;
}

void sd_free_buffer(uint8_t* buffer) {
    if (buffer) {
        free(buffer);  // Works for both heap and PSRAM allocations
    }
}

uint64_t sd_get_free_space() {
    if (!_sdLoaderReady) return 0;
    return SD_MMC.totalBytes() - SD_MMC.usedBytes();
}

void sd_create_asset_dirs() {
    if (!SD_MMC.begin()) return;
    
    SD_MMC.mkdir(SD_ASSETS_PATH);
    SD_MMC.mkdir(SD_THEMES_PATH);
    SD_MMC.mkdir(SD_SOUNDS_PATH);
    SD_MMC.mkdir(SD_IR_CODES_PATH);
    SD_MMC.mkdir(SD_RF_SIGNALS_PATH);
    SD_MMC.mkdir(SD_IMAGES_PATH);
    
    LOG_UI("Asset directories created on SD card");
}

String sd_load_theme(const char* themeName) {
    char path[64];
    snprintf(path, sizeof(path), "themes/%s.json", themeName);
    return sd_load_text(path);
}

size_t sd_load_ir_code(const char* brand, const char* model, uint8_t* buffer, size_t maxSize) {
    if (!buffer || maxSize == 0) return 0;
    
    char path[96];
    snprintf(path, sizeof(path), "ir_codes/%s/%s.bin", brand, model);
    
    size_t size = 0;
    uint8_t* data = sd_load_file(path, &size);
    
    if (!data) return 0;
    
    size_t copySize = (size < maxSize) ? size : maxSize;
    memcpy(buffer, data, copySize);
    
    sd_free_buffer(data);
    return copySize;
}
