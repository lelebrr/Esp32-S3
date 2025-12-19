/**
 * @file ui_menu_grid.cpp
 * @brief Implementação do menu principal em grid
 */

#include "ui_menu_grid.h"
#include "../ui_main.h"
#include "../ui_themes.h"
#include <debug_log.h>

MenuGrid menuGrid;

// Itens do menu principal
static MenuItem mainMenuItems[] = {
    {"📡", "Scan",     menu_scan_click,     true},
    {"📶", "Redes",    menu_networks_click, true},
    {"⚔️",  "Ataques",  menu_attacks_click,  true},
    {"📊", "Stats",    menu_stats_click,    true},
    {"📁", "Arquivos", menu_files_click,    true},
    {"🎯", "BLE",      menu_ble_click,      true},
    {"⚙️",  "Config",   menu_settings_click, true},
    {"ℹ️",  "Sobre",    menu_about_click,    true}
};

MenuGrid::MenuGrid() : _container(nullptr), _grid(nullptr), _visible(false) {
    memset(_buttons, 0, sizeof(_buttons));
    memset(_labels, 0, sizeof(_labels));
}

void MenuGrid::create(lv_obj_t *parent) {
    // Container principal
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, lv_pct(100), lv_pct(60));
    lv_obj_align(_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_container, lv_color_hex(0x0f0f23), 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_90, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 16, 0);
    lv_obj_set_style_pad_all(_container, 10, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Grid layout - 2 linhas x 4 colunas
    static lv_coord_t col_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
    };
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    _grid = lv_obj_create(_container);
    lv_obj_set_size(_grid, lv_pct(100), lv_pct(100));
    lv_obj_center(_grid);
    lv_obj_set_style_bg_opa(_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_grid, 0, 0);
    lv_obj_set_style_pad_all(_grid, 5, 0);
    lv_obj_set_layout(_grid, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(_grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(_grid, row_dsc, 0);
    lv_obj_set_style_pad_column(_grid, 8, 0);
    lv_obj_set_style_pad_row(_grid, 8, 0);
    lv_obj_clear_flag(_grid, LV_OBJ_FLAG_SCROLLABLE);

    // Cria os 8 botões
    for (int i = 0; i < 8; i++) {
        int row = i / 4;
        int col = i % 4;
        createButton(i, &mainMenuItems[i], row, col);
    }

    _visible = true;
}

void MenuGrid::createButton(int index, MenuItem *item, int row, int col) {
    // Botão
    lv_obj_t *btn = lv_btn_create(_grid);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1a1a3e), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn, 12, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x3a3a6e), 0);

    // Container interno para centralizar conteúdo
    lv_obj_t *content = lv_obj_create(btn);
    lv_obj_set_size(content, lv_pct(100), lv_pct(100));
    lv_obj_center(content);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Ícone (emoji grande)
    lv_obj_t *icon = lv_label_create(content);
    lv_label_set_text(icon, item->icon);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);

    // Label
    lv_obj_t *label = lv_label_create(content);
    lv_label_set_text(label, item->label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xcccccc), 0);

    // Callback
    lv_obj_add_event_cb(btn, onButtonClick, LV_EVENT_CLICKED, (void *)(intptr_t)index);

    // Desabilitar se necessário
    if (!item->enabled) {
        lv_obj_add_state(btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0a0a1a), LV_STATE_DISABLED);
    }

    _buttons[index] = btn;
    _labels[index] = label;
}

void MenuGrid::onButtonClick(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    if (index >= 0 && index < 8 && mainMenuItems[index].onClick) { mainMenuItems[index].onClick(); }
}

void MenuGrid::show() {
    if (_container) lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = true;
}

void MenuGrid::hide() {
    if (_container) lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = false;
}

void MenuGrid::update() {
    // Pode atualizar badges/notificações nos botões
}

// ========== Implementação dos callbacks de menu ==========

void menu_scan_click() {
    LOG_UI("Scan clicked");
    // TODO: Iniciar scan WiFi
    extern void ui_start_wifi_scan();
    ui_start_wifi_scan();
}

void menu_networks_click() {
    LOG_UI("Networks clicked");
    ui_set_screen(UI_SCREEN_WIFI);
}

void menu_attacks_click() {
    LOG_UI("Attacks clicked");
    ui_set_screen(UI_SCREEN_ATTACKS);
}

void menu_stats_click() {
    LOG_UI("Stats clicked");
    ui_set_screen(UI_SCREEN_STATS);
}

void menu_files_click() {
    LOG_UI("Files clicked");
    ui_set_screen(UI_SCREEN_FILES);
}

void menu_ble_click() {
    LOG_UI("BLE clicked");
    ui_set_screen(UI_SCREEN_BLE);
}

void menu_settings_click() {
    LOG_UI("Settings clicked");
    ui_set_screen(UI_SCREEN_SETTINGS);
}

void menu_about_click() {
    LOG_UI("About clicked");
    ui_set_screen(UI_SCREEN_ABOUT);
}
