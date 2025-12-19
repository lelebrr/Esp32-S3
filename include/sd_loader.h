#pragma once
/**
 * @file sd_loader.h
 * @brief SD Card Asset Loader - Load resources from SD card to save Flash/DRAM
 * 
 * This module provides lazy loading of assets from the SD card:
 * - Themes (JSON)
 * - Sounds (WAV)
 * - IR codes (database)
 * - RF signals (library)
 * - Images (PNG)
 */

#include <Arduino.h>
#include <SD_MMC.h>

// Asset paths on SD card
#define SD_ASSETS_PATH       "/assets"
#define SD_THEMES_PATH       "/assets/themes"
#define SD_SOUNDS_PATH       "/assets/sounds"
#define SD_IR_CODES_PATH     "/assets/ir_codes"
#define SD_RF_SIGNALS_PATH   "/assets/rf_signals"
#define SD_IMAGES_PATH       "/assets/images"

// Memory allocation strategy
#ifndef SD_LOADER_USE_PSRAM
#define SD_LOADER_USE_PSRAM 1  // Use PSRAM for loaded assets if available
#endif

/**
 * @brief Initialize the SD loader system
 * @return true if SD card is available and assets directory exists
 */
bool sd_loader_init();

/**
 * @brief Check if an asset exists on SD card
 * @param path Relative path within assets directory
 * @return true if file exists
 */
bool sd_asset_exists(const char* path);

/**
 * @brief Load a file into a dynamically allocated buffer
 * @param path Relative path within assets directory
 * @param outSize Pointer to receive the file size
 * @return Pointer to allocated buffer (caller must free), or nullptr on failure
 * @note Uses PSRAM if available, falls back to heap
 */
uint8_t* sd_load_file(const char* path, size_t* outSize);

/**
 * @brief Load a text file into a String
 * @param path Relative path within assets directory
 * @return File contents as String, or empty string on failure
 */
String sd_load_text(const char* path);

/**
 * @brief Free a buffer allocated by sd_load_file
 * @param buffer Pointer to buffer to free
 */
void sd_free_buffer(uint8_t* buffer);

/**
 * @brief Get remaining free space on SD card
 * @return Free space in bytes
 */
uint64_t sd_get_free_space();

/**
 * @brief Create assets directory structure if missing
 */
void sd_create_asset_dirs();

// Specific asset loaders

/**
 * @brief Load a theme JSON from SD
 * @param themeName Name of the theme (without extension)
 * @return JSON content as String
 */
String sd_load_theme(const char* themeName);

/**
 * @brief Load an IR code database entry
 * @param brand Device brand
 * @param model Device model  
 * @param buffer Output buffer for raw codes
 * @param maxSize Maximum buffer size
 * @return Number of bytes loaded, 0 on failure
 */
size_t sd_load_ir_code(const char* brand, const char* model, uint8_t* buffer, size_t maxSize);

/**
 * @brief Check if SD loader is ready
 */
bool sd_loader_is_ready();
