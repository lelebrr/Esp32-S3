/**
 * @file watch_mode.cpp
 * @brief Watch Mode (DISABLED - Missing GlobalState/MascotManager members)
 */
#include "watch_mode.h"
#include "globals.h"
#include "../ui_themes.h"
#include <debug_log.h>

WatchMode watch_mode;

WatchMode::WatchMode() : _active(false), _screen(nullptr), _lastUpdate(0),
    _timeLabel(nullptr), _dateLabel(nullptr), _batteryIcon(nullptr),
    _wifiIcon(nullptr), _statsLabel(nullptr) {
    memset(&_config, 0, sizeof(_config));
    memset(&_stats, 0, sizeof(_stats));
}

void WatchMode::begin() {
    LOG_UI("WatchMode::begin() - disabled due to missing dependencies");
    loadConfig();
}

void WatchMode::enter() {
    if (!_screen) {
        _screen = lv_obj_create(nullptr);
        lv_obj_set_style_bg_color(_screen, THEME_BG, 0);
        
        lv_obj_t *msg = lv_label_create(_screen);
        lv_label_set_text(msg, "Watch Mode disabled\n(dependencies not defined)");
        lv_obj_set_style_text_color(msg, THEME_TEXT_DIM, 0);
        lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
    }
    _active = true;
    lv_scr_load(_screen);
}

void WatchMode::exit() {
    _active = false;
    if (_screen) {
        lv_obj_del(_screen);
        _screen = nullptr;
        _timeLabel = nullptr;
        _dateLabel = nullptr;
        _batteryIcon = nullptr;
        _wifiIcon = nullptr;
        _statsLabel = nullptr;
    }
}

void WatchMode::update() {}

void WatchMode::setWatchface(WatchfaceType type) {
    _config.watchface_id = (uint8_t)type;
}

void WatchMode::nextWatchface() {
    _config.watchface_id = (_config.watchface_id + 1) % WATCHFACE_COUNT;
}

void WatchMode::prevWatchface() {
    _config.watchface_id = (_config.watchface_id + WATCHFACE_COUNT - 1) % WATCHFACE_COUNT;
}

void WatchMode::saveConfig() {
    LOG_UI("WatchMode::saveConfig() - stub");
}

void WatchMode::loadConfig() {
    LOG_UI("WatchMode::loadConfig() - stub");
    _config.watchface_id = WATCHFACE_DIGITAL;
    _config.show_seconds = true;
    _config.show_date = true;
    _config.show_battery = true;
    _config.show_wifi_status = true;
    _config.show_handshake_count = true;
    _config.background_sniffing = false;
    _config.auto_capture = false;
    _config.brightness_level = 75;
    _config.screen_timeout = 30;
}

void WatchMode::getTime(uint8_t &hour, uint8_t &minute, uint8_t &second) {
    // Stub
    hour = 12;
    minute = 0;
    second = 0;
}

void WatchMode::getDate(uint8_t &day, uint8_t &month, uint16_t &year) {
    // Stub
    day = 1;
    month = 1;
    year = 2025;
}

const char* WatchMode::getDayOfWeek() {
    return "Mon";
}

void WatchMode::enableBackgroundSniffing(bool enable) {
    _config.background_sniffing = enable;
}

void WatchMode::enableAutoCapture(bool enable) {
    _config.auto_capture = enable;
}

// Private render stubs
void WatchMode::renderDigital() {}
void WatchMode::renderAnalog() {}
void WatchMode::renderCyber() {}
void WatchMode::renderMascot() {}
void WatchMode::renderHacker() {}
void WatchMode::renderMinimal() {}
void WatchMode::animateTransition() {}
