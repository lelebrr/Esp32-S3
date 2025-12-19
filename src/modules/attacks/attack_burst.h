/**
 * @file attack_burst.h
 * @brief High-Speed Attack Utilities for ESP32-S3
 * 
 * Features:
 * - NFC fault injection burst (PN532 + IRF520 field boost)
 * - WiFi deauth packet bursts (1200+ pkt/s @ 240MHz)
 * - PSRAM buffer allocation for large payloads
 * - Precision timing with microsecond delays
 * 
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef ATTACK_BURST_H
#define ATTACK_BURST_H

#include <Arduino.h>

#ifdef CYD_S3

#include "s3_performance.h"
#include "power_gating.h"

// ============================================================================
// BURST CONFIGURATION
// ============================================================================

#ifndef DEAUTH_BURST_RATE
#define DEAUTH_BURST_RATE     1200   // Packets per second target
#endif

#ifndef DEAUTH_BURST_DELAY_US
#define DEAUTH_BURST_DELAY_US 833    // 1000000 / 1200
#endif

#ifndef NFC_BURST_FREQ_HZ
#define NFC_BURST_FREQ_HZ     100    // Field on/off cycles per second
#endif

#ifndef NFC_BURST_DELAY_US
#define NFC_BURST_DELAY_US    10000  // 1000000 / 100
#endif

// ============================================================================
// ATTACK STATE
// ============================================================================

typedef struct {
    bool active;
    uint32_t packetsTransmitted;
    uint32_t errorsCount;
    uint32_t startTime;
    float currentPPS;  // Packets per second
} burst_state_t;

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

/**
 * @brief Enter attack burst mode
 * - Locks CPU at 240MHz
 * - Disables light sleep
 * - Powers on required modules
 */
void attack_burst_begin();

/**
 * @brief Exit attack burst mode
 * - Restores normal PM
 * - Updates stats
 */
void attack_burst_end();

/**
 * @brief Send WiFi deauth burst
 * @param bssid Target AP BSSID
 * @param channel WiFi channel
 * @param count Number of packets to send
 * @return Packets successfully sent
 */
uint32_t attack_deauth_burst(const uint8_t* bssid, uint8_t channel, uint32_t count);

/**
 * @brief NFC fault injection burst
 * Uses PN532 with IRF520 field boost for rapid field cycling
 * @param cycles Number of on/off cycles
 */
void attack_nfc_fault_burst(uint16_t cycles);

/**
 * @brief Rapid NFC card reading with field manipulation
 * @param callback Function called for each successful read
 * @param duration_ms Duration to run
 * @return Number of successful reads
 */
uint32_t attack_nfc_skim_burst(
    void (*callback)(const uint8_t* uid, uint8_t len),
    uint32_t duration_ms
);

/**
 * @brief Get burst attack statistics
 */
burst_state_t* attack_get_burst_state();

/**
 * @brief Allocate PSRAM buffer for attack payload
 * @param size Buffer size in bytes
 * @return Aligned buffer pointer or NULL
 */
inline void* attack_alloc_payload(size_t size) {
    return psram_alloc_aligned(size);
}

/**
 * @brief Precision microsecond burst delay
 * Does not yield, use for timing-critical bursts
 */
#define ATTACK_DELAY_US(us) BURST_DELAY_US(us)

// ============================================================================
// ATTACK HELPERS
// ============================================================================

/**
 * @brief Generate deauth frame for target
 */
void attack_build_deauth_frame(uint8_t* buffer, const uint8_t* bssid, uint16_t seq);

/**
 * @brief Audio beep on successful attack
 */
void attack_success_beep();

#endif // CYD_S3

#endif // ATTACK_BURST_H
