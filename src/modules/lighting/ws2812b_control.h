#ifndef __WS2812B_CONTROL_H__
#define __WS2812B_CONTROL_H__

#include <Arduino.h>

// Configuração dos LEDs WS2812B
#define WS2812B_PIN         16      // GPIO para dados
#define WS2812B_NUM_LEDS    4       // Número de LEDs
#define WS2812B_BRIGHTNESS  50      // Brilho padrão (0-255)

// Estrutura de cor RGB
struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Cores predefinidas
extern const RGBColor COLOR_RED;
extern const RGBColor COLOR_GREEN;
extern const RGBColor COLOR_BLUE;
extern const RGBColor COLOR_YELLOW;
extern const RGBColor COLOR_CYAN;
extern const RGBColor COLOR_MAGENTA;
extern const RGBColor COLOR_WHITE;
extern const RGBColor COLOR_ORANGE;
extern const RGBColor COLOR_PURPLE;
extern const RGBColor COLOR_OFF;

// Efeitos disponíveis
enum LightingEffect {
    EFFECT_STATIC = 0,
    EFFECT_BLINK,
    EFFECT_FADE,
    EFFECT_RAINBOW,
    EFFECT_BREATHE,
    EFFECT_SCANNER,
    EFFECT_RANDOM,
    EFFECT_CHASE,
    // Novos efeitos avançados
    EFFECT_INDEPENDENT_PULSE,
    EFFECT_COLOR_WAVE,
    EFFECT_PING_PONG,
    EFFECT_FIRE,
    EFFECT_ICE,
    EFFECT_POLICE,
    EFFECT_RAINBOW_SPIN,
    EFFECT_HEARTBEAT,
    EFFECT_COUNT
};

// Nomes dos efeitos em português
extern const char* EFFECT_NAMES[];

// Funções de controle
void initWS2812B();
void updateWS2812B();  // Chamado no loop para efeitos animados

// Controle individual
void setLedColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void setLedColor(uint8_t led, RGBColor color);

// Controle global
void setAllLeds(uint8_t r, uint8_t g, uint8_t b);
void setAllLeds(RGBColor color);
void setLedBrightnessLevel(uint8_t brightness);
uint8_t getLedBrightnessLevel();

// Liga/Desliga
void turnOnLeds();
void turnOffLeds();
bool areLedsOn();

// Efeitos
void setEffect(LightingEffect effect);
LightingEffect getCurrentEffect();
void setEffectSpeed(uint8_t speed);  // 1-10

// Notificações
void notifySuccess();   // Verde piscando
void notifyError();     // Vermelho piscando
void notifyWarning();   // Amarelo piscando
void notifyCapture();   // Azul pulsando
void notifyWiFi(bool connected);  // Ciano se conectado

// Configurações persistentes
void saveLightingConfig();
void loadLightingConfig();

// Efeitos avançados
void effectIndependentPulse();
void effectColorWave();
void effectPingPong();
void effectFire();
void effectIce();
void effectPolice();
void effectRainbowSpin();
void effectHeartbeat();

// Controle individual de cores por LED
void setIndividualLedColor(uint8_t led, RGBColor color);
RGBColor getIndividualLedColor(uint8_t led);

#endif // __WS2812B_CONTROL_H__
