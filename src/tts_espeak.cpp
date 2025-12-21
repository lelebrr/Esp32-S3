#include <Arduino.h>
#include "tts_espeak.h"
#include "Audio.h"
#include "pin_config.h"

Audio audio;

void setup_tts() {
    // Configura I2S com pinos do pin_config.h
    audio.setPinout(PIN_I2S_BCK, PIN_I2S_WS, PIN_I2S_DOUT);
    audio.setVolume(21); // Max
    Serial.println("[TTS] Audio System Initialized (I2S)");
}

void think_and_speak(const char* text) {
    Serial.printf("[VOICE] %s\n", text);
    // Como não temos a lib espeak-ng binária real aqui,
    // simulamos tentando tocar um arquivo WAV associado se existir,
    // ou apenas logamos.
    // Futuro: audio.connecttohost("http://api/tts...");
    
    // Se houver SD e arquivo com nome do texto (hash), tocar:
    // String path = "/voice/" + String(text) + ".wav";
    // if(SD.exists(path)) audio.connecttoFS(SD, path.c_str());
}

void loop_tts() {
    audio.loop();
}
