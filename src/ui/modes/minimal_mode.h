/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Modo Minimal (UI Simplificada)
 * Interface minimalista para economia de bateria
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <lvgl.h>

class MinimalMode {
public:
    MinimalMode();

    void begin();
    void enter();
    void exit();
    void update();

    bool isActive() const { return _active; }

    // Configurações
    void setShowTime(bool show);
    void setShowStats(bool show);
    void setShowMascot(bool show);

private:
    bool _active;
    lv_obj_t *_screen;
    lv_obj_t *_timeLabel;
    lv_obj_t *_statsLabel;
    lv_obj_t *_mascotEmoji;

    bool _showTime;
    bool _showStats;
    bool _showMascot;

    uint32_t _lastUpdate;

    void createUI();
    void updateTime();
    void updateStats();
};

extern MinimalMode minimal_mode;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO (all inline to avoid multiple definition)
// ═══════════════════════════════════════════════════════════════════════════

// definition moved to .cpp

inline MinimalMode::MinimalMode() {
    _active = false;
    _screen = nullptr;
    _timeLabel = nullptr;
    _statsLabel = nullptr;
    _mascotEmoji = nullptr;
    _showTime = true;
    _showStats = true;
    _showMascot = true;
    _lastUpdate = 0;
}

inline void MinimalMode::begin() { Serial.println("[Minimal] Mode inicializado"); }

inline void MinimalMode::enter() {
    if (_active) return;

    _active = true;
    createUI();

    lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    Serial.println("[Minimal] Modo ativado");
}

inline void MinimalMode::exit() {
    if (!_active) return;

    _active = false;

    if (_screen) {
        lv_obj_del(_screen);
        _screen = nullptr;
    }

    Serial.println("[Minimal] Modo desativado");
}

inline void MinimalMode::update() {
    if (!_active) return;

    uint32_t now = millis();
    if (now - _lastUpdate < 1000) return;
    _lastUpdate = now;

    updateTime();
    updateStats();
}

inline void MinimalMode::createUI() {
    // Tela preta pura
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);

    // Container central
    lv_obj_t *container = lv_obj_create(_screen);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_center(container);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Hora grande
    if (_showTime) {
        _timeLabel = lv_label_create(container);
        lv_label_set_text(_timeLabel, "00:00");
        lv_obj_set_style_text_font(_timeLabel, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(_timeLabel, lv_color_white(), 0);
    }

    // Stats minimalistas
    if (_showStats) {
        _statsLabel = lv_label_create(container);
        lv_label_set_text(_statsLabel, "📡 0  🤝 0");
        lv_obj_set_style_text_font(_statsLabel, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(_statsLabel, lv_color_hex(0x666666), 0);
    }

    // Mascote emoji (pequeno)
    if (_showMascot) {
        _mascotEmoji = lv_label_create(container);
        lv_label_set_text(_mascotEmoji, "🐉");
        lv_obj_set_style_text_font(_mascotEmoji, &lv_font_montserrat_28, 0);
    }
}

inline void MinimalMode::updateTime() {
    if (!_timeLabel) return;

    time_t now = time(nullptr);
    struct tm *tm_info = localtime(&now);

    static char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", tm_info->tm_hour, tm_info->tm_min);
    lv_label_set_text(_timeLabel, buf);
}

inline void MinimalMode::updateStats() {
    if (!_statsLabel) return;

    extern int accessPointsCount;
    extern int num_HS;
    static char buf[32];
    snprintf(buf, sizeof(buf), "📡 %d  🤝 %d", accessPointsCount, num_HS);
    lv_label_set_text(_statsLabel, buf);
}

inline void MinimalMode::setShowTime(bool show) { _showTime = show; }
inline void MinimalMode::setShowStats(bool show) { _showStats = show; }
inline void MinimalMode::setShowMascot(bool show) { _showMascot = show; }
