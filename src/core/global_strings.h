/**
 * @file global_strings.h
 * @brief Constantes de string globais para evitar duplicação
 * @author Lele Origin Team
 *
 * Este arquivo contém strings comuns usadas em múltiplos lugares
 * do código. Centralizar evita duplicação e economiza Flash.
 */

#ifndef __GLOBAL_STRINGS_H__
#define __GLOBAL_STRINGS_H__

#include <Arduino.h>

// ============================================================================
// FORMATO DE MAC ADDRESS
// ============================================================================
#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"

// ============================================================================
// MENSAGENS DE ERRO COMUNS
// ============================================================================
#define STR_ERR_CONFIG_OPEN     "Failed to open config file"
#define STR_ERR_CONFIG_WRITE    "Failed to write config file"
#define STR_ERR_CONFIG_READ     "Failed to read config file"
#define STR_ERR_FILE_OPEN       "Failed to open file"
#define STR_ERR_FILE_WRITE      "Failed to write file"
#define STR_ERR_SDCARD_MOUNT    "SD Card not mounted"
#define STR_ERR_LITTLEFS_MOUNT  "LittleFS not mounted"

// ============================================================================
// MENSAGENS DE STATUS
// ============================================================================
#define STR_STAT_SDCARD_OK      "SDCARD mounted successfully"
#define STR_STAT_SDCARD_UNMOUNT "SD Card Unmounted"
#define STR_STAT_CONNECTING     "Connecting to: "
#define STR_STAT_CONNECTED      "Connected to: "
#define STR_STAT_WEBSERVER_START "Webserver started"

// ============================================================================
// MENSAGENS DE DEBUG (podem ser removidas em release)
// ============================================================================
#ifdef DEBUG_BUILD
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

// ============================================================================
// MACROS PARA SERIAL COM F() AUTOMÁTICO
// ============================================================================

// Usar estas macros em vez de Serial.println() diretamente
// Economiza RAM movendo strings para Flash
#define SERIAL_INFO(msg)    Serial.println(F(msg))
#define SERIAL_ERROR(msg)   Serial.println(F("[ERROR] " msg))
#define SERIAL_WARN(msg)    Serial.println(F("[WARN] " msg))
#define SERIAL_DEBUG(msg)   DEBUG_PRINTLN(F(msg))

// Para strings dinâmicas (não podem usar F())
#define SERIAL_VAR(msg)     Serial.println(msg)

#endif // __GLOBAL_STRINGS_H__
