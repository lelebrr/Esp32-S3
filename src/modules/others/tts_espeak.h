#ifndef TTS_ESPEAK_H
#define TTS_ESPEAK_H

#include <Arduino.h>
#include <esp_heap_caps.h>

// eSpeak-NG includes
#include "espeak-ng/speak_lib.h"
#include "espeak-ng/espeak_ng.h"

// Buffer size for TTS in PSRAM (1KB should be enough for short phrases)
#define TTS_BUFFER_SIZE 1024

// PSRAM voice buffer
extern uint8_t *voice_buffer;

// TTS functions
bool init_tts();
void think_and_speak(const char* phrase);
void think_and_speak(String phrase);
bool is_tts_speaking();
void wait_tts_complete();

// FreeRTOS task for async TTS
void tts_task(void *pvParameters);
extern TaskHandle_t ttsTaskHandle;
extern bool new_phrase_available;
extern char current_phrase[256];

#endif // TTS_ESPEAK_H
