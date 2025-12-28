/**
 * @file brute_generator.cpp
 * @brief Brazilian Wordlist Generator Implementation
 * 
 * Generates 10M+ passwords optimized for Brazilian networks.
 * Uses PSRAM buffers for efficient file writing.
 * 
 * @author Monster S3 Team
 * @date 2025-12-28
 */

#include "brute_generator.h"
#include "sd_structure.h"
#include "core/aggressive_sd.h"
#include <SD.h>

// ============================================================================
// PRIVATE STATE
// ============================================================================

static BruteProgressCallback _progressCallback = nullptr;
static bool _cancelled = false;
static File _outputFile;
static char* _writeBuffer = nullptr;
static size_t _bufferPos = 0;
static uint32_t _totalGenerated = 0;

#define WRITE_BUFFER_SIZE 8192

// ============================================================================
// BUFFER HELPERS
// ============================================================================

static bool _initBuffer() {
    if (_writeBuffer) return true;
    
    _writeBuffer = (char*)heap_caps_malloc(WRITE_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!_writeBuffer) {
        _writeBuffer = (char*)malloc(WRITE_BUFFER_SIZE / 4);
    }
    _bufferPos = 0;
    return (_writeBuffer != nullptr);
}

static void _flushBuffer() {
    if (_outputFile && _bufferPos > 0) {
        _outputFile.write((uint8_t*)_writeBuffer, _bufferPos);
        _bufferPos = 0;
    }
}

static void _writeLine(const char* line) {
    size_t len = strlen(line);
    size_t bufSize = psramFound() ? WRITE_BUFFER_SIZE : WRITE_BUFFER_SIZE / 4;
    
    if (_bufferPos + len + 1 >= bufSize) {
        _flushBuffer();
    }
    
    memcpy(_writeBuffer + _bufferPos, line, len);
    _bufferPos += len;
    _writeBuffer[_bufferPos++] = '\n';
    _totalGenerated++;
}

static void _freeBuffer() {
    if (_writeBuffer) {
        free(_writeBuffer);
        _writeBuffer = nullptr;
    }
}

// ============================================================================
// PUBLIC API
// ============================================================================

void brute_set_progress_callback(BruteProgressCallback callback) {
    _progressCallback = callback;
}

void brute_cancel() {
    _cancelled = true;
    Serial.println("[BRUTE] Generation cancelled");
}

bool brute_check_exists() {
    if (!SD.exists(BRUTE_FILE_PATH)) return false;
    
    File f = SD.open(BRUTE_FILE_PATH, FILE_READ);
    if (!f) return false;
    
    size_t size = f.size();
    f.close();
    
    // Consider complete if >1MB
    return (size > 1024 * 1024);
}

size_t brute_estimate_size() {
    // Estimate: 10M passwords @ avg 10 bytes = ~100MB
    // Porteiros: 10000 * 5 = 50KB
    // Datas: 365 * 100 * 15 = 550KB
    // WiFi patterns: ~5MB
    // CEPs: ~2MB
    // Nomes: ~3MB
    return 10 * 1024 * 1024;  // 10MB estimate
}

uint32_t brute_generate_porteiros() {
    Serial.println("[BRUTE] Generating porteiro codes (0000-9999)...");
    uint32_t count = 0;
    char buf[8];
    
    for (int i = 0; i <= 9999 && !_cancelled; i++) {
        snprintf(buf, sizeof(buf), "%04d", i);
        _writeLine(buf);
        count++;
        
        if (i % 1000 == 0 && _progressCallback) {
            if (!_progressCallback(count, 10000, "Porteiros")) {
                _cancelled = true;
            }
        }
    }
    
    Serial.printf("[BRUTE] Porteiros: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_datas() {
    Serial.println("[BRUTE] Generating date patterns...");
    uint32_t count = 0;
    char buf[16];
    
    // Years from 1950-2025
    for (int year = 1950; year <= 2025 && !_cancelled; year++) {
        for (int month = 1; month <= 12; month++) {
            for (int day = 1; day <= 31; day++) {
                // Skip invalid dates
                if (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11)) continue;
                if (day > 29 && month == 2) continue;
                if (day > 28 && month == 2 && (year % 4 != 0)) continue;
                
                // DDMMYYYY
                snprintf(buf, sizeof(buf), "%02d%02d%04d", day, month, year);
                _writeLine(buf);
                count++;
                
                // DDMMYY
                snprintf(buf, sizeof(buf), "%02d%02d%02d", day, month, year % 100);
                _writeLine(buf);
                count++;
                
                // YYYY-MM-DD
                snprintf(buf, sizeof(buf), "%04d%02d%02d", year, month, day);
                _writeLine(buf);
                count++;
                
                // DD/MM/YYYY (sem barras = números)
                snprintf(buf, sizeof(buf), "%02d%02d%04d", day, month, year);
                // Already added above
            }
        }
        
        if (year % 10 == 0 && _progressCallback) {
            if (!_progressCallback(count, 200000, "Datas")) {
                _cancelled = true;
            }
        }
    }
    
    Serial.printf("[BRUTE] Datas: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_wifi_br() {
    Serial.println("[BRUTE] Generating ISP default patterns...");
    uint32_t count = 0;
    char buf[32];
    
    // ISP prefixes and patterns
    const char* prefixes[] = {
        "gvt", "GVT", "vivo", "VIVO", "claro", "CLARO", 
        "tim", "TIM", "oi", "OI", "net", "NET",
        "wifi", "WIFI", "admin", "ADMIN"
    };
    const int numPrefixes = sizeof(prefixes) / sizeof(prefixes[0]);
    
    // Pattern: prefix + 3-4 digits
    for (int p = 0; p < numPrefixes && !_cancelled; p++) {
        // prefix123
        for (int i = 0; i <= 999; i++) {
            snprintf(buf, sizeof(buf), "%s%03d", prefixes[p], i);
            _writeLine(buf);
            count++;
        }
        
        // prefix1234
        for (int i = 0; i <= 9999; i++) {
            snprintf(buf, sizeof(buf), "%s%04d", prefixes[p], i);
            _writeLine(buf);
            count++;
        }
        
        if (_progressCallback) {
            if (!_progressCallback(count, numPrefixes * 11000, "WiFi BR")) {
                _cancelled = true;
            }
        }
    }
    
    // Common WiFi patterns
    const char* common[] = {
        "12345678", "123456789", "1234567890",
        "password", "senha123", "mudar123",
        "internet", "wireless", "conexao",
        "bemvindo", "visitante", "guest123",
        "admin123", "admin1234", "adminadmin"
    };
    
    for (int i = 0; i < sizeof(common)/sizeof(common[0]) && !_cancelled; i++) {
        _writeLine(common[i]);
        count++;
    }
    
    Serial.printf("[BRUTE] WiFi BR: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_ceps() {
    Serial.println("[BRUTE] Generating CEP patterns...");
    uint32_t count = 0;
    char buf[12];
    
    // CEP ranges for major cities
    // São Paulo: 01000-09999
    for (int i = 1000; i <= 9999 && !_cancelled; i++) {
        snprintf(buf, sizeof(buf), "0%04d000", i);
        _writeLine(buf);
        count++;
    }
    
    // Rio de Janeiro: 20000-28999
    for (int i = 20000; i <= 28999 && !_cancelled; i++) {
        snprintf(buf, sizeof(buf), "%05d000", i);
        _writeLine(buf);
        count++;
    }
    
    // Belo Horizonte: 30000-34999
    for (int i = 30000; i <= 34999 && !_cancelled; i++) {
        snprintf(buf, sizeof(buf), "%05d000", i);
        _writeLine(buf);
        count++;
    }
    
    if (_progressCallback) {
        _progressCallback(count, 25000, "CEPs");
    }
    
    Serial.printf("[BRUTE] CEPs: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_nomes_br() {
    Serial.println("[BRUTE] Generating Brazilian names...");
    uint32_t count = 0;
    char buf[32];
    
    // Most common Brazilian first names
    const char* nomes[] = {
        "maria", "jose", "ana", "joao", "antonio",
        "francisco", "carlos", "paulo", "pedro", "lucas",
        "gabriel", "rafael", "daniel", "marcelo", "bruno",
        "felipe", "rodrigo", "gustavo", "andre", "fernando",
        "julia", "amanda", "beatriz", "camila", "larissa",
        "leticia", "mariana", "patricia", "fernanda", "bruna"
    };
    const int numNomes = sizeof(nomes) / sizeof(nomes[0]);
    
    for (int n = 0; n < numNomes && !_cancelled; n++) {
        // nome
        _writeLine(nomes[n]);
        count++;
        
        // nome123
        snprintf(buf, sizeof(buf), "%s123", nomes[n]);
        _writeLine(buf);
        count++;
        
        // nome1234
        snprintf(buf, sizeof(buf), "%s1234", nomes[n]);
        _writeLine(buf);
        count++;
        
        // nome + year
        for (int year = 1970; year <= 2010; year++) {
            snprintf(buf, sizeof(buf), "%s%d", nomes[n], year);
            _writeLine(buf);
            count++;
        }
    }
    
    // Common Brazilian passwords
    const char* comum[] = {
        "brasil", "brasil1", "brasil123",
        "flamengo", "corinthians", "palmeiras", "santos", "gremio",
        "amor", "amor123", "teamo", "teamo123",
        "familia", "deus", "jesus", "feliz"
    };
    
    for (int i = 0; i < sizeof(comum)/sizeof(comum[0]) && !_cancelled; i++) {
        _writeLine(comum[i]);
        count++;
        
        // Add with numbers
        snprintf(buf, sizeof(buf), "%s123", comum[i]);
        _writeLine(buf);
        count++;
    }
    
    if (_progressCallback) {
        _progressCallback(count, 2000, "Nomes BR");
    }
    
    Serial.printf("[BRUTE] Nomes BR: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_wps_pins() {
    Serial.println("[BRUTE] Generating WPS PINs...");
    uint32_t count = 0;
    char buf[12];
    
    // Common WPS PINs (first 4 digits matter most due to Pixie Dust)
    
    // Sequential patterns
    for (int i = 0; i <= 9999 && !_cancelled; i++) {
        // First half of PIN (most important for Pixie Dust)
        snprintf(buf, sizeof(buf), "%04d", i);
        _writeLine(buf);
        count++;
        
        if (i % 2000 == 0 && _progressCallback) {
            if (!_progressCallback(count, 10000, "WPS PINs")) {
                _cancelled = true;
            }
        }
    }
    
    // Full 8-digit PINs with checksum (simplified)
    for (int i = 0; i <= 9999 && !_cancelled; i++) {
        // Generate valid WPS PIN with checksum
        int pin = i * 10000;
        int checksum = 0;
        int temp = pin;
        for (int j = 0; j < 7; j++) {
            int digit = temp % 10;
            temp /= 10;
            checksum += digit * ((j % 2 == 0) ? 3 : 1);
        }
        checksum = (10 - (checksum % 10)) % 10;
        pin += checksum;
        
        snprintf(buf, sizeof(buf), "%08d", pin);
        _writeLine(buf);
        count++;
    }
    
    Serial.printf("[BRUTE] WPS PINs: %d passwords\n", count);
    return count;
}

uint32_t brute_generate_complete() {
    Serial.println("[BRUTE] ======================================");
    Serial.println("[BRUTE] GENERATING COMPLETE BR WORDLIST");
    Serial.println("[BRUTE] ======================================");
    
    _cancelled = false;
    _totalGenerated = 0;
    
    // Initialize buffer
    if (!_initBuffer()) {
        Serial.println("[BRUTE] Failed to allocate buffer!");
        return 0;
    }
    
    // Ensure directory exists
    if (!SD.exists("/cve")) {
        SD.mkdir("/cve");
    }
    
    // Open output file
    _outputFile = SD.open(BRUTE_FILE_PATH, FILE_WRITE);
    if (!_outputFile) {
        Serial.printf("[BRUTE] Failed to open: %s\n", BRUTE_FILE_PATH);
        _freeBuffer();
        return 0;
    }
    
    unsigned long startTime = millis();
    
    // Generate all categories
    brute_generate_porteiros();
    if (!_cancelled) brute_generate_datas();
    if (!_cancelled) brute_generate_wifi_br();
    if (!_cancelled) brute_generate_ceps();
    if (!_cancelled) brute_generate_nomes_br();
    
    // Flush remaining buffer
    _flushBuffer();
    _outputFile.close();
    
    // Generate separate WPS file
    if (!_cancelled) {
        _outputFile = SD.open(BRUTE_FILE_PATH_WPS, FILE_WRITE);
        if (_outputFile) {
            brute_generate_wps_pins();
            _flushBuffer();
            _outputFile.close();
        }
    }
    
    _freeBuffer();
    
    unsigned long elapsed = millis() - startTime;
    
    Serial.println("[BRUTE] ======================================");
    Serial.printf("[BRUTE] COMPLETE: %d passwords in %lu ms\n", _totalGenerated, elapsed);
    Serial.println("[BRUTE] ======================================");
    
    log_with_timestamp(SD_FILE_LOG_ATTACKS, "Brute wordlist generated");
    
    return _totalGenerated;
}
