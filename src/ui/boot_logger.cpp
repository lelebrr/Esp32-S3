/**
 * @file boot_logger.cpp
 * @brief Sistema de log para tela de boot
 */

#include "boot_logger.h"

BootLogger bootLogger;

BootLogger::BootLogger() 
    : _progress(0), _currentStep(0), _totalSteps(8) {
    _logs.reserve(MAX_BOOT_LOGS);
}

void BootLogger::addLog(const char* text, uint32_t color, bool isError) {
    BootLogEntry entry;
    strncpy(entry.text, text, MAX_LOG_LENGTH - 1);
    entry.text[MAX_LOG_LENGTH - 1] = '\0';
    entry.color = color;
    entry.isError = isError;
    
    _logs.push_back(entry);
    
    // Remove logs antigos se exceder o máximo
    while (_logs.size() > MAX_BOOT_LOGS) {
        _logs.erase(_logs.begin());
    }
}

void BootLogger::logOK(const char* module, const char* message) {
    char buf[MAX_LOG_LENGTH];
    snprintf(buf, sizeof(buf), "[%s] ✓ %s", module, message);
    addLog(buf, LOG_COLOR_GREEN, false);
}

void BootLogger::logError(const char* module, const char* message) {
    char buf[MAX_LOG_LENGTH];
    snprintf(buf, sizeof(buf), "[%s] ✗ %s", module, message);
    addLog(buf, LOG_COLOR_RED, true);
}

void BootLogger::logProgress(int current, int total, const char* message) {
    char buf[MAX_LOG_LENGTH];
    snprintf(buf, sizeof(buf), "[%d/%d] %s", current, total, message);
    addLog(buf, LOG_COLOR_CYAN, false);
    
    _currentStep = current;
    _totalSteps = total;
    _progress = (current * 100) / total;
}

void BootLogger::clear() {
    _logs.clear();
    _progress = 0;
    _currentStep = 0;
}

void BootLogger::setStep(int current, int total) {
    _currentStep = current;
    _totalSteps = total;
    _progress = (current * 100) / total;
}
