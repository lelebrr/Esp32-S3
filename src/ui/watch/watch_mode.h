/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Modo Relógio Smartwatch (ETAPA 5)
 * Sistema de watchfaces com funcionalidades em background
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <lvgl.h>

// ═══════════════════════════════════════════════════════════════════════════
// WATCHFACE TYPES
// ═══════════════════════════════════════════════════════════════════════════

enum WatchfaceType : uint8_t {
  WATCHFACE_DIGITAL = 0, // ⌚ Digital Clássico
  WATCHFACE_ANALOG,      // 🕐 Analógico Elegante
  WATCHFACE_CYBER,       // 📊 Cyberpunk Stats
  WATCHFACE_MASCOT,      // 🐉 Mascot Watch
  WATCHFACE_HACKER,      // 💀 Hacker Terminal
  WATCHFACE_MINIMAL,     // 🌙 Minimal Dark
  WATCHFACE_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

struct WatchConfig {
  uint8_t watchface_id;      // 0-5 (índice do mostrador)
  bool show_seconds;         // Mostrar segundos
  bool show_date;            // Mostrar data
  bool show_battery;         // Mostrar bateria
  bool show_wifi_status;     // Mostrar status WiFi
  bool show_handshake_count; // Mostrar contador
  bool background_sniffing;  // Sniffing em background
  bool auto_capture;         // Captura automática
  uint8_t brightness_level;  // Brilho (0-100)
  uint16_t screen_timeout;   // Timeout de tela (segundos)
};

struct WatchStats {
  uint32_t networks_found;
  uint32_t handshakes_captured;
  uint32_t probes_seen;
  uint32_t deauths_sent;
  uint32_t uptime_minutes;
};

// ═══════════════════════════════════════════════════════════════════════════
// WATCH MODE CLASS
// ═══════════════════════════════════════════════════════════════════════════

class WatchMode {
public:
  WatchMode();

  // Lifecycle
  void begin();
  void enter();
  void exit();
  void update();

  // State
  bool isActive() const { return _active; }

  // Watchface
  void setWatchface(WatchfaceType type);
  WatchfaceType getWatchface() const {
    return (WatchfaceType)_config.watchface_id;
  }
  void nextWatchface();
  void prevWatchface();

  // Configuration
  WatchConfig &getConfig() { return _config; }
  void saveConfig();
  void loadConfig();

  // Stats
  WatchStats &getStats() { return _stats; }

  // Time
  void getTime(uint8_t &hour, uint8_t &minute, uint8_t &second);
  void getDate(uint8_t &day, uint8_t &month, uint16_t &year);
  const char *getDayOfWeek();

  // Background tasks
  void enableBackgroundSniffing(bool enable);
  void enableAutoCapture(bool enable);

private:
  bool _active;
  WatchConfig _config;
  WatchStats _stats;
  lv_obj_t *_screen;
  uint32_t _lastUpdate;

  // Watchface rendering
  void renderDigital();
  void renderAnalog();
  void renderCyber();
  void renderMascot();
  void renderHacker();
  void renderMinimal();

  // UI elements
  lv_obj_t *_timeLabel;
  lv_obj_t *_dateLabel;
  lv_obj_t *_batteryIcon;
  lv_obj_t *_wifiIcon;
  lv_obj_t *_statsLabel;

  // Animation
  void animateTransition();
};

extern WatchMode watch_mode;
