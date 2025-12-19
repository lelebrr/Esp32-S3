#pragma once

#include <Arduino.h>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════
// SOUND INFO
// ═══════════════════════════════════════════════════════════════════════════
struct SoundInfo {
  char filename[64];
  uint32_t filesize;
  uint16_t duration_ms; // Estimated duration
  bool isPreset;        // Built-in sound
};

// ═══════════════════════════════════════════════════════════════════════════
// SOUND CONFIG
// ═══════════════════════════════════════════════════════════════════════════
struct SoundsConfig {
  uint8_t masterVolume; // 0-100%
  bool soundOnHandshake;
  bool soundOnBoot;
  bool soundOnAttack;
  uint8_t handshakeSoundId;
  uint8_t bootSoundId;
  uint8_t attackSoundId;
};

// ═══════════════════════════════════════════════════════════════════════════
// SOUNDS MANAGER CLASS
// ═══════════════════════════════════════════════════════════════════════════
class SoundsManager {
public:
  SoundsManager();

  // Initialization
  void begin();

  // Sound library
  int scanSounds();
  std::vector<SoundInfo> getLibrary() const { return _library; }
  int getSoundCount() const { return _library.size(); }

  // Playback
  bool play(uint8_t soundId);
  bool playByName(const char *filename);
  bool playPreset(uint8_t presetId);
  void stop();
  bool isPlaying() const { return _isPlaying; }

  // Volume
  void setVolume(uint8_t percent);
  uint8_t getVolume() const { return _config.masterVolume; }

  // Event sounds
  void setHandshakeSound(uint8_t soundId) {
    _config.handshakeSoundId = soundId;
  }
  void setBootSound(uint8_t soundId) { _config.bootSoundId = soundId; }
  void setAttackSound(uint8_t soundId) { _config.attackSoundId = soundId; }

  void enableHandshakeSound(bool enable) { _config.soundOnHandshake = enable; }
  void enableBootSound(bool enable) { _config.soundOnBoot = enable; }
  void enableAttackSound(bool enable) { _config.soundOnAttack = enable; }

  // Config
  void setConfig(const SoundsConfig &config);
  SoundsConfig getConfig() const { return _config; }

  // File operations
  bool deleteSound(const char *filename);

  // Trigger events
  void playHandshakeSound();
  void playBootSound();
  void playAttackSound();

  // Test
  void test(uint8_t soundId);

private:
  SoundsConfig _config;
  std::vector<SoundInfo> _library;
  bool _isPlaying;
  uint8_t _currentSoundId;

  bool loadWav(const char *path);
};

// ═══════════════════════════════════════════════════════════════════════════
// PRESET SOUNDS (20 built-in)
// ═══════════════════════════════════════════════════════════════════════════
enum PresetSound : uint8_t {
  PRESET_BEEP_SHORT = 0,
  PRESET_BEEP_DOUBLE,
  PRESET_BEEP_LONG,
  PRESET_KA_CHING,
  PRESET_CONFIRM,
  PRESET_ERROR,
  PRESET_WARNING,
  PRESET_SUCCESS,
  PRESET_NOTIFICATION,
  PRESET_ALERT,
  PRESET_POWERUP,
  PRESET_POWERDOWN,
  PRESET_CLICK,
  PRESET_SWIPE,
  PRESET_UNLOCK,
  PRESET_LOCK,
  PRESET_CAPTURE,
  PRESET_ATTACK,
  PRESET_SCAN,
  PRESET_COMPLETE,
  PRESET_COUNT
};

#define SOUNDS_DIR "/sounds"
#define SOUND_EXTENSION ".wav"
#define SOUND_MAX_SIZE (150 * 1024) // 150KB max per sound

extern SoundsManager sounds_manager;
