#ifndef TTS_ESPEAK_H
#define TTS_ESPEAK_H

#include <Arduino.h>

void setup_tts();
void speak_text(const char* text);

#endif
