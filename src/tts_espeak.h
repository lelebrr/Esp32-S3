/**
 * @file tts_espeak.h
 * @brief TTS Real com I2S DMA - PT-BR
 */

#ifndef TTS_ESPEAK_H
#define TTS_ESPEAK_H

#include <Arduino.h>

// Inicialização
void setup_tts();
void loop_tts();

// API de voz
void tts_speak(const char* phrase_key);
void think_and_speak(const char* text);

// Controles
void tts_set_volume(float vol);
void tts_stop();
bool tts_is_playing();
void tts_test();

// Frases disponíveis (chaves):
// - "falha_injetada"
// - "deauth_rodando"
// - "ble_spamando"
// - "ataque_iniciado"
// - "ataque_parado"
// - "subghz_ativado"
// - "replay_ativo"
// - "codigo_enviado"
// - "relay_ativo"
// - "ia_escolheu"
// - "cartao_lido"
// - "modo_stealth"
// - "sistema_pronto"
// - "bateria_baixa"
// - "wifi_conectado"
// - "brute_force_completo"

#endif // TTS_ESPEAK_H
