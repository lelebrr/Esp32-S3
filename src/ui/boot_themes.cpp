/**
 * @file boot_themes.cpp
 * @brief Implementação dos 10 temas de boot screen
 */

#include "boot_themes.h"
#include "ui_particles.h"
#include <debug_log.h>

BootThemeManager bootThemes;

// Constantes
#define BOOT_DURATION_MS 4000
#define LCD_WIDTH 368
#define LCD_HEIGHT 448

// Mensagens de status reais (PROGMEM - economiza DRAM)
static const char* const BOOT_STATUS[] PROGMEM = {
    "Initializing hardware...",
    "Loading NEURA9 model...",
    "Starting WiFi engine...",
    "Activating BLE stack...",
    "Mounting SD card...",
    "LeleWatch Ready!"
};
#define BOOT_STATUS_COUNT 6

BootThemeManager::BootThemeManager()
    : _currentTheme(BOOT_THEME_CYBER_DRAGON), _screen(nullptr), _logoLabel(nullptr), _subtitleLabel(nullptr),
      _progressBar(nullptr), _statusLabel(nullptr), _centerCircle(nullptr), _dragonLabel(nullptr),
      _startTime(0), _lastFrame(0), _frameCount(0), _progress(0), _complete(false), _initialized(false),
      _onComplete(nullptr) {

    for (int i = 0; i < MAX_BOOT_PARTICLES; i++) {
        _particles[i].active = false;
        _particleObjs[i] = nullptr;
    }
}

void BootThemeManager::begin() {
    loadTheme();
    _initialized = true;
}

void BootThemeManager::loadTheme() {
    _prefs.begin("boot", true);
    _currentTheme = (BootTheme)_prefs.getInt("theme", BOOT_THEME_CYBER_DRAGON);
    if (_currentTheme >= BOOT_THEME_COUNT) { _currentTheme = BOOT_THEME_CYBER_DRAGON; }
    _prefs.end();
}

void BootThemeManager::saveTheme() {
    _prefs.begin("boot", false);
    _prefs.putInt("theme", (int)_currentTheme);
    _prefs.end();
}

void BootThemeManager::setTheme(BootTheme theme, bool save) {
    if (theme < BOOT_THEME_COUNT) {
        _currentTheme = theme;
        if (save) saveTheme();
    }
}

void BootThemeManager::nextTheme() {
    int t = (int)_currentTheme + 1;
    if (t >= BOOT_THEME_COUNT) t = 0;
    _currentTheme = (BootTheme)t;
}

void BootThemeManager::prevTheme() {
    int t = (int)_currentTheme - 1;
    if (t < 0) t = BOOT_THEME_COUNT - 1;
    _currentTheme = (BootTheme)t;
}

void BootThemeManager::createBaseScreen() {
    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, BOOT_COLOR_BLACK, 0);
    lv_scr_load(_screen);
}

void BootThemeManager::startAnimation() {
    _startTime = millis();
    _lastFrame = _startTime;
    _progress = 0;
    _complete = false;
    _frameCount = 0;

    createBaseScreen();

    // Inicia tema específico
    switch (_currentTheme) {
        case BOOT_THEME_CYBER_DRAGON: initCyberDragon(); break;
        case BOOT_THEME_MATRIX_RAIN: initMatrixRain(); break;
        case BOOT_THEME_HOLOGRAPHIC: initHolographic(); break;
        case BOOT_THEME_NEURAL_BOOT: initNeuralBoot(); break;
        case BOOT_THEME_GLITCH_CORE: initGlitchCore(); break;
        case BOOT_THEME_VOID_PORTAL: initVoidPortal(); break;
        case BOOT_THEME_TRON_LEGACY: initTronLegacy(); break;
        case BOOT_THEME_QUANTUM: initQuantum(); break;
        case BOOT_THEME_BLADE_RUNNER: initBladeRunner(); break;
        case BOOT_THEME_AMOLED_NEON: initAmoledNeon(); break;
        default: initCyberDragon(); break;
    }

    LOG_UI("BOOT: Tema %s iniciado", THEME_NAMES[_currentTheme].name);
}

bool BootThemeManager::update() {
    if (_complete) return true;

    unsigned long now = millis();
    if (now - _lastFrame < 33) return false; // ~30 FPS
    _lastFrame = now;
    _frameCount++;

    // Calcula progresso
    unsigned long elapsed = now - _startTime;
    _progress = min(100, (int)(elapsed * 100 / BOOT_DURATION_MS));

    // Atualiza tema específico
    switch (_currentTheme) {
        case BOOT_THEME_CYBER_DRAGON: updateCyberDragon(); break;
        case BOOT_THEME_MATRIX_RAIN: updateMatrixRain(); break;
        case BOOT_THEME_HOLOGRAPHIC: updateHolographic(); break;
        case BOOT_THEME_NEURAL_BOOT: updateNeuralBoot(); break;
        case BOOT_THEME_GLITCH_CORE: updateGlitchCore(); break;
        case BOOT_THEME_VOID_PORTAL: updateVoidPortal(); break;
        case BOOT_THEME_TRON_LEGACY: updateTronLegacy(); break;
        case BOOT_THEME_QUANTUM: updateQuantum(); break;
        case BOOT_THEME_BLADE_RUNNER: updateBladeRunner(); break;
        case BOOT_THEME_AMOLED_NEON: updateAmoledNeon(); break;
        default: break;
    }

    updateProgress();

    if (elapsed >= BOOT_DURATION_MS) {
        _complete = true;
        if (_onComplete) _onComplete();
        return true;
    }

    return false;
}

void BootThemeManager::updateProgress() {
    if (_progressBar) { lv_bar_set_value(_progressBar, _progress, LV_ANIM_ON); }

    if (_statusLabel) {
        int msgIdx = (_progress * BOOT_STATUS_COUNT) / 100;
        if (msgIdx >= BOOT_STATUS_COUNT) msgIdx = BOOT_STATUS_COUNT - 1;
        lv_label_set_text(_statusLabel, BOOT_STATUS[msgIdx]);
    }
}

// ============================================================================
// TEMA 1: CYBER-DRAGON PULSE (Favorito)
// ============================================================================
void BootThemeManager::initCyberDragon() {
    // Círculo pulsante central
    _centerCircle = lv_obj_create(_screen);
    lv_obj_set_size(_centerCircle, 120, 120);
    lv_obj_align(_centerCircle, LV_ALIGN_CENTER, 0, -60);
    lv_obj_set_style_radius(_centerCircle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(_centerCircle, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_centerCircle, 3, 0);
    lv_obj_set_style_border_color(_centerCircle, BOOT_COLOR_CYAN, 0);
    lv_obj_set_style_shadow_width(_centerCircle, 30, 0);
    lv_obj_set_style_shadow_color(_centerCircle, BOOT_COLOR_CYAN, 0);
    lv_obj_clear_flag(_centerCircle, LV_OBJ_FLAG_SCROLLABLE);

    // Dragão ASCII no centro
    _dragonLabel = lv_label_create(_centerCircle);
    lv_label_set_text(_dragonLabel, "(>^.^)>");
    lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_dragonLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_center(_dragonLabel);

    // Logo LeleWatch
    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 50);

    // Subtítulo
    _subtitleLabel = lv_label_create(_screen);
    lv_label_set_text(_subtitleLabel, "NEURA9 AWAKENING...");
    lv_obj_set_style_text_font(_subtitleLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_subtitleLabel, BOOT_COLOR_MAGENTA, 0);
    lv_obj_align(_subtitleLabel, LV_ALIGN_CENTER, 0, 85);

    // Barra de progresso
    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 280, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(_progressBar, 2, 0);

    // Status
    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, BOOT_STATUS[0]);
    lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0x888888), 0);
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -30);

    // Inicializa partículas
    particles.init(_screen);
    particles.setEffect(PARTICLE_FLOAT);
    particles.setContinuous(true, 3);
}

void BootThemeManager::updateCyberDragon() {
    // Pulso no círculo
    float pulse = (sin(_frameCount * 0.1f) + 1) / 2;
    int size = 120 + (int)(pulse * 15);
    lv_obj_set_size(_centerCircle, size, size);
    lv_obj_set_style_shadow_opa(_centerCircle, 100 + (uint8_t)(pulse * 155), 0);

    // Alterna cor do logo (efeito holográfico)
    if (_frameCount % 20 < 3) {
        lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_MAGENTA, 0);
    } else {
        lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_CYAN, 0);
    }

    // Partículas
    particles.update();
    if (_frameCount % 30 == 0) { particles.emit(3, LCD_WIDTH / 2, LCD_HEIGHT / 2); }
}

// ============================================================================
// TEMA 2: MATRIX RAIN
// ============================================================================
void BootThemeManager::initMatrixRain() {
    // Colunas de "chuva" Matrix
    for (int i = 0; i < 6; i++) {
        lv_obj_t *col = lv_label_create(_screen);
        lv_label_set_text(col, "01\n10\n11\n00\n01\n10");
        lv_obj_set_style_text_font(col, &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_color(col, BOOT_COLOR_MATRIX, 0);
        lv_obj_set_pos(col, 30 + i * 60, -100 + random(0, 200));
    }

    // Ovo de dragão
    _dragonLabel = lv_label_create(_screen);
    lv_label_set_text(_dragonLabel, "(@)");
    lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(_dragonLabel, BOOT_COLOR_MATRIX, 0);
    lv_obj_align(_dragonLabel, LV_ALIGN_CENTER, 0, -30);

    // Logo
    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch v2.0");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_MATRIX, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 50);

    // Barra
    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 200, 6);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, lv_color_hex(0x003300), LV_PART_MAIN);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_MATRIX, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_set_style_text_color(_statusLabel, BOOT_COLOR_MATRIX, 0);
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void BootThemeManager::updateMatrixRain() {
    // Atualiza ovo rachando
    if (_progress < 30) {
        lv_label_set_text(_dragonLabel, "(@)");
    } else if (_progress < 60) {
        lv_label_set_text(_dragonLabel, "(%)");
    } else if (_progress < 90) {
        lv_label_set_text(_dragonLabel, "(**)");
    } else {
        lv_label_set_text(_dragonLabel, "(>^.^)>");
    }
}

// ============================================================================
// TEMA 3: HOLOGRAPHIC CORE REACTOR
// ============================================================================
void BootThemeManager::initHolographic() {
    // Anel externo
    lv_obj_t *ring1 = lv_obj_create(_screen);
    lv_obj_set_size(ring1, 150, 150);
    lv_obj_align(ring1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_radius(ring1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(ring1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ring1, 2, 0);
    lv_obj_set_style_border_color(ring1, BOOT_COLOR_BLUE, 0);

    // Anel interno
    _centerCircle = lv_obj_create(_screen);
    lv_obj_set_size(_centerCircle, 100, 100);
    lv_obj_align(_centerCircle, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_radius(_centerCircle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_centerCircle, BOOT_COLOR_PURPLE, 0);
    lv_obj_set_style_bg_opa(_centerCircle, LV_OPA_30, 0);
    lv_obj_set_style_border_width(_centerCircle, 2, 0);
    lv_obj_set_style_border_color(_centerCircle, BOOT_COLOR_PURPLE, 0);
    lv_obj_clear_flag(_centerCircle, LV_OBJ_FLAG_SCROLLABLE);

    // Dragão silhueta
    _dragonLabel = lv_label_create(_centerCircle);
    lv_label_set_text(_dragonLabel, "(^.^)");
    lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_dragonLabel, BOOT_COLOR_WHITE, 0);
    lv_obj_center(_dragonLabel);

    // Logo holográfico
    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_BLUE, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 60);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 200, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_BLUE, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -25);
}

void BootThemeManager::updateHolographic() {
    // Efeito holográfico tremendo
    int offsetX = (random(0, 3) - 1);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, offsetX, 60);

    // Alterna RGB
    if (_frameCount % 10 == 0) {
        lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_PURPLE, 0);
    } else if (_frameCount % 10 == 3) {
        lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_CYAN, 0);
    } else {
        lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_BLUE, 0);
    }
}

// ============================================================================
// TEMA 4: NEURAL BOOT (NEURA9)
// ============================================================================
void BootThemeManager::initNeuralBoot() {
    // Texto NEURA9
    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "NEURA9");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, -60);

    _subtitleLabel = lv_label_create(_screen);
    lv_label_set_text(_subtitleLabel, "AWAKENING...");
    lv_obj_set_style_text_font(_subtitleLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_subtitleLabel, BOOT_COLOR_MAGENTA, 0);
    lv_obj_align(_subtitleLabel, LV_ALIGN_CENTER, 0, -25);

    // Rede neural (linhas simples)
    for (int i = 0; i < 5; i++) {
        lv_obj_t *node = lv_obj_create(_screen);
        lv_obj_set_size(node, 10, 10);
        lv_obj_set_pos(node, 100 + i * 40, 200 + (i % 2) * 30);
        lv_obj_set_style_radius(node, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(node, BOOT_COLOR_CYAN, 0);
        lv_obj_set_style_border_width(node, 0, 0);
    }

    // Barra vertical
    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 8, 150);
    lv_obj_align(_progressBar, LV_ALIGN_RIGHT_MID, -30, 0);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_CYAN, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_set_style_text_color(_statusLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void BootThemeManager::updateNeuralBoot() {
    // Muda texto no fim
    if (_progress > 80) {
        lv_label_set_text(_logoLabel, "LeleWatch");
        lv_label_set_text(_subtitleLabel, "ONLINE");
        lv_obj_set_style_text_color(_subtitleLabel, BOOT_COLOR_CYAN, 0);
    }
}

// ============================================================================
// TEMA 5: GLITCH CORE 2077
// ============================================================================
void BootThemeManager::initGlitchCore() {
    // Scanlines
    for (int i = 0; i < 10; i++) {
        lv_obj_t *line = lv_obj_create(_screen);
        lv_obj_set_size(line, LCD_WIDTH, 1);
        lv_obj_set_pos(line, 0, i * 45);
        lv_obj_set_style_bg_color(line, lv_color_hex(0x111111), 0);
        lv_obj_set_style_border_width(line, 0, 0);
    }

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_MAGENTA, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, -30);

    _dragonLabel = lv_label_create(_screen);
    lv_label_set_text(_dragonLabel, "<%|%>");
    lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_dragonLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_dragonLabel, LV_ALIGN_CENTER, 0, 30);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 250, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_MAGENTA, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -25);
}

void BootThemeManager::updateGlitchCore() {
    // Forte efeito glitch
    if (random(0, 10) < 3) {
        int ox = random(-8, 9);
        int oy = random(-3, 4);
        lv_obj_align(_logoLabel, LV_ALIGN_CENTER, ox, -30 + oy);

        // Cores glitch
        uint32_t colors[] = {0xFF00FF, 0x00FFFF, 0xFF0090, 0xFFFFFF};
        lv_obj_set_style_text_color(_logoLabel, lv_color_hex(colors[random(0, 4)]), 0);
    }

    // Fragmenta dragão
    const char *fragments[] = {"<%|%>", "<#|#>", "<!|!>", "(>^.^)>"};
    lv_label_set_text(_dragonLabel, fragments[(_frameCount / 15) % 4]);
}

// ============================================================================
// TEMAS 6-10 (Simplificados)
// ============================================================================
void BootThemeManager::initVoidPortal() {
    _centerCircle = lv_obj_create(_screen);
    lv_obj_set_size(_centerCircle, 140, 140);
    lv_obj_align(_centerCircle, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_radius(_centerCircle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_centerCircle, BOOT_COLOR_PURPLE, 0);
    lv_obj_set_style_bg_opa(_centerCircle, LV_OPA_50, 0);
    lv_obj_set_style_border_width(_centerCircle, 4, 0);
    lv_obj_set_style_border_color(_centerCircle, BOOT_COLOR_MAGENTA, 0);
    lv_obj_set_style_shadow_width(_centerCircle, 40, 0);
    lv_obj_set_style_shadow_color(_centerCircle, BOOT_COLOR_PURPLE, 0);

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_MAGENTA, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 80);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 200, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_PURPLE, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void BootThemeManager::updateVoidPortal() {
    float rot = _frameCount * 2.0f;
    // Efeito de rotação simulado com tamanho
    float scale = (sin(rot * 0.05f) + 1) / 2;
    int size = 130 + (int)(scale * 20);
    lv_obj_set_size(_centerCircle, size, size);
}

void BootThemeManager::initTronLegacy() {
    // Grid lines (simulação)
    for (int i = 0; i < 5; i++) {
        lv_obj_t *hline = lv_obj_create(_screen);
        lv_obj_set_size(hline, LCD_WIDTH, 1);
        lv_obj_set_pos(hline, 0, 150 + i * 40);
        lv_obj_set_style_bg_color(hline, BOOT_COLOR_CYAN, 0);
        lv_obj_set_style_bg_opa(hline, LV_OPA_30, 0);
        lv_obj_set_style_border_width(hline, 0, 0);
    }

    _dragonLabel = lv_label_create(_screen);
    lv_label_set_text(_dragonLabel, ">>>");
    lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_dragonLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_dragonLabel, LV_ALIGN_CENTER, 0, -50);

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_WHITE, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 20);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 250, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_CYAN, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void BootThemeManager::updateTronLegacy() {
    // Move dragão
    int x = (int)(sin(_frameCount * 0.05f) * 50);
    lv_obj_align(_dragonLabel, LV_ALIGN_CENTER, x, -50);
}

void BootThemeManager::initQuantum() {
    _centerCircle = lv_obj_create(_screen);
    lv_obj_set_size(_centerCircle, 80, 80);
    lv_obj_align(_centerCircle, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_radius(_centerCircle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_centerCircle, BOOT_COLOR_PURPLE, 0);
    lv_obj_set_style_border_width(_centerCircle, 0, 0);

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_WHITE, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 40);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 200, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_PURPLE, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);

    particles.init(_screen);
    particles.setEffect(PARTICLE_ORBIT);
    particles.setContinuous(true, 5);
}

void BootThemeManager::updateQuantum() {
    particles.update();

    // Pulso no núcleo
    float pulse = (sin(_frameCount * 0.15f) + 1) / 2;
    lv_obj_set_style_bg_opa(_centerCircle, 150 + (uint8_t)(pulse * 100), 0);
}

void BootThemeManager::initBladeRunner() {
    // Chuva (gotas verticais)
    for (int i = 0; i < 8; i++) {
        lv_obj_t *drop = lv_obj_create(_screen);
        lv_obj_set_size(drop, 2, 20 + random(0, 30));
        lv_obj_set_pos(drop, 40 + i * 40, random(0, LCD_HEIGHT));
        lv_obj_set_style_bg_color(drop, BOOT_COLOR_CYAN, 0);
        lv_obj_set_style_bg_opa(drop, 100, 0);
        lv_obj_set_style_border_width(drop, 0, 0);
    }

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "LeleWatch");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_ORANGE, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, -20);

    // Reflexo
    lv_obj_t *reflect = lv_label_create(_screen);
    lv_label_set_text(reflect, "LeleWatch");
    lv_obj_set_style_text_font(reflect, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(reflect, BOOT_COLOR_ORANGE, 0);
    lv_obj_set_style_text_opa(reflect, LV_OPA_30, 0);
    lv_obj_align(reflect, LV_ALIGN_CENTER, 0, 50);

    _progressBar = lv_bar_create(_screen);
    lv_obj_set_size(_progressBar, 200, 4);
    lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_range(_progressBar, 0, 100);
    lv_obj_set_style_bg_color(_progressBar, BOOT_COLOR_ORANGE, LV_PART_INDICATOR);

    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, "");
    lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void BootThemeManager::updateBladeRunner() {
    // Gotas são estáticas nesta versão simplificada
}

void BootThemeManager::initAmoledNeon() {
    // Minimalista - só linha diagonal
    lv_obj_t *line = lv_obj_create(_screen);
    lv_obj_set_size(line, 4, 0);
    lv_obj_align(line, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(line, BOOT_COLOR_CYAN, 0);
    lv_obj_set_style_border_width(line, 0, 0);

    _logoLabel = lv_label_create(_screen);
    lv_label_set_text(_logoLabel, "");
    lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_CYAN, 0);
    lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 0);

    _progressBar = nullptr; // Sem barra neste tema
    _statusLabel = nullptr;
}

void BootThemeManager::updateAmoledNeon() {
    // Texto aparece gradualmente
    int chars = (_progress * 9) / 100;
    const char *fullText = "LeleWatch";
    char buf[12] = {0};
    strncpy(buf, fullText, min(chars, 9));
    lv_label_set_text(_logoLabel, buf);

    // Flash no final
    if (_progress >= 95) { lv_obj_set_style_text_color(_logoLabel, BOOT_COLOR_WHITE, 0); }
}
