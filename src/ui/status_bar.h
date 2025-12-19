#pragma once

/**
 * @file status_bar.h
 * @brief Status bar estilo Pwnagotchi (CH, APS, UP, PWND, MODE)
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Modo de operação (inspirado no Pwnagotchi)
 */
enum OperationMode {
  MODE_MANU = 0, // Manual - conectado USB, sem ataques
  MODE_AUTO = 1, // Automático - operação normal
  MODE_AI = 2    // AI - usando NEURA9
};

/**
 * @brief Componente de status bar para UI
 */
class StatusBar {
public:
  StatusBar();

  /**
   * @brief Cria a status bar em um container pai
   * @param parent Container LVGL pai
   */
  void create(lv_obj_t *parent);

  /**
   * @brief Atualiza todos os valores exibidos
   */
  void update();

  /**
   * @brief Define canal atual
   * @param channel Canal (1-14) ou 0 para '*' (scanning)
   */
  void setChannel(uint8_t channel);

  /**
   * @brief Define contagem de APs
   * @param current APs no canal atual
   * @param total APs totais visíveis
   */
  void setAPs(uint16_t current, uint16_t total);

  /**
   * @brief Define uptime
   * @param seconds Segundos desde boot
   */
  void setUptime(uint32_t seconds);

  /**
   * @brief Define contagem de PWNs
   * @param session Handshakes na sessão
   * @param total Handshakes totais
   * @param lastSSID SSID do último handshake
   */
  void setPwnd(uint16_t session, uint16_t total,
               const char *lastSSID = nullptr);

  /**
   * @brief Define modo de operação
   */
  void setMode(OperationMode mode);

  /**
   * @brief Define nível da bateria
   * @param percent Porcentagem (0-100)
   * @param charging Se está carregando
   */
  void setBattery(uint8_t percent, bool charging);

  /**
   * @brief Define status da memória
   */
  void setMemory(uint32_t freeHeap, float tempC);

  /**
   * @brief Exibe/oculta a status bar
   */
  void show();
  void hide();
  bool isVisible() const { return _visible; }

private:
  lv_obj_t *_container;
  lv_obj_t *_lblChannel;
  lv_obj_t *_lblAPs;
  lv_obj_t *_lblUptime;
  lv_obj_t *_lblPwnd;
  lv_obj_t *_lblMode;
  lv_obj_t *_lblBattery;
  lv_obj_t *_lblMem;

  uint8_t _channel;
  uint16_t _apsChannel;
  uint16_t _apsTotal;
  uint32_t _uptime;
  uint16_t _pwndSession;
  uint16_t _pwndTotal;
  char _lastSSID[33];
  OperationMode _mode;
  uint8_t _battPercent;
  bool _battCharging;
  uint32_t _freeHeap;
  float _tempC;
  bool _visible;

  void createLabel(lv_obj_t **label, const char *text, lv_coord_t x);
  void updateLabels();
};

extern StatusBar statusBar;
