/**
 * @file debug_log.h
 * @brief Sistema de logging condicional para otimização de performance
 *
 * Este header substitui chamadas diretas Serial.print() por macros condicionais
 * que podem ser desabilitadas em builds de produção para melhorar performance.
 *
 * Uso:
 *   LOG_DEBUG("Mensagem de debug");
 *   LOG_INFO("Info: %s", variavel);
 *   LOG_WARN("Aviso importante");
 *   LOG_ERROR("Erro crítico: %d", codigo);
 *
 * Para desabilitar em produção, defina -DDISABLE_DEBUG_LOG=1 no platformio.ini
 */

#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#include <Arduino.h>

// ============================================================================
// Configuração de níveis de log
// ============================================================================

// Níveis de log (use -DLOG_LEVEL=X no platformio.ini)
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_VERBOSE 5

// Nível padrão se não definido
#ifndef LOG_LEVEL
#ifdef DISABLE_DEBUG_LOG
#define LOG_LEVEL LOG_LEVEL_ERROR
#else
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif
#endif

// ============================================================================
// Macros de logging
// ============================================================================

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) Serial.printf("[WARN] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) Serial.printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOG_VERBOSE(fmt, ...) Serial.printf("[VERBOSE] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_VERBOSE(fmt, ...) ((void)0)
#endif

// ============================================================================
// Macros de conveniência para módulos específicos
// ============================================================================

// BLE Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_BLE(fmt, ...) Serial.printf("[BLE] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_BLE(fmt, ...) ((void)0)
#endif

// WiFi Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_WIFI(fmt, ...) Serial.printf("[WIFI] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_WIFI(fmt, ...) ((void)0)
#endif

// RF Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_RF(fmt, ...) Serial.printf("[RF] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_RF(fmt, ...) ((void)0)
#endif

// IR Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_IR(fmt, ...) Serial.printf("[IR] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_IR(fmt, ...) ((void)0)
#endif

// UI Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_UI(fmt, ...) Serial.printf("[UI] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_UI(fmt, ...) ((void)0)
#endif

// RFID Module
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_RFID(fmt, ...) Serial.printf("[RFID] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_RFID(fmt, ...) ((void)0)
#endif

// Notifications
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_NOTIFY(fmt, ...) Serial.printf("[NOTIFY] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_NOTIFY(fmt, ...) ((void)0)
#endif

// Sounds
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_SOUND(fmt, ...) Serial.printf("[SOUNDS] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_SOUND(fmt, ...) ((void)0)
#endif

// ============================================================================
// Macro para logs simples (sem formatação)
// ============================================================================

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_PRINTLN(msg) Serial.println(msg)
#define LOG_PRINT(msg) Serial.print(msg)
#else
#define LOG_PRINTLN(msg) ((void)0)
#define LOG_PRINT(msg) ((void)0)
#endif

// ============================================================================
// Helpers para timing/performance (apenas em debug)
// ============================================================================

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOG_TIMING_START(name) unsigned long _timing_##name = millis()
#define LOG_TIMING_END(name) Serial.printf("[TIMING] %s: %lu ms\n", #name, millis() - _timing_##name)
#else
#define LOG_TIMING_START(name) ((void)0)
#define LOG_TIMING_END(name) ((void)0)
#endif

// ============================================================================
// Helper para dump de memória (apenas em debug)
// ============================================================================

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOG_MEMORY()                                                                                         \
    Serial.printf(                                                                                           \
        "[MEM] Heap: %d/%d | PSRAM: %d/%d\n",                                                                \
        ESP.getFreeHeap(),                                                                                   \
        ESP.getHeapSize(),                                                                                   \
        ESP.getFreePsram(),                                                                                  \
        ESP.getPsramSize()                                                                                   \
    )
#else
#define LOG_MEMORY() ((void)0)
#endif

#endif // __DEBUG_LOG_H__
