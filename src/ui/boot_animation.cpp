/**
 * @file boot_animation.cpp
 * @brief LeleWatch Cyberpunk Boot Animation
 *
 * Animated circuit patterns with lightning rays converging on logo
 * Inspired by cyberpunk tech aesthetics
 */

#include "boot_animation.h"
#ifndef LCD_WIDTH
#define LCD_WIDTH TFT_WIDTH
#endif
#ifndef LCD_HEIGHT
#define LCD_HEIGHT TFT_HEIGHT
#endif
#include "boot_logger.h"
#include "sounds_manager.h"
#include <debug_log.h>

// Instância global
BootAnimation bootAnimation;

// Cores cyberpunk
#define COLOR_NEON_CYAN lv_color_hex(0x00FFF5)
#define COLOR_NEON_MAGENTA lv_color_hex(0xFF00FF)
#define COLOR_NEON_GREEN lv_color_hex(0x00FF41)
#define COLOR_DARK_BG lv_color_hex(0x0D0D0D)
#define COLOR_CIRCUIT lv_color_hex(0x003B3B)
#define COLOR_GRAY lv_color_hex(0x666666)

// Configuração dos raios de circuito
#define MAX_CIRCUIT_RAYS 12
#define RAY_SEGMENTS 8

// Estrutura para raios animados
struct CircuitRay {
    int16_t startX, startY;
    int16_t endX, endY;
    int16_t currentSegment;
    uint32_t color;
    bool active;
    int16_t segments[RAY_SEGMENTS][4]; // x1,y1,x2,y2 para cada segmento
};

static CircuitRay rays[MAX_CIRCUIT_RAYS];
static lv_obj_t *canvasCircuit = nullptr;
static lv_color_t *canvasBuf = nullptr;
static lv_obj_t *logLabels[MAX_BOOT_LOGS];
static lv_obj_t *logContainer = nullptr;
static int lastLogCount = 0;

// Funções auxiliares para desenho
static void generateRayPath(CircuitRay &ray, int centerX, int centerY);
static void drawCircuitRays(lv_obj_t *canvas);
static void updateRayAnimation();

BootAnimation::BootAnimation()
    : _screen(nullptr), _logoLabel(nullptr), _progressBar(nullptr), _statusLabel(nullptr),
      _versionLabel(nullptr), _scanLine(nullptr), _state(BOOT_ANIM_LOGO), _startTime(0), _lastFrame(0),
      _frameIndex(0), _progress(0), _complete(false), _onComplete(nullptr), _typingIndex(0),
      _fullLogoText("LeleWatch"), _lastTyping(0) {}

void BootAnimation::start() {
    _startTime = millis();
    _lastFrame = _startTime;
    _state = BOOT_ANIM_LOGO;
    _progress = 0;
    _complete = false;
    _frameIndex = 0;
    lastLogCount = 0;

    // Tocar som de boot
    sounds_manager.playByName("/sounds/system/hello.wav");

    // Limpa logs anteriores
    bootLogger.clear();
    bootLogger.addLog("╔════════════════════════════════════════╗", LOG_COLOR_CYAN, false);
    bootLogger.addLog("║       LeleWatch v2.1 - Iniciando...      ║", LOG_COLOR_CYAN, false);
    bootLogger.addLog("╚════════════════════════════════════════╝", LOG_COLOR_CYAN, false);

    // Cria tela de boot
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, COLOR_DARK_BG, 0);
    lv_scr_load(_screen);

    // === CANVAS PARA CIRCUITOS ANIMADOS ===
    static lv_color_t cbuf[LCD_WIDTH * LCD_HEIGHT / 4]; // Buffer menor para performance
    canvasBuf = cbuf;
    canvasCircuit = lv_canvas_create(_screen);
    lv_canvas_set_buffer(canvasCircuit, canvasBuf, LCD_WIDTH, LCD_HEIGHT / 4, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvasCircuit, LV_ALIGN_TOP_MID, 0, 0);
    lv_canvas_fill_bg(canvasCircuit, COLOR_DARK_BG, LV_OPA_COVER);

    // Inicializa raios de circuito
    int centerX = LCD_WIDTH / 2;
    int centerY = 80;
    for (int i = 0; i < MAX_CIRCUIT_RAYS; i++) {
        rays[i].active = true;
        rays[i].currentSegment = 0;
        rays[i].color = (i % 2 == 0) ? lv_color_to32(COLOR_NEON_CYAN) : lv_color_to32(COLOR_NEON_MAGENTA);

        // Pontos de origem nos cantos/bordas
        float angle = (i * 360.0f / MAX_CIRCUIT_RAYS) * 3.14159f / 180.0f;
        rays[i].startX = centerX + (int)(cos(angle) * 180);
        rays[i].startY = centerY + (int)(sin(angle) * 100);
        rays[i].endX = centerX;
        rays[i].endY = centerY;

        generateRayPath(rays[i], centerX, centerY);
    }

    // === LOGO CENTRAL ===
    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "");
    lv_obj_set_style_text_color(_logoLabel, COLOR_NEON_CYAN, 0);
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_TOP_MID, 0, 60);

    // Glow intenso no logo
    lv_obj_set_style_shadow_width(_logoLabel, 25, 0);
    lv_obj_set_style_shadow_color(_logoLabel, COLOR_NEON_CYAN, 0);
    lv_obj_set_style_shadow_opa(_logoLabel, LV_OPA_80, 0);
    lv_obj_set_style_shadow_spread(_logoLabel, 5, 0);

    _typingIndex = 0;
    _fullLogoText = "LeleWatch";
    _lastTyping = millis();

    // === ÁREA DE LOGS ===
    logContainer = lv_obj_create(_screen);
    lv_obj_set_size(logContainer, LCD_WIDTH - 20, 180);
    lv_obj_align(logContainer, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_bg_color(logContainer, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_bg_opa(logContainer, LV_OPA_80, 0);
    lv_obj_set_style_border_width(logContainer, 1, 0);
    lv_obj_set_style_border_color(logContainer, COLOR_NEON_CYAN, 0);
    lv_obj_set_style_radius(logContainer, 8, 0);
    lv_obj_set_style_pad_all(logContainer, 8, 0);
    lv_obj_set_flex_flow(logContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(logContainer, 2, 0);
    lv_obj_clear_flag(logContainer, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < MAX_BOOT_LOGS; i++) {
        logLabels[i] = lv_label_create(logContainer);
        lv_label_set_text(logLabels[i], "");
        lv_obj_set_style_text_font(logLabels[i], &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_color(logLabels[i], COLOR_NEON_GREEN, 0);
    }

    // === BARRA DE PROGRESSO ===
    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, LCD_WIDTH - 40, 16);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_bar_set_value(_progressBar, 0, LV_ANIM_OFF);

    lv_obj_set_style_bg_color(_progressBar, COLOR_CIRCUIT, LV_PART_MAIN);
    lv_obj_set_style_bg_color(_progressBar, COLOR_NEON_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(_progressBar, 8, 0);
    lv_obj_set_style_radius(_progressBar, 8, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(_progressBar, 2, 0);
    lv_obj_set_style_border_color(_progressBar, COLOR_NEON_CYAN, 0);
    lv_obj_set_style_shadow_width(_progressBar, 15, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_color(_progressBar, COLOR_NEON_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_opa(_progressBar, LV_OPA_70, LV_PART_INDICATOR);

    // Label de porcentagem
    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "0%");
    lv_obj_set_style_text_color(_statusLabel, COLOR_NEON_MAGENTA, 0);
    lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -38);

    // Versão
    _versionLabel = lv_label_create(_screen);
    lv_label_set_text(_versionLabel, "ESP32-S3 | 368x448 | LeleWatch v2.1");
    lv_obj_set_style_text_color(_versionLabel, COLOR_GRAY, 0);
    lv_obj_set_style_text_font(_versionLabel, &lv_font_montserrat_10, 0);
    lv_obj_align(_versionLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

    // === SCANLINE EFFECT ===
    _scanLine = lv_obj_create(_screen);
    lv_obj_set_size(_scanLine, LCD_WIDTH, 2);
    lv_obj_set_pos(_scanLine, 0, 0);
    lv_obj_set_style_bg_color(_scanLine, COLOR_NEON_MAGENTA, 0);
    lv_obj_set_style_bg_opa(_scanLine, LV_OPA_30, 0);
    lv_obj_set_style_border_width(_scanLine, 0, 0);

    LOG_INFO("BOOT: LeleWatch boot animation started");
}

bool BootAnimation::update() {
    if (_complete) return true;

    unsigned long now = millis();
    unsigned long elapsed = now - _startTime;

    if (now - _lastFrame < 30) return false;
    _lastFrame = now;

    // === ANIMA RAIOS DE CIRCUITO ===
    updateRayAnimation();
    if (canvasCircuit) {
        lv_canvas_fill_bg(canvasCircuit, COLOR_DARK_BG, LV_OPA_COVER);
        drawCircuitRays(canvasCircuit);
    }

    // === TYPING EFFECT NO LOGO ===
    if (_typingIndex < (int)strlen(_fullLogoText)) {
        if (now - _lastTyping > 100) {
            _typingIndex++;
            char partial[32];
            strncpy(partial, _fullLogoText, _typingIndex);
            partial[_typingIndex] = '\0';

            if ((now / 150) % 2 == 0) { strcat(partial, "_"); }

            lv_label_set_text(_logoLabel, partial);
            _lastTyping = now;
        }
    } else {
        lv_label_set_text(_logoLabel, _fullLogoText);

        // Glow pulsante com alternância cyan/magenta
        int glowPhase = (int)(elapsed / 500) % 2;
        int glow = 15 + (int)((sin(elapsed * 0.008) + 1) * 10);
        lv_obj_set_style_shadow_width(_logoLabel, glow, 0);

        if (glowPhase == 0) {
            lv_obj_set_style_shadow_color(_logoLabel, COLOR_NEON_CYAN, 0);
            lv_obj_set_style_text_color(_logoLabel, COLOR_NEON_CYAN, 0);
        } else {
            lv_obj_set_style_shadow_color(_logoLabel, COLOR_NEON_MAGENTA, 0);
            lv_obj_set_style_text_color(_logoLabel, COLOR_NEON_MAGENTA, 0);
        }
    }

    // === ATUALIZA LOGS NA TELA ===
    const auto &logs = bootLogger.getLogs();
    int logCount = logs.size();

    if (logCount != lastLogCount) {
        for (int i = 0; i < MAX_BOOT_LOGS; i++) {
            if (i < logCount) {
                lv_label_set_text(logLabels[i], logs[i].text);
                lv_obj_set_style_text_color(logLabels[i], lv_color_hex(logs[i].color), 0);
            } else {
                lv_label_set_text(logLabels[i], "");
            }
        }
        lastLogCount = logCount;
    }

    // === ATUALIZA PROGRESSO ===
    _progress = bootLogger.getProgress();
    lv_bar_set_value(_progressBar, _progress, LV_ANIM_ON);

    char progressText[16];
    snprintf(progressText, sizeof(progressText), "%d%%", _progress);
    lv_label_set_text(_statusLabel, progressText);

    // === SCANLINE ===
    if (_scanLine) {
        int scanY = (int)(elapsed / 8) % LCD_HEIGHT;
        lv_obj_set_pos(_scanLine, 0, scanY);
    }

    // Check se completou
    if (elapsed >= BOOT_ANIM_DURATION_MS && _progress >= 100) {
        _complete = true;
        LOG_INFO("BOOT: Animation complete");
        if (_onComplete) _onComplete();
    }

    return _complete;
}

// Gera caminho em zigue-zague para simular circuito
static void generateRayPath(CircuitRay &ray, int centerX, int centerY) {
    int x = ray.startX;
    int y = ray.startY;
    int dx = (centerX - x) / RAY_SEGMENTS;
    int dy = (centerY - y) / RAY_SEGMENTS;

    for (int i = 0; i < RAY_SEGMENTS; i++) {
        ray.segments[i][0] = x;
        ray.segments[i][1] = y;

        // Adiciona variação para parecer circuito (ângulos retos)
        if (i % 2 == 0) {
            x += dx + (rand() % 20 - 10);
            // Movimento horizontal primeiro
        } else {
            y += dy + (rand() % 20 - 10);
            // Depois vertical
        }

        // Garante que termina no centro
        if (i == RAY_SEGMENTS - 1) {
            x = centerX;
            y = centerY;
        }

        ray.segments[i][2] = x;
        ray.segments[i][3] = y;
    }
}

// Atualiza animação dos raios
static void updateRayAnimation() {
    for (int i = 0; i < MAX_CIRCUIT_RAYS; i++) {
        if (rays[i].active) {
            rays[i].currentSegment++;
            if (rays[i].currentSegment >= RAY_SEGMENTS) {
                rays[i].currentSegment = 0; // Loop
            }
        }
    }
}

// Desenha os raios no canvas
static void drawCircuitRays(lv_obj_t *canvas) {
    lv_draw_line_dsc_t lineDsc;
    lv_draw_line_dsc_init(&lineDsc);
    lineDsc.width = 2;
    lineDsc.opa = LV_OPA_COVER;

    for (int i = 0; i < MAX_CIRCUIT_RAYS; i++) {
        if (!rays[i].active) continue;

        // Cor alternada cyan/magenta
        lineDsc.color = (i % 2 == 0) ? COLOR_NEON_CYAN : COLOR_NEON_MAGENTA;

        // Desenha segmentos até o atual
        for (int s = 0; s <= rays[i].currentSegment && s < RAY_SEGMENTS; s++) {
            lv_point_t pts[2];
            pts[0].x = rays[i].segments[s][0];
            pts[0].y = rays[i].segments[s][1];
            pts[1].x = rays[i].segments[s][2];
            pts[1].y = rays[i].segments[s][3];

            // Segmento ativo brilha mais
            if (s == rays[i].currentSegment) {
                lineDsc.width = 3;
                lineDsc.opa = LV_OPA_COVER;
            } else {
                lineDsc.width = 1;
                lineDsc.opa = LV_OPA_50;
            }

            lv_canvas_draw_line(canvas, pts, 2, &lineDsc);
        }

        // Desenha "nó" no ponto atual
        if (rays[i].currentSegment < RAY_SEGMENTS) {
            lv_draw_rect_dsc_t rectDsc;
            lv_draw_rect_dsc_init(&rectDsc);
            rectDsc.bg_color = lineDsc.color;
            rectDsc.bg_opa = LV_OPA_COVER;
            rectDsc.radius = 3;

            int nx = rays[i].segments[rays[i].currentSegment][2];
            int ny = rays[i].segments[rays[i].currentSegment][3];
            lv_canvas_draw_rect(canvas, nx - 3, ny - 3, 6, 6, &rectDsc);
        }
    }
}

// Métodos vazios para compatibilidade
void BootAnimation::drawLogo() {}
void BootAnimation::drawMatrixRain() {}
void BootAnimation::drawScanLines() {}
void BootAnimation::drawLoadingBar(int progress) {}
void BootAnimation::drawStatusText(const char *text) {}
void BootAnimation::drawGlitchEffect() {}
