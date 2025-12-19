#pragma once

/**
 * @file ui_kids_mode.h
 * @brief Modo Kids - Interface simplificada e segura para crianças
 *
 * Desativa todas as funções de ataque e mostra apenas
 * o dragão como um Tamagotchi virtual
 */

#include <lvgl.h>

/**
 * @brief Configuração do Modo Kids
 */
struct KidsModeConfig {
  bool enabled;
  bool hideAttacks;     // Esconde botões de ataque
  bool hideNetworks;    // Esconde informações de rede
  bool simpleUI;        // UI simplificada
  bool bigButtons;      // Botões grandes
  bool colorfulTheme;   // Tema colorido
  uint8_t parentPin[4]; // PIN para desativar modo
};

/**
 * @brief Modo Kids - Transforma LeleWatch em Tamagotchi
 */
class KidsMode {
public:
  KidsMode();

  /**
   * @brief Inicializa o modo kids
   */
  void begin();

  /**
   * @brief Ativa o modo kids
   */
  void enable();

  /**
   * @brief Desativa o modo kids (requer PIN)
   * @return true se desativou com sucesso
   */
  bool disable(const char *pin);

  /**
   * @brief Verifica se está ativo
   */
  bool isEnabled() const { return _config.enabled; }

  /**
   * @brief Cria a UI do modo kids
   */
  void createUI();

  /**
   * @brief Atualiza animações
   */
  void update();

  /**
   * @brief Alimenta o dragão
   */
  void feedDragon();

  /**
   * @brief Brinca com o dragão
   */
  void playWithDragon();

  /**
   * @brief Coloca dragão para dormir
   */
  void sleepDragon();

  /**
   * @brief Obtém humor atual do dragão (0-100)
   */
  uint8_t getDragonMood() const { return _dragonMood; }

  /**
   * @brief Obtém fome do dragão (0-100)
   */
  uint8_t getDragonHunger() const { return _dragonHunger; }

  /**
   * @brief Configuração
   */
  void setConfig(const KidsModeConfig &config);
  KidsModeConfig getConfig() const { return _config; }

  /**
   * @brief Define PIN parental
   */
  void setParentPin(const char *pin);

private:
  KidsModeConfig _config;
  lv_obj_t *_screen;
  lv_obj_t *_dragonLabel;
  lv_obj_t *_moodBar;
  lv_obj_t *_hungerBar;
  lv_obj_t *_feedBtn;
  lv_obj_t *_playBtn;
  lv_obj_t *_sleepBtn;
  lv_obj_t *_statusLabel;

  uint8_t _dragonMood;
  uint8_t _dragonHunger;
  uint8_t _dragonEnergy;
  uint32_t _lastUpdateTime;

  void updateStats();
  void updateDragonFace();
  void showHeart();
  void showStar();
};

extern KidsMode kidsMode;
