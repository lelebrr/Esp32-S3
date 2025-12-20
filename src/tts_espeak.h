/**
 * @file tts_espeak.h
 * Monster S3 Firmware - TTS Audio Header
 */

#ifndef TTS_ESPEAK_H
#define TTS_ESPEAK_H

#include <stdint.h>

// Voice prompt types
typedef enum {
    TTS_NONE = 0,
    TTS_BOOT,
    TTS_ATTACK_START,
    TTS_SUCCESS,
    TTS_FAIL,
    TTS_BATTERY_LOW,
    TTS_STEALTH_MODE,
    TTS_WIFI_FOUND,
    TTS_NFC_CLONED
} TTSPrompt;

// Initialize TTS system
void setup_tts();

// Play a voice prompt
void speak(TTSPrompt prompt);

// Play custom text (fallback to serial)
void think_and_speak(const char* text);

// Volume control (0-21)
void tts_set_volume(uint8_t vol);

// Enable/disable TTS
void tts_set_enabled(bool enabled);

// Check if playing
bool tts_is_playing();

// Main loop (call frequently)
void loop_tts();

#endif // TTS_ESPEAK_H
