/**
 * @file nfc_driver.cpp
 * @brief PN532 NFC Driver Implementation
 * 
 * Implements NFC card reading, writing, cloning, and emulation
 * using the Adafruit PN532 library over I2C.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "nfc_driver.h"
#include "core/aggressive_sd.h"
#include <ArduinoJson.h>

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================
// PN532 I2C initialization (no IRQ/RESET pins used)
Adafruit_PN532 NFCDriver::_nfc(PIN_I2C_SDA, PIN_I2C_SCL);
bool NFCDriver::_initialized = false;
NFCCard NFCDriver::_lastCard = {0};
uint8_t NFCDriver::_defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ============================================================================
// INITIALIZATION
// ============================================================================
bool NFCDriver::init() {
    if (_initialized) return true;
    
    Serial.println("[NFC] Initializing PN532...");
    
    // Power on PN532 via MOSFET
    pinMode(PIN_PN532_EN, OUTPUT);
    digitalWrite(PIN_PN532_EN, HIGH);
    delay(100);
    
    // Initialize I2C (already done by s3_driver, but ensure)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    
    // Try to connect to PN532
    _nfc.begin();
    
    uint32_t versiondata = _nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[NFC] ERROR: PN532 not found!");
        digitalWrite(PIN_PN532_EN, LOW);
        return false;
    }
    
    // Print firmware info
    Serial.printf("[NFC] Found PN532 chip. Firmware: %d.%d\n",
                  (versiondata >> 24) & 0xFF,
                  (versiondata >> 16) & 0xFF);
    
    // Configure for ISO14443A (Mifare)
    _nfc.SAMConfig();
    
    _initialized = true;
    Serial.println("[NFC] PN532 ready!");
    return true;
}

bool NFCDriver::isReady() {
    return _initialized;
}

// ============================================================================
// CARD READING
// ============================================================================
NFCCard NFCDriver::readCard(uint32_t timeout_ms) {
    NFCCard card = {0};
    
    if (!_initialized) {
        if (!init()) return card;
    }
    
    uint8_t uid[7] = {0};
    uint8_t uidLength = 0;
    
    // Wait for card with timeout
    bool success = _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, timeout_ms);
    
    if (success) {
        card.valid = true;
        card.uidLength = uidLength;
        memcpy(card.uid, uid, uidLength);
        card.type = detectCardType(uid, uidLength);
        card.timestamp = millis();
        
        Serial.printf("[NFC] Card detected! UID: %s\n", uidToString(uid, uidLength).c_str());
        Serial.printf("[NFC] Type: %s\n", getTypeName(card.type));
        
        // Try to read first block (block 4 = first data block)
        if (card.type == NFC_TYPE_MIFARE_CLASSIC_1K || card.type == NFC_TYPE_MIFARE_CLASSIC_4K) {
            if (_nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, _defaultKey)) {
                _nfc.mifareclassic_ReadDataBlock(4, card.data);
            }
        }
        
        _lastCard = card;
    }
    
    return card;
}

bool NFCDriver::readBlock(uint8_t blockNumber, uint8_t* data, uint8_t* key) {
    if (!_initialized) return false;
    
    uint8_t* authKey = (key != nullptr) ? key : _defaultKey;
    
    // Need to have a card already detected
    if (!_lastCard.valid) {
        Serial.println("[NFC] No card present");
        return false;
    }
    
    // Authenticate the block
    uint8_t keyType = 0; // Key A
    if (!_nfc.mifareclassic_AuthenticateBlock(_lastCard.uid, _lastCard.uidLength, blockNumber, keyType, authKey)) {
        Serial.printf("[NFC] Auth failed for block %d\n", blockNumber);
        return false;
    }
    
    // Read the block
    if (!_nfc.mifareclassic_ReadDataBlock(blockNumber, data)) {
        Serial.printf("[NFC] Read failed for block %d\n", blockNumber);
        return false;
    }
    
    return true;
}

bool NFCDriver::writeBlock(uint8_t blockNumber, uint8_t* data, uint8_t* key) {
    if (!_initialized) return false;
    
    uint8_t* authKey = (key != nullptr) ? key : _defaultKey;
    
    if (!_lastCard.valid) {
        Serial.println("[NFC] No card present");
        return false;
    }
    
    // Don't write to sector trailer blocks (block % 4 == 3)
    if (blockNumber % 4 == 3) {
        Serial.println("[NFC] Cannot write to sector trailer!");
        return false;
    }
    
    // Don't write to block 0 (manufacturer block)
    if (blockNumber == 0) {
        Serial.println("[NFC] Cannot write to block 0!");
        return false;
    }
    
    // Authenticate
    if (!_nfc.mifareclassic_AuthenticateBlock(_lastCard.uid, _lastCard.uidLength, blockNumber, 0, authKey)) {
        Serial.printf("[NFC] Auth failed for block %d\n", blockNumber);
        return false;
    }
    
    // Write
    if (!_nfc.mifareclassic_WriteDataBlock(blockNumber, data)) {
        Serial.printf("[NFC] Write failed for block %d\n", blockNumber);
        return false;
    }
    
    Serial.printf("[NFC] Block %d written successfully\n", blockNumber);
    return true;
}

// ============================================================================
// CLONING
// ============================================================================
bool NFCDriver::cloneToFile(const char* filename) {
    if (!_initialized || !_lastCard.valid) {
        Serial.println("[NFC] No card to clone");
        return false;
    }
    
    if (!AggressiveSD::isReady()) {
        Serial.println("[NFC] SD card not ready");
        return false;
    }
    
    // Create JSON document
    JsonDocument doc;
    
    // Card metadata
    doc["type"] = getTypeName(_lastCard.type);
    doc["uid"] = uidToString(_lastCard.uid, _lastCard.uidLength);
    doc["uidLength"] = _lastCard.uidLength;
    doc["timestamp"] = _lastCard.timestamp;
    
    // Read all blocks
    JsonArray blocks = doc["blocks"].to<JsonArray>();
    
    int maxBlocks = (_lastCard.type == NFC_TYPE_MIFARE_CLASSIC_4K) ? 256 : 64;
    
    for (int block = 0; block < maxBlocks; block++) {
        uint8_t blockData[16] = {0};
        
        // Authenticate each sector (every 4 blocks)
        if (block % 4 == 0) {
            if (!_nfc.mifareclassic_AuthenticateBlock(_lastCard.uid, _lastCard.uidLength, block, 0, _defaultKey)) {
                // Try key B
                if (!_nfc.mifareclassic_AuthenticateBlock(_lastCard.uid, _lastCard.uidLength, block, 1, _defaultKey)) {
                    Serial.printf("[NFC] Auth failed for sector %d\n", block / 4);
                    continue;
                }
            }
        }
        
        if (_nfc.mifareclassic_ReadDataBlock(block, blockData)) {
            // Convert to hex string
            char hexStr[33];
            for (int i = 0; i < 16; i++) {
                sprintf(&hexStr[i * 2], "%02X", blockData[i]);
            }
            blocks.add(hexStr);
        } else {
            blocks.add("READ_ERROR");
        }
    }
    
    // Serialize to string
    String output;
    serializeJsonPretty(doc, output);
    
    // Write to file
    bool result = AggressiveSD::writeFile(filename, output.c_str());
    
    if (result) {
        Serial.printf("[NFC] Card cloned to %s\n", filename);
    }
    
    return result;
}

bool NFCDriver::emulateFromFile(const char* filename) {
    // PN532 has limited emulation capabilities
    // This would require card emulation mode which is complex
    Serial.println("[NFC] Card emulation from file - requires Gen2 magic card");
    return false;
}

// ============================================================================
// NDEF EMULATION
// ============================================================================
bool NFCDriver::emulateNDEF(const char* url) {
    if (!_initialized) return false;
    
    Serial.printf("[NFC] Emulating NDEF URL: %s\n", url);
    
    // PN532 supports NDEF target mode
    // Create NDEF message with URL
    uint8_t ndefMessage[128];
    uint8_t len = 0;
    
    // NDEF header
    ndefMessage[len++] = 0x03; // NDEF message
    ndefMessage[len++] = strlen(url) + 5; // Length
    ndefMessage[len++] = 0xD1; // Record header: MB=1, ME=1, CF=0, SR=1, IL=0, TNF=1
    ndefMessage[len++] = 0x01; // Type length
    ndefMessage[len++] = strlen(url) + 1; // Payload length
    ndefMessage[len++] = 'U';  // Type: URI
    ndefMessage[len++] = 0x01; // URI identifier: http://www.
    
    // Copy URL (without http://www.)
    memcpy(&ndefMessage[len], url, strlen(url));
    len += strlen(url);
    
    ndefMessage[len++] = 0xFE; // Terminator
    
    // Note: Full emulation requires tgInitAsTarget which is complex
    // For now, we log the NDEF and could write to a writable tag
    
    Serial.println("[NFC] NDEF message prepared. Write to tag to use.");
    Serial.printf("[NFC] Message length: %d bytes\n", len);
    
    return true;
}

void NFCDriver::stopEmulation() {
    // Nothing to stop in current implementation
    Serial.println("[NFC] Emulation stopped");
}

// ============================================================================
// FAULT INJECTION
// ============================================================================
void NFCDriver::sendFaultPattern(uint8_t pattern) {
    if (!_initialized) return;
    
    Serial.printf("[NFC] Sending fault pattern %d\n", pattern);
    
    // Send raw commands based on pattern
    switch (pattern) {
        case 0:
            // Malformed REQA
            {
                uint8_t cmd[] = {0x26, 0x00, 0x00}; // Invalid REQA
                // _nfc.sendCommandCheckAck(cmd, 3); // Would need raw access
            }
            break;
            
        case 1:
            // Invalid authentication
            {
                uint8_t badKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                // Attempt auth with bad key (will fail, but sends traffic)
                if (_lastCard.valid) {
                    _nfc.mifareclassic_AuthenticateBlock(_lastCard.uid, _lastCard.uidLength, 0, 0, badKey);
                }
            }
            break;
            
        case 2:
            // Rapid polling
            for (int i = 0; i < 100; i++) {
                uint8_t uid[7];
                uint8_t uidLen;
                _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 10);
                delay(5);
            }
            break;
            
        case 3:
            // Continuous field toggle
            _nfc.SAMConfig();
            delay(10);
            // Toggle RF field by re-init
            _nfc.begin();
            _nfc.SAMConfig();
            break;
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================
NFCCard NFCDriver::getLastCard() {
    return _lastCard;
}

const char* NFCDriver::getTypeName(NFCCardType type) {
    switch (type) {
        case NFC_TYPE_MIFARE_CLASSIC_1K: return "Mifare Classic 1K";
        case NFC_TYPE_MIFARE_CLASSIC_4K: return "Mifare Classic 4K";
        case NFC_TYPE_MIFARE_ULTRALIGHT: return "Mifare Ultralight";
        case NFC_TYPE_NTAG_213: return "NTAG 213";
        case NFC_TYPE_NTAG_215: return "NTAG 215";
        case NFC_TYPE_NTAG_216: return "NTAG 216";
        case NFC_TYPE_ISO14443A: return "ISO14443A";
        default: return "Unknown";
    }
}

String NFCDriver::uidToString(uint8_t* uid, uint8_t length) {
    String result = "";
    for (uint8_t i = 0; i < length; i++) {
        if (i > 0) result += ":";
        if (uid[i] < 0x10) result += "0";
        result += String(uid[i], HEX);
    }
    result.toUpperCase();
    return result;
}

NFCCardType NFCDriver::detectCardType(uint8_t* uid, uint8_t uidLen) {
    // Detect based on UID length and SAK/ATQA (simplified)
    if (uidLen == 4) {
        // Classic 1K has 4-byte UID
        return NFC_TYPE_MIFARE_CLASSIC_1K;
    } else if (uidLen == 7) {
        // Ultralight and NTAG have 7-byte UID
        return NFC_TYPE_MIFARE_ULTRALIGHT;
    }
    return NFC_TYPE_ISO14443A;
}
