/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Frame Rate Controller (Otimização LVGL)
 * Controle dinâmico de FPS baseado na atividade
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <debug_log.h>
#include <lvgl.h>


// Modos de renderização
enum RenderMode : uint8_t {
    RENDER_FULL = 0, // 60 FPS - animações suaves
    RENDER_NORMAL,   // 30 FPS - uso normal
    RENDER_ECO,      // 15 FPS - economia
    RENDER_MINIMAL,  // 5 FPS - quase estático
    RENDER_PAUSED    // 0 FPS - sem updates
};

class FrameRateController {
public:
    FrameRateController();

    void begin();
    void update();

    // Modo de renderização
    void setMode(RenderMode mode);
    RenderMode getMode() const { return _mode; }

    // FPS direto
    void setTargetFPS(uint8_t fps);
    uint8_t getTargetFPS() const { return _targetFPS; }
    uint8_t getCurrentFPS() const { return _currentFPS; }

    // Activity tracking
    void registerActivity();
    bool shouldRender() const;

    // Stats
    uint32_t getFrameCount() const { return _frameCount; }
    float getAverageRenderTime() const { return _avgRenderTime; }

private:
    RenderMode _mode;
    uint8_t _targetFPS;
    uint8_t _currentFPS;
    uint32_t _lastFrame;
    uint32_t _lastActivity;
    uint32_t _frameCount;
    uint32_t _renderTimes[10];
    uint8_t _renderTimeIdx;
    float _avgRenderTime;

    void calculateFPS();
};

extern FrameRateController fps_controller;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

FrameRateController fps_controller;

FrameRateController::FrameRateController() {
    _mode = RENDER_NORMAL;
    _targetFPS = 30;
    _currentFPS = 0;
    _lastFrame = 0;
    _lastActivity = 0;
    _frameCount = 0;
    _renderTimeIdx = 0;
    _avgRenderTime = 0;
    memset(_renderTimes, 0, sizeof(_renderTimes));
}

void FrameRateController::begin() {
    _lastFrame = millis();
    _lastActivity = millis();
    LOG_INFO("FPS Controller iniciado, target: %d FPS", _targetFPS);
}

void FrameRateController::update() {
    uint32_t now = millis();

    // Auto-reduce FPS quando idle
    uint32_t idle = now - _lastActivity;

    if (idle > 30000 && _mode < RENDER_MINIMAL) {
        setMode(RENDER_MINIMAL);
    } else if (idle > 10000 && _mode < RENDER_ECO) {
        setMode(RENDER_ECO);
    } else if (idle > 3000 && _mode < RENDER_NORMAL) {
        setMode(RENDER_NORMAL);
    }

    calculateFPS();
}

void FrameRateController::setMode(RenderMode mode) {
    _mode = mode;

    switch (mode) {
        case RENDER_FULL: _targetFPS = 60; break;
        case RENDER_NORMAL: _targetFPS = 30; break;
        case RENDER_ECO: _targetFPS = 15; break;
        case RENDER_MINIMAL: _targetFPS = 5; break;
        case RENDER_PAUSED: _targetFPS = 0; break;
    }

    // Ajusta LVGL timer
    if (_targetFPS > 0) { lv_timer_set_period(lv_timer_get_next(NULL), 1000 / _targetFPS); }

    LOG_INFO("FPS Modo: %d, Target: %d FPS", mode, _targetFPS);
}

void FrameRateController::setTargetFPS(uint8_t fps) { _targetFPS = constrain(fps, 1, 60); }

void FrameRateController::registerActivity() {
    _lastActivity = millis();

    // Restaura FPS normal
    if (_mode > RENDER_NORMAL) { setMode(RENDER_NORMAL); }
}

bool FrameRateController::shouldRender() const {
    if (_targetFPS == 0) return false;

    uint32_t frameInterval = 1000 / _targetFPS;
    return (millis() - _lastFrame) >= frameInterval;
}

void FrameRateController::calculateFPS() {
    static uint32_t lastCalc = 0;
    static uint32_t framesSinceCalc = 0;

    uint32_t now = millis();
    framesSinceCalc++;

    if (now - lastCalc >= 1000) {
        _currentFPS = framesSinceCalc;
        framesSinceCalc = 0;
        lastCalc = now;
    }
}
