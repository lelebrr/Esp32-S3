#include "notifications_engine.h"
#include "../hardware/audio_driver.h"
#include "globals.h"
#include "sounds_manager.h"
#include <Arduino.h>
#include <debug_log.h>

NotificationsEngine notifications_engine;

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════
NotificationsEngine::NotificationsEngine()
    : _isPlaying(false), _startTime(0), _currentEffect(NOTIFY_EFFECT_FLASH_TOTAL), _currentColor(0x00FFF5),
      _currentDuration(500) {

    // Default configuration
    _config.enabled = true;
    _config.effect = NOTIFY_EFFECT_NEON_BORDER;
    _config.color = 0x00FFF5;
    _config.duration_ms = 800;
    _config.fade = FADE_MEDIUM;
    _config.vibration = VIBRATE_SHORT;
    _config.play_sound = true;
    _config.sound_id = 0;

    // Enable all events by default
    for (int i = 0; i < EVENT_COUNT; i++) { _config.event_enabled[i] = true; }
}

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════
void NotificationsEngine::begin() { LOG_NOTIFY("Notifications engine initialized"); }

void NotificationsEngine::update() {
    if (!_isPlaying) return;

    float progress = calculateProgress();

    if (progress >= 1.0f) {
        _isPlaying = false;
        // Final callback with progress 1.0 to clear effect
        if (_effectCallback) { _effectCallback(_currentEffect, _currentColor, 1.0f); }
        return;
    }

    // Call render callback with current progress
    if (_effectCallback) { _effectCallback(_currentEffect, _currentColor, progress); }
}

// ═══════════════════════════════════════════════════════════════════════════
// NOTIFICATION TRIGGERS
// ═══════════════════════════════════════════════════════════════════════════
void NotificationsEngine::notify(NotificationEvent event) {
    if (!_config.enabled) return;
    if (event >= EVENT_COUNT) return;
    if (!_config.event_enabled[event]) return;

    LOG_NOTIFY("Event triggered: %d", event);

    // Start visual effect
    _currentEffect = _config.effect;
    _currentColor = _config.color;
    _currentDuration = _config.duration_ms;
    _startTime = millis();
    _isPlaying = true;

    // Play vibration
    playVibration(_config.vibration);

    // Play sound if enabled
    if (_config.play_sound) { playSound(_config.sound_id); }
}

void NotificationsEngine::notifyCustom(NotificationEffect effect, uint32_t color, uint16_t duration_ms) {
    if (!_config.enabled) return;

    _currentEffect = effect;
    _currentColor = color;
    _currentDuration = duration_ms;
    _startTime = millis();
    _isPlaying = true;

    LOG_NOTIFY("Custom notification: effect=%d, color=0x%06X, duration=%dms", effect, color, duration_ms);
}

void NotificationsEngine::test() {
    LOG_NOTIFY("Testing notification...");
    notifyCustom(_config.effect, _config.color, _config.duration_ms);
    playVibration(_config.vibration);
    if (_config.play_sound) { playSound(_config.sound_id); }
}

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════
void NotificationsEngine::setConfig(const NotificationConfig &config) {
    _config = config;
    LOG_NOTIFY("Configuration updated");
}

void NotificationsEngine::setEffect(NotificationEffect effect) {
    if (effect < NOTIFY_EFFECT_COUNT) { _config.effect = effect; }
}

void NotificationsEngine::setColor(uint32_t color) { _config.color = color; }

void NotificationsEngine::setDuration(uint16_t ms) { _config.duration_ms = constrain(ms, 300, 3000); }

void NotificationsEngine::setFade(FadeIntensity fade) { _config.fade = fade; }

void NotificationsEngine::setVibrationPattern(VibrationPattern pattern) {
    if (pattern < VIBRATE_PATTERN_COUNT) { _config.vibration = pattern; }
}

void NotificationsEngine::enableEvent(NotificationEvent event, bool enabled) {
    if (event < EVENT_COUNT) { _config.event_enabled[event] = enabled; }
}

void NotificationsEngine::enableSound(bool enabled, uint8_t soundId) {
    _config.play_sound = enabled;
    _config.sound_id = soundId;
}

// ═══════════════════════════════════════════════════════════════════════════
// VIBRATION PATTERNS (using buzzer PWM for haptic feedback)
// ═══════════════════════════════════════════════════════════════════════════
void NotificationsEngine::playVibration(VibrationPattern pattern) {
    const char *patternNames[] = {"SHORT", "DOUBLE", "LONG", "HEART", "ALERT", "SOS"};

    if (pattern < VIBRATE_PATTERN_COUNT) { LOG_NOTIFY("Vibration: %s", patternNames[pattern]); }

    // Use buzzer for haptic feedback via audio driver
    // Low frequency tones create vibration-like tactile feedback
    switch (pattern) {
        case VIBRATE_SHORT:
            // Single short pulse - 50ms at low frequency
            audioDriver.playTone(100, 50);
            break;

        case VIBRATE_DOUBLE:
            // Two short pulses with gap
            audioDriver.playTone(100, 50);
            delay(80);
            audioDriver.playTone(100, 50);
            break;

        case VIBRATE_LONG:
            // Single long pulse - 300ms
            audioDriver.playTone(80, 300);
            break;

        case VIBRATE_HEART:
            // Heartbeat pattern: ··· — ···
            audioDriver.playTone(120, 30);
            delay(50);
            audioDriver.playTone(120, 30);
            delay(50);
            audioDriver.playTone(120, 30);
            delay(100);
            audioDriver.playTone(80, 200);
            delay(100);
            audioDriver.playTone(120, 30);
            delay(50);
            audioDriver.playTone(120, 30);
            delay(50);
            audioDriver.playTone(120, 30);
            break;

        case VIBRATE_ALERT:
            // Alert pattern: ——·——
            audioDriver.playTone(100, 150);
            delay(100);
            audioDriver.playTone(100, 150);
            delay(80);
            audioDriver.playTone(150, 50);
            delay(80);
            audioDriver.playTone(100, 150);
            delay(100);
            audioDriver.playTone(100, 150);
            break;

        case VIBRATE_SOS:
            // SOS pattern: ···———···
            for (int i = 0; i < 3; i++) {
                audioDriver.playTone(120, 50);
                delay(80);
            }
            delay(50);
            for (int i = 0; i < 3; i++) {
                audioDriver.playTone(80, 150);
                delay(100);
            }
            delay(50);
            for (int i = 0; i < 3; i++) {
                audioDriver.playTone(120, 50);
                delay(80);
            }
            break;

        default: break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SOUND PLAYBACK
// ═══════════════════════════════════════════════════════════════════════════
void NotificationsEngine::playSound(uint8_t soundId) {
    LOG_NOTIFY("Playing sound ID: %d", soundId);

    // Map notification sound IDs to audio driver sound types
    // IDs 0-10 are built-in sounds via audio driver
    // IDs 11+ are custom WAV files from SD card
    if (soundId <= SOUND_BEEP) {
        // Use audio driver for built-in tones
        audioDriver.playSound((SoundType)soundId);
    } else {
        // Custom sound file from SD card: /sounds/notify_XX.wav
        char path[32];
        snprintf(path, sizeof(path), "/sounds/notify_%02d.wav", soundId);

        // Use sounds_manager for WAV playback (implemented in sounds_manager.cpp)
        extern class SoundsManager sounds_manager;
        sounds_manager.playByName(path);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════════════════
float NotificationsEngine::calculateProgress() {
    if (_currentDuration == 0) return 1.0f;

    uint32_t elapsed = millis() - _startTime;
    float progress = (float)elapsed / (float)_currentDuration;

    // Apply fade curve based on intensity
    switch (_config.fade) {
        case FADE_SOFT:
            // Smooth sine curve
            progress = (1.0f - cos(progress * PI)) / 2.0f;
            break;
        case FADE_MEDIUM:
            // Quadratic ease-out
            progress = 1.0f - (1.0f - progress) * (1.0f - progress);
            break;
        case FADE_AGGRESSIVE:
            // Linear (no fade adjustment)
            break;
    }

    return min(progress, 1.0f);
}
