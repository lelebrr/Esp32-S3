/**
 * @file resource_downloader.h
 * @brief HTTP Resource Downloader for ESP32-S3
 * 
 * Downloads TTS voices, Brazilian wordlists, and CVE databases via HTTP.
 * Uses PSRAM buffers for efficient streaming and SHA256 verification.
 * 
 * @author Monster S3 Team
 * @date 2025-12-28
 */

#ifndef RESOURCE_DOWNLOADER_H
#define RESOURCE_DOWNLOADER_H

#include <Arduino.h>

// ============================================================================
// DOWNLOAD RESULT CODES
// ============================================================================

enum DownloadResult {
    DL_OK = 0,
    DL_ERROR_NO_WIFI,
    DL_ERROR_HTTP_FAILED,
    DL_ERROR_FILE_WRITE,
    DL_ERROR_CHECKSUM,
    DL_ERROR_NO_MEMORY,
    DL_ERROR_TIMEOUT,
    DL_ERROR_CANCELLED
};

// ============================================================================
// PROGRESS CALLBACK
// ============================================================================

/**
 * @brief Progress callback function type
 * @param downloaded Bytes downloaded so far
 * @param total Total bytes (0 if unknown)
 * @param filename Current file being downloaded
 * @return true to continue, false to cancel
 */
typedef bool (*DownloadProgressCallback)(size_t downloaded, size_t total, const char* filename);

// ============================================================================
// DOWNLOAD URLS (configurable via /config/urls.json)
// ============================================================================

#define URL_TTS_PTBR_ESPEAK    "https://raw.githubusercontent.com/espeak-ng/espeak-ng/master/dictsource/pt_dict"
#define URL_WORDLIST_ROCKYOU   "https://github.com/danielmiessler/SecLists/raw/master/Passwords/Leaked-Databases/rockyou-70.txt"
#define URL_WORDLIST_BR        "https://raw.githubusercontent.com/kkrypt0nn/wordlists/main/wordlists/passwords/common_passwords.txt"
#define URL_CVE_BR             "https://cve.circl.lu/api/query?vendor=br"

// ============================================================================
// PUBLIC API
// ============================================================================

/**
 * @brief Initialize the resource downloader
 * Sets up HTTP client and checks WiFi status
 */
void downloader_init();

/**
 * @brief Set progress callback for download operations
 * @param callback Function to call with progress updates
 */
void downloader_set_progress_callback(DownloadProgressCallback callback);

/**
 * @brief Download all TTS voice files for PT-BR
 * @return DownloadResult status code
 */
DownloadResult download_tts_voices();

/**
 * @brief Download Brazilian wordlists for brute-force attacks
 * Downloads: rockyou-BR, common BR passwords, ISP defaults
 * @return DownloadResult status code
 */
DownloadResult download_wordlists_br();

/**
 * @brief Update CVE database with Brazilian entries
 * @return DownloadResult status code
 */
DownloadResult download_cve_database();

/**
 * @brief Download all resources in one call
 * Calls: download_tts_voices, download_wordlists_br, download_cve_database
 * @return DownloadResult status code (first error encountered)
 */
DownloadResult download_all_resources();

/**
 * @brief Download a single file from URL to SD path
 * @param url Source URL
 * @param sdPath Destination path on SD card
 * @param expectedSha256 Optional SHA256 hash to verify (NULL to skip)
 * @return DownloadResult status code
 */
DownloadResult download_file(const char* url, const char* sdPath, const char* expectedSha256 = nullptr);

/**
 * @brief Check if essential resources are present on SD
 * @return true if all critical files exist
 */
bool resources_check_complete();

/**
 * @brief Get statistics about last download operation
 * @param bytesDownloaded Output: total bytes downloaded
 * @param filesDownloaded Output: number of files downloaded
 * @param errors Output: number of errors encountered
 */
void downloader_get_stats(size_t* bytesDownloaded, int* filesDownloaded, int* errors);

/**
 * @brief Cancel ongoing download
 */
void downloader_cancel();

#endif // RESOURCE_DOWNLOADER_H
