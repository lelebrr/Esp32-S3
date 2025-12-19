/*
 * ⚙️ UI SETTINGS
 * 
 * O painel de controle da nave.
 * Configura tudo: WiFi, Som, Ataques e a Personalidade do Dragão.
 * 
 * Submenus:
 * 1. Display & Som
 * 2. Rede & Wireless
 * 3. Ataques Automáticos
 * 4. Sistema & Energia
 * 5. Personalidade do Dragão
 */

#include "ui_settings.h"
#include "globals.h"
#include "../hardware/audio_driver.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_settings_ai.h"
#include "ui_settings_attack.h"
#include "ui_settings_display.h"
// #include "ui_settings_dragon.h" - Removed
#include "ui_settings_network.h"
#include "ui_settings_system.h"
#include "ui_themes.h"

// Objetos LVGL
static lv_obj_t *scr_settings = nullptr;
static bool settings_screen_active = false;

// ============================================================
// CALLBACKS DOS BOTÕES DE CATEGORIA
// ============================================================
static void on_display_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_display_show();
}

static void on_network_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_network_show();
}

static void on_attack_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_attack_show();
}

static void on_system_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_system_show();
}

/*
static void on_dragon_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_dragon_show();
}
*/

static void on_ai_click(lv_event_t *e) {
  audioDriver.playSound(SOUND_CLICK);
  settings_screen_active = false;
  ui_settings_ai_show();
}

static void on_back_click(lv_event_t *e) {
  settings_screen_active = false;
  ui_main_show();
}

// ============================================================
// CRIAÇÃO DE BOTÃO DE CATEGORIA
// ============================================================
// ============================================================
// CRIAÇÃO DE BOTÃO DE CATEGORIA (GRID STYLE)
// ============================================================
static void create_grid_button(lv_obj_t *parent, const char *emoji,
                               const char *title, lv_event_cb_t cb, int col,
                               int row) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 165, 120); // Metade da largura (mais ou menos)
  lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1,
                       LV_GRID_ALIGN_STRETCH, row, 1);

  ui_apply_glass_effect(btn);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, THEME_PRIMARY, 0);
  lv_obj_set_style_radius(btn, 16, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  // Container do Conteúdo
  lv_obj_t *cont = lv_obj_create(btn);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont, 0, 0);
  lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(cont, 5, 0);

  // Emoji/Ícone Gigante
  lv_obj_t *ico = lv_label_create(cont);
  lv_label_set_text(ico, emoji);
  lv_obj_set_style_text_font(ico, &lv_font_montserrat_48, 0); // Ícone grande
  lv_obj_set_style_pad_bottom(ico, 10, 0);

  // Title
  lv_obj_t *lbl = lv_label_create(cont);
  lv_label_set_text(lbl, title);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(lbl, THEME_TEXT, 0);
  lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
lv_obj_t *ui_settings_create() {
  scr_settings = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_settings, THEME_BG, 0);

  // Cabeçalho
  lv_obj_t *header = lv_obj_create(scr_settings);
  lv_obj_set_size(header, LCD_WIDTH, 50);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header, THEME_PANEL, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  // Botão voltar (Dica 7 Padronizada)
  ui_create_back_btn(scr_settings, on_back_click);

  // Título
  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, LV_SYMBOL_SETTINGS " Configurações");
  lv_obj_set_style_text_color(title, THEME_ACCENT, 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 10, 0);

  // Container scrollável para as categorias
  lv_obj_t *content = lv_obj_create(scr_settings);
  lv_obj_set_size(content, LCD_WIDTH, LCD_HEIGHT - 50);
  lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(content, 0, 0);
  lv_obj_set_scrollbar_mode(content, LV_SCROLLBAR_MODE_OFF);

  // Grid Layout (Organização)
  static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};

  lv_obj_set_layout(content, LV_LAYOUT_GRID);
  lv_obj_set_style_grid_column_dsc_array(content, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(content, row_dsc, 0);
  lv_obj_set_style_pad_all(content, 10, 0);
  lv_obj_set_style_pad_gap(content, 10, 0);

  // === BOTÕES DE CATEGORIA (GRID) ===
  create_grid_button(content, "🖥️", "Display", on_display_click, 0, 0);
  create_grid_button(content, "📡", "Wireless", on_network_click, 1, 0);
  create_grid_button(content, "⚔️", "Ataques", on_attack_click, 0, 1);
  create_grid_button(content, "🔋", "Sistema", on_system_click, 1, 1);
  // create_grid_button(content, "🐉", "Dragão", on_dragon_click, 0, 2); // Removed
  create_grid_button(content, "🧠", "AI Voice", on_ai_click, 1, 2);

  return scr_settings;
}

void ui_settings_show() {
  if (!scr_settings) {
    ui_settings_create();
  }
  settings_screen_active = true;
  ui_switch_screen(scr_settings);
}

void ui_settings_update() {
  // Atualização periódica se necessário
}

bool ui_settings_is_active() {
  return settings_screen_active || ui_settings_display_is_active() ||
         ui_settings_network_is_active() || ui_settings_attack_is_active() ||
         ui_settings_system_is_active() || 
         ui_settings_ai_is_active();
}

// ============================================================
// QUICK SETTINGS (Dica 6)
// ============================================================
static lv_obj_t *panel_qs = nullptr;

static void qs_close_cb(lv_event_t *e) {
  if (panel_qs) {
    lv_obj_del(panel_qs);
    panel_qs = nullptr;
  }
}

void ui_quick_settings_show() {
  if (panel_qs)
    return; // Já tá aberto

  // Overlay to close on click outside
  lv_obj_t *overlay = lv_obj_create(lv_layer_top());
  panel_qs = overlay; // Manage lifecycle together
  lv_obj_set_size(overlay, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(overlay, 150, 0);
  lv_obj_add_event_cb(overlay, qs_close_cb, LV_EVENT_CLICKED, nullptr);

  // Painel principal deslizando do topo
  lv_obj_t *panel = lv_obj_create(overlay);
  lv_obj_set_size(panel, LCD_WIDTH - 20, 300);
  lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 10);
  ui_apply_glass_effect(panel);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  // Title
  lv_obj_t *title = lv_label_create(panel);
  lv_label_set_text(title, "Quick Settings");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(title, THEME_ACCENT, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

  // Grid de controles (Dica 43)
  // Linha 1: Slider de Brilho
  lv_obj_t *lbl_brt = lv_label_create(panel);
  lv_label_set_text(lbl_brt, LV_SYMBOL_EYE_OPEN " Brilho");
  lv_obj_align(lbl_brt, LV_ALIGN_TOP_LEFT, 0, 30);

  lv_obj_t *slider_brt = lv_slider_create(panel);
  lv_obj_set_width(slider_brt, 200);
  lv_obj_align(slider_brt, LV_ALIGN_TOP_RIGHT, 0, 35);
  lv_slider_set_value(slider_brt, 100, LV_ANIM_OFF);

  // Linha 2: Volume
  lv_obj_t *lbl_vol = lv_label_create(panel);
  lv_label_set_text(lbl_vol, LV_SYMBOL_VOLUME_MAX " Som");
  lv_obj_align(lbl_vol, LV_ALIGN_TOP_LEFT, 0, 70);

  lv_obj_t *slider_vol = lv_slider_create(panel);
  lv_obj_set_width(slider_vol, 200);
  lv_obj_align(slider_vol, LV_ALIGN_TOP_RIGHT, 0, 75);
  lv_slider_set_value(slider_vol, 80, LV_ANIM_OFF);

  // Linha 3: Botões (Avião, Sleep)
  lv_obj_t *btn_plane = lv_btn_create(panel);
  lv_obj_set_size(btn_plane, 100, 40);
  lv_obj_align(btn_plane, LV_ALIGN_BOTTOM_LEFT, 0, -50);
  lv_obj_t *lbl_p = lv_label_create(btn_plane);
  lv_label_set_text(lbl_p, "Avião");
  lv_obj_center(lbl_p);

  lv_obj_t *btn_sleep = lv_btn_create(panel);
  lv_obj_set_size(btn_sleep, 100, 40);
  lv_obj_align(btn_sleep, LV_ALIGN_BOTTOM_RIGHT, 0, -50);
  lv_obj_t *lbl_s = lv_label_create(btn_sleep);
  lv_label_set_text(lbl_s, "Sleep"); // Light Sleep 30s (Rapidão)
  lv_obj_center(lbl_s);

  // Slider de Potência TX (complexo, vai ser label + slider embaixo)
  lv_obj_t *lbl_tx = lv_label_create(panel);
  lv_label_set_text(lbl_tx, "TX Power");
  lv_obj_align(lbl_tx, LV_ALIGN_LEFT_MID, 0, 20); // Meio

  lv_obj_t *slider_tx = lv_slider_create(panel);
  lv_obj_set_width(slider_tx, 150);
  lv_obj_align(slider_tx, LV_ALIGN_RIGHT_MID, 0, 20);
  lv_slider_set_range(slider_tx, 0, 20); // dBm
  lv_slider_set_value(slider_tx, 20, LV_ANIM_OFF);
}
