/**
 * @file aggressive_sd.h
 * @brief Aggressive SD Card Boot - Header
 * 
 * Implements early SD card initialization on a dedicated SPI bus (HSPI).
 * This ensures the SD card is mounted BEFORE any other initialization.
 */

#ifndef AGGRESSIVE_SD_H
#define AGGRESSIVE_SD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "pin_config.h"

// SD Card Status Codes
enum SDStatus {
    SD_STATUS_OK = 0,
    SD_STATUS_NO_CARD,
    SD_STATUS_MOUNT_FAILED,
    SD_STATUS_FS_ERROR
};

/**
 * @brief Aggressive SD Card Manager
 * 
 * Initializes SD card on a dedicated HSPI bus to avoid conflicts
 * with the TFT display on FSPI.
 */
class AggressiveSD {
public:
    /**
     * @brief Initialize the SD card subsystem
     * Must be called early in setup(), before display init.
     * @return SDStatus code
     */
    static SDStatus init();

    /**
     * @brief Check if SD card is mounted and ready
     * @return true if SD is available
     */
    static bool isReady();

    /**
     * @brief Get total SD card size in bytes
     */
    static uint64_t getCardSize();

    /**
     * @brief Get used space in bytes
     */
    static uint64_t getUsedSpace();

    /**
     * @brief Get free space in bytes
     */
    static uint64_t getFreeSpace();

    /**
     * @brief Get the underlying SPI bus instance
     * Useful for sharing with CC1101 or other SPI-B devices.
     */
    static SPIClass* getSPI();

    /**
     * @brief Check if a file exists on the SD card
     */
    static bool fileExists(const char* path);

    /**
     * @brief Create directories recursively
     */
    static bool createDir(const char* path);

    /**
     * @brief Write string to file (overwrites)
     */
    static bool writeFile(const char* path, const char* content);

    /**
     * @brief Append string to file
     */
    static bool appendFile(const char* path, const char* content);

    /**
     * @brief Read entire file to string
     */
    static String readFile(const char* path);

    /**
     * @brief Delete a file
     */
    static bool deleteFile(const char* path);

private:
    static SPIClass _spi;
    static bool _ready;
    static uint8_t _cardType;
};

/**
 * @brief Early boot SD initialization
 * Call this at the very start of setup() for aggressive boot.
 * @return true if SD mounted successfully
 */
bool aggressive_boot_logic();

#endif // AGGRESSIVE_SD_H
