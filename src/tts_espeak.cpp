/**
 * @file tts_espeak.cpp
 * @brief TTS Real com WAV do SD + I2S DMA - PT-BR
 * 
 * Reproduz arquivos WAV pré-gravados do cartão SD via I2S.
 * Arquivos devem estar em /tts/frases/ no formato 16-bit PCM 44100Hz mono.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include <Arduino.h>
#include "tts_espeak.h"
#include "pin_config.h"
#include <driver/i2s.h>
#include <SD.h>

// ============================================================================
// CONFIGURAÇÃO
// ============================================================================

#define TTS_SAMPLE_RATE     22050
#define TTS_BUFFER_SIZE     512
#define TTS_I2S_NUM         I2S_NUM_0

// Estado do TTS
static bool tts_initialized = false;
static bool tts_playing = false;
static float tts_volume = 0.8f;
static File current_wav_file;
static TaskHandle_t tts_task_handle = NULL;

// Mapeamento de frases para arquivos
struct PhraseMapping {
    const char* key;
    const char* filename;
};

static const PhraseMapping phrase_map[] = {
    {"falha_injetada",       "/tts/frases/falha.wav"},
    {"deauth_rodando",       "/tts/frases/deauth.wav"},
    {"ble_spamando",         "/tts/frases/ble.wav"},
    {"ataque_iniciado",      "/tts/frases/inicio.wav"},
    {"ataque_parado",        "/tts/frases/parado.wav"},
    {"subghz_ativado",       "/tts/frases/subghz.wav"},
    {"replay_ativo",         "/tts/frases/replay.wav"},
    {"codigo_enviado",       "/tts/frases/codigo.wav"},
    {"relay_ativo",          "/tts/frases/relay.wav"},
    {"ia_escolheu",          "/tts/frases/ia.wav"},
    {"cartao_lido",          "/tts/frases/cartao.wav"},
    {"modo_stealth",         "/tts/frases/stealth.wav"},
    {"sistema_pronto",       "/tts/frases/pronto.wav"},
    {"bateria_baixa",        "/tts/frases/bateria.wav"},
    {"wifi_conectado",       "/tts/frases/wifi.wav"},
    {"brute_force_completo", "/tts/frases/brute.wav"},
    {"vou_deauth",           "/tts/frases/vou_deauth.wav"},
    {nullptr, nullptr}
};

// ============================================================================
// I2S HELPERS
// ============================================================================

static bool init_i2s_tts() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = TTS_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_BCK,
        .ws_io_num = PIN_I2S_WS,
        .data_out_num = PIN_I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t err = i2s_driver_install(TTS_I2S_NUM, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[TTS] I2S install failed: %d\n", err);
        return false;
    }

    err = i2s_set_pin(TTS_I2S_NUM, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("[TTS] I2S set pin failed: %d\n", err);
        return false;
    }

    return true;
}

// ============================================================================
// WAV FILE PARSING
// ============================================================================

struct WavHeader {
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t data_size;
};

static bool parse_wav_header(File &file, WavHeader &header) {
    char chunk_id[5] = {0};
    
    // RIFF header
    file.read((uint8_t*)chunk_id, 4);
    if (strcmp(chunk_id, "RIFF") != 0) {
        Serial.println("[TTS] Invalid WAV: no RIFF");
        return false;
    }
    
    file.seek(8);
    file.read((uint8_t*)chunk_id, 4);
    if (strcmp(chunk_id, "WAVE") != 0) {
        Serial.println("[TTS] Invalid WAV: no WAVE");
        return false;
    }
    
    // Find fmt chunk
    file.seek(12);
    while (file.available()) {
        file.read((uint8_t*)chunk_id, 4);
        uint32_t chunk_size;
        file.read((uint8_t*)&chunk_size, 4);
        
        if (strcmp(chunk_id, "fmt ") == 0) {
            uint16_t audio_format;
            file.read((uint8_t*)&audio_format, 2);
            file.read((uint8_t*)&header.num_channels, 2);
            file.read((uint8_t*)&header.sample_rate, 4);
            file.seek(file.position() + 6); // skip byte rate, block align
            file.read((uint8_t*)&header.bits_per_sample, 2);
            break;
        } else {
            file.seek(file.position() + chunk_size);
        }
    }
    
    // Find data chunk
    while (file.available()) {
        file.read((uint8_t*)chunk_id, 4);
        uint32_t chunk_size;
        file.read((uint8_t*)&chunk_size, 4);
        
        if (strcmp(chunk_id, "data") == 0) {
            header.data_size = chunk_size;
            return true;
        } else {
            file.seek(file.position() + chunk_size);
        }
    }
    
    return false;
}

// ============================================================================
// PLAYBACK TASK
// ============================================================================

static void tts_playback_task(void *param) {
    const char* filepath = (const char*)param;
    
    File wav = SD.open(filepath, FILE_READ);
    if (!wav) {
        Serial.printf("[TTS] Failed to open: %s\n", filepath);
        tts_playing = false;
        vTaskDelete(NULL);
        return;
    }
    
    WavHeader header;
    if (!parse_wav_header(wav, header)) {
        wav.close();
        tts_playing = false;
        vTaskDelete(NULL);
        return;
    }
    
    Serial.printf("[TTS] Playing: %s (%d Hz, %d-bit, %d ch)\n", 
                  filepath, header.sample_rate, header.bits_per_sample, header.num_channels);
    
    // Adjust I2S sample rate if needed
    i2s_set_sample_rates(TTS_I2S_NUM, header.sample_rate);
    
    // Read and play
    int16_t buffer[TTS_BUFFER_SIZE];
    size_t bytes_written;
    uint32_t bytes_read = 0;
    
    while (bytes_read < header.data_size && tts_playing) {
        size_t to_read = min((size_t)(header.data_size - bytes_read), 
                             sizeof(buffer));
        size_t actual = wav.read((uint8_t*)buffer, to_read);
        
        if (actual == 0) break;
        
        // Apply volume
        for (int i = 0; i < actual / 2; i++) {
            buffer[i] = (int16_t)(buffer[i] * tts_volume);
        }
        
        i2s_write(TTS_I2S_NUM, buffer, actual, &bytes_written, portMAX_DELAY);
        bytes_read += actual;
    }
    
    // Fade out
    memset(buffer, 0, sizeof(buffer));
    i2s_write(TTS_I2S_NUM, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    
    wav.close();
    tts_playing = false;
    Serial.println("[TTS] Playback complete");
    
    vTaskDelete(NULL);
}

// ============================================================================
// PUBLIC API
// ============================================================================

void setup_tts() {
    if (tts_initialized) return;
    
    Serial.println("[TTS] Initializing TTS system...");
    
    // Check SD card
    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[TTS] SD card not available - TTS disabled");
        return;
    }
    
    // Check TTS folder exists
    if (!SD.exists("/tts/frases")) {
        Serial.println("[TTS] Creating /tts/frases directory");
        SD.mkdir("/tts");
        SD.mkdir("/tts/frases");
    }
    
    // Init I2S
    if (!init_i2s_tts()) {
        Serial.println("[TTS] I2S init failed - TTS disabled");
        return;
    }
    
    tts_initialized = true;
    Serial.println("[TTS] TTS system ready (WAV from SD)");
}

void tts_speak(const char* phrase_key) {
    if (!tts_initialized) {
        Serial.printf("[VOICE] %s (TTS not initialized)\n", phrase_key);
        return;
    }
    
    if (tts_playing) {
        tts_stop();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    
    // Find file for phrase
    const char* filepath = nullptr;
    for (int i = 0; phrase_map[i].key != nullptr; i++) {
        if (strcmp(phrase_map[i].key, phrase_key) == 0) {
            filepath = phrase_map[i].filename;
            break;
        }
    }
    
    if (filepath == nullptr) {
        Serial.printf("[TTS] Unknown phrase: %s\n", phrase_key);
        return;
    }
    
    // Check file exists
    if (!SD.exists(filepath)) {
        Serial.printf("[VOICE] %s (file missing: %s)\n", phrase_key, filepath);
        return;
    }
    
    tts_playing = true;
    
    // Start playback task
    static char path_buf[64];
    strncpy(path_buf, filepath, sizeof(path_buf));
    xTaskCreatePinnedToCore(tts_playback_task, "TTS", 4096, path_buf, 2, &tts_task_handle, 0);
}

void think_and_speak(const char* text) {
    // For dynamic text, just log (full TTS synthesis not supported on ESP32)
    Serial.printf("[VOICE] %s\n", text);
    
    // Try to find closest phrase match
    if (strstr(text, "deauth") != nullptr) {
        tts_speak("vou_deauth");
    } else if (strstr(text, "BLE") != nullptr || strstr(text, "spam") != nullptr) {
        tts_speak("ble_spamando");
    } else if (strstr(text, "iniciado") != nullptr || strstr(text, "start") != nullptr) {
        tts_speak("ataque_iniciado");
    } else if (strstr(text, "parado") != nullptr || strstr(text, "stop") != nullptr) {
        tts_speak("ataque_parado");
    }
}

void tts_set_volume(float vol) {
    tts_volume = constrain(vol, 0.0f, 1.0f);
    Serial.printf("[TTS] Volume set to %.0f%%\n", tts_volume * 100);
}

void tts_stop() {
    if (!tts_playing) return;
    
    tts_playing = false;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    i2s_zero_dma_buffer(TTS_I2S_NUM);
    Serial.println("[TTS] Playback stopped");
}

bool tts_is_playing() {
    return tts_playing;
}

void tts_test() {
    Serial.println("[TTS] Testing TTS system...");
    tts_speak("sistema_pronto");
}

void loop_tts() {
    // Nothing needed - playback runs in separate task
}
