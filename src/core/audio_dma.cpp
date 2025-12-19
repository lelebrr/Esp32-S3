/**
 * @file audio_dma.cpp
 * @brief I2S DMA Audio Driver Implementation
 */

#include "audio_dma.h"

#ifdef HAS_I2S_DAC

#include <globals.h>
#include <esp_heap_caps.h>
#include <math.h>
#include "power_gating.h"

// ============================================================================
// STATIC VARIABLES
// ============================================================================

static bool isInitialized = false;
static uint8_t currentVolume = 100;

static audio_dma_stats_t stats = {
    .state = AUDIO_STATE_IDLE,
    .sampleRate = I2S_SAMPLE_RATE,
    .bitsPerSample = 16,
    .bytesWritten = 0,
    .usePSRAM = false
};

// ============================================================================
// INITIALIZATION
// ============================================================================

bool audio_dma_init(uint32_t sampleRate, uint8_t bits) {
    if (isInitialized) {
        audio_dma_stop();
    }
    
    // Power on audio module via MOSFET
    #ifdef HAS_MOSFET
    power_module_enable(MODULE_PAM8403);
    delay(50);  // Allow power stabilization
    #endif
    
    // I2S configuration with DMA
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = sampleRate,
        .bits_per_sample = (bits == 32) ? I2S_BITS_PER_SAMPLE_32BIT : I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,
        .dma_buf_count = I2S_DMA_BUF_COUNT,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = true,  // Accurate clock
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    // Pin configuration for PCM5102A
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRCK_PIN,
        .data_out_num = I2S_DOUT_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    // Install driver
    esp_err_t err = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        #if CORE_DEBUG_LEVEL > 0
        Serial.printf("[AUDIO] Driver install failed: %d\n", err);
        #endif
        return false;
    }
    
    // Set pins
    err = i2s_set_pin(I2S_NUM, &pin_config);
    if (err != ESP_OK) {
        #if CORE_DEBUG_LEVEL > 0
        Serial.printf("[AUDIO] Pin config failed: %d\n", err);
        #endif
        i2s_driver_uninstall(I2S_NUM);
        return false;
    }
    
    // Clear DMA buffers
    i2s_zero_dma_buffer(I2S_NUM);
    
    // Update stats
    stats.sampleRate = sampleRate;
    stats.bitsPerSample = bits;
    stats.usePSRAM = psramFound();
    stats.state = AUDIO_STATE_IDLE;
    
    isInitialized = true;
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.printf("[AUDIO] DMA initialized: %dHz, %dbit, %d buffers\n", 
                  sampleRate, bits, I2S_DMA_BUF_COUNT);
    #endif
    
    return true;
}

// ============================================================================
// PLAYBACK CONTROL
// ============================================================================

size_t audio_dma_write(const void* samples, size_t len, uint32_t timeout_ms) {
    if (!isInitialized || !samples || len == 0) return 0;
    
    stats.state = AUDIO_STATE_PLAYING;
    
    size_t bytesWritten = 0;
    esp_err_t err = i2s_write(I2S_NUM, samples, len, &bytesWritten, timeout_ms / portTICK_PERIOD_MS);
    
    if (err == ESP_OK) {
        stats.bytesWritten += bytesWritten;
    }
    
    return bytesWritten;
}

void audio_dma_stop() {
    if (!isInitialized) return;
    
    i2s_stop(I2S_NUM);
    i2s_zero_dma_buffer(I2S_NUM);
    i2s_driver_uninstall(I2S_NUM);
    
    // Power off audio module
    #ifdef HAS_MOSFET
    power_module_disable(MODULE_PAM8403);
    #endif
    
    stats.state = AUDIO_STATE_IDLE;
    isInitialized = false;
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[AUDIO] DMA stopped");
    #endif
}

void audio_dma_pause() {
    if (!isInitialized) return;
    i2s_stop(I2S_NUM);
    stats.state = AUDIO_STATE_PAUSED;
}

void audio_dma_resume() {
    if (!isInitialized) return;
    i2s_start(I2S_NUM);
    stats.state = AUDIO_STATE_PLAYING;
}

bool audio_dma_set_rate(uint32_t sampleRate) {
    if (!isInitialized) return false;
    
    esp_err_t err = i2s_set_sample_rates(I2S_NUM, sampleRate);
    if (err == ESP_OK) {
        stats.sampleRate = sampleRate;
        return true;
    }
    return false;
}

void audio_dma_set_volume(uint8_t volume) {
    currentVolume = (volume > 100) ? 100 : volume;
}

// ============================================================================
// STATUS
// ============================================================================

audio_dma_stats_t* audio_dma_get_stats() {
    return &stats;
}

bool audio_dma_is_playing() {
    return stats.state == AUDIO_STATE_PLAYING;
}

// ============================================================================
// BUFFER MANAGEMENT
// ============================================================================

void* audio_dma_alloc_buffer(size_t size) {
    if (psramFound()) {
        return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    }
    return malloc(size);
}

void audio_dma_free_buffer(void* buffer) {
    if (buffer) free(buffer);
}

// ============================================================================
// BEEP/TONE GENERATION
// ============================================================================

void audio_dma_beep(uint16_t freq, uint16_t duration) {
    if (!audio_dma_init(22050, 16)) return;  // Lower sample rate for tones
    
    // Calculate samples
    uint32_t numSamples = (22050 * duration) / 1000;
    size_t bufferSize = numSamples * 4;  // Stereo 16-bit
    
    int16_t* buffer = (int16_t*)audio_dma_alloc_buffer(bufferSize);
    if (!buffer) {
        audio_dma_stop();
        return;
    }
    
    // Generate sine wave
    float omega = 2.0f * M_PI * freq / 22050.0f;
    int16_t amplitude = 16383 * currentVolume / 100;
    
    for (uint32_t i = 0; i < numSamples; i++) {
        int16_t sample = (int16_t)(amplitude * sinf(omega * i));
        buffer[i * 2] = sample;      // Left
        buffer[i * 2 + 1] = sample;  // Right
    }
    
    // Play
    audio_dma_write(buffer, bufferSize, 1000);
    delay(duration);  // Wait for playback
    
    // Cleanup
    audio_dma_free_buffer(buffer);
    audio_dma_stop();
}

#endif // HAS_I2S_DAC
