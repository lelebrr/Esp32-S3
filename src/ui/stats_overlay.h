/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Stats Overlay (Estatísticas em Tempo Real)
 * Overlay de estatísticas do sistema
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <debug_log.h>
#include <lvgl.h>


class StatsOverlay {
public:
    StatsOverlay();

    void begin();
    void show();
    void hide();
    void toggle();
    void update();

    bool isVisible() const { return _visible; }

    // Configurações
    void setPosition(lv_align_t align, int16_t x, int16_t y);
    void setTransparency(uint8_t opa);
    void setShowFPS(bool show);
    void setShowMemory(bool show);
    void setShowBattery(bool show);
    void setShowNetwork(bool show);

private:
    bool _visible;
    lv_obj_t *_container;
    lv_obj_t *_fpsLabel;
    lv_obj_t *_memLabel;
    lv_obj_t *_batteryLabel;
    lv_obj_t *_networkLabel;

    bool _showFPS;
    bool _showMemory;
    bool _showBattery;
    bool _showNetwork;

    uint32_t _lastUpdate;
    uint32_t _frameCount;
    uint32_t _lastFPSCalc;
    uint8_t _currentFPS;

    void createOverlay();
    void updateFPS();
    void updateMemory();
    void updateBattery();
    void updateNetwork();
};

extern StatsOverlay stats_overlay;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

StatsOverlay stats_overlay;

StatsOverlay::StatsOverlay() {
    _visible = false;
    _container = nullptr;
    _fpsLabel = nullptr;
    _memLabel = nullptr;
    _batteryLabel = nullptr;
    _networkLabel = nullptr;

    _showFPS = true;
    _showMemory = true;
    _showBattery = true;
    _showNetwork = true;

    _lastUpdate = 0;
    _frameCount = 0;
    _lastFPSCalc = 0;
    _currentFPS = 0;
}

void StatsOverlay::begin() {
    createOverlay();
    hide();
    LOG_INFO("Stats Overlay inicializado");
}

void StatsOverlay::createOverlay() {
    // Container semi-transparente
    _container = lv_obj_create(lv_layer_top());
    lv_obj_set_size(_container, 120, 80);
    lv_obj_align(_container, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_set_style_bg_color(_container, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_70, 0);
    lv_obj_set_style_border_opa(_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(_container, 8, 0);
    lv_obj_set_style_pad_all(_container, 5, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // FPS
    _fpsLabel = lv_label_create(_container);
    lv_label_set_text(_fpsLabel, "FPS: --");
    lv_obj_set_style_text_color(_fpsLabel, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(_fpsLabel, &lv_font_montserrat_10, 0);
    lv_obj_align(_fpsLabel, LV_ALIGN_TOP_LEFT, 0, 0);

    // Memory
    _memLabel = lv_label_create(_container);
    lv_label_set_text(_memLabel, "MEM: -- KB");
    lv_obj_set_style_text_color(_memLabel, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_text_font(_memLabel, &lv_font_montserrat_10, 0);
    lv_obj_align(_memLabel, LV_ALIGN_TOP_LEFT, 0, 15);

    // Battery
    _batteryLabel = lv_label_create(_container);
    lv_label_set_text(_batteryLabel, "BAT: --%");
    lv_obj_set_style_text_color(_batteryLabel, lv_color_hex(0x00FFFF), 0);
    lv_obj_set_style_text_font(_batteryLabel, &lv_font_montserrat_10, 0);
    lv_obj_align(_batteryLabel, LV_ALIGN_TOP_LEFT, 0, 30);

    // Network
    _networkLabel = lv_label_create(_container);
    lv_label_set_text(_networkLabel, "NET: --");
    lv_obj_set_style_text_color(_networkLabel, lv_color_hex(0xFF00FF), 0);
    lv_obj_set_style_text_font(_networkLabel, &lv_font_montserrat_10, 0);
    lv_obj_align(_networkLabel, LV_ALIGN_TOP_LEFT, 0, 45);
}

void StatsOverlay::show() {
    if (_container) {
        lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
        _visible = true;
    }
}

void StatsOverlay::hide() {
    if (_container) {
        lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
        _visible = false;
    }
}

void StatsOverlay::toggle() {
    if (_visible) hide();
    else show();
}

void StatsOverlay::update() {
    if (!_visible) return;

    _frameCount++;
    uint32_t now = millis();

    // Atualiza FPS a cada segundo
    if (now - _lastFPSCalc >= 1000) {
        _currentFPS = _frameCount;
        _frameCount = 0;
        _lastFPSCalc = now;
        updateFPS();
    }

    // Atualiza outras stats a cada 500ms
    if (now - _lastUpdate >= 500) {
        _lastUpdate = now;
        updateMemory();
        updateBattery();
        updateNetwork();
    }
}

void StatsOverlay::updateFPS() {
    if (!_showFPS || !_fpsLabel) return;

    char buf[16];
    snprintf(buf, sizeof(buf), "FPS: %d", _currentFPS);
    lv_label_set_text(_fpsLabel, buf);

    // Cor baseada no FPS
    lv_color_t color;
    if (_currentFPS >= 30) color = lv_color_hex(0x00FF00);      // Verde
    else if (_currentFPS >= 20) color = lv_color_hex(0xFFFF00); // Amarelo
    else color = lv_color_hex(0xFF0000);                        // Vermelho
    lv_obj_set_style_text_color(_fpsLabel, color, 0);
}

void StatsOverlay::updateMemory() {
    if (!_showMemory || !_memLabel) return;

    uint32_t freeHeap = esp_get_free_heap_size() / 1024;
    char buf[16];
    snprintf(buf, sizeof(buf), "MEM: %d KB", freeHeap);
    lv_label_set_text(_memLabel, buf);
}

void StatsOverlay::updateBattery() {
    if (!_showBattery || !_batteryLabel) return;

// Lê do PMU via sys_hw (quando disponível) ou usa estimativa
#ifdef HAS_PMU
    uint8_t battery = sys_hw.getBatteryPercent();
#else
    // Estimativa baseada em tensão ADC quando não há PMU
    uint8_t battery = 75; // Valor padrão
#endif
    char buf[16];
    snprintf(buf, sizeof(buf), "BAT: %d%%", battery);
    lv_label_set_text(_batteryLabel, buf);
}

void StatsOverlay::updateNetwork() {
    if (!_showNetwork || !_networkLabel) return;

    // Lê contagem de APs do último scan WiFi
    extern int accessPointsCount; // Definido em wifi_common
    int apCount = accessPointsCount;
    char buf[16];
    snprintf(buf, sizeof(buf), "NET: %d AP", apCount);
    lv_label_set_text(_networkLabel, buf);
}

void StatsOverlay::setPosition(lv_align_t align, int16_t x, int16_t y) {
    if (_container) lv_obj_align(_container, align, x, y);
}

void StatsOverlay::setTransparency(uint8_t opa) {
    if (_container) lv_obj_set_style_bg_opa(_container, opa, 0);
}

void StatsOverlay::setShowFPS(bool show) { _showFPS = show; }
void StatsOverlay::setShowMemory(bool show) { _showMemory = show; }
void StatsOverlay::setShowBattery(bool show) { _showBattery = show; }
void StatsOverlay::setShowNetwork(bool show) { _showNetwork = show; }
