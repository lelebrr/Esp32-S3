#pragma once

/**
 * @file easter_eggs.h
 * @brief Easter eggs e segredos do LeleWatch
 *
 * Inclui Konami Code e outras surpresas
 */

#include <Arduino.h>
#include <lvgl.h>

// Konami Code: ↑↑↓↓←→←→BA
#define KONAMI_CODE_LENGTH 10

enum TouchDirection {
  TOUCH_UP = 0,
  TOUCH_DOWN,
  TOUCH_LEFT,
  TOUCH_RIGHT,
  TOUCH_TAP_A, // Tap esquerdo
  TOUCH_TAP_B  // Tap direito
};

/**
 * @brief Gerenciador de Easter Eggs
 */
class EasterEggs {
public:
  EasterEggs();

  /**
   * @brief Inicializa o sistema
   */
  void begin();

  /**
   * @brief Processa input de toque/gesto
   * @param direction Direção do gesto
   * @return true se um easter egg foi ativado
   */
  bool processInput(TouchDirection direction);

  /**
   * @brief Processa swipe baseado em coordenadas
   * @param startX, startY Ponto inicial
   * @param endX, endY Ponto final
   */
  void processSwipe(int16_t startX, int16_t startY, int16_t endX, int16_t endY);

  /**
   * @brief Processa tap em posição
   * @param x, y Coordenadas do tap
   * @param screenWidth Largura da tela
   */
  void processTap(int16_t x, int16_t y, int16_t screenWidth);

  /**
   * @brief Verifica se Konami Code foi inserido
   */
  bool isKonamiCodeActive() const { return _konamiActive; }

  /**
   * @brief Reseta todos os easter eggs
   */
  void reset();

  /**
   * @brief Callback quando easter egg é ativado
   */
  void onEasterEgg(void (*callback)(int eggId));

private:
  // Konami Code sequence: UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT B A
  static const TouchDirection KONAMI_CODE[KONAMI_CODE_LENGTH];

  TouchDirection _inputBuffer[KONAMI_CODE_LENGTH];
  int _inputIndex;
  bool _konamiActive;
  uint32_t _lastInputTime;

  void (*_onEasterEgg)(int eggId);

  bool checkKonamiCode();
  void activateKonami();
  void showDragonDance();
};

extern EasterEggs easterEggs;
