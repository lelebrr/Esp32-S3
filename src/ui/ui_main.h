#pragma once

/**
 * @file ui_main.h
 * @brief Header da interface principal WavePwn
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Enumeração das telas disponíveis
 */
enum UIScreen {
  UI_SCREEN_MAIN,     // Tela principal com mascote
  UI_SCREEN_WIFI,     // Scanner WiFi
  UI_SCREEN_BLE,      // Ferramentas BLE
  UI_SCREEN_ATTACKS,  // Menu de ataques
  UI_SCREEN_SETTINGS, // Configurações
  UI_SCREEN_ABOUT,    // Sobre/Info
  UI_SCREEN_LOCK,     // Lock Screen (Tip 34)
  UI_SCREEN_CAPTURES, // Captures Gallery (Tip 39)
  UI_SCREEN_PLUGINS,  // Plugins Grid (Tip 40)
  UI_SCREEN_WIFI_CHAOS,
  UI_SCREEN_BLE_CHAOS,
  UI_SCREEN_STATS,    // New
  UI_SCREEN_FILES,    // New
  UI_SCREEN_IOT_PWN   // IOT PWN Module
};

/**
 * @brief Expressões do Mascote
 */
enum MascotFace {
  MASCOT_FACE_NORMAL,   // 🐉
  MASCOT_FACE_HAPPY,    // 🎉 / 💎
  MASCOT_FACE_ANGRY,    // 😈 / 🔥
  MASCOT_FACE_SLEEP,    // 💤
  MASCOT_FACE_CONFUSED, // 👀
  MASCOT_FACE_COOL      // 😎
};

/**
 * @brief Define a expressão do mascote
 */
void ui_set_mascot_face(MascotFace face);

/**
 * @brief Inicializa a UI principal
 * @return true se inicializada com sucesso
 */
bool ui_main_init();

/**
 * @brief Atualiza o texto do humor do mascote
 * @param text Texto a ser exibido
 */
void ui_set_mood_text(const char *text);

/**
 * @brief Obtém a tela atual
 * @return Tela atualmente ativa
 */
UIScreen ui_get_current_screen();

/**
 * @brief Muda para uma tela específica
 * @param screen Tela de destino
 */
/**
 * @brief Exibe a tela principal
 */
void ui_main_show();

void ui_set_screen(UIScreen screen);

/**
 * @brief Obtém a área de conteúdo central (Zona 2)
 */
lv_obj_t *ui_get_content_area();

/**
 * @brief Limpa a área de conteúdo
 */
void ui_clear_content_area();
