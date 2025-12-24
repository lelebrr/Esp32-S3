/**
 * @file sd_structure.cpp
 * @brief SD Card Structure Manager - Implementation
 * 
 * Gerencia estrutura de diretórios no SD card 128GB.
 * Auto-criação de pastas, migração flash->SD, backup, CVE update.
 */

#include "sd_structure.h"
#include "core/aggressive_sd.h"
#include "rtc_driver.h"
#include <SPIFFS.h>
#include <mbedtls/sha256.h>
#include <HTTPClient.h>
#include <WiFi.h>

// ============================================
// Private Variables
// ============================================

static uint32_t _bootCount = 0;
static PSRAMCache* _cachePool[8] = {nullptr};
static const int _maxCacheEntries = 8;

// Lista de diretórios a criar
static const char* _directories[] = {
    SD_PATH_ATTACKS,
    SD_PATH_ATTACKS_BLE,
    SD_PATH_ATTACKS_WIFI,
    SD_PATH_ATTACKS_NFC,
    SD_PATH_ATTACKS_SUBGHZ,
    SD_PATH_ATTACKS_IR,
    SD_PATH_WIFI_PMKID,
    SD_PATH_WIFI_EVILTWIN,
    SD_PATH_NFC_DUMPS,
    SD_PATH_SUBGHZ_CAPTURED,
    SD_PATH_IR_TV,
    SD_PATH_IR_AC,
    SD_PATH_AI,
    SD_PATH_TTS,
    SD_PATH_TTS_VOICES,
    SD_PATH_TTS_PHRASES,
    SD_PATH_CVE,
    SD_PATH_LOGS,
    SD_PATH_CONFIG,
    SD_PATH_BACKUP
};
static const int _numDirectories = sizeof(_directories) / sizeof(_directories[0]);

// ============================================
// Private Functions
// ============================================

/**
 * @brief Cria diretório recursivamente
 */
static bool _mkdirRecursive(const char* path) {
    if (SD.exists(path)) return true;
    
    String pathStr = String(path);
    String acc = "";
    
    for (int i = 0; i < pathStr.length(); i++) {
        acc += pathStr[i];
        if (pathStr[i] == '/' && acc.length() > 1) {
            if (!SD.exists(acc.c_str())) {
                if (!SD.mkdir(acc.c_str())) {
                    Serial.printf("[SD] Failed mkdir: %s\n", acc.c_str());
                    return false;
                }
            }
        }
    }
    
    if (!SD.exists(path)) {
        return SD.mkdir(path);
    }
    return true;
}

/**
 * @brief Copia arquivo do SPIFFS pro SD
 */
static bool _copySpiffsToSD(const char* src, const char* dst) {
    File srcFile = SPIFFS.open(src, "r");
    if (!srcFile) return false;
    
    File dstFile = SD.open(dst, FILE_WRITE);
    if (!dstFile) {
        srcFile.close();
        return false;
    }
    
    uint8_t* buf = (uint8_t*)heap_caps_malloc(4096, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        buf = (uint8_t*)malloc(1024);
        if (!buf) {
            srcFile.close();
            dstFile.close();
            return false;
        }
    }
    
    size_t bufSize = psramFound() ? 4096 : 1024;
    
    while (srcFile.available()) {
        size_t readBytes = srcFile.read(buf, bufSize);
        dstFile.write(buf, readBytes);
    }
    
    free(buf);
    srcFile.close();
    dstFile.close();
    
    Serial.printf("[SD] Copied: %s -> %s\n", src, dst);
    return true;
}

/**
 * @brief Lê contador de boots do arquivo
 */
static uint32_t _readBootCount() {
    if (!SD.exists("/config/boot_count.txt")) return 0;
    
    File f = SD.open("/config/boot_count.txt", "r");
    if (!f) return 0;
    
    String val = f.readStringUntil('\n');
    f.close();
    return val.toInt();
}

/**
 * @brief Salva contador de boots
 */
static void _saveBootCount(uint32_t count) {
    File f = SD.open("/config/boot_count.txt", FILE_WRITE);
    if (!f) return;
    
    f.println(count);
    f.close();
}

// ============================================
// Public Functions
// ============================================

SDResult setup_sd_structure() {
    if (!AggressiveSD::isReady()) {
        Serial.println("[SD-STRUCT] SD not ready!");
        return SD_RESULT_ERROR_SD_NOT_READY;
    }
    
    Serial.println("[SD-STRUCT] Creating directory structure...");
    
    int created = 0;
    int existing = 0;
    
    for (int i = 0; i < _numDirectories; i++) {
        if (SD.exists(_directories[i])) {
            existing++;
        } else {
            if (_mkdirRecursive(_directories[i])) {
                Serial.printf("[SD-STRUCT] Created: %s\n", _directories[i]);
                created++;
            } else {
                Serial.printf("[SD-STRUCT] FAILED: %s\n", _directories[i]);
                return SD_RESULT_ERROR_CREATE_DIR;
            }
        }
    }
    
    Serial.printf("[SD-STRUCT] OK - %d created, %d existing\n", created, existing);
    
    // Cria settings padrão se não existir
    create_default_settings();
    
    // Cria arquivos de log vazios
    if (!SD.exists(SD_FILE_LOG_ATTACKS)) {
        AggressiveSD::writeFile(SD_FILE_LOG_ATTACKS, "");
    }
    if (!SD.exists(SD_FILE_LOG_BATTERY)) {
        AggressiveSD::writeFile(SD_FILE_LOG_BATTERY, "");
    }
    if (!SD.exists(SD_FILE_LOG_ERROR)) {
        AggressiveSD::writeFile(SD_FILE_LOG_ERROR, "");
    }
    
    // Inicializa Q-table vazia se não existir
    if (!SD.exists(SD_FILE_QTABLE)) {
        File f = SD.open(SD_FILE_QTABLE, FILE_WRITE);
        if (f) {
            // Header mínimo: versão + tamanho
            uint8_t header[8] = {0x51, 0x54, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}; // "QT" + v1
            f.write(header, 8);
            f.close();
            Serial.println("[SD-STRUCT] Created empty Q-table");
        }
    }
    
    // Inicializa history.json
    if (!SD.exists(SD_FILE_AI_HISTORY)) {
        AggressiveSD::writeFile(SD_FILE_AI_HISTORY, "{\"rewards\":[],\"last_update\":0}");
    }
    
    return SD_RESULT_OK;
}

SDResult move_flash_to_sd() {
    if (!AggressiveSD::isReady()) {
        return SD_RESULT_ERROR_SD_NOT_READY;
    }
    
    // Tenta montar SPIFFS
    if (!SPIFFS.begin(false)) {
        Serial.println("[SD-STRUCT] SPIFFS not available, skipping migration");
        return SD_RESULT_OK;
    }
    
    Serial.println("[SD-STRUCT] Checking SPIFFS for files to migrate...");
    
    int migrated = 0;
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    
    while (file) {
        String name = file.name();
        String sdPath = "";
        
        // Mapeia arquivos pro SD
        if (name.endsWith(".tflite")) {
            sdPath = SD_FILE_MODEL;
        } else if (name.endsWith(".bin") && name.indexOf("q_table") >= 0) {
            sdPath = SD_FILE_QTABLE;
        } else if (name.indexOf("payload") >= 0) {
            sdPath = String(SD_PATH_ATTACKS) + name;
        } else if (name.endsWith(".json") && name.indexOf("settings") >= 0) {
            sdPath = SD_FILE_SETTINGS;
        }
        
        if (sdPath.length() > 0 && !SD.exists(sdPath.c_str())) {
            if (_copySpiffsToSD(name.c_str(), sdPath.c_str())) {
                SPIFFS.remove(name.c_str());
                migrated++;
            }
        }
        
        file = root.openNextFile();
    }
    
    SPIFFS.end();
    
    if (migrated > 0) {
        Serial.printf("[SD-STRUCT] Migrated %d files from SPIFFS\n", migrated);
    }
    
    return SD_RESULT_OK;
}

SDResult update_cve_database() {
    if (!AggressiveSD::isReady()) {
        return SD_RESULT_ERROR_SD_NOT_READY;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[CVE] WiFi not connected!");
        return SD_RESULT_ERROR_NETWORK;
    }
    
    Serial.println("[CVE] Updating BR database...");
    
    HTTPClient http;
    http.begin(CVE_UPDATE_URL_BRUTE);
    http.setTimeout(30000);
    
    int httpCode = http.GET();
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[CVE] HTTP Error: %d\n", httpCode);
        http.end();
        return SD_RESULT_ERROR_NETWORK;
    }
    
    // Backup do arquivo atual
    if (SD.exists(SD_FILE_BRUTE_BR)) {
        _copySpiffsToSD(SD_FILE_BRUTE_BR, SD_FILE_CVE_BAK);
    }
    
    // Salva novo arquivo com streaming
    File f = SD.open(SD_FILE_BRUTE_BR, FILE_WRITE);
    if (!f) {
        http.end();
        return SD_RESULT_ERROR_CREATE_DIR;
    }
    
    WiFiClient* stream = http.getStreamPtr();
    uint8_t buf[1024];
    size_t total = 0;
    
    while (http.connected() && stream->available()) {
        size_t len = stream->readBytes(buf, sizeof(buf));
        f.write(buf, len);
        total += len;
        
        // Progress a cada 100KB
        if (total % 102400 == 0) {
            Serial.printf("[CVE] Downloaded: %d KB\n", total / 1024);
        }
    }
    
    f.close();
    http.end();
    
    Serial.printf("[CVE] Updated! Total: %d bytes\n", total);
    log_with_timestamp(SD_FILE_LOG_ATTACKS, "CVE database updated");
    
    return SD_RESULT_OK;
}

bool backup_q_table() {
    if (!AggressiveSD::isReady()) return false;
    if (!SD.exists(SD_FILE_QTABLE)) return false;
    
    Serial.println("[BACKUP] Backing up Q-table...");
    
    File src = SD.open(SD_FILE_QTABLE, "r");
    if (!src) return false;
    
    File dst = SD.open(SD_FILE_QTABLE_BAK, FILE_WRITE);
    if (!dst) {
        src.close();
        return false;
    }
    
    uint8_t* buf = (uint8_t*)heap_caps_malloc(4096, MALLOC_CAP_SPIRAM);
    if (!buf) buf = (uint8_t*)malloc(1024);
    size_t bufSize = psramFound() ? 4096 : 1024;
    
    while (src.available()) {
        size_t r = src.read(buf, bufSize);
        dst.write(buf, r);
    }
    
    free(buf);
    src.close();
    dst.close();
    
    Serial.println("[BACKUP] Q-table backup OK");
    log_with_timestamp(SD_FILE_LOG_ATTACKS, "Q-table backup created");
    
    return true;
}

void check_auto_backup() {
    _bootCount = _readBootCount() + 1;
    _saveBootCount(_bootCount);
    
    Serial.printf("[BOOT] Boot count: %u\n", _bootCount);
    
    if (_bootCount % BACKUP_INTERVAL_BOOTS == 0) {
        Serial.printf("[BOOT] Auto-backup triggered (every %d boots)\n", BACKUP_INTERVAL_BOOTS);
        backup_q_table();
    }
}

bool log_with_timestamp(const char* logFile, const char* message) {
    if (!AggressiveSD::isReady()) return false;
    
    char entry[256];
    
    // Tenta pegar timestamp do RTC
    #ifdef RTC_AVAILABLE
    DateTime now = RTCDriver::getTime();
    snprintf(entry, sizeof(entry), "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second(),
             message);
    #else
    // Fallback pra millis()
    uint32_t ms = millis();
    uint32_t s = ms / 1000;
    uint32_t m = s / 60;
    uint32_t h = m / 60;
    snprintf(entry, sizeof(entry), "[%02u:%02u:%02u.%03u] %s\n",
             h % 24, m % 60, s % 60, ms % 1000, message);
    #endif
    
    return AggressiveSD::appendFile(logFile, entry);
}

bool verify_payload_sha256(const char* path, const char* expectedHash) {
    if (!AggressiveSD::isReady()) return false;
    if (!SD.exists(path)) return false;
    
    File f = SD.open(path, "r");
    if (!f) return false;
    
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); // 0 = SHA256
    
    uint8_t* buf = (uint8_t*)heap_caps_malloc(4096, MALLOC_CAP_SPIRAM);
    if (!buf) buf = (uint8_t*)malloc(1024);
    size_t bufSize = psramFound() ? 4096 : 1024;
    
    while (f.available()) {
        size_t r = f.read(buf, bufSize);
        mbedtls_sha256_update(&ctx, buf, r);
    }
    
    free(buf);
    f.close();
    
    uint8_t hash[32];
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    
    // Converte pra hex string
    char hashStr[65];
    for (int i = 0; i < 32; i++) {
        sprintf(hashStr + (i * 2), "%02x", hash[i]);
    }
    hashStr[64] = '\0';
    
    bool match = (strcmp(hashStr, expectedHash) == 0);
    
    if (!match) {
        Serial.printf("[SHA256] MISMATCH: %s\n", path);
        Serial.printf("  Expected: %s\n", expectedHash);
        Serial.printf("  Got:      %s\n", hashStr);
        log_with_timestamp(SD_FILE_LOG_ERROR, "SHA256 mismatch on payload");
    }
    
    return match;
}

PSRAMCache* cache_to_psram(const char* path) {
    if (!psramFound()) {
        Serial.println("[CACHE] PSRAM not available!");
        return nullptr;
    }
    
    if (!AggressiveSD::isReady()) return nullptr;
    if (!SD.exists(path)) return nullptr;
    
    // Procura slot livre ou reutiliza mais antigo
    int slot = -1;
    uint32_t oldest = UINT32_MAX;
    
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (_cachePool[i] == nullptr) {
            slot = i;
            break;
        }
        if (_cachePool[i]->lastAccess < oldest) {
            oldest = _cachePool[i]->lastAccess;
            slot = i;
        }
    }
    
    // Libera slot se ocupado
    if (_cachePool[slot] != nullptr) {
        cache_free(_cachePool[slot]);
    }
    
    File f = SD.open(path, "r");
    if (!f) return nullptr;
    
    size_t fileSize = f.size();
    if (fileSize > PSRAM_CACHE_MAX_SIZE) {
        Serial.printf("[CACHE] File too large: %d bytes\n", fileSize);
        f.close();
        return nullptr;
    }
    
    uint8_t* data = (uint8_t*)heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM);
    if (!data) {
        Serial.println("[CACHE] PSRAM alloc failed!");
        f.close();
        return nullptr;
    }
    
    f.read(data, fileSize);
    f.close();
    
    PSRAMCache* cache = (PSRAMCache*)heap_caps_malloc(sizeof(PSRAMCache), MALLOC_CAP_SPIRAM);
    if (!cache) {
        free(data);
        return nullptr;
    }
    
    cache->data = data;
    cache->size = fileSize;
    cache->path = path;
    cache->lastAccess = millis();
    cache->valid = true;
    
    _cachePool[slot] = cache;
    
    Serial.printf("[CACHE] Loaded %s (%d bytes) to PSRAM slot %d\n", path, fileSize, slot);
    
    return cache;
}

void cache_free(PSRAMCache* cache) {
    if (!cache) return;
    
    if (cache->data) {
        heap_caps_free(cache->data);
    }
    
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (_cachePool[i] == cache) {
            _cachePool[i] = nullptr;
            break;
        }
    }
    
    heap_caps_free(cache);
}

bool load_settings(JsonDocument& doc) {
    if (!AggressiveSD::isReady()) return false;
    if (!SD.exists(SD_FILE_SETTINGS)) {
        create_default_settings();
    }
    
    String content = AggressiveSD::readFile(SD_FILE_SETTINGS);
    if (content.length() == 0) return false;
    
    DeserializationError err = deserializeJson(doc, content);
    return (err == DeserializationError::Ok);
}

bool save_settings(JsonDocument& doc) {
    if (!AggressiveSD::isReady()) return false;
    
    String output;
    serializeJsonPretty(doc, output);
    
    return AggressiveSD::writeFile(SD_FILE_SETTINGS, output.c_str());
}

void create_default_settings() {
    if (SD.exists(SD_FILE_SETTINGS)) return;
    
    Serial.println("[SD-STRUCT] Creating default settings.json");
    
    const char* defaultSettings = R"({
  "mode": "stealth",
  "volume": 50,
  "cpu_freq_mhz": 240,
  "display_brightness": 80,
  "auto_backup_interval": 10,
  "tts_enabled": true,
  "wifi_enabled": true,
  "ble_enabled": true,
  "nfc_enabled": true,
  "subghz_enabled": true,
  "ir_enabled": true,
  "ai_learning": true,
  "stealth_leds": false
})";
    
    AggressiveSD::writeFile(SD_FILE_SETTINGS, defaultSettings);
}

int list_attack_files(const char* attackType, String* files, int maxFiles) {
    if (!AggressiveSD::isReady()) return 0;
    
    String basePath = String(SD_PATH_ATTACKS) + "/" + attackType;
    
    if (!SD.exists(basePath.c_str())) return 0;
    
    File dir = SD.open(basePath.c_str());
    if (!dir || !dir.isDirectory()) return 0;
    
    int count = 0;
    File entry = dir.openNextFile();
    
    while (entry && count < maxFiles) {
        if (!entry.isDirectory()) {
            files[count++] = entry.name();
        }
        entry = dir.openNextFile();
    }
    
    return count;
}
