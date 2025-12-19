/**
 * @file ui_menu_voice.cpp
 * @brief Implementação do submenu de voz
 */

#include "ui_menu_voice.h"
#include "globals.h"
// #include "../voice/voice_assistant.h" // File not found - disabled
#include "ui_home.h"
#include <debug_log.h>

static lv_obj_t *_screen = nullptr;
static lv_obj_t *_statusLabel = nullptr;
static lv_obj_t *_toggleBtn = nullptr;
static bool _initialized = false;

#define VOICE_COLOR_BG lv_color_hex(0x0a0a1a)
#define VOICE_COLOR_PANEL lv_color_hex(0x12122a)
#define VOICE_COLOR_ACCENT lv_color_hex(0x9933ff)
#define VOICE_COLOR_ON lv_color_hex(0x00ff88)
#define VOICE_COLOR_OFF lv_color_hex(0xff3333)
#define VOICE_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
    ui_menu_voice_hide();
    ui_home_show();
}

static void btn_toggle_cb(lv_event_t *e) {
    g_state.voice_enabled = !g_state.voice_enabled;
    // voiceAssistant.setListening(g_state.voice_enabled); // Not defined - disabled

    if (g_state.voice_enabled) {
        lv_obj_set_style_bg_color(_toggleBtn, VOICE_COLOR_ON, 0);
        lv_label_set_text(_statusLabel, "🎤 Assistente ATIVO");
    } else {
        lv_obj_set_style_bg_color(_toggleBtn, VOICE_COLOR_OFF, 0);
        lv_label_set_text(_statusLabel, "🔇 Assistente INATIVO");
    }
}

static void btn_test_cb(lv_event_t *e) { /* voiceAssistant.speak(TTS_HELLO); */ LOG_UI("Test voice - disabled"); }

static void btn_commands_cb(lv_event_t *e) { LOG_UI("Show commands list"); }

static void create_menu_item(lv_obj_t *parent, const char *icon, const char *text, lv_event_cb_t cb, int y) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 50);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(btn, VOICE_COLOR_PANEL, 0);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, VOICE_COLOR_ACCENT, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *iconLbl = lv_label_create(btn);
    lv_label_set_text(iconLbl, icon);
    lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_18, 0);
    lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *textLbl = lv_label_create(btn);
    lv_label_set_text(textLbl, text);
    lv_obj_set_style_text_color(textLbl, VOICE_COLOR_TEXT, 0);
    lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_voice_init() {
    if (_initialized) return;

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, VOICE_COLOR_BG, 0);

    // Header
    lv_obj_t *header = lv_obj_create(_screen);
    lv_obj_set_size(header, lv_pct(100), 50);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, VOICE_COLOR_PANEL, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btnBack = lv_btn_create(header);
    lv_obj_set_size(btnBack, 40, 35);
    lv_obj_align(btnBack, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_opa(btnBack, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btnBack, btn_back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *backArrow = lv_label_create(btnBack);
    lv_label_set_text(backArrow, "←");
    lv_obj_set_style_text_font(backArrow, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(backArrow, VOICE_COLOR_ACCENT, 0);
    lv_obj_center(backArrow);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "🎤 Assistente");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, VOICE_COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Status label
    _statusLabel = lv_label_create(_screen);
    lv_label_set_text(_statusLabel, g_state.voice_enabled ? "🎤 Assistente ATIVO" : "🔇 Assistente INATIVO");
    lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0xffffff), 0);
    lv_obj_align(_statusLabel, LV_ALIGN_TOP_MID, 0, 70);

    // Botão toggle grande
    _toggleBtn = lv_btn_create(_screen);
    lv_obj_set_size(_toggleBtn, lv_pct(80), 60);
    lv_obj_align(_toggleBtn, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_bg_color(_toggleBtn, g_state.voice_enabled ? VOICE_COLOR_ON : VOICE_COLOR_OFF, 0);
    lv_obj_set_style_radius(_toggleBtn, 30, 0);
    lv_obj_add_event_cb(_toggleBtn, btn_toggle_cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *toggleLabel = lv_label_create(_toggleBtn);
    lv_label_set_text(toggleLabel, "ATIVAR / DESATIVAR");
    lv_obj_set_style_text_font(toggleLabel, &lv_font_montserrat_14, 0);
    lv_obj_center(toggleLabel);

    // Info
    lv_obj_t *info = lv_label_create(_screen);
    lv_label_set_text(info, "Wake word: \"Hey Dragon\"");
    lv_obj_set_style_text_color(info, VOICE_COLOR_TEXT, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 175);

    // Outros botões
    create_menu_item(_screen, "🔊", "Testar Voz", btn_test_cb, 210);
    create_menu_item(_screen, "📋", "Lista de Comandos", btn_commands_cb, 270);

    _initialized = true;
}

void ui_menu_voice_show() {
    if (!_initialized) ui_menu_voice_init();
    if (_screen) lv_scr_load(_screen);
}

void ui_menu_voice_hide() {}
