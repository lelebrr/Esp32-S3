/**
 * @file ui_status_bar.cpp
 * @brief Header e Footer fixos para a UI
 */

#include "ui_status_bar.h"
#include "../ui_themes.h"
#include "core/pin_definitions.h"

// Instâncias globais (renomeadas para evitar conflito com status_bar.cpp existente)
UIStatusBar uiStatusBar;
UINavBar uiNavBar;

// Cores
#define COLOR_BG_DARK lv_color_hex(0x0D0D0D)
#define COLOR_CYAN lv_color_hex(0x00FFF5)
#define COLOR_GREEN lv_color_hex(0x00FF41)
#define COLOR_RED lv_color_hex(0xFF0055)
#define COLOR_YELLOW lv_color_hex(0xFFCC00)
#define COLOR_WHITE lv_color_hex(0xFFFFFF)
#define COLOR_GRAY lv_color_hex(0x666666)

// ============================================================================
// UIStatusBar (Header)
// ============================================================================

UIStatusBar::UIStatusBar()
    : _container(nullptr), _batteryLabel(nullptr), _wifiLabel(nullptr), _bleLabel(nullptr),
      _timeLabel(nullptr), _statusLabel(nullptr), _batteryPercent(100), _charging(false),
      _wifiConnected(false), _bleActive(false) {}

void UIStatusBar::create(lv_obj_t *parent) {
    // Container do header
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LCD_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_align(_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(_container, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_90, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_border_side(_container, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(_container, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(_container, 1, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_style_pad_all(_container, 6, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Layout flex horizontal
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        _container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER
    );

    // ==== LADO ESQUERDO (Bateria + WiFi + BLE) ====
    lv_obj_t *leftGroup = lv_obj_create(_container);
    lv_obj_set_size(leftGroup, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(leftGroup, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(leftGroup, 0, 0);
    lv_obj_set_style_pad_all(leftGroup, 0, 0);
    lv_obj_set_flex_flow(leftGroup, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(leftGroup, 8, 0);
    lv_obj_clear_flag(leftGroup, LV_OBJ_FLAG_SCROLLABLE);

    // Bateria
    _batteryLabel = lv_label_create(leftGroup);
    lv_label_set_text(_batteryLabel, "🔋100%");
    lv_obj_set_style_text_font(_batteryLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_batteryLabel, COLOR_GREEN, 0);

    // WiFi
    _wifiLabel = lv_label_create(leftGroup);
    lv_label_set_text(_wifiLabel, "📡");
    lv_obj_set_style_text_font(_wifiLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_wifiLabel, COLOR_GRAY, 0);

    // BLE
    _bleLabel = lv_label_create(leftGroup);
    lv_label_set_text(_bleLabel, "🔵");
    lv_obj_set_style_text_font(_bleLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_bleLabel, COLOR_GRAY, 0);

    // ==== CENTRO (Hora) ====
    _timeLabel = lv_label_create(_container);
    lv_label_set_text(_timeLabel, "00:00");
    lv_obj_set_style_text_font(_timeLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_timeLabel, COLOR_WHITE, 0);

    // ==== LADO DIREITO (Status) ====
    _statusLabel = lv_label_create(_container);
    lv_label_set_text(_statusLabel, "Idle");
    lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(_statusLabel, COLOR_CYAN, 0);
}

void UIStatusBar::setBattery(int percent, bool charging) {
    _batteryPercent = percent;
    _charging = charging;

    char buf[16];
    const char *icon = charging ? "⚡" : "🔋";
    snprintf(buf, sizeof(buf), "%s%d%%", icon, percent);
    lv_label_set_text(_batteryLabel, buf);

    // Cor baseada no nível
    lv_color_t color;
    if (percent > 50) {
        color = COLOR_GREEN;
    } else if (percent > 20) {
        color = COLOR_YELLOW;
    } else {
        color = COLOR_RED;
    }
    lv_obj_set_style_text_color(_batteryLabel, color, 0);
}

void UIStatusBar::setWiFi(bool connected, int strength) {
    _wifiConnected = connected;
    lv_obj_set_style_text_color(_wifiLabel, connected ? COLOR_GREEN : COLOR_GRAY, 0);
}

void UIStatusBar::setBLE(bool active) {
    _bleActive = active;
    lv_obj_set_style_text_color(_bleLabel, active ? COLOR_CYAN : COLOR_GRAY, 0);
}

void UIStatusBar::setTime(int hour, int minute) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
    lv_label_set_text(_timeLabel, buf);
}

void UIStatusBar::setStatus(const char *text) { lv_label_set_text(_statusLabel, text); }

void UIStatusBar::update() {
    // Atualiza hora do RTC (se disponível)
    // TODO: Integrar com RTC real
}

void UIStatusBar::show() {
    if (_container) lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

void UIStatusBar::hide() {
    if (_container) lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

// ============================================================================
// UINavBar (Footer)
// ============================================================================

UINavBar::UINavBar()
    : _container(nullptr), _prevBtn(nullptr), _selectBtn(nullptr), _nextBtn(nullptr), _prevLabel(nullptr),
      _selectLabel(nullptr), _nextLabel(nullptr), _onPrev(nullptr), _onSelect(nullptr), _onNext(nullptr) {}

void UINavBar::create(lv_obj_t *parent) {
    // Container do footer
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LCD_WIDTH, NAV_BAR_HEIGHT);
    lv_obj_align(_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_container, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_90, 0);
    lv_obj_set_style_border_side(_container, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_color(_container, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(_container, 1, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_style_pad_all(_container, 4, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Layout flex
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(_container, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Botão PREV
    _prevBtn = lv_btn_create(_container);
    lv_obj_set_size(_prevBtn, 90, 28);
    lv_obj_set_style_bg_color(_prevBtn, lv_color_hex(0x1a1a3e), 0);
    lv_obj_set_style_bg_color(_prevBtn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_prevBtn, 6, 0);
    lv_obj_add_event_cb(_prevBtn, onPrevClick, LV_EVENT_CLICKED, this);

    _prevLabel = lv_label_create(_prevBtn);
    lv_label_set_text(_prevLabel, "◀ PREV");
    lv_obj_set_style_text_font(_prevLabel, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(_prevLabel, COLOR_CYAN, 0);
    lv_obj_center(_prevLabel);

    // Botão SELECT (maior)
    _selectBtn = lv_btn_create(_container);
    lv_obj_set_size(_selectBtn, 100, 28);
    lv_obj_set_style_bg_color(_selectBtn, COLOR_CYAN, 0);
    lv_obj_set_style_bg_color(_selectBtn, lv_color_hex(0x00CCBB), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_selectBtn, 6, 0);
    lv_obj_add_event_cb(_selectBtn, onSelectClick, LV_EVENT_CLICKED, this);

    _selectLabel = lv_label_create(_selectBtn);
    lv_label_set_text(_selectLabel, "● SELECT");
    lv_obj_set_style_text_font(_selectLabel, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(_selectLabel, lv_color_hex(0x000000), 0);
    lv_obj_center(_selectLabel);

    // Botão NEXT
    _nextBtn = lv_btn_create(_container);
    lv_obj_set_size(_nextBtn, 90, 28);
    lv_obj_set_style_bg_color(_nextBtn, lv_color_hex(0x1a1a3e), 0);
    lv_obj_set_style_bg_color(_nextBtn, lv_color_hex(0x2a2a5e), LV_STATE_PRESSED);
    lv_obj_set_style_radius(_nextBtn, 6, 0);
    lv_obj_add_event_cb(_nextBtn, onNextClick, LV_EVENT_CLICKED, this);

    _nextLabel = lv_label_create(_nextBtn);
    lv_label_set_text(_nextLabel, "NEXT ▶");
    lv_obj_set_style_text_font(_nextLabel, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(_nextLabel, COLOR_CYAN, 0);
    lv_obj_center(_nextLabel);
}

void UINavBar::setOnPrev(void (*callback)()) { _onPrev = callback; }
void UINavBar::setOnSelect(void (*callback)()) { _onSelect = callback; }
void UINavBar::setOnNext(void (*callback)()) { _onNext = callback; }

void UINavBar::setLabels(const char *prev, const char *select, const char *next) {
    if (prev) lv_label_set_text(_prevLabel, prev);
    if (select) lv_label_set_text(_selectLabel, select);
    if (next) lv_label_set_text(_nextLabel, next);
}

void UINavBar::show() {
    if (_container) lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

void UINavBar::hide() {
    if (_container) lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

void UINavBar::onPrevClick(lv_event_t *e) {
    UINavBar *self = (UINavBar *)lv_event_get_user_data(e);
    if (self && self->_onPrev) self->_onPrev();
}

void UINavBar::onSelectClick(lv_event_t *e) {
    UINavBar *self = (UINavBar *)lv_event_get_user_data(e);
    if (self && self->_onSelect) self->_onSelect();
}

void UINavBar::onNextClick(lv_event_t *e) {
    UINavBar *self = (UINavBar *)lv_event_get_user_data(e);
    if (self && self->_onNext) self->_onNext();
}
