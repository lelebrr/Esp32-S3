/**
 * @file led_driver.h
 * @brief Driver WS2812B RGB LED com RMT
 * 
 * Usa periférico RMT do ESP32-S3 para controlar LEDs WS2812B.
 * Suporta até 10 LEDs em cadeia.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <Arduino.h>

#define LED_COUNT       10
#define LED_PIN         48  // Shared with PIEZO on DevKit

// Cores predefinidas (GRB order)
#define LED_OFF         0x000000
#define LED_RED         0x00FF00
#define LED_GREEN       0xFF0000
#define LED_BLUE        0x0000FF
#define LED_WHITE       0xFFFFFF
#define LED_YELLOW      0xFFFF00
#define LED_CYAN        0xFF00FF
#define LED_MAGENTA     0x00FFFF
#define LED_ORANGE      0x80FF00
#define LED_PURPLE      0x0080FF

class LEDDriver {
public:
    /**
     * @brief Inicializa driver RMT para WS2812B
     */
    static void init();
    
    /**
     * @brief Define cor de um LED específico
     * @param index Índice do LED (0-9)
     * @param color Cor em formato GRB 24-bit
     */
    static void setPixel(uint8_t index, uint32_t color);
    
    /**
     * @brief Define cor de um LED em RGB
     * @param index Índice do LED
     * @param r Red (0-255)
     * @param g Green (0-255)
     * @param b Blue (0-255)
     */
    static void setPixelRGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    
    /**
     * @brief Define todos os LEDs para mesma cor
     * @param color Cor GRB
     */
    static void setAll(uint32_t color);
    
    /**
     * @brief Aplica cores aos LEDs (enviar para hardware)
     */
    static void show();
    
    /**
     * @brief Desliga todos os LEDs
     */
    static void clear();
    
    /**
     * @brief Define brilho global (0-255)
     */
    static void setBrightness(uint8_t brightness);
    
    // ============================================
    // Efeitos de ataque
    // ============================================
    
    /**
     * @brief Blink sucesso (verde 3x)
     */
    static void blinkSuccess();
    
    /**
     * @brief Blink erro (vermelho 3x)
     */
    static void blinkError();
    
    /**
     * @brief Blink ataque ativo (azul pulsante)
     */
    static void blinkAttacking();
    
    /**
     * @brief Rainbow loading effect
     */
    static void rainbowCycle();
    
    /**
     * @brief Scanner effect (Larson scanner)
     */
    static void scanner(uint32_t color);

private:
    static uint32_t _pixels[LED_COUNT];
    static uint8_t _brightness;
    static bool _initialized;
    
    static uint32_t applyBrightness(uint32_t color);
};

#endif // LED_DRIVER_H
