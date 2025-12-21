/**
 * @file aggressive_sd.cpp
 * @brief Aggressive SD Card Boot - Implementation
 * 
 * Uses HSPI (SPI2) on pins 39-42 for SD card to avoid conflict
 * with DisplayDisplay on FSPI (SPI3).
 */

#include "core/aggressive_sd.h"

// Static member initialization
SPIClass AggressiveSD::_spi(HSPI);
bool AggressiveSD::_ready = false;
uint8_t AggressiveSD::_cardType = CARD_NONE;

SDStatus AggressiveSD::init() {
    Serial.println("[SD] Aggressive SD Boot Init...");
    
    // Initialize HSPI with our custom pins
    _spi.begin(40, 42, 41, 39);
    _spi.setFrequency(20000000);

    // Configure CS pin as output and set high (inactive)
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);

    // Try to mount SD card with 40MHz clock
    if (!SD.begin(39, _spi)) {
        _spi.setFrequency(40000000);
        Serial.println("[SD] Mount FAILED!");
        _ready = false;
        return SD_STATUS_MOUNT_FAILED;
    }
    
    // Get card type
    _cardType = SD.cardType();
    if (_cardType == CARD_NONE) {
        Serial.println("[SD] No card attached!");
        _ready = false;
        return SD_STATUS_NO_CARD;
    }
    
    // Card type info
    const char* cardTypeStr = "UNKNOWN";
    switch (_cardType) {
        case CARD_MMC:  cardTypeStr = "MMC"; break;
        case CARD_SD:   cardTypeStr = "SD"; break;
        case CARD_SDHC: cardTypeStr = "SDHC"; break;
    }
    
    Serial.printf("[SD] Card Type: %s\n", cardTypeStr);
    Serial.printf("[SD] Card Size: %llu MB\n", SD.cardSize() / (1024 * 1024));
    Serial.printf("[SD] Total Size: %llu MB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("[SD] Used: %llu MB\n", SD.usedBytes() / (1024 * 1024));
    
    // Create essential directories
    if (!SD.exists("/logs")) SD.mkdir("/logs");
    if (!SD.exists("/config")) SD.mkdir("/config");
    if (!SD.exists("/payloads")) SD.mkdir("/payloads");
    if (!SD.exists("/captures")) SD.mkdir("/captures");
    if (!SD.exists("/web")) SD.mkdir("/web");
    
    _ready = true;
    Serial.println("[SD] ✅ SD Card Ready!");
    return SD_STATUS_OK;
}

bool AggressiveSD::isReady() {
    return _ready;
}

uint64_t AggressiveSD::getCardSize() {
    return _ready ? SD.cardSize() : 0;
}

uint64_t AggressiveSD::getUsedSpace() {
    return _ready ? SD.usedBytes() : 0;
}

uint64_t AggressiveSD::getFreeSpace() {
    return _ready ? (SD.totalBytes() - SD.usedBytes()) : 0;
}

SPIClass* AggressiveSD::getSPI() {
    return &_spi;
}

bool AggressiveSD::fileExists(const char* path) {
    return _ready && SD.exists(path);
}

bool AggressiveSD::createDir(const char* path) {
    if (!_ready) return false;
    return SD.mkdir(path);
}

bool AggressiveSD::writeFile(const char* path, const char* content) {
    if (!_ready) return false;
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("[SD] Failed to open %s for writing\n", path);
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    return written > 0;
}

bool AggressiveSD::appendFile(const char* path, const char* content) {
    if (!_ready) return false;
    
    File file = SD.open(path, FILE_APPEND);
    if (!file) return false;
    
    size_t written = file.print(content);
    file.close();
    return written > 0;
}

String AggressiveSD::readFile(const char* path) {
    if (!_ready) return "";
    
    File file = SD.open(path);
    if (!file) return "";
    
    String content = file.readString();
    file.close();
    return content;
}

bool AggressiveSD::deleteFile(const char* path) {
    if (!_ready) return false;
    return SD.remove(path);
}

// ============================================
// Global boot function
// ============================================
bool aggressive_boot_logic() {
    Serial.println("=====================================");
    Serial.println("   AGGRESSIVE SD BOOT - LELE ORIGIN");
    Serial.println("=====================================");
    
    SDStatus status = AggressiveSD::init();
    
    if (status != SD_STATUS_OK) {
        Serial.println("[BOOT] ⚠️ SD Card not available. Running in limited mode.");
        return false;
    }
    
    // Log boot event
    char logEntry[128];
    snprintf(logEntry, sizeof(logEntry), "[%lu] System Boot OK\n", millis());
    AggressiveSD::appendFile("/logs/boot.log", logEntry);
    
    Serial.println("[BOOT] ✅SD-First Boot Complete!");
    return true;
}
