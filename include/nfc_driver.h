/**
 * @file nfc_driver.h
 * @brief PN532 NFC Driver for Monster S3 Firmware
 * 
 * Supports:
 * - Mifare Classic 1K/4K read/write
 * - Card UID reading and cloning
 * - NDEF message creation and emulation
 * - Fault injection mode
 * 
 * Hardware: PN532 via I2C (pins 8/9)
 * Library: Adafruit PN532
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef NFC_DRIVER_H
#define NFC_DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "pin_config.h"

// ============================================================================
// NFC CARD TYPES
// ============================================================================
typedef enum {
    NFC_TYPE_UNKNOWN = 0,
    NFC_TYPE_MIFARE_CLASSIC_1K,
    NFC_TYPE_MIFARE_CLASSIC_4K,
    NFC_TYPE_MIFARE_ULTRALIGHT,
    NFC_TYPE_NTAG_213,
    NFC_TYPE_NTAG_215,
    NFC_TYPE_NTAG_216,
    NFC_TYPE_ISO14443A
} NFCCardType;

// ============================================================================
// NFC CARD DATA STRUCTURE
// ============================================================================
struct NFCCard {
    bool valid;
    NFCCardType type;
    uint8_t uid[7];
    uint8_t uidLength;
    uint8_t data[64];       // First sector data
    uint32_t timestamp;
    int8_t rssi;
};

// ============================================================================
// NFC DRIVER CLASS
// ============================================================================
class NFCDriver {
public:
    /**
     * @brief Initialize PN532 NFC module
     * @return true if initialization successful
     */
    static bool init();
    
    /**
     * @brief Check if NFC module is ready
     */
    static bool isReady();
    
    /**
     * @brief Wait for a card and read its UID
     * @param timeout_ms Timeout in milliseconds (0 = infinite)
     * @return NFCCard structure with card data
     */
    static NFCCard readCard(uint32_t timeout_ms = 1000);
    
    /**
     * @brief Read a specific block from Mifare Classic
     * @param blockNumber Block number (0-63 for 1K, 0-255 for 4K)
     * @param data Buffer to store block data (16 bytes)
     * @param key Authentication key (6 bytes, default 0xFF)
     * @return true if successful
     */
    static bool readBlock(uint8_t blockNumber, uint8_t* data, uint8_t* key = nullptr);
    
    /**
     * @brief Write a block to Mifare Classic
     * @param blockNumber Block number
     * @param data Data to write (16 bytes)
     * @param key Authentication key
     * @return true if successful
     */
    static bool writeBlock(uint8_t blockNumber, uint8_t* data, uint8_t* key = nullptr);
    
    /**
     * @brief Clone a card (read all sectors and save to SD)
     * @param filename Output filename on SD card
     * @return true if successful
     */
    static bool cloneToFile(const char* filename);
    
    /**
     * @brief Emulate a card from file
     * @param filename Input filename from SD card
     * @return true if emulation started
     */
    static bool emulateFromFile(const char* filename);
    
    /**
     * @brief Start card emulation with custom NDEF
     * @param url URL to encode in NDEF
     * @return true if emulation started
     */
    static bool emulateNDEF(const char* url);
    
    /**
     * @brief Stop card emulation
     */
    static void stopEmulation();
    
    /**
     * @brief Send malformed data for fault injection
     * @param pattern Pattern type (0-3)
     */
    static void sendFaultPattern(uint8_t pattern);
    
    /**
     * @brief Get last read card
     */
    static NFCCard getLastCard();
    
    /**
     * @brief Get card type name
     */
    static const char* getTypeName(NFCCardType type);
    
    /**
     * @brief Format UID as hex string
     */
    static String uidToString(uint8_t* uid, uint8_t length);

private:
    static Adafruit_PN532 _nfc;
    static bool _initialized;
    static NFCCard _lastCard;
    static uint8_t _defaultKey[6];
    
    static NFCCardType detectCardType(uint8_t* uid, uint8_t uidLen);
};

#endif // NFC_DRIVER_H
