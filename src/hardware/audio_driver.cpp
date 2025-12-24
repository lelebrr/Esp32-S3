/**
 * @file audio_driver.cpp
 * @brief Audio Driver with I2S Output (PCM5102A)
 * 
 * Provides audio output via I2S DAC for:
 * - Alert sounds
 * - TTS feedback (future)
 * - Attack notification beeps
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "hardware/audio_driver.h"
#include "pin_config.h"
#include <driver/i2s.h>

// Static member initialization
bool AudioDriver::initialized = false;

// I2S configuration
#define I2S_SAMPLE_RATE     44100
#define I2S_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define I2S_DMA_BUF_COUNT   8
#define I2S_DMA_BUF_LEN     64

void AudioDriver::init() {
    if (initialized) return;
    if (!AUDIO_ENABLED) {
        Serial.println("[AUDIO] Audio disabled in config");
        return;
    }

    Serial.println("[AUDIO] Initializing I2S for PCM5102A...");

    // I2S configuration
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = I2S_DMA_BUF_COUNT,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    // I2S pin configuration
    i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_BCK,
        .ws_io_num = PIN_I2S_WS,
        .data_out_num = PIN_I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // Install and start I2S driver
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO] I2S driver install failed: %d\n", err);
        return;
    }

    err = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO] I2S set pin failed: %d\n", err);
        return;
    }

    initialized = true;
    Serial.println("[AUDIO] I2S initialized on PCM5102A");
}

void AudioDriver::playTone(int frequency, int duration) {
    if (!initialized) init();
    if (!initialized) return;

    Serial.printf("[AUDIO] Playing tone: %d Hz for %d ms\n", frequency, duration);

    // Generate sine wave samples
    const int samples_per_cycle = I2S_SAMPLE_RATE / frequency;
    const int total_samples = (I2S_SAMPLE_RATE * duration) / 1000;
    
    int16_t sample_buffer[64];
    size_t bytes_written;
    int sample_index = 0;

    for (int i = 0; i < total_samples; i += 32) {
        for (int j = 0; j < 32 && (i + j) < total_samples; j++) {
            // Simple sine approximation
            float phase = (float)(sample_index % samples_per_cycle) / samples_per_cycle;
            float sine = sin(phase * 2.0f * 3.14159f);
            int16_t sample = (int16_t)(sine * 16000);  // ~50% volume
            
            sample_buffer[j * 2] = sample;      // Left channel
            sample_buffer[j * 2 + 1] = sample;  // Right channel
            sample_index++;
        }

        i2s_write(I2S_NUM_0, sample_buffer, 64 * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    }

    // Fade out to avoid click
    for (int i = 0; i < 32; i++) {
        sample_buffer[i * 2] = 0;
        sample_buffer[i * 2 + 1] = 0;
    }
    i2s_write(I2S_NUM_0, sample_buffer, 64 * sizeof(int16_t), &bytes_written, portMAX_DELAY);
}

void AudioDriver::playPiezoTone(int frequency, int duration) {
    if (!PIEZO_ENABLED) {
        Serial.println("[PIEZO] Piezo disabled in config");
        return;
    }

    pinMode(PIN_PIEZO, OUTPUT);
    tone(PIN_PIEZO, frequency, duration);
    Serial.printf("[PIEZO] Playing tone: %d Hz for %d ms\n", frequency, duration);
    delay(duration);
    noTone(PIN_PIEZO);
}

void AudioDriver::playBeep(int count, int frequency, int duration) {
    for (int i = 0; i < count; i++) {
        if (initialized) {
            playTone(frequency, duration);
        } else {
            playPiezoTone(frequency, duration);
        }
        if (i < count - 1) delay(duration / 2);
    }
}

void AudioDriver::playSuccess() {
    playTone(1000, 100);
    playTone(1500, 100);
    playTone(2000, 200);
}

void AudioDriver::playError() {
    playTone(500, 200);
    playTone(300, 400);
}

void AudioDriver::playAlert() {
    playTone(2500, 100);
    delay(50);
    playTone(2500, 100);
    delay(50);
    playTone(2500, 100);
}

void AudioDriver::stopAudio() {
    if (!initialized) return;
    
    // Clear I2S buffer
    i2s_zero_dma_buffer(I2S_NUM_0);
    Serial.println("[AUDIO] Audio stopped");
}

bool AudioDriver::isInitialized() {
    return initialized;
}
