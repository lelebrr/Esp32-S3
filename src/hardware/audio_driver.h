#pragma once

/**
 * @file audio_driver.h
 * @brief Driver de áudio unificado - stubs para compilação
 *
 * Este arquivo define interfaces de áudio que podem ser implementadas
 * para dispositivos com buzzer/speaker ou mantidas como stubs.
 */

#include <Arduino.h>

// ═══════════════════════════════════════════════════════════════════════════
// SOUND TYPES
// ═══════════════════════════════════════════════════════════════════════════
enum SoundType : uint8_t {
    SOUND_CLICK = 0,
    SOUND_SUCCESS,
    SOUND_ERROR,
    SOUND_ALERT,
    SOUND_BOOT,
    SOUND_HANDSHAKE,
    SOUND_ATTACK_START,
    SOUND_ATTACK_COMPLETE,
    SOUND_SCAN_START,
    SOUND_BEEP,
    SOUND_TYPE_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// AUDIO DRIVER CLASS
// ═══════════════════════════════════════════════════════════════════════════
class AudioDriver {
public:
    AudioDriver() : _volume(80), _isPlaying(false), _isRecording(false) {}

    // Initialization
    void begin() {}

    // Volume control (0-100)
    void setVolume(uint8_t percent) { _volume = constrain(percent, 0, 100); }
    uint8_t getVolume() const { return _volume; }

    // Play built-in sound
    void playSound(SoundType type) {
        // Stub - pode ser implementado com buzzer PWM
        _isPlaying = true;
        _currentSound = type;
        // Para CYD sem speaker, isso é no-op
    }

    // Play tone (frequency in Hz, duration in ms)
    void playTone(uint16_t frequency, uint16_t duration_ms) {
        // Stub - pode usar ledcWriteTone em pino de buzzer
        _isPlaying = true;
        delay(duration_ms);
        _isPlaying = false;
    }

    // Play raw PCM data
    void playRaw(const int16_t *data, size_t samples) {
        // Stub - requer I2S para implementação real
        (void)data;
        (void)samples;
    }

    // Stop playback
    void stop() { _isPlaying = false; }
    bool isPlaying() const { return _isPlaying; }

    // Recording (for walkie-talkie feature)
    void setMicGain(uint8_t gain) { _micGain = gain; }
    void startRecording() { _isRecording = true; }
    void stopRecording() { _isRecording = false; }
    bool isRecording() const { return _isRecording; }

    // Read audio samples from mic (returns number of samples read)
    size_t readAudioSamples(int16_t *buffer, size_t maxSamples) {
        // Stub - requer I2S mic para implementação real
        (void)buffer;
        (void)maxSamples;
        return 0;
    }

private:
    uint8_t _volume;
    uint8_t _micGain;
    bool _isPlaying;
    bool _isRecording;
    SoundType _currentSound;
};

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL INSTANCE
// ═══════════════════════════════════════════════════════════════════════════
extern AudioDriver audioDriver;
