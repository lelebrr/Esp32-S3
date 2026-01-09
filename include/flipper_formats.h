/**
 * @file flipper_formats.h
 * @brief Flipper Zero Compatible File Format Parser
 *
 * Implements parsers for Flipper file formats:
 * - .ir (Infrared signals)
 * - .sub (SubGHz signals)
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#ifndef FLIPPER_FORMATS_H
#define FLIPPER_FORMATS_H

#include <Arduino.h>
#include <stdint.h>

// Maximum sizes
#define FLIPPER_MAX_NAME_LEN 64
#define FLIPPER_MAX_RAW_LEN 512
#define FLIPPER_MAX_PROTOCOL_LEN 32

/**
 * @brief Flipper IR file structure
 *
 * Example .ir file format:
 * Filetype: IR signals file
 * Version: 1
 * name: Power
 * type: parsed
 * protocol: NEC
 * address: 04 00 00 00
 * command: 08 00 00 00
 */
typedef struct {
    char name[FLIPPER_MAX_NAME_LEN];
    char protocol[FLIPPER_MAX_PROTOCOL_LEN]; // NEC, RC5, RC6, Sony, Samsung32, etc.
    uint32_t address;
    uint32_t command;
    uint16_t frequency; // Default 38000 Hz
    bool isRaw;
    uint16_t rawData[FLIPPER_MAX_RAW_LEN];
    size_t rawLen;
} FlipperIR;

/**
 * @brief Flipper SubGHz file structure
 *
 * Example .sub file format:
 * Filetype: Flipper SubGhz RAW File
 * Version: 1
 * Frequency: 433920000
 * Preset: FuriHalSubGhzPresetOok650Async
 * Protocol: RAW
 * RAW_Data: 500 -500 1000 -1000 ...
 */
typedef struct {
    char name[FLIPPER_MAX_NAME_LEN];
    uint32_t frequency;                      // In Hz (e.g., 433920000)
    char preset[FLIPPER_MAX_PROTOCOL_LEN];   // FuriHalSubGhzPreset...
    char protocol[FLIPPER_MAX_PROTOCOL_LEN]; // RAW, Princeton, CAME, etc.
    int16_t rawData[FLIPPER_MAX_RAW_LEN];    // Timing data (positive = high, negative = low)
    size_t rawLen;
    // For decoded protocols
    uint32_t code;
    uint8_t bits;
} FlipperSubGHz;

// ============================================================================
// PARSING FUNCTIONS
// ============================================================================

/**
 * @brief Parse a Flipper .ir file
 * @param filename Full path to the .ir file
 * @param out Pointer to FlipperIR struct to fill
 * @return true on success, false on failure
 */
bool flipper_parse_ir(const char *filename, FlipperIR *out);

/**
 * @brief Parse a Flipper .sub file
 * @param filename Full path to the .sub file
 * @param out Pointer to FlipperSubGHz struct to fill
 * @return true on success, false on failure
 */
bool flipper_parse_subghz(const char *filename, FlipperSubGHz *out);

// ============================================================================
// EXPORT FUNCTIONS
// ============================================================================

/**
 * @brief Export IR signal to Flipper .ir format
 * @param filename Output filename
 * @param data IR data to export
 * @return true on success
 */
bool flipper_export_ir(const char *filename, const FlipperIR *data);

/**
 * @brief Export SubGHz signal to Flipper .sub format
 * @param filename Output filename
 * @param data SubGHz data to export
 * @return true on success
 */
bool flipper_export_subghz(const char *filename, const FlipperSubGHz *data);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Convert IR protocol name to IRProtocolType enum
 * @param protocolName Protocol name string (e.g., "NEC", "RC5")
 * @return Protocol type enum value
 */
int flipper_protocol_to_enum(const char *protocolName);

/**
 * @brief Get protocol name string from enum
 * @param proto Protocol enum
 * @return Protocol name string
 */
const char *flipper_enum_to_protocol(int proto);

/**
 * @brief List all .ir files in a directory
 * @param directory Directory path
 * @param files Array to fill with filenames
 * @param maxFiles Maximum number of files to list
 * @return Number of files found
 */
int flipper_list_ir_files(const char *directory, char files[][64], int maxFiles);

/**
 * @brief List all .sub files in a directory
 * @param directory Directory path
 * @param files Array to fill with filenames
 * @param maxFiles Maximum number of files to list
 * @return Number of files found
 */
int flipper_list_subghz_files(const char *directory, char files[][64], int maxFiles);

#endif // FLIPPER_FORMATS_H
