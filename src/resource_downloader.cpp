/**
 * @file resource_downloader.cpp
 * @brief HTTP Resource Downloader Implementation
 *
 * Downloads TTS voices, Brazilian wordlists, and CVE databases.
 * Uses PSRAM buffers for efficient streaming.
 *
 * @author MorphNode Team
 * @date 2025-12-28
 */

#include "resource_downloader.h"
#include "core/aggressive_sd.h"
#include "sd_structure.h"
#include <HTTPClient.h>
#include <SD.h>
#include <WiFi.h>
#include <mbedtls/sha256.h>


// ============================================================================
// PRIVATE STATE
// ============================================================================

static DownloadProgressCallback _progressCallback = nullptr;
static bool _cancelled = false;
static size_t _totalBytesDownloaded = 0;
static int _filesDownloaded = 0;
static int _errors = 0;

// Buffer size for streaming (4KB in PSRAM)
#define DL_BUFFER_SIZE 4096
#define DL_TIMEOUT_MS 30000

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static bool _checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[DL] WiFi not connected!");
        return false;
    }
    return true;
}

static uint8_t *_allocBuffer() {
    uint8_t *buf = (uint8_t *)heap_caps_malloc(DL_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        buf = (uint8_t *)malloc(DL_BUFFER_SIZE / 4); // Fallback to smaller heap buffer
    }
    return buf;
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void downloader_init() {
    _cancelled = false;
    _totalBytesDownloaded = 0;
    _filesDownloaded = 0;
    _errors = 0;
    Serial.println("[DL] Resource downloader initialized");
}

void downloader_set_progress_callback(DownloadProgressCallback callback) { _progressCallback = callback; }

void downloader_cancel() {
    _cancelled = true;
    Serial.println("[DL] Download cancelled by user");
}

void downloader_get_stats(size_t *bytesDownloaded, int *filesDownloaded, int *errors) {
    if (bytesDownloaded) *bytesDownloaded = _totalBytesDownloaded;
    if (filesDownloaded) *filesDownloaded = _filesDownloaded;
    if (errors) *errors = _errors;
}

DownloadResult download_file(const char *url, const char *sdPath, const char *expectedSha256) {
    if (!_checkWiFi()) return DL_ERROR_NO_WIFI;
    if (_cancelled) return DL_ERROR_CANCELLED;

    Serial.printf("[DL] Downloading: %s\n", url);
    Serial.printf("[DL] -> %s\n", sdPath);

    HTTPClient http;
    http.begin(url);
    http.setTimeout(DL_TIMEOUT_MS);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[DL] HTTP Error: %d\n", httpCode);
        http.end();
        _errors++;
        return DL_ERROR_HTTP_FAILED;
    }

    int contentLength = http.getSize();
    Serial.printf("[DL] Content-Length: %d bytes\n", contentLength);

    // Allocate buffer
    uint8_t *buffer = _allocBuffer();
    if (!buffer) {
        Serial.println("[DL] Failed to allocate buffer!");
        http.end();
        _errors++;
        return DL_ERROR_NO_MEMORY;
    }

    size_t bufferSize = psramFound() ? DL_BUFFER_SIZE : DL_BUFFER_SIZE / 4;

    // Open file for writing
    File file = SD.open(sdPath, FILE_WRITE);
    if (!file) {
        Serial.printf("[DL] Failed to open: %s\n", sdPath);
        free(buffer);
        http.end();
        _errors++;
        return DL_ERROR_FILE_WRITE;
    }

    // Setup SHA256 if verification needed
    mbedtls_sha256_context sha_ctx;
    bool doVerify = (expectedSha256 != nullptr);
    if (doVerify) {
        mbedtls_sha256_init(&sha_ctx);
        mbedtls_sha256_starts(&sha_ctx, 0);
    }

    // Stream download
    WiFiClient *stream = http.getStreamPtr();
    size_t downloaded = 0;
    unsigned long lastProgress = 0;

    while (http.connected() && (contentLength < 0 || downloaded < contentLength)) {
        if (_cancelled) {
            file.close();
            SD.remove(sdPath); // Remove partial file
            free(buffer);
            http.end();
            if (doVerify) mbedtls_sha256_free(&sha_ctx);
            return DL_ERROR_CANCELLED;
        }

        size_t available = stream->available();
        if (available > 0) {
            size_t toRead = min(available, bufferSize);
            size_t bytesRead = stream->readBytes(buffer, toRead);

            file.write(buffer, bytesRead);
            downloaded += bytesRead;

            if (doVerify) { mbedtls_sha256_update(&sha_ctx, buffer, bytesRead); }

            // Progress callback every 10KB
            if (_progressCallback && (downloaded - lastProgress > 10240)) {
                lastProgress = downloaded;
                const char *filename = strrchr(sdPath, '/');
                filename = filename ? filename + 1 : sdPath;
                if (!_progressCallback(downloaded, contentLength, filename)) { _cancelled = true; }
            }
        }

        vTaskDelay(1); // Yield to other tasks
    }

    file.close();
    free(buffer);
    http.end();

    // Verify SHA256 if provided
    if (doVerify) {
        uint8_t hash[32];
        mbedtls_sha256_finish(&sha_ctx, hash);
        mbedtls_sha256_free(&sha_ctx);

        char hashStr[65];
        for (int i = 0; i < 32; i++) { sprintf(hashStr + (i * 2), "%02x", hash[i]); }
        hashStr[64] = '\0';

        if (strcmp(hashStr, expectedSha256) != 0) {
            Serial.printf("[DL] SHA256 mismatch!\n");
            Serial.printf("[DL] Expected: %s\n", expectedSha256);
            Serial.printf("[DL] Got:      %s\n", hashStr);
            SD.remove(sdPath);
            _errors++;
            return DL_ERROR_CHECKSUM;
        }
        Serial.println("[DL] SHA256 verified OK");
    }

    _totalBytesDownloaded += downloaded;
    _filesDownloaded++;
    Serial.printf("[DL] Downloaded: %d bytes\n", downloaded);
    log_with_timestamp(SD_FILE_LOG_ATTACKS, sdPath);

    return DL_OK;
}

DownloadResult download_tts_voices() {
    Serial.println("[DL] === Downloading TTS Voices PT-BR ===");

    // Create TTS directories if needed
    if (!SD.exists(SD_PATH_TTS_VOICES)) { SD.mkdir(SD_PATH_TTS_VOICES); }

    // Download eSpeak PT-BR dictionary
    DownloadResult result = download_file(
        URL_TTS_PTBR_ESPEAK,
        "/tts/voices/pt_dict",
        nullptr // No SHA256 verification for now
    );

    if (result != DL_OK) {
        Serial.printf("[DL] TTS voice download failed: %d\n", result);
        return result;
    }

    Serial.println("[DL] TTS voices download complete");
    return DL_OK;
}

DownloadResult download_wordlists_br() {
    Serial.println("[DL] === Downloading Brazilian Wordlists ===");

    // Ensure CVE directory exists
    if (!SD.exists(SD_PATH_CVE)) { SD.mkdir(SD_PATH_CVE); }

    // Download common passwords
    DownloadResult result = download_file(URL_WORDLIST_BR, "/cve/common_br.txt", nullptr);

    if (result != DL_OK && result != DL_ERROR_CANCELLED) {
        Serial.printf("[DL] Warning: Common wordlist failed: %d\n", result);
        // Continue with next file
    }

    Serial.println("[DL] Wordlist downloads complete");
    return DL_OK;
}

DownloadResult download_cve_database() {
    Serial.println("[DL] === Downloading CVE Database ===");

    // Backup existing CVE file
    if (SD.exists(SD_FILE_CVE_BR)) {
        Serial.println("[DL] Backing up existing CVE database...");
        // Copy to backup
        File src = SD.open(SD_FILE_CVE_BR, FILE_READ);
        if (src) {
            File dst = SD.open(SD_FILE_CVE_BAK, FILE_WRITE);
            if (dst) {
                uint8_t buf[1024];
                while (src.available()) {
                    size_t r = src.read(buf, sizeof(buf));
                    dst.write(buf, r);
                }
                dst.close();
            }
            src.close();
        }
    }

    DownloadResult result = download_file(URL_CVE_BR, SD_FILE_CVE_BR, nullptr);

    if (result != DL_OK) {
        Serial.printf("[DL] CVE download failed: %d, restoring backup\n", result);
        // Restore from backup if download failed
        if (SD.exists(SD_FILE_CVE_BAK)) {
            SD.remove(SD_FILE_CVE_BR);
            SD.rename(SD_FILE_CVE_BAK, SD_FILE_CVE_BR);
        }
        return result;
    }

    Serial.println("[DL] CVE database updated");
    return DL_OK;
}

DownloadResult download_all_resources() {
    Serial.println("[DL] ======================================");
    Serial.println("[DL] DOWNLOADING ALL RESOURCES");
    Serial.println("[DL] ======================================");

    downloader_init();

    DownloadResult result;

    // TTS Voices
    result = download_tts_voices();
    if (result != DL_OK && result != DL_ERROR_CANCELLED) {
        Serial.printf("[DL] TTS download error: %d (continuing...)\n", result);
    }
    if (_cancelled) return DL_ERROR_CANCELLED;

    // Wordlists
    result = download_wordlists_br();
    if (result != DL_OK && result != DL_ERROR_CANCELLED) {
        Serial.printf("[DL] Wordlist download error: %d (continuing...)\n", result);
    }
    if (_cancelled) return DL_ERROR_CANCELLED;

    // CVE Database
    result = download_cve_database();
    if (result != DL_OK && result != DL_ERROR_CANCELLED) {
        Serial.printf("[DL] CVE download error: %d (continuing...)\n", result);
    }

    Serial.println("[DL] ======================================");
    Serial.printf(
        "[DL] COMPLETE: %d files, %d bytes, %d errors\n", _filesDownloaded, _totalBytesDownloaded, _errors
    );
    Serial.println("[DL] ======================================");

    return (_errors == 0) ? DL_OK : DL_ERROR_HTTP_FAILED;
}

bool resources_check_complete() {
    // Check essential files exist
    bool hasVoices = SD.exists("/tts/voices/pt_dict") || SD.exists("/tts/frases/pronto.wav");
    bool hasWordlist = SD.exists(SD_FILE_BRUTE_BR) || SD.exists("/cve/common_br.txt");
    bool hasQTable = SD.exists(SD_FILE_QTABLE);

    Serial.printf(
        "[DL] Resources check: voices=%d, wordlist=%d, qtable=%d\n", hasVoices, hasWordlist, hasQTable
    );

    return hasVoices && hasWordlist && hasQTable;
}
