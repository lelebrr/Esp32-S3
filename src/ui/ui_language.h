#pragma once

/**
 * @file ui_language.h
 * @brief Sistema de suporte a múltiplos idiomas (PT-BR, EN, ES)
 */

#include <Arduino.h>

/**
 * @brief Idiomas suportados
 */
enum Language {
  LANG_PT_BR = 0, // Português Brasil (padrão)
  LANG_EN_US,     // English US
  LANG_ES,        // Español
  LANG_COUNT
};

/**
 * @brief IDs de strings traduzíveis
 */
enum StringId {
  // Menu Principal
  STR_WIFI_SCAN = 0,
  STR_BLE_SCAN,
  STR_ATTACKS,
  STR_TOOLS,
  STR_SETTINGS,
  STR_AI_ASSISTANT,

  // Ações
  STR_START,
  STR_STOP,
  STR_SCAN,
  STR_ATTACK,
  STR_CAPTURE,
  STR_SAVE,
  STR_CANCEL,
  STR_BACK,
  STR_NEXT,
  STR_CONFIRM,

  // Status
  STR_SCANNING,
  STR_ATTACKING,
  STR_IDLE,
  STR_CONNECTED,
  STR_DISCONNECTED,
  STR_HANDSHAKE_CAPTURED,
  STR_NETWORKS_FOUND,
  STR_DEVICES_FOUND,

  // Configurações
  STR_DISPLAY,
  STR_NETWORK,
  STR_SOUND,
  STR_SYSTEM,
  STR_DRAGON,
  STR_BRIGHTNESS,
  STR_THEME,
  STR_LANGUAGE,
  STR_VOLUME,

  // Dragão
  STR_DRAGON_NAME,
  STR_DRAGON_PERSONALITY,
  STR_DRAGON_MOOD,

  // Diversos
  STR_LOADING,
  STR_ERROR,
  STR_SUCCESS,
  STR_WARNING,
  STR_BATTERY,
  STR_TIME,
  STR_DATE,
  STR_VERSION,

  // Lock Screen
  STR_SWIPE_TO_UNLOCK,
  STR_ENTER_PIN,
  STR_DRAW_PATTERN,
  STR_WRONG_PIN,
  STR_WRONG_PATTERN,

  // Total
  STR_COUNT
};

/**
 * @brief Gerenciador de idiomas
 */
class LanguageManager {
public:
  LanguageManager();

  /**
   * @brief Inicializa com idioma padrão
   */
  void begin();

  /**
   * @brief Define idioma atual
   */
  void setLanguage(Language lang);
  Language getLanguage() const { return _currentLang; }

  /**
   * @brief Obtém string traduzida
   */
  const char *get(StringId id) const;

  /**
   * @brief Atalho para obter string
   */
  const char *operator[](StringId id) const { return get(id); }

  /**
   * @brief Nome do idioma atual
   */
  const char *getLanguageName() const;

  /**
   * @brief Código do idioma (pt-BR, en-US, es)
   */
  const char *getLanguageCode() const;

private:
  Language _currentLang;

  // Tabelas de strings para cada idioma (PROGMEM)
  static const char* const _strings_ptbr[] PROGMEM;
  static const char* const _strings_en[] PROGMEM;
  static const char* const _strings_es[] PROGMEM;

  const char* const* getTable() const;
};

extern LanguageManager lang;

// Macro de conveniência
#define L(id) lang.get(id)
