/**
 * @file tts_espeak.cpp
 * Monster S3 Firmware - TTS Audio System
 * PCM5102A DAC via I2S + WAV files from SD
 * 
 * Plays pre-recorded PT-BR voice prompts for attacks and status
 */

#include <Arduino.h>
#include "tts_espeak.h"
#include "Audio.h"
#include "pin_config.h"
#include <SD.h>

// Audio I2S instance
static Audio audio;
static bool tts_initialized = false;
static bool tts_enabled = true;
static uint8_t tts_volume = 21; // 0-21

// Voice prompt paths on SD card
static const char* VOICE_PATH_ATTACK_START = "/voice/ataque_iniciado.wav";
static const char* VOICE_PATH_SUCCESS = "/voice/sucesso.wav";
static const char* VOICE_PATH_FAIL = "/voice/falha.wav";
static const char* VOICE_PATH_BATTERY_LOW = "/voice/bateria_baixa.wav";
static const char* VOICE_PATH_STEALTH_MODE = "/voice/modo_stealth.wav";
static const char* VOICE_PATH_BOOT = "/voice/boot.wav";
static const char* VOICE_PATH_WIFI_FOUND = "/voice/wifi_encontrado.wav";
static const char* VOICE_PATH_NFC_CLONED = "/voice/nfc_clonado.wav";

/**
 * @brief Initialize TTS audio system with I2S
 */
void setup_tts() {
    // Configure I2S with PCM5102A DAC pins
    audio.setPinout(PIN_I2S_BCK, PIN_I2S_WS, PIN_I2S_DOUT);
    audio.setVolume(tts_volume);
    
    // Check if voice files exist on SD
    if (SD.exists(VOICE_PATH_BOOT)) {
        tts_initialized = true;
        Serial.println("[TTS] Audio System Initialized (I2S + SD WAV)");
        
        // Play boot sound
        speak(TTS_BOOT);
    } else {
        Serial.println("[TTS] WARNING: Voice files not found on SD!");
        Serial.println("[TTS] Expected path: /voice/*.wav");
        tts_initialized = false;
    }
}

/**
 * @brief Play a voice prompt by type
 * @param prompt Type of voice prompt to play
 */
void speak(TTSPrompt prompt) {
    if (!tts_enabled) {
        Serial.println("[TTS] Muted");
        return;
    }
    
    const char* path = nullptr;
    const char* fallback_text = nullptr;
    
    switch (prompt) {
        case TTS_ATTACK_START:
            path = VOICE_PATH_ATTACK_START;
            fallback_text = "Ataque iniciado";
            break;
        case TTS_SUCCESS:
            path = VOICE_PATH_SUCCESS;
            fallback_text = "Sucesso";
            break;
        case TTS_FAIL:
            path = VOICE_PATH_FAIL;
            fallback_text = "Falha";
            break;
        case TTS_BATTERY_LOW:
            path = VOICE_PATH_BATTERY_LOW;
            fallback_text = "Bateria baixa";
            break;
        case TTS_STEALTH_MODE:
            path = VOICE_PATH_STEALTH_MODE;
            fallback_text = "Modo stealth ativado";
            break;
        case TTS_BOOT:
            path = VOICE_PATH_BOOT;
            fallback_text = "Monster S3 pronto";
            break;
        case TTS_WIFI_FOUND:
            path = VOICE_PATH_WIFI_FOUND;
            fallback_text = "WiFi encontrado";
            break;
        case TTS_NFC_CLONED:
            path = VOICE_PATH_NFC_CLONED;
            fallback_text = "NFC clonado";
            break;
        default:
            return;
    }
    
    // Try to play WAV file
    if (tts_initialized && SD.exists(path)) {
        audio.connecttoFS(SD, path);
        Serial.printf("[TTS] Playing: %s\n", path);
    } else {
        // Fallback to serial output if no audio
        Serial.printf("[VOICE] %s\n", fallback_text);
    }
}

/**
 * @brief Play custom text (logs to serial, future: dynamic TTS)
 * @param text Text to speak
 */
void think_and_speak(const char* text) {
    Serial.printf("[VOICE] %s\n", text);
    
    // For now, we can only play pre-recorded prompts
    // Future: implement eSpeak-NG for dynamic TTS
}

/**
 * @brief Set TTS volume (0-21)
 */
void tts_set_volume(uint8_t vol) {
    tts_volume = (vol > 21) ? 21 : vol;
    audio.setVolume(tts_volume);
    Serial.printf("[TTS] Volume: %d\n", tts_volume);
}

/**
 * @brief Enable/disable TTS output
 */
void tts_set_enabled(bool enabled) {
    tts_enabled = enabled;
    Serial.printf("[TTS] %s\n", enabled ? "Enabled" : "Muted");
}

/**
 * @brief Check if TTS is currently playing
 */
bool tts_is_playing() {
    return audio.isRunning();
}

/**
 * @brief TTS main loop - must be called frequently
 */
void loop_tts() {
    audio.loop();
}
