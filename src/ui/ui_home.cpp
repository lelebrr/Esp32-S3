/**
 * @file ui_home.cpp
 * @brief Tela Home - Menu Carousel Estilo Lele
 *
 * Substituido avatar por menu carousel com 7 categorias
 */

#include "ui_home.h"
#include "globals.h"
#include "screens/ui_menu_carousel.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"
#include <debug_log.h>

// Variáveis Estáticas
static bool _initialized = false;
static bool _carouselCreated = false;

void ui_home_init() {
    _initialized = true;
    _carouselCreated = false;
}

void ui_home_show() {
    lv_obj_t *content = ui_get_content_area();
    if (!content) return;

    ui_clear_content_area();

    // Se o carousel já foi criado, apenas mostra
    if (_carouselCreated) {
        menuCarousel.show();
        LOG_UI("Menu carousel mostrado");
        return;
    }

    // Cria o menu carousel (primeira vez)
    LOG_UI("Criando menu carousel...");

    // Estica o content para usar a área toda
    lv_obj_set_size(content, LCD_WIDTH, LCD_HEIGHT - 50); // Deixa espaço pro status bar
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    // Cria o carousel dentro do content area
    menuCarousel.create(content);
    _carouselCreated = true;

    LOG_UI("Menu carousel criado!");
}

void ui_home_update() {
    if (!_initialized) return;

    // Atualiza o carousel se necessário
    menuCarousel.update();
}

void ui_home_set_assistant_text(const char *text) {
    // Não usado mais com carousel
    (void)text;
}

void ui_home_open_menu(HomeMenuCategory category) {
    // Navega para a categoria correspondente no carousel
    switch (category) {
        case MENU_WIFI:
            menuCarousel.goTo(0); // WiFi é índice 0
            break;
        case MENU_BLE:
            menuCarousel.goTo(1); // BLE é índice 1
            break;
        case MENU_TOOLS:
            menuCarousel.goTo(4); // Files/Tools é índice 4
            break;
        case MENU_SETTINGS:
            menuCarousel.goTo(5); // Settings é índice 5
            break;
        default: break;
    }
}
