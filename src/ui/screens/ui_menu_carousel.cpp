/**
 * @file ui_menu_carousel.cpp
 * @brief Menu estilo Lele - Uma categoria por tela com ícones gigantes
 *
 * Inspirado no Lele Firmware mas 1 MILHÃO de vezes melhor!
 */

#include "ui_menu_carousel.h"
#include "../ui_main.h"
#include "../ui_themes.h"
#include <debug_log.h>

MenuCarousel menuCarousel;

// Categorias do menu principal
static CarouselItem menuItems[] = {
    {"📡", "WiFi Attacks", COLOR_GREEN,  category_wifi_click,     true},
    {"🔵", "Bluetooth",    COLOR_CYAN,   category_ble_click,      true},
    {"📺", "IR Remote",    COLOR_ORANGE, category_ir_click,       true},
    {"🔌", "USB BadUSB",   COLOR_PURPLE, category_usb_click,      true},
    {"📁", "Files",        COLOR_CYAN,   category_files_click,    true},
    {"⚙️",  "Settings",     COLOR_WHITE,  category_settings_click, true},
    {"ℹ️",  "About",        COLOR_GRAY,   category_about_click,    true}
};
static const int MENU_ITEMS_COUNT = sizeof(menuItems) / sizeof(menuItems[0]);

MenuCarousel::MenuCarousel()
    : _container(nullptr), _iconLabel(nullptr), _nameLabel(nullptr), _indicatorContainer(nullptr),
      _prevBtn(nullptr), _nextBtn(nullptr), _selectBtn(nullptr), _currentIndex(0), _visible(false),
      _swiping(false) {

    for (int i = 0; i < MENU_ITEMS_COUNT; i++) { _items.push_back(menuItems[i]); }
}

void MenuCarousel::create(lv_obj_t *parent) {
    // Container principal - tela cheia
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(_container, 0, 0);
    lv_obj_set_style_bg_color(_container, lv_color_hex(COLOR_BG_DARK), 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_style_pad_all(_container, 0, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Habilita eventos de swipe
    lv_obj_add_flag(_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(_container, onSwipeEvent, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(_container, onSwipeEvent, LV_EVENT_RELEASED, this);

    // ==================== ÁREA DO ÍCONE (centro-superior) ====================

    // Ícone gigante - 120px no centro
    _iconLabel = lv_label_create(_container);
    lv_obj_set_style_text_font(_iconLabel, &lv_font_montserrat_48, 0); // Maior fonte disponível
    lv_obj_align(_iconLabel, LV_ALIGN_CENTER, 0, -60);
    lv_obj_set_style_text_color(_iconLabel, lv_color_hex(COLOR_CYAN), 0);

    // Torna o ícone clicável (seleciona a categoria)
    lv_obj_add_flag(_iconLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(_iconLabel, onSelectClick, LV_EVENT_CLICKED, this);

    // Nome da categoria
    _nameLabel = lv_label_create(_container);
    lv_obj_set_style_text_font(_nameLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(_nameLabel, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_color(_nameLabel, lv_color_hex(COLOR_WHITE), 0);

    // ==================== INDICADORES (bolinhas) ====================

    _indicatorContainer = lv_obj_create(_container);
    lv_obj_set_size(_indicatorContainer, LV_HOR_RES, 20);
    lv_obj_align(_indicatorContainer, LV_ALIGN_BOTTOM_MID, 0, -80);
    lv_obj_set_style_bg_opa(_indicatorContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_indicatorContainer, 0, 0);
    lv_obj_set_flex_flow(_indicatorContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        _indicatorContainer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER
    );
    lv_obj_set_style_pad_column(_indicatorContainer, 8, 0);
    lv_obj_clear_flag(_indicatorContainer, LV_OBJ_FLAG_SCROLLABLE);

    createIndicators();

    // ==================== BOTÕES DE NAVEGAÇÃO ====================

    // Container para botões
    lv_obj_t *navContainer = lv_obj_create(_container);
    lv_obj_set_size(navContainer, LV_HOR_RES - 20, 60);
    lv_obj_align(navContainer, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_opa(navContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(navContainer, 0, 0);
    lv_obj_set_flex_flow(navContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        navContainer, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER
    );
    lv_obj_clear_flag(navContainer, LV_OBJ_FLAG_SCROLLABLE);

    // Botão PREV
    _prevBtn = lv_btn_create(navContainer);
    lv_obj_set_size(_prevBtn, 100, 50);
    lv_obj_set_style_bg_color(_prevBtn, lv_color_hex(0x1a1a3e), 0);
    lv_obj_set_style_bg_color(_prevBtn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_prevBtn, 12, 0);
    lv_obj_set_style_border_width(_prevBtn, 2, 0);
    lv_obj_set_style_border_color(_prevBtn, lv_color_hex(COLOR_CYAN), 0);
    lv_obj_add_event_cb(_prevBtn, onPrevClick, LV_EVENT_CLICKED, this);

    lv_obj_t *prevLabel = lv_label_create(_prevBtn);
    lv_label_set_text(prevLabel, "◀ PREV");
    lv_obj_set_style_text_font(prevLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(prevLabel, lv_color_hex(COLOR_CYAN), 0);
    lv_obj_center(prevLabel);

    // Botão SELECT (maior, no centro)
    _selectBtn = lv_btn_create(navContainer);
    lv_obj_set_size(_selectBtn, 120, 50);
    lv_obj_set_style_bg_color(_selectBtn, lv_color_hex(COLOR_CYAN), 0);
    lv_obj_set_style_bg_color(_selectBtn, lv_color_hex(0x00CCBB), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_selectBtn, 12, 0);
    // Usar SHORT_CLICKED para evitar conflito com swipe
    lv_obj_add_event_cb(_selectBtn, onSelectClick, LV_EVENT_SHORT_CLICKED, this);
    lv_obj_add_event_cb(_selectBtn, onSelectClick, LV_EVENT_CLICKED, this);

    lv_obj_t *selectLabel = lv_label_create(_selectBtn);
    lv_label_set_text(selectLabel, "● SELECT");
    lv_obj_set_style_text_font(selectLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(selectLabel, lv_color_hex(0x000000), 0);
    lv_obj_center(selectLabel);

    // Botão NEXT
    _nextBtn = lv_btn_create(navContainer);
    lv_obj_set_size(_nextBtn, 100, 50);
    lv_obj_set_style_bg_color(_nextBtn, lv_color_hex(0x1a1a3e), 0);
    lv_obj_set_style_bg_color(_nextBtn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_nextBtn, 12, 0);
    lv_obj_set_style_border_width(_nextBtn, 2, 0);
    lv_obj_set_style_border_color(_nextBtn, lv_color_hex(COLOR_CYAN), 0);
    lv_obj_add_event_cb(_nextBtn, onNextClick, LV_EVENT_CLICKED, this);

    lv_obj_t *nextLabel = lv_label_create(_nextBtn);
    lv_label_set_text(nextLabel, "NEXT ▶");
    lv_obj_set_style_text_font(nextLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(nextLabel, lv_color_hex(COLOR_CYAN), 0);
    lv_obj_center(nextLabel);

    // Atualiza display inicial
    updateDisplay();
    _visible = true;
}

void MenuCarousel::createIndicators() {
    for (size_t i = 0; i < _items.size(); i++) {
        lv_obj_t *dot = lv_obj_create(_indicatorContainer);
        lv_obj_set_size(dot, 10, 10);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, lv_color_hex(0x333333), 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }
}

void MenuCarousel::updateIndicators() {
    uint32_t childCount = lv_obj_get_child_cnt(_indicatorContainer);
    for (uint32_t i = 0; i < childCount; i++) {
        lv_obj_t *dot = lv_obj_get_child(_indicatorContainer, i);
        if ((int)i == _currentIndex) {
            lv_obj_set_style_bg_color(dot, lv_color_hex(_items[_currentIndex].color), 0);
            lv_obj_set_size(dot, 14, 14);
        } else {
            lv_obj_set_style_bg_color(dot, lv_color_hex(0x333333), 0);
            lv_obj_set_size(dot, 10, 10);
        }
    }
}

void MenuCarousel::updateDisplay() {
    if (_currentIndex < 0 || _currentIndex >= (int)_items.size()) return;

    CarouselItem &item = _items[_currentIndex];

    // Atualiza ícone (com tamanho grande via padding/escala)
    lv_label_set_text(_iconLabel, item.icon);
    lv_obj_set_style_text_color(_iconLabel, lv_color_hex(item.color), 0);

    // Atualiza nome
    lv_label_set_text(_nameLabel, item.name);
    lv_obj_set_style_text_color(_nameLabel, lv_color_hex(item.color), 0);

    // Atualiza indicadores
    updateIndicators();

    // Atualiza cor do botão SELECT
    lv_obj_set_style_bg_color(_selectBtn, lv_color_hex(item.color), 0);
}

void MenuCarousel::animateTransition(bool toRight) {
    // Animação simples de fade
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, _iconLabel);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, 150);
    lv_anim_set_exec_cb(&a, [](void *obj, int32_t v) { lv_obj_set_style_opa((lv_obj_t *)obj, v, 0); });
    lv_anim_start(&a);
}

void MenuCarousel::next() {
    _currentIndex++;
    if (_currentIndex >= (int)_items.size()) {
        _currentIndex = 0; // Loop
    }
    animateTransition(true);
    updateDisplay();
    LOG_UI("Carousel Next -> %d: %s", _currentIndex, _items[_currentIndex].name);
}

void MenuCarousel::prev() {
    _currentIndex--;
    if (_currentIndex < 0) {
        _currentIndex = _items.size() - 1; // Loop
    }
    animateTransition(false);
    updateDisplay();
    LOG_UI("Carousel Prev -> %d: %s", _currentIndex, _items[_currentIndex].name);
}

void MenuCarousel::select() {
    LOG_DEBUG("select() called, index=%d", _currentIndex);
    if (_currentIndex >= 0 && _currentIndex < (int)_items.size()) {
        LOG_DEBUG(
            "Item: %s, enabled=%d, onClick=%p",
            _items[_currentIndex].name,
            _items[_currentIndex].enabled,
            (void *)_items[_currentIndex].onClick
        );
        if (_items[_currentIndex].enabled && _items[_currentIndex].onClick) {
            LOG_UI("Selected: %s - CALLING onClick", _items[_currentIndex].name);
            _items[_currentIndex].onClick();
        } else {
            LOG_WARN("Item disabled or no onClick!");
        }
    } else {
        LOG_ERROR("Invalid index!");
    }
}

void MenuCarousel::goTo(int index) {
    if (index >= 0 && index < (int)_items.size()) {
        _currentIndex = index;
        updateDisplay();
    }
}

void MenuCarousel::show() {
    if (_container) lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = true;
}

void MenuCarousel::hide() {
    if (_container) lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = false;
}

void MenuCarousel::update() {
    // Pode ser usado para animações contínuas
}

// ==================== CALLBACKS ====================

void MenuCarousel::onPrevClick(lv_event_t *e) {
    MenuCarousel *self = (MenuCarousel *)lv_event_get_user_data(e);
    if (self) self->prev();
}

void MenuCarousel::onNextClick(lv_event_t *e) {
    MenuCarousel *self = (MenuCarousel *)lv_event_get_user_data(e);
    if (self) self->next();
}

void MenuCarousel::onSelectClick(lv_event_t *e) {
    LOG_DEBUG("onSelectClick CALLED!");
    MenuCarousel *self = (MenuCarousel *)lv_event_get_user_data(e);
    if (self) {
        LOG_DEBUG("Calling select() for index %d", self->_currentIndex);
        self->select();
    } else {
        LOG_ERROR("self is NULL!");
    }
}

void MenuCarousel::onSwipeEvent(lv_event_t *e) {
    MenuCarousel *self = (MenuCarousel *)lv_event_get_user_data(e);
    if (!self) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) return;

    lv_point_t point;
    lv_indev_get_point(indev, &point);

    if (code == LV_EVENT_PRESSING && !self->_swiping) {
        self->_touchStart = point;
        self->_swiping = true;
    } else if (code == LV_EVENT_RELEASED && self->_swiping) {
        int dx = point.x - self->_touchStart.x;

        // Swipe threshold: 50px
        if (dx < -50) {
            self->next();
        } else if (dx > 50) {
            self->prev();
        }

        self->_swiping = false;
    }
}

// ==================== CALLBACKS DE CATEGORIA ====================

void category_wifi_click() {
    LOG_UI("Category: WiFi Attacks");
    ui_set_screen(UI_SCREEN_ATTACKS);
}

void category_ble_click() {
    LOG_UI("Category: Bluetooth");
    ui_set_screen(UI_SCREEN_BLE);
}

void category_ir_click() {
    LOG_UI("Category: IR Remote");
    // ui_set_screen(UI_SCREEN_IR);
    LOG_WARN("Category: IR not implemented yet");
}

void category_usb_click() {
    LOG_UI("Category: USB BadUSB");
    // ui_set_screen(UI_SCREEN_USB);
    LOG_WARN("Category: USB not implemented yet");
}

void category_files_click() {
    LOG_UI("Category: Files");
    ui_set_screen(UI_SCREEN_FILES);
}

void category_settings_click() {
    LOG_UI("Category: Settings");
    ui_set_screen(UI_SCREEN_SETTINGS);
}

void category_about_click() {
    LOG_UI("Category: About");
    ui_set_screen(UI_SCREEN_ABOUT);
}
