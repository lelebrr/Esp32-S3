/**
 * @file ui_lock_screen.cpp
 * @brief Tela de Bloqueio - SIMPLIFICADA (sem mascote)
 */

#include "ui_lock_screen.h"
#include "ui_themes.h"
#include <time.h>

LockScreen lockScreen;

LockScreen::LockScreen()
    : _screen(nullptr), _clockLabel(nullptr), _dateLabel(nullptr),
      _emojiLabel(nullptr), _statusLabel(nullptr), _unlockHint(nullptr),
      _pinDisplay(nullptr), _locked(true), _visible(false),
      _inputPatternLength(0), _inputPinLength(0), _lastActivityTime(0),
      _onUnlock(nullptr) {

  _config.enabled = false;
  _config.lockType = LOCK_NONE;
  memset(_config.pin, 0, sizeof(_config.pin));
  memset(_config.pattern, 0, sizeof(_config.pattern));
  _config.patternLength = 0;
  _config.showEmoji = false; // Desabilitado
  _config.showStats = true;
  _config.autoLockSeconds = 60;

  for (int i = 0; i < 9; i++) { _patternGrid[i] = nullptr; }
  memset(_inputPattern, 0, sizeof(_inputPattern));
  memset(_inputPin, 0, sizeof(_inputPin));
}

void LockScreen::begin() { createUI(); }

void LockScreen::createUI() {
  if (_screen) return;

  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Relógio grande
  _clockLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_clockLabel, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(_clockLabel, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(_clockLabel, "00:00");
  lv_obj_align(_clockLabel, LV_ALIGN_TOP_MID, 0, 80);

  // Data
  _dateLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_dateLabel, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(_dateLabel, lv_color_hex(0x888888), 0);
  lv_label_set_text(_dateLabel, "Lele Origin");
  lv_obj_align(_dateLabel, LV_ALIGN_TOP_MID, 0, 140);

  // Status
  _statusLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_statusLabel, THEME_ACCENT, 0);
  lv_label_set_text(_statusLabel, "");
  lv_obj_align(_statusLabel, LV_ALIGN_CENTER, 0, 60);

  // Hint de desbloqueio
  _unlockHint = lv_label_create(_screen);
  lv_obj_set_style_text_font(_unlockHint, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_unlockHint, lv_color_hex(0x666666), 0);
  lv_label_set_text(_unlockHint, "Deslize para desbloquear");
  lv_obj_align(_unlockHint, LV_ALIGN_BOTTOM_MID, 0, -40);
}

void LockScreen::show() {
  if (!_screen) createUI();
  _visible = true;
  _locked = true;
  updateClock();
  lv_scr_load(_screen);
}

void LockScreen::hide() {
  _visible = false;
  _locked = false;
}

bool LockScreen::tryUnlock() {
  bool success = _config.lockType == LOCK_NONE || validatePattern() || validatePin();
  if (success) {
    animateUnlock();
    if (_onUnlock) _onUnlock();
  } else {
    showError();
  }
  return success;
}

void LockScreen::update() {
  if (!_visible) return;
  updateClock();
}

void LockScreen::updateClock() {
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  if (timeinfo && _clockLabel) {
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    lv_label_set_text(_clockLabel, timeStr);
  }
}

void LockScreen::setConfig(const LockScreenConfig &config) { _config = config; }
void LockScreen::setPin(const char *pin) { strncpy(_config.pin, pin, 4); }
void LockScreen::setPattern(const uint8_t *pattern, uint8_t length) {
  _config.patternLength = (length > 9) ? 9 : length;
  memcpy(_config.pattern, pattern, _config.patternLength);
}
void LockScreen::onUnlock(void (*callback)()) { _onUnlock = callback; }
void LockScreen::handlePatternInput(int) {}
void LockScreen::handlePinInput(char) {}
bool LockScreen::validatePattern() { return _inputPatternLength == _config.patternLength; }
bool LockScreen::validatePin() { return strcmp(_inputPin, _config.pin) == 0; }
void LockScreen::showError() { _inputPatternLength = 0; _inputPinLength = 0; }
void LockScreen::animateUnlock() { hide(); }
