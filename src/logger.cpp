/**
 * @file logger.cpp
 * @brief Logging System for Monster S3
 * 
 * Logs events to SD card:
 * - /logs/attacks.log - Attack executions and results
 * - /logs/battery.log - Battery status over time
 * - /logs/system.log - System events
 * - /logs/ai.log - AI decisions and learning
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "logger.h"
#include "pin_config.h"
#include <SD.h>

// ============================================================================
// PATHS
// ============================================================================

#define LOG_DIR         "/logs"
#define ATTACKS_LOG     "/logs/attacks.log"
#define BATTERY_LOG     "/logs/battery.log"
#define SYSTEM_LOG      "/logs/system.log"
#define AI_LOG          "/logs/ai.log"

// ============================================================================
// STATE
// ============================================================================

static bool logger_initialized = false;

// ============================================================================
// HELPERS
// ============================================================================

static String get_timestamp() {
    // Format: YYYY-MM-DD HH:MM:SS
    // Using millis since boot (RTC integration TODO)
    unsigned long ms = millis();
    unsigned long sec = ms / 1000;
    unsigned long min = sec / 60;
    unsigned long hr = min / 60;
    
    char buf[20];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu.%03lu",
             hr % 24, min % 60, sec % 60, ms % 1000);
    return String(buf);
}

static void write_log(const char* path, const char* message) {
    if (!logger_initialized) return;
    
    File file = SD.open(path, FILE_APPEND);
    if (file) {
        file.printf("[%s] %s\n", get_timestamp().c_str(), message);
        file.close();
    } else {
        Serial.printf("[LOG] Failed to write to %s\n", path);
    }
}

// ============================================================================
// PUBLIC API
// ============================================================================

void log_init() {
    if (logger_initialized) return;
    
    Serial.println("[LOG] Initializing logging system...");
    
    // Check SD
    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[LOG] SD card not available - logging disabled");
        return;
    }
    
    // Create logs directory
    if (!SD.exists(LOG_DIR)) {
        Serial.println("[LOG] Creating /logs directory");
        SD.mkdir(LOG_DIR);
    }
    
    logger_initialized = true;
    
    // Log startup
    log_system("Logger initialized");
    Serial.println("[LOG] Logging system ready");
}

void log_attack(const char* attack_name, bool success) {
    if (!logger_initialized) {
        Serial.printf("[LOG] Attack: %s - %s\n", attack_name, success ? "SUCCESS" : "FAIL");
        return;
    }
    
    char buf[128];
    snprintf(buf, sizeof(buf), "ATTACK %s: %s",
             attack_name, success ? "SUCCESS" : "FAIL");
    
    write_log(ATTACKS_LOG, buf);
    Serial.printf("[LOG] %s\n", buf);
}

void log_battery(int level, float voltage) {
    if (!logger_initialized) return;
    
    char buf[64];
    snprintf(buf, sizeof(buf), "BAT %d%% %.2fV", level, voltage);
    write_log(BATTERY_LOG, buf);
}

void log_system(const char* event) {
    if (!logger_initialized) {
        Serial.printf("[LOG] System: %s\n", event);
        return;
    }
    
    char buf[128];
    snprintf(buf, sizeof(buf), "SYS %s", event);
    write_log(SYSTEM_LOG, buf);
}

void log_ai(int state, int action, float reward) {
    if (!logger_initialized) return;
    
    char buf[64];
    snprintf(buf, sizeof(buf), "AI S:%d A:%d R:%.2f", state, action, reward);
    write_log(AI_LOG, buf);
}

void log_flush() {
    // SD library auto-flushes on close
    // This is a placeholder for future buffered writes
    Serial.println("[LOG] Flushed");
}
