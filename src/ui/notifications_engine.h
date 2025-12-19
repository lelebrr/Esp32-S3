#pragma once

#include <Arduino.h>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════════
// VISUAL NOTIFICATION TYPES
// ═══════════════════════════════════════════════════════════════════════════
enum NotificationEffect : uint8_t {
  NOTIFY_EFFECT_FLASH_TOTAL = 0, // Full screen flash
  NOTIFY_EFFECT_NEON_BORDER,     // Pulsing neon border
  NOTIFY_EFFECT_CENTER_PULSE,    // Circular pulse from center
  NOTIFY_EFFECT_SCANLINE,        // Vertical scanline sweep
  NOTIFY_EFFECT_GLITCH,          // Full glitch effect
  NOTIFY_EFFECT_DRAGON_BLINK,    // Dragon blinks eyes
  NOTIFY_EFFECT_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// VIBRATION PATTERNS
// ═══════════════════════════════════════════════════════════════════════════
enum VibrationPattern : uint8_t {
  VIBRATE_SHORT = 0, // · (50ms)
  VIBRATE_DOUBLE,    // ·· (50ms, 50ms pause, 50ms)
  VIBRATE_LONG,      // — (300ms)
  VIBRATE_HEART,     // ··· — ···
  VIBRATE_ALERT,     // ——·——
  VIBRATE_SOS,       // ···———···
  VIBRATE_PATTERN_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// NOTIFICATION EVENTS
// ═══════════════════════════════════════════════════════════════════════════
enum NotificationEvent : uint8_t {
  EVENT_HANDSHAKE_CAPTURED = 0,
  EVENT_WPS_PIN_FOUND,
  EVENT_BLE_DEVICE_NEAR,
  EVENT_BATTERY_LOW,
  EVENT_SD_ALMOST_FULL,
  EVENT_ATTACK_COMPLETE,
  EVENT_USB_CONNECTED,
  EVENT_CUSTOM,
  EVENT_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// FADE INTENSITY
// ═══════════════════════════════════════════════════════════════════════════
enum FadeIntensity : uint8_t { FADE_SOFT = 0, FADE_MEDIUM, FADE_AGGRESSIVE };

// ═══════════════════════════════════════════════════════════════════════════
// NOTIFICATION CONFIG
// ═══════════════════════════════════════════════════════════════════════════
struct NotificationConfig {
  bool enabled;               // Master enable
  NotificationEffect effect;  // Visual effect type
  uint32_t color;             // RGB color (0xRRGGBB)
  uint16_t duration_ms;       // Effect duration (300-3000ms)
  FadeIntensity fade;         // Fade intensity
  VibrationPattern vibration; // Vibration pattern
  bool play_sound;            // Play associated sound
  uint8_t sound_id;           // Sound file ID

  // Per-event enables
  bool event_enabled[EVENT_COUNT];
};

// ═══════════════════════════════════════════════════════════════════════════
// NOTIFICATION ENGINE CLASS
// ═══════════════════════════════════════════════════════════════════════════
class NotificationsEngine {
public:
  NotificationsEngine();

  // Initialization
  void begin();
  void update();

  // Trigger notifications
  void notify(NotificationEvent event);
  void notifyCustom(NotificationEffect effect, uint32_t color,
                    uint16_t duration_ms);
  void test(); // Test current configuration

  // Configuration
  void setConfig(const NotificationConfig &config);
  NotificationConfig getConfig() const { return _config; }

  void setEffect(NotificationEffect effect);
  void setColor(uint32_t color);
  void setDuration(uint16_t ms);
  void setFade(FadeIntensity fade);
  void setVibrationPattern(VibrationPattern pattern);
  void enableEvent(NotificationEvent event, bool enabled);
  void enableSound(bool enabled, uint8_t soundId = 0);

  // State
  bool isPlaying() const { return _isPlaying; }

  // Callbacks for rendering
  using EffectCallback =
      std::function<void(NotificationEffect, uint32_t, float)>;
  void setEffectCallback(EffectCallback cb) { _effectCallback = cb; }

private:
  NotificationConfig _config;
  bool _isPlaying;
  uint32_t _startTime;
  NotificationEffect _currentEffect;
  uint32_t _currentColor;
  uint16_t _currentDuration;

  EffectCallback _effectCallback;

  void playVibration(VibrationPattern pattern);
  void playSound(uint8_t soundId);
  float calculateProgress();
};

// ═══════════════════════════════════════════════════════════════════════════
// COLOR PALETTE (48 predefined + RGB picker)
// ═══════════════════════════════════════════════════════════════════════════
const uint32_t NOTIFICATION_COLORS[] = {
    // Neon Primary
    0x00FFF5, // Cyan
    0xFF00FF, // Magenta
    0xBD00FF, // Purple
    0x00FF9D, // Green
    0xFFCC00, // Yellow
    0xFF0055, // Red
    0x00A8FF, // Blue
    0xFF6600, // Orange

    // Neon Secondary
    0xFF0080, // Pink
    0x00FF00, // Lime
    0x39FF14, // Phosphor Green
    0xFE01B1, // Hot Pink
    0x01FEFE, // Electric Cyan
    0xFE0101, // Bright Red
    0xFFFF00, // Pure Yellow
    0x7B00FF, // Violet

    // Soft variants
    0x5CFFFA, // Light Cyan
    0xFF5CFF, // Light Magenta
    0xD15CFF, // Light Purple
    0x5CFFB0, // Light Green
    0xFFE05C, // Light Yellow
    0xFF5C7A, // Light Red

    // Dark variants
    0x008B8B, // Dark Cyan
    0x8B008B, // Dark Magenta
    0x4B0082, // Indigo
    0x006400, // Dark Green
    0xB8860B, // Dark Yellow
    0x8B0000, // Dark Red

    // Warm colors
    0xFF4500, // Orange Red
    0xFF6347, // Tomato
    0xFF7F50, // Coral
    0xFFA07A, // Light Salmon
    0xFFD700, // Gold
    0xFFA500, // Orange

    // Cool colors
    0x00CED1, // Dark Turquoise
    0x48D1CC, // Medium Turquoise
    0x40E0D0, // Turquoise
    0x00FFFF, // Aqua
    0x7FFFD4, // Aquamarine
    0x66CDAA, // Medium Aquamarine

    // Special
    0xFFFFFF, // White
    0x000000, // Black (for contrast)
    0x808080, // Gray
    0xC0C0C0, // Silver
    0xFFD700, // Gold
    0xB87333, // Copper
    0x8B4513, // Saddle Brown
    0x228B22, // Forest Green
};

#define NOTIFICATION_COLOR_COUNT                                               \
  (sizeof(NOTIFICATION_COLORS) / sizeof(NOTIFICATION_COLORS[0]))

extern NotificationsEngine notifications_engine;
