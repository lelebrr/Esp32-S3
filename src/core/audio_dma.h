/**
 * @file audio_dma.h
 * @brief I2S DMA Audio Driver for PCM5102A DAC
 * 
 * Features:
 * - DMA-based audio output (no CPU blocking)
 * - 8 buffers x 1024 samples in PSRAM
 * - APLL for accurate 44.1kHz sample rate
 * - Integration with PAM8403 amplifier
 * 
 * Hardware:
 * - PCM5102A: BCK=40, WS/LRCK=39, DATA=41
 * - PAM8403: Powered via IRF520 MOSFET
 * 
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef AUDIO_DMA_H
#define AUDIO_DMA_H

#include <Arduino.h>

#ifdef HAS_I2S_DAC

#include <driver/i2s.h>

// ============================================================================
// I2S CONFIGURATION
// ============================================================================

#ifndef I2S_NUM
#define I2S_NUM           I2S_NUM_0
#endif

#ifndef I2S_SAMPLE_RATE
#define I2S_SAMPLE_RATE   44100
#endif

#ifndef I2S_BITS
#define I2S_BITS          I2S_BITS_PER_SAMPLE_16BIT
#endif

#ifndef I2S_DMA_BUF_COUNT
#define I2S_DMA_BUF_COUNT 8
#endif

#ifndef I2S_DMA_BUF_LEN
#define I2S_DMA_BUF_LEN   1024
#endif

// ============================================================================
// AUDIO STATE
// ============================================================================

typedef enum {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_PLAYING,
    AUDIO_STATE_PAUSED,
    AUDIO_STATE_ERROR
} audio_state_t;

typedef struct {
    audio_state_t state;
    uint32_t sampleRate;
    uint8_t bitsPerSample;
    uint32_t bytesWritten;
    bool usePSRAM;
} audio_dma_stats_t;

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

/**
 * @brief Initialize I2S DMA driver
 * @param sampleRate Sample rate in Hz (default 44100)
 * @param bits Bits per sample (16 or 32)
 * @return true on success
 */
bool audio_dma_init(uint32_t sampleRate = I2S_SAMPLE_RATE, uint8_t bits = 16);

/**
 * @brief Write audio samples to DMA buffer (non-blocking)
 * @param samples Pointer to sample data
 * @param len Length in bytes
 * @param timeout_ms Timeout for DMA buffer availability
 * @return Bytes written
 */
size_t audio_dma_write(const void* samples, size_t len, uint32_t timeout_ms = 100);

/**
 * @brief Stop audio playback and flush buffers
 */
void audio_dma_stop();

/**
 * @brief Pause audio playback
 */
void audio_dma_pause();

/**
 * @brief Resume audio playback
 */
void audio_dma_resume();

/**
 * @brief Set sample rate dynamically
 */
bool audio_dma_set_rate(uint32_t sampleRate);

/**
 * @brief Set volume (0-100)
 * Note: Software volume control
 */
void audio_dma_set_volume(uint8_t volume);

/**
 * @brief Get current audio statistics
 */
audio_dma_stats_t* audio_dma_get_stats();

/**
 * @brief Check if audio is currently playing
 */
bool audio_dma_is_playing();

/**
 * @brief Allocate audio buffer in PSRAM
 * @param size Buffer size in bytes
 * @return Pointer to buffer or NULL
 */
void* audio_dma_alloc_buffer(size_t size);

/**
 * @brief Free audio buffer
 */
void audio_dma_free_buffer(void* buffer);

/**
 * @brief Play beep/tone for feedback
 * @param freq Frequency in Hz
 * @param duration Duration in ms
 */
void audio_dma_beep(uint16_t freq, uint16_t duration);

#endif // HAS_I2S_DAC

#endif // AUDIO_DMA_H
