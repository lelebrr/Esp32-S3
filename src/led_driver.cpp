/**
 * @file led_driver.cpp
 * @brief Driver WS2812B RGB LED usando RMT do ESP32-S3
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "led_driver.h"
#include <driver/rmt.h>

// Static member initialization
uint32_t LEDDriver::_pixels[LED_COUNT] = {0};
uint8_t LEDDriver::_brightness = 50;  // 20% default
bool LEDDriver::_initialized = false;

// RMT configuration for WS2812B
#define RMT_TX_CHANNEL  RMT_CHANNEL_0
#define RMT_CLK_DIV     2

// WS2812B timing (in RMT ticks at 40MHz = 25ns per tick)
#define T0H  14  // 0.35us
#define T0L  36  // 0.90us
#define T1H  28  // 0.70us
#define T1L  22  // 0.55us
#define RESET_US 280  // Reset pulse > 280us

static rmt_item32_t _rmt_items[LED_COUNT * 24 + 1];

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void LEDDriver::init() {
    if (_initialized) return;
    
    Serial.println("[LED] Initializing WS2812B driver...");
    
    // Configure RMT channel
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX((gpio_num_t)LED_PIN, RMT_TX_CHANNEL);
    config.clk_div = RMT_CLK_DIV;
    
    esp_err_t err = rmt_config(&config);
    if (err != ESP_OK) {
        Serial.printf("[LED] RMT config failed: %d\n", err);
        return;
    }
    
    err = rmt_driver_install(RMT_TX_CHANNEL, 0, 0);
    if (err != ESP_OK) {
        Serial.printf("[LED] RMT driver install failed: %d\n", err);
        return;
    }
    
    _initialized = true;
    clear();
    
    Serial.printf("[LED] WS2812B ready: %d LEDs on GPIO%d\n", LED_COUNT, LED_PIN);
}

// ============================================================================
// CONTROLE DE PIXELS
// ============================================================================

void LEDDriver::setPixel(uint8_t index, uint32_t color) {
    if (index >= LED_COUNT) return;
    _pixels[index] = color;
}

void LEDDriver::setPixelRGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    // WS2812B usa ordem GRB
    uint32_t color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    setPixel(index, color);
}

void LEDDriver::setAll(uint32_t color) {
    for (int i = 0; i < LED_COUNT; i++) {
        _pixels[i] = color;
    }
}

void LEDDriver::clear() {
    setAll(LED_OFF);
    show();
}

void LEDDriver::setBrightness(uint8_t brightness) {
    _brightness = brightness;
}

uint32_t LEDDriver::applyBrightness(uint32_t color) {
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t r = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    g = (g * _brightness) / 255;
    r = (r * _brightness) / 255;
    b = (b * _brightness) / 255;
    
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

void LEDDriver::show() {
    if (!_initialized) {
        init();
        if (!_initialized) return;
    }
    
    int itemIdx = 0;
    
    for (int led = 0; led < LED_COUNT; led++) {
        uint32_t color = applyBrightness(_pixels[led]);
        
        // Send 24 bits (MSB first)
        for (int bit = 23; bit >= 0; bit--) {
            if (color & (1 << bit)) {
                // Bit 1
                _rmt_items[itemIdx].level0 = 1;
                _rmt_items[itemIdx].duration0 = T1H;
                _rmt_items[itemIdx].level1 = 0;
                _rmt_items[itemIdx].duration1 = T1L;
            } else {
                // Bit 0
                _rmt_items[itemIdx].level0 = 1;
                _rmt_items[itemIdx].duration0 = T0H;
                _rmt_items[itemIdx].level1 = 0;
                _rmt_items[itemIdx].duration1 = T0L;
            }
            itemIdx++;
        }
    }
    
    // Reset pulse
    _rmt_items[itemIdx].level0 = 0;
    _rmt_items[itemIdx].duration0 = RESET_US * 40 / 1000;
    _rmt_items[itemIdx].level1 = 0;
    _rmt_items[itemIdx].duration1 = 0;
    
    // Send data
    rmt_write_items(RMT_TX_CHANNEL, _rmt_items, itemIdx + 1, true);
}

// ============================================================================
// EFEITOS DE ATAQUE
// ============================================================================

void LEDDriver::blinkSuccess() {
    for (int i = 0; i < 3; i++) {
        setAll(LED_GREEN);
        show();
        delay(150);
        clear();
        delay(100);
    }
}

void LEDDriver::blinkError() {
    for (int i = 0; i < 3; i++) {
        setAll(LED_RED);
        show();
        delay(150);
        clear();
        delay(100);
    }
}

void LEDDriver::blinkAttacking() {
    static uint8_t brightness = 0;
    static bool increasing = true;
    
    if (increasing) {
        brightness += 15;
        if (brightness >= 255) {
            brightness = 255;
            increasing = false;
        }
    } else {
        brightness -= 15;
        if (brightness <= 10) {
            brightness = 10;
            increasing = true;
        }
    }
    
    uint32_t color = ((uint32_t)0 << 16) | ((uint32_t)0 << 8) | brightness;
    setAll(color);
    show();
}

void LEDDriver::rainbowCycle() {
    static uint16_t hue = 0;
    
    for (int i = 0; i < LED_COUNT; i++) {
        uint16_t pixelHue = hue + (i * 65536 / LED_COUNT);
        
        // Simple HSV to RGB (hue only, full saturation/value)
        uint8_t region = (pixelHue / 10923) % 6;
        uint8_t remainder = (pixelHue % 10923) * 255 / 10923;
        
        uint8_t r, g, b;
        switch (region) {
            case 0: r = 255; g = remainder; b = 0; break;
            case 1: r = 255 - remainder; g = 255; b = 0; break;
            case 2: r = 0; g = 255; b = remainder; break;
            case 3: r = 0; g = 255 - remainder; b = 255; break;
            case 4: r = remainder; g = 0; b = 255; break;
            default: r = 255; g = 0; b = 255 - remainder; break;
        }
        
        setPixelRGB(i, r, g, b);
    }
    
    show();
    hue += 256;
}

void LEDDriver::scanner(uint32_t color) {
    static int pos = 0;
    static bool forward = true;
    
    clear();
    
    // Create scanner effect with trail
    for (int i = 0; i < LED_COUNT; i++) {
        int distance = abs(i - pos);
        if (distance == 0) {
            setPixel(i, color);
        } else if (distance == 1) {
            setPixel(i, applyBrightness(color) / 2);
        } else if (distance == 2) {
            setPixel(i, applyBrightness(color) / 4);
        }
    }
    
    show();
    
    if (forward) {
        pos++;
        if (pos >= LED_COUNT - 1) forward = false;
    } else {
        pos--;
        if (pos <= 0) forward = true;
    }
}

void LEDDriver::pulseEffect(uint32_t color1, uint32_t color2, uint16_t speed) {
    static uint8_t phase = 0;
    static bool rising = true;
    
    // Interpolate between colors
    uint8_t g1 = (color1 >> 16) & 0xFF, r1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
    uint8_t g2 = (color2 >> 16) & 0xFF, r2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;
    
    uint8_t g = g1 + ((g2 - g1) * phase / 255);
    uint8_t r = r1 + ((r2 - r1) * phase / 255);
    uint8_t b = b1 + ((b2 - b1) * phase / 255);
    
    uint32_t color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    setAll(color);
    show();
    
    if (rising) {
        phase += 10;
        if (phase >= 250) rising = false;
    } else {
        phase -= 10;
        if (phase <= 5) rising = true;
    }
}

void LEDDriver::matrixRain() {
    static uint8_t drops[LED_COUNT] = {0};
    static uint8_t speeds[LED_COUNT] = {0};
    
    // Initialize speeds randomly
    static bool init = false;
    if (!init) {
        for (int i = 0; i < LED_COUNT; i++) {
            speeds[i] = random(5, 20);
        }
        init = true;
    }
    
    // Update each LED
    for (int i = 0; i < LED_COUNT; i++) {
        drops[i] += speeds[i];
        
        // Green with varying intensity
        uint8_t intensity = 255 - drops[i];
        setPixelRGB(i, 0, intensity, intensity / 4);  // Greenish with hint of blue
    }
    
    show();
}
