/**
 * @file brute_generator.h
 * @brief Brazilian Wordlist Generator for ESP32-S3
 *
 * Generates 10M+ passwords targeted at Brazilian systems:
 * - Porteiro codes (0000-9999)
 * - Date patterns (DDMMYYYY, DDMMYY)
 * - ISP defaults (GVT, Vivo, Claro, TIM, Oi)
 * - CEP-based passwords
 * - Common Brazilian patterns
 *
 * @author MorphNode Team
 * @date 2025-12-28
 */

#ifndef BRUTE_GENERATOR_H
#define BRUTE_GENERATOR_H

#include <Arduino.h>

// ============================================================================
// GENERATION OPTIONS
// ============================================================================

#define BRUTE_FILE_PATH "/cve/brute_br.txt"
#define BRUTE_FILE_PATH_WIFI "/cve/brute_wifi_br.txt"
#define BRUTE_FILE_PATH_WPS "/cve/brute_wps_br.txt"

// ============================================================================
// PROGRESS CALLBACK
// ============================================================================

/**
 * @brief Progress callback for generation
 * @param generated Number of passwords generated so far
 * @param total Estimated total (may be approximate)
 * @param category Current category being generated
 * @return true to continue, false to cancel
 */
typedef bool (*BruteProgressCallback)(uint32_t generated, uint32_t total, const char *category);

// ============================================================================
// PUBLIC API
// ============================================================================

/**
 * @brief Set progress callback for generation
 * @param callback Function to call with updates
 */
void brute_set_progress_callback(BruteProgressCallback callback);

/**
 * @brief Generate complete Brazilian wordlist
 * Generates all patterns and saves to SD card
 * @return Number of passwords generated (0 on error)
 */
uint32_t brute_generate_complete();

/**
 * @brief Generate porteiro codes (0000-9999)
 * @return Number generated
 */
uint32_t brute_generate_porteiros();

/**
 * @brief Generate date-based passwords
 * Includes: DDMMYYYY, DDMMYY, DD/MM/YYYY, etc
 * @return Number generated
 */
uint32_t brute_generate_datas();

/**
 * @brief Generate ISP default passwords
 * Includes GVT, Vivo, Claro, TIM, Oi patterns
 * @return Number generated
 */
uint32_t brute_generate_wifi_br();

/**
 * @brief Generate CEP-based passwords
 * @return Number generated
 */
uint32_t brute_generate_ceps();

/**
 * @brief Generate common Brazilian names/words
 * @return Number generated
 */
uint32_t brute_generate_nomes_br();

/**
 * @brief Generate WPS PIN list (8-digit)
 * Optimized for WPS attacks
 * @return Number generated
 */
uint32_t brute_generate_wps_pins();

/**
 * @brief Get estimated file size for complete wordlist
 * @return Size in bytes
 */
size_t brute_estimate_size();

/**
 * @brief Check if wordlist exists and is complete
 * @return true if brute_br.txt exists with >1MB
 */
bool brute_check_exists();

/**
 * @brief Cancel ongoing generation
 */
void brute_cancel();

#endif // BRUTE_GENERATOR_H
