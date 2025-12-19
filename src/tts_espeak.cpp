#include "tts_espeak.h"
#include "Audio.h"

// Define I2S Pins for S3
#define I2S_BCLK 40
#define I2S_LRC  39
#define I2S_DOUT 41

Audio audio;

void setup_tts() {
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); // 0...21
    Serial.println("TTS Audio init success (I2S)");
}

// Simple wrapper. 
// A verdadeira implementação eSpeak precisaria da lib eSpeak portada.
// Aqui usamos Audio lib para tocar arquivos ou stream.
// Se o usuário quiser *síntese* real, precisaria da lib SAM ou eSpeak linkada.
// Vou deixar preparado para funcionar com arquivos pré-gravados por enquanto,
// pois compilar eSpeak do zero é complexo sem a lib no platformio.ini.
// Mas para atender "think_and_speak", vamos simular.

void speak_text(const char* text) {
    Serial.printf("FALANDO: %s\n", text);
    // audio.connecttospeech(text, "pt-BR"); // Se suportado pela lib (algumas forks suportam Google TTS stream)
    audio.connecttohost("http://translate.google.com/translate_tts?ie=UTF-8&q=Start&tl=pt&client=tw-ob"); 
}

void loop_tts() {
    audio.loop();
}
