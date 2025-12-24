/**
 * @file sd_structure.h
 * @brief SD Card Structure Manager - Pentest ESP32-S3
 * 
 * Gerencia estrutura de diretórios no SD card 128GB.
 * Funções de auto-criação, migração flash->SD, backup,
 * atualização CVE BR e cache PSRAM.
 */

#ifndef SD_STRUCTURE_H
#define SD_STRUCTURE_H

#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>

// ============================================
// SD Card Directory Paths
// ============================================

// Root paths
#define SD_PATH_ATTACKS       "/attacks"
#define SD_PATH_AI            "/ai"
#define SD_PATH_TTS           "/tts"
#define SD_PATH_CVE           "/cve"
#define SD_PATH_LOGS          "/logs"
#define SD_PATH_CONFIG        "/config"
#define SD_PATH_BACKUP        "/backup"

// Attack subpaths
#define SD_PATH_ATTACKS_BLE   "/attacks/ble"
#define SD_PATH_ATTACKS_WIFI  "/attacks/wifi"
#define SD_PATH_ATTACKS_NFC   "/attacks/nfc"
#define SD_PATH_ATTACKS_SUBGHZ "/attacks/subghz"
#define SD_PATH_ATTACKS_IR    "/attacks/ir"

// WiFi subpaths
#define SD_PATH_WIFI_PMKID    "/attacks/wifi/pmkid_captures"
#define SD_PATH_WIFI_EVILTWIN "/attacks/wifi/evil_twin_templates"

// NFC subpaths
#define SD_PATH_NFC_DUMPS     "/attacks/nfc/dumps"

// SubGHz subpaths
#define SD_PATH_SUBGHZ_CAPTURED "/attacks/subghz/captured"

// IR subpaths
#define SD_PATH_IR_TV         "/attacks/ir/tv_codes"
#define SD_PATH_IR_AC         "/attacks/ir/ac_codes"

// TTS subpaths
#define SD_PATH_TTS_VOICES    "/tts/voices"
#define SD_PATH_TTS_PHRASES   "/tts/phrases"

// ============================================
// File Paths
// ============================================

// AI files
#define SD_FILE_QTABLE        "/ai/q_table.bin"
#define SD_FILE_MODEL         "/ai/model.tflite"
#define SD_FILE_AI_HISTORY    "/ai/history.json"

// CVE files
#define SD_FILE_CVE_BR        "/cve/cve_br_2025.json"
#define SD_FILE_BRUTE_BR      "/cve/brute_br.txt"

// Log files
#define SD_FILE_LOG_ATTACKS   "/logs/attacks.log"
#define SD_FILE_LOG_BATTERY   "/logs/battery.log"
#define SD_FILE_LOG_ERROR     "/logs/error.log"
#define SD_FILE_LOG_BOOT      "/logs/boot.log"

// Config files
#define SD_FILE_SETTINGS      "/config/settings.json"
#define SD_FILE_PINMAP        "/config/pinmap.json"

// Backup files
#define SD_FILE_QTABLE_BAK    "/backup/q_table_backup.bin"
#define SD_FILE_CVE_BAK       "/backup/cve_backup.json"

// ============================================
// CVE Update URLs
// ============================================

#define CVE_UPDATE_URL_BRUTE  "https://bruteforce-br.fly.dev/lista_br.txt"
#define CVE_UPDATE_URL_CVE    "https://bruteforce-br.fly.dev/cve_br.json"

// ============================================
// Cache Configuration
// ============================================

#define PSRAM_CACHE_MAX_SIZE  (1024 * 1024)  // 1MB max cache
#define BACKUP_INTERVAL_BOOTS 10             // Backup a cada 10 boots

/**
 * @brief Estrutura de cache em PSRAM
 */
struct PSRAMCache {
    uint8_t* data;
    size_t size;
    const char* path;
    uint32_t lastAccess;
    bool valid;
};

/**
 * @brief Resultado de operações SD
 */
enum SDResult {
    SD_RESULT_OK = 0,
    SD_RESULT_ERROR_SD_NOT_READY,
    SD_RESULT_ERROR_CREATE_DIR,
    SD_RESULT_ERROR_FILE_NOT_FOUND,
    SD_RESULT_ERROR_COPY_FAILED,
    SD_RESULT_ERROR_CHECKSUM,
    SD_RESULT_ERROR_NETWORK,
    SD_RESULT_ERROR_MEMORY
};

// ============================================
// Public Functions
// ============================================

/**
 * @brief Cria estrutura completa de diretórios no SD card
 * Deve ser chamado após aggressive_boot_logic() no setup()
 * @return SDResult código de resultado
 */
SDResult setup_sd_structure();

/**
 * @brief Move arquivos do flash interno (SPIFFS/LittleFS) pro SD
 * Procura payloads, models, configs no flash e copia pro SD
 * @return SDResult código de resultado
 */
SDResult move_flash_to_sd();

/**
 * @brief Atualiza base de CVE BR via HTTP GET
 * Baixa lista de brute force e CVEs brasileiros
 * @return SDResult código de resultado
 */
SDResult update_cve_database();

/**
 * @brief Faz backup da Q-table de IA
 * Copia q_table.bin para backup/
 * @return true se sucesso
 */
bool backup_q_table();

/**
 * @brief Incrementa contador de boots e faz backup se necessário
 * Chama backup_q_table() a cada BACKUP_INTERVAL_BOOTS boots
 */
void check_auto_backup();

/**
 * @brief Log com timestamp do DS3231
 * @param logFile Caminho do arquivo de log
 * @param message Mensagem a logar
 * @return true se escreveu com sucesso
 */
bool log_with_timestamp(const char* logFile, const char* message);

/**
 * @brief Verifica SHA256 de um arquivo de payload
 * @param path Caminho do arquivo
 * @param expectedHash Hash esperado (hex string 64 chars)
 * @return true se hash bate
 */
bool verify_payload_sha256(const char* path, const char* expectedHash);

/**
 * @brief Carrega arquivo pro cache PSRAM
 * @param path Caminho do arquivo
 * @return Ponteiro pro cache ou NULL se falhou
 */
PSRAMCache* cache_to_psram(const char* path);

/**
 * @brief Libera cache PSRAM
 * @param cache Ponteiro pro cache
 */
void cache_free(PSRAMCache* cache);

/**
 * @brief Retorna settings.json parseado
 * @param doc Documento JSON pra preencher
 * @return true se carregou com sucesso
 */
bool load_settings(JsonDocument& doc);

/**
 * @brief Salva settings.json
 * @param doc Documento JSON a salvar
 * @return true se salvou com sucesso
 */
bool save_settings(JsonDocument& doc);

/**
 * @brief Cria arquivo de settings padrão se não existir
 */
void create_default_settings();

/**
 * @brief Lista todos os arquivos de uma pasta de ataque
 * @param attackType "ble", "wifi", "nfc", "subghz", "ir"
 * @param files Array pra preencher
 * @param maxFiles Tamanho máximo do array
 * @return Número de arquivos encontrados
 */
int list_attack_files(const char* attackType, String* files, int maxFiles);

#endif // SD_STRUCTURE_H
