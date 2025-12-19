#include "ws2812b_control.h"
#include "core/config.h"
#include <Preferences.h>
#include <soc/gpio_struct.h>

// NVS para persistência
static Preferences ledPrefs;
static const char *LED_NVS_NAMESPACE = "led_config";

// Usamos manipulação direta de GPIO para WS2812B
// Alternativa: usar biblioteca FastLED ou Adafruit NeoPixel

// Cores predefinidas
const RGBColor COLOR_RED = {255, 0, 0};
const RGBColor COLOR_GREEN = {0, 255, 0};
const RGBColor COLOR_BLUE = {0, 0, 255};
const RGBColor COLOR_YELLOW = {255, 255, 0};
const RGBColor COLOR_CYAN = {0, 255, 255};
const RGBColor COLOR_MAGENTA = {255, 0, 255};
const RGBColor COLOR_WHITE = {255, 255, 255};
const RGBColor COLOR_ORANGE = {255, 128, 0};
const RGBColor COLOR_PURPLE = {128, 0, 255};
const RGBColor COLOR_OFF = {0, 0, 0};

// Nomes dos efeitos
const char *EFFECT_NAMES[] = {
    "Estatico", "Piscar", "Fade", "Arco-Iris", "Respirar", "Scanner", "Aleatorio", "Perseguir"
};

// Estado interno
static RGBColor ledColors[WS2812B_NUM_LEDS];
static uint8_t currentBrightness = WS2812B_BRIGHTNESS;
static bool ledsEnabled = true;
static LightingEffect currentEffect = EFFECT_STATIC;
static uint8_t effectSpeed = 5;
static unsigned long lastEffectUpdate = 0;
static uint8_t effectStep = 0;

// Buffer para envio
static uint8_t ledBuffer[WS2812B_NUM_LEDS * 3];

// Função para enviar dados ao WS2812B (implementação simplificada)
static void sendWS2812BData() {
    if (!ledsEnabled) {
        // Envia zeros se desligado
        for (int i = 0; i < WS2812B_NUM_LEDS * 3; i++) { ledBuffer[i] = 0; }
    } else {
        // Aplica brilho às cores
        for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
            // WS2812B usa ordem GRB
            ledBuffer[i * 3 + 0] = (ledColors[i].g * currentBrightness) / 255;
            ledBuffer[i * 3 + 1] = (ledColors[i].r * currentBrightness) / 255;
            ledBuffer[i * 3 + 2] = (ledColors[i].b * currentBrightness) / 255;
        }
    }

// Envia dados via RMT ou bit-banging
#ifdef ESP32
// Implementação usando FastLED quando disponível
#if __has_include(<FastLED.h>)
    // FastLED já está configurado externamente
    // Esta função atualiza o buffer interno
#else
    // Fallback: bit-banging básico para WS2812B
    // Para produção, recomenda-se usar FastLED
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    for (int i = 0; i < WS2812B_NUM_LEDS * 3; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            if (ledBuffer[i] & (1 << bit)) {
                GPIO.out_w1ts = (1 << WS2812B_PIN);
                delayMicroseconds(1);
                GPIO.out_w1tc = (1 << WS2812B_PIN);
            } else {
                GPIO.out_w1ts = (1 << WS2812B_PIN);
                GPIO.out_w1tc = (1 << WS2812B_PIN);
                delayMicroseconds(1);
            }
        }
    }
    portEXIT_CRITICAL(&mux);
#endif
#endif
}

void initWS2812B() {
    pinMode(WS2812B_PIN, OUTPUT);
    digitalWrite(WS2812B_PIN, LOW);

    // Inicializa todas as cores como desligadas
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) { ledColors[i] = COLOR_OFF; }

    loadLightingConfig();
    sendWS2812BData();

    Serial.println("WS2812B inicializado");
}

void updateWS2812B() {
    if (!ledsEnabled) return;

    unsigned long now = millis();
    unsigned long interval = 100 - (effectSpeed * 10); // 10-100ms

    if (now - lastEffectUpdate < interval) return;
    lastEffectUpdate = now;

    switch (currentEffect) {
        case EFFECT_STATIC:
            // Não faz nada, cores estáticas
            break;

        case EFFECT_BLINK:
            effectStep = !effectStep;
            if (effectStep) {
                sendWS2812BData();
            } else {
                // Envia zeros temporariamente
                for (int i = 0; i < WS2812B_NUM_LEDS * 3; i++) { ledBuffer[i] = 0; }
            }
            break;

        case EFFECT_FADE: {
            static int fadeDir = 1;
            static int fadeVal = 0;
            fadeVal += fadeDir * 10;
            if (fadeVal >= 255) {
                fadeVal = 255;
                fadeDir = -1;
            }
            if (fadeVal <= 0) {
                fadeVal = 0;
                fadeDir = 1;
            }

            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                ledBuffer[i * 3 + 0] = (ledColors[i].g * fadeVal) / 255;
                ledBuffer[i * 3 + 1] = (ledColors[i].r * fadeVal) / 255;
                ledBuffer[i * 3 + 2] = (ledColors[i].b * fadeVal) / 255;
            }
            break;
        }

        case EFFECT_RAINBOW: {
            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                uint8_t hue = (effectStep + i * 64) % 256;
                // Conversão HSV para RGB simplificada
                uint8_t r, g, b;
                if (hue < 85) {
                    r = hue * 3;
                    g = 255 - hue * 3;
                    b = 0;
                } else if (hue < 170) {
                    hue -= 85;
                    r = 255 - hue * 3;
                    g = 0;
                    b = hue * 3;
                } else {
                    hue -= 170;
                    r = 0;
                    g = hue * 3;
                    b = 255 - hue * 3;
                }
                ledBuffer[i * 3 + 0] = (g * currentBrightness) / 255;
                ledBuffer[i * 3 + 1] = (r * currentBrightness) / 255;
                ledBuffer[i * 3 + 2] = (b * currentBrightness) / 255;
            }
            effectStep += 5;
            break;
        }

        case EFFECT_BREATHE: {
            static int breatheVal = 0;
            static int breatheDir = 5;
            breatheVal += breatheDir;
            if (breatheVal >= 255) {
                breatheVal = 255;
                breatheDir = -5;
            }
            if (breatheVal <= 0) {
                breatheVal = 0;
                breatheDir = 5;
            }

            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                ledBuffer[i * 3 + 0] = (ledColors[i].g * breatheVal) / 255;
                ledBuffer[i * 3 + 1] = (ledColors[i].r * breatheVal) / 255;
                ledBuffer[i * 3 + 2] = (ledColors[i].b * breatheVal) / 255;
            }
            break;
        }

        case EFFECT_SCANNER: {
            // Efeito Knight Rider
            static int scanPos = 0;
            static int scanDir = 1;

            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                if (i == scanPos) {
                    ledBuffer[i * 3 + 0] = (ledColors[i].g * currentBrightness) / 255;
                    ledBuffer[i * 3 + 1] = (ledColors[i].r * currentBrightness) / 255;
                    ledBuffer[i * 3 + 2] = (ledColors[i].b * currentBrightness) / 255;
                } else {
                    ledBuffer[i * 3] = ledBuffer[i * 3 + 1] = ledBuffer[i * 3 + 2] = 0;
                }
            }

            scanPos += scanDir;
            if (scanPos >= WS2812B_NUM_LEDS - 1) scanDir = -1;
            if (scanPos <= 0) scanDir = 1;
            break;
        }

        case EFFECT_RANDOM:
            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                ledBuffer[i * 3 + 0] = random(256) * currentBrightness / 255;
                ledBuffer[i * 3 + 1] = random(256) * currentBrightness / 255;
                ledBuffer[i * 3 + 2] = random(256) * currentBrightness / 255;
            }
            break;

        case EFFECT_CHASE: {
            static uint8_t chasePos = 0;
            for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
                if ((i + chasePos) % 2 == 0) {
                    ledBuffer[i * 3 + 0] = (ledColors[i].g * currentBrightness) / 255;
                    ledBuffer[i * 3 + 1] = (ledColors[i].r * currentBrightness) / 255;
                    ledBuffer[i * 3 + 2] = (ledColors[i].b * currentBrightness) / 255;
                } else {
                    ledBuffer[i * 3] = ledBuffer[i * 3 + 1] = ledBuffer[i * 3 + 2] = 0;
                }
            }
            chasePos++;
            break;
        }

        default: break;
    }
}

void setLedColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
    if (led < WS2812B_NUM_LEDS) {
        ledColors[led].r = r;
        ledColors[led].g = g;
        ledColors[led].b = b;
        sendWS2812BData();
    }
}

void setLedColor(uint8_t led, RGBColor color) { setLedColor(led, color.r, color.g, color.b); }

void setAllLeds(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        ledColors[i].r = r;
        ledColors[i].g = g;
        ledColors[i].b = b;
    }
    sendWS2812BData();
}

void setAllLeds(RGBColor color) { setAllLeds(color.r, color.g, color.b); }

void setLedBrightnessLevel(uint8_t brightness) {
    currentBrightness = brightness;
    sendWS2812BData();
}

uint8_t getLedBrightnessLevel() { return currentBrightness; }

void turnOnLeds() {
    ledsEnabled = true;
    sendWS2812BData();
}

void turnOffLeds() {
    ledsEnabled = false;
    sendWS2812BData();
}

bool areLedsOn() { return ledsEnabled; }

void setEffect(LightingEffect effect) {
    currentEffect = effect;
    effectStep = 0;
}

LightingEffect getCurrentEffect() { return currentEffect; }

void setEffectSpeed(uint8_t speed) {
    if (speed < 1) speed = 1;
    if (speed > 10) speed = 10;
    effectSpeed = speed;
}

// Notificações visuais
void notifySuccess() {
    RGBColor prevColors[WS2812B_NUM_LEDS];
    memcpy(prevColors, ledColors, sizeof(prevColors));

    for (int i = 0; i < 3; i++) {
        setAllLeds(COLOR_GREEN);
        delay(100);
        setAllLeds(COLOR_OFF);
        delay(100);
    }

    memcpy(ledColors, prevColors, sizeof(ledColors));
    sendWS2812BData();
}

void notifyError() {
    RGBColor prevColors[WS2812B_NUM_LEDS];
    memcpy(prevColors, ledColors, sizeof(prevColors));

    for (int i = 0; i < 3; i++) {
        setAllLeds(COLOR_RED);
        delay(100);
        setAllLeds(COLOR_OFF);
        delay(100);
    }

    memcpy(ledColors, prevColors, sizeof(ledColors));
    sendWS2812BData();
}

void notifyWarning() {
    RGBColor prevColors[WS2812B_NUM_LEDS];
    memcpy(prevColors, ledColors, sizeof(prevColors));

    for (int i = 0; i < 3; i++) {
        setAllLeds(COLOR_YELLOW);
        delay(150);
        setAllLeds(COLOR_OFF);
        delay(150);
    }

    memcpy(ledColors, prevColors, sizeof(ledColors));
    sendWS2812BData();
}

void notifyCapture() {
    RGBColor prevColors[WS2812B_NUM_LEDS];
    memcpy(prevColors, ledColors, sizeof(prevColors));

    for (int i = 0; i < 2; i++) {
        for (int b = 0; b < 255; b += 15) {
            setAllLeds(0, 0, b);
            delay(10);
        }
        for (int b = 255; b > 0; b -= 15) {
            setAllLeds(0, 0, b);
            delay(10);
        }
    }

    memcpy(ledColors, prevColors, sizeof(ledColors));
    sendWS2812BData();
}

void notifyWiFi(bool connected) {
    if (connected) {
        setAllLeds(COLOR_CYAN);
    } else {
        setAllLeds(COLOR_OFF);
    }
}

void saveLightingConfig() {
    // Salvar em NVS
    ledPrefs.begin(LED_NVS_NAMESPACE, false);
    ledPrefs.putUChar("brightness", currentBrightness);
    ledPrefs.putUChar("effect", (uint8_t)currentEffect);
    ledPrefs.putUChar("speed", effectSpeed);
    ledPrefs.putBool("enabled", ledsEnabled);
    // Salvar cores individuais
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        String key = "led" + String(i);
        uint32_t color = (ledColors[i].r << 16) | (ledColors[i].g << 8) | ledColors[i].b;
        ledPrefs.putUInt(key.c_str(), color);
    }
    ledPrefs.end();
    Serial.println("[LED] Configuracao salva em NVS");
}

void loadLightingConfig() {
    // Carregar de NVS
    ledPrefs.begin(LED_NVS_NAMESPACE, true);
    if (ledPrefs.isKey("brightness")) {
        currentBrightness = ledPrefs.getUChar("brightness", WS2812B_BRIGHTNESS);
        currentEffect = (LightingEffect)ledPrefs.getUChar("effect", EFFECT_STATIC);
        effectSpeed = ledPrefs.getUChar("speed", 5);
        ledsEnabled = ledPrefs.getBool("enabled", true);
        // Carregar cores individuais
        for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
            String key = "led" + String(i);
            uint32_t color = ledPrefs.getUInt(key.c_str(), 0);
            ledColors[i].r = (color >> 16) & 0xFF;
            ledColors[i].g = (color >> 8) & 0xFF;
            ledColors[i].b = color & 0xFF;
        }
        Serial.println("[LED] Configuracao carregada de NVS");
    } else {
        Serial.println("[LED] Usando configuracao padrao");
    }
    ledPrefs.end();
}
// =====================================================================
// EFEITOS AVANÇADOS - Cada LED pode ter cor/comportamento diferente
// =====================================================================

// Cores individuais para cada LED (configuráveis)
static RGBColor ledIndividualColors[WS2812B_NUM_LEDS] = {
    {255, 0,   0  }, // LED 1 - Vermelho
    {0,   255, 0  }, // LED 2 - Verde
    {0,   0,   255}, // LED 3 - Azul
    {255, 255, 0  }  // LED 4 - Amarelo
};

// =====================================================================
// Efeito: Cores Independentes Pulsando
// Cada LED pulsa em sua própria cor em velocidades diferentes
// =====================================================================
void effectIndependentPulse() {
    static uint8_t phase[4] = {0, 64, 128, 192}; // Fases defasadas
    static uint8_t speed[4] = {3, 5, 4, 6};      // Velocidades diferentes

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        phase[i] += speed[i];
        uint8_t brightness = (sin(phase[i] * 3.14159 / 128.0) + 1) * 127;

        ledBuffer[i * 3 + 0] = (ledIndividualColors[i].g * brightness) / 255;
        ledBuffer[i * 3 + 1] = (ledIndividualColors[i].r * brightness) / 255;
        ledBuffer[i * 3 + 2] = (ledIndividualColors[i].b * brightness) / 255;
    }
}

// =====================================================================
// Efeito: Onda de Cores
// Uma onda de cor viaja através dos 4 LEDs
// =====================================================================
void effectColorWave() {
    static uint8_t wavePos = 0;
    wavePos += 10;

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint8_t hue = wavePos + (i * 64); // 64 = 256/4 LEDs
        uint8_t r, g, b;

        // Conversão HSV para RGB
        if (hue < 85) {
            r = hue * 3;
            g = 255 - hue * 3;
            b = 0;
        } else if (hue < 170) {
            hue -= 85;
            r = 255 - hue * 3;
            g = 0;
            b = hue * 3;
        } else {
            hue -= 170;
            r = 0;
            g = hue * 3;
            b = 255 - hue * 3;
        }

        ledBuffer[i * 3 + 0] = (g * currentBrightness) / 255;
        ledBuffer[i * 3 + 1] = (r * currentBrightness) / 255;
        ledBuffer[i * 3 + 2] = (b * currentBrightness) / 255;
    }
}

// =====================================================================
// Efeito: Ping Pong
// Luz vai e volta entre os LEDs
// =====================================================================
void effectPingPong() {
    static int pos = 0;
    static int dir = 1;
    static uint8_t trail = 0;

    trail++;
    if (trail >= 8) {
        trail = 0;
        pos += dir;
        if (pos >= WS2812B_NUM_LEDS - 1) {
            pos = WS2812B_NUM_LEDS - 1;
            dir = -1;
        }
        if (pos <= 0) {
            pos = 0;
            dir = 1;
        }
    }

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        if (i == pos) {
            // LED principal - branco brilhante
            ledBuffer[i * 3 + 0] = currentBrightness;
            ledBuffer[i * 3 + 1] = currentBrightness;
            ledBuffer[i * 3 + 2] = currentBrightness;
        } else {
            // Rastro - fade gradual
            int dist = abs(i - pos);
            uint8_t fade = 255 / (dist + 1) / 3;
            ledBuffer[i * 3 + 0] = (fade * currentBrightness) / 255;
            ledBuffer[i * 3 + 1] = (fade * currentBrightness) / 255;
            ledBuffer[i * 3 + 2] = (fade * currentBrightness) / 255;
        }
    }
}

// =====================================================================
// Efeito: Fogo
// Simula chamas com cores vermelho/laranja/amarelo
// =====================================================================
void effectFire() {
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint8_t flicker = random(100, 255);
        uint8_t r = flicker;
        uint8_t g = flicker / 3; // Menos verde = mais vermelho
        uint8_t b = 0;

        ledBuffer[i * 3 + 0] = (g * currentBrightness) / 255;
        ledBuffer[i * 3 + 1] = (r * currentBrightness) / 255;
        ledBuffer[i * 3 + 2] = (b * currentBrightness) / 255;
    }
}

// =====================================================================
// Efeito: Gelo
// Tons de azul/ciano pulsando
// =====================================================================
void effectIce() {
    static uint8_t phase = 0;
    phase += 2;

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint8_t sparkle = random(200, 255);
        uint8_t pulse = (sin((phase + i * 30) * 3.14159 / 128.0) + 1) * 64;

        uint8_t r = 0;
        uint8_t g = pulse + 50;
        uint8_t b = sparkle;

        ledBuffer[i * 3 + 0] = (g * currentBrightness) / 255;
        ledBuffer[i * 3 + 1] = (r * currentBrightness) / 255;
        ledBuffer[i * 3 + 2] = (b * currentBrightness) / 255;
    }
}

// =====================================================================
// Efeito: Polícia
// Vermelho e azul alternando
// =====================================================================
void effectPolice() {
    static uint8_t state = 0;
    state++;

    bool flash = (state / 5) % 2; // Alterna a cada 5 ciclos

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        if ((i < 2 && flash) || (i >= 2 && !flash)) {
            // Vermelho
            ledBuffer[i * 3 + 0] = 0;
            ledBuffer[i * 3 + 1] = currentBrightness;
            ledBuffer[i * 3 + 2] = 0;
        } else {
            // Azul
            ledBuffer[i * 3 + 0] = 0;
            ledBuffer[i * 3 + 1] = 0;
            ledBuffer[i * 3 + 2] = currentBrightness;
        }
    }
}

// =====================================================================
// Efeito: Arco-Íris Giratório
// Cada LED mostra uma cor do arco-iris, girando
// =====================================================================
void effectRainbowSpin() {
    static uint8_t offset = 0;
    offset += 3;

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint8_t hue = offset + (i * 64); // Distribui cores igualmente
        uint8_t r, g, b;

        uint8_t region = hue / 43;
        uint8_t remainder = (hue - (region * 43)) * 6;

        switch (region) {
            case 0:
                r = 255;
                g = remainder;
                b = 0;
                break;
            case 1:
                r = 255 - remainder;
                g = 255;
                b = 0;
                break;
            case 2:
                r = 0;
                g = 255;
                b = remainder;
                break;
            case 3:
                r = 0;
                g = 255 - remainder;
                b = 255;
                break;
            case 4:
                r = remainder;
                g = 0;
                b = 255;
                break;
            default:
                r = 255;
                g = 0;
                b = 255 - remainder;
                break;
        }

        ledBuffer[i * 3 + 0] = (g * currentBrightness) / 255;
        ledBuffer[i * 3 + 1] = (r * currentBrightness) / 255;
        ledBuffer[i * 3 + 2] = (b * currentBrightness) / 255;
    }
}

// =====================================================================
// Efeito: Batimento Cardíaco
// Pulso duplo como batimento do coração
// =====================================================================
void effectHeartbeat() {
    static uint8_t phase = 0;
    static uint8_t brightness = 0;
    phase++;

    // Padrão de batimento: dois pulsos rápidos, pausa longa
    if (phase < 10) brightness = phase * 25;
    else if (phase < 20) brightness = 255 - (phase - 10) * 25;
    else if (phase < 30) brightness = (phase - 20) * 25;
    else if (phase < 40) brightness = 255 - (phase - 30) * 25;
    else if (phase < 100) brightness = 0;
    else phase = 0;

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        // Vermelho para coração
        ledBuffer[i * 3 + 0] = 0;
        ledBuffer[i * 3 + 1] = (brightness * currentBrightness) / 255;
        ledBuffer[i * 3 + 2] = 0;
    }
}

// Função para definir cor individual de um LED
void setIndividualLedColor(uint8_t led, RGBColor color) {
    if (led < WS2812B_NUM_LEDS) { ledIndividualColors[led] = color; }
}

// Função para obter cor individual de um LED
RGBColor getIndividualLedColor(uint8_t led) {
    if (led < WS2812B_NUM_LEDS) { return ledIndividualColors[led]; }
    return COLOR_OFF;
}
