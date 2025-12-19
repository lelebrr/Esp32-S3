/**
 * @file lvgl_perf.cpp
 * @brief Implementação das otimizações de performance LVGL
 */

#include "lvgl_perf.h"
#include <debug_log.h>
#include <esp_heap_caps.h>

LVGLPerformance lvglPerf;

// ═══════════════════════════════════════════════════════════════════════════
// SPRITE CACHE
// ═══════════════════════════════════════════════════════════════════════════

SpriteCache::SpriteCache() : _cachedCount(0), _usedMemory(0), _maxMemory(0) {
    for (int i = 0; i < MAX_CACHED_SPRITES; i++) {
        _cache[i].path[0] = '\0';
        _cache[i].image = nullptr;
        _cache[i].size = 0;
        _cache[i].lastAccess = 0;
    }
}

void SpriteCache::begin(size_t maxMemory) {
    if (maxMemory == 0) {
        // Usa 20% da PSRAM disponível
        _maxMemory = heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 5;
    } else {
        _maxMemory = maxMemory;
    }
    LOG_DEBUG("[SPRITE_CACHE] Max memory: %d KB", _maxMemory / 1024);
}

const lv_img_dsc_t *SpriteCache::load(const char *path) {
    // Verifica se já está em cache
    int idx = findInCache(path);
    if (idx >= 0) {
        _cache[idx].lastAccess = millis();
        return _cache[idx].image;
    }

    // Cache cheio? Remove o mais antigo
    if (_cachedCount >= MAX_CACHED_SPRITES) { evictOldest(); }

    LOG_WARN("PERF: Sprite cache load from FS not implemented");
    // Por agora, retorna nullptr (implementação futura)
    return nullptr;
}

void SpriteCache::unload(const char *path) {
    int idx = findInCache(path);
    if (idx >= 0) {
        if (_cache[idx].image) {
            if (_cache[idx].image->data) { heap_caps_free((void *)_cache[idx].image->data); }
            heap_caps_free(_cache[idx].image);
        }
        _usedMemory -= _cache[idx].size;
        _cache[idx].path[0] = '\0';
        _cache[idx].image = nullptr;
        _cache[idx].size = 0;
        _cachedCount--;
    }
}

void SpriteCache::clear() {
    for (int i = 0; i < MAX_CACHED_SPRITES; i++) {
        if (_cache[i].image) {
            if (_cache[i].image->data) { heap_caps_free((void *)_cache[i].image->data); }
            heap_caps_free(_cache[i].image);
        }
        _cache[i].path[0] = '\0';
        _cache[i].image = nullptr;
        _cache[i].size = 0;
    }
    _cachedCount = 0;
    _usedMemory = 0;
}

int SpriteCache::findInCache(const char *path) {
    for (int i = 0; i < MAX_CACHED_SPRITES; i++) {
        if (_cache[i].path[0] && strcmp(_cache[i].path, path) == 0) { return i; }
    }
    return -1;
}

void SpriteCache::evictOldest() {
    int oldest = -1;
    uint32_t oldestTime = UINT32_MAX;

    for (int i = 0; i < MAX_CACHED_SPRITES; i++) {
        if (_cache[i].path[0] && _cache[i].lastAccess < oldestTime) {
            oldestTime = _cache[i].lastAccess;
            oldest = i;
        }
    }

    if (oldest >= 0) { unload(_cache[oldest].path); }
}

// ═══════════════════════════════════════════════════════════════════════════
// LVGL PERFORMANCE MANAGER
// ═══════════════════════════════════════════════════════════════════════════

LVGLPerformance::LVGLPerformance()
    : _currentFPS(0), _avgRenderTime(0), _frameCount(0), _lastFPSUpdate(0), _renderTimeSum(0) {

    _config.partialUpdate = true;
    _config.use16BitColor = true;
    _config.disableAntiAlias = false;
    _config.usePSRAMBuffers = true;
    _config.maxFPS = 30;
    _config.enableGPU = true;
}

void LVGLPerformance::begin() {
    _spriteCache.begin();
    _lastFPSUpdate = millis();
    applyConfig(_config);
    LOG_DEBUG("[LVGL_PERF] Performance manager initialized");
}

void LVGLPerformance::applyConfig(const LVGLPerfConfig &config) {
    _config = config;

    // Aplica configurações ao LVGL
    if (config.disableAntiAlias) {
        // Desativa anti-aliasing globalmente
        // lv_obj_set_style_opa(lv_layer_top(), LV_OPA_COVER, 0);
    }

    // Limita FPS
    if (config.maxFPS > 0 && config.maxFPS <= 60) {
        // LVGL 8: lv_disp_set_default não existe, usar lv_timer internamente
    }

    LOG_DEBUG(
        "[LVGL_PERF] Config applied: 16bit=%d, AA=%d, FPS=%d",
        config.use16BitColor,
        !config.disableAntiAlias,
        config.maxFPS
    );
}

void LVGLPerformance::enablePowerSaving(bool enable) {
    if (enable) {
        _config.maxFPS = 15;
        _config.disableAntiAlias = true;
        _config.partialUpdate = true;
    } else {
        _config.maxFPS = 30;
        _config.disableAntiAlias = false;
    }
    applyConfig(_config);
}

void LVGLPerformance::enableHighPerformance(bool enable) {
    if (enable) {
        _config.maxFPS = 60;
        _config.disableAntiAlias = false;
        _config.partialUpdate = false;
    } else {
        _config.maxFPS = 30;
        _config.partialUpdate = true;
    }
    applyConfig(_config);
}

void LVGLPerformance::forceGC() {
    // Força garbage collection
    _spriteCache.clear();

    // Força LVGL a limpar objetos deletados
    lv_obj_t *scr = lv_scr_act();
    if (scr) { lv_obj_invalidate(scr); }

    LOG_DEBUG("[LVGL_PERF] Garbage collection completed");
}

void LVGLPerformance::setRefreshArea(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Define área de refresh limitada
    lv_area_t area;
    area.x1 = x;
    area.y1 = y;
    area.x2 = x + w - 1;
    area.y2 = y + h - 1;

    lv_obj_t *scr = lv_scr_act();
    if (scr) { lv_obj_invalidate_area(scr, &area); }
}

void LVGLPerformance::resetRefreshArea() {
    lv_obj_t *scr = lv_scr_act();
    if (scr) { lv_obj_invalidate(scr); }
}

void LVGLPerformance::updateMetrics() {
    _frameCount++;
    uint32_t now = millis();

    if (now - _lastFPSUpdate >= 1000) {
        _currentFPS = _frameCount;
        _avgRenderTime = _frameCount > 0 ? _renderTimeSum / _frameCount : 0;

        _frameCount = 0;
        _renderTimeSum = 0;
        _lastFPSUpdate = now;
    }
}
