/**
 * @file ui_focus_mode.cpp
 * @brief Modos Focus e Minimal - STUB (dependências removidas)
 */

#include "ui_focus_mode.h"
#include "ui_themes.h"
#include <globals.h>
#include <time.h>

FocusMode focusMode;
MinimalMode minimalMode;

// ═══════════════════════════════════════════════════════════════════════════
// FOCUS MODE - Stub
// ═══════════════════════════════════════════════════════════════════════════

FocusMode::FocusMode()
    : _screen(nullptr), _clockLabel(nullptr), _emojiLabel(nullptr), _batteryLabel(nullptr),
      _attackLabel(nullptr), _attackIndicator(nullptr), _enabled(false), _originalBrightness(100),
      _lastUpdate(0) {
    _config.showClock = true;
    _config.showEmoji = false; // Desabilitado
    _config.showBattery = true;
    _config.showAttackStatus = true;
    _config.hideNotifications = true;
    _config.reducedAnimations = true;
    _config.autoDimScreen = true;
    _config.dimBrightness = 30;
}

void FocusMode::begin() {}
void FocusMode::enable() { _enabled = true; }
void FocusMode::disable() { _enabled = false; }
void FocusMode::toggle() { _enabled = !_enabled; }
void FocusMode::createUI() {}
void FocusMode::update() {}
void FocusMode::updateClock() {}
void FocusMode::updateBattery() {}
void FocusMode::setAttackStatus(const char*, bool) {}
void FocusMode::setConfig(const FocusModeConfig &config) { _config = config; }
void FocusMode::applyDimming() {}
void FocusMode::restoreBrightness() {}

// ═══════════════════════════════════════════════════════════════════════════
// MINIMAL MODE - Stub
// ═══════════════════════════════════════════════════════════════════════════

MinimalMode::MinimalMode()
    : _screen(nullptr), _messageLabel(nullptr), _activityDot(nullptr), _enabled(false), _lastPulse(0) {}

void MinimalMode::begin() {}
void MinimalMode::enable() { _enabled = true; }
void MinimalMode::disable() { _enabled = false; }
void MinimalMode::setMessage(const char*) {}
void MinimalMode::showActivity(bool) {}
void MinimalMode::update() {}
