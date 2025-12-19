/**
 * @file status_bar.cpp
 * @brief Implementação da status bar estilo Pwnagotchi
 */

#include "status_bar.h"
#include "globals.h"
#include "ui_themes.h"

StatusBar statusBar;

StatusBar::StatusBar()
    : _container(nullptr), _channel(0), _apsChannel(0), _apsTotal(0), _uptime(0), _pwndSession(0),
      _pwndTotal(0), _mode(MODE_AUTO), _battPercent(100), _battCharging(false), _freeHeap(0), _tempC(0),
      _visible(true) {
    memset(_lastSSID, 0, sizeof(_lastSSID));
}

void StatusBar::create(lv_obj_t *parent) {
    // Container principal - barra fixa no topo (Tip 1, 2, 9, 10)
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LCD_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_align(_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(_container, getTheme().panel, 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_border_side(_container, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(_container, COLOR_NEON_BLUE, 0);
    lv_obj_set_style_border_width(_container, 1, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Estilo de texto para labels (Tip 15)
    static lv_style_t style_label;
    lv_style_init(&style_label);
    lv_style_set_text_font(&style_label, FONT_SMALL);
    lv_style_set_text_color(&style_label, COLOR_NEON_GREEN);

    // Battery Icon + % (Left) - Aligned to Margin (Tip 10)
    _lblBattery = lv_label_create(_container);
    lv_obj_add_style(_lblBattery, &style_label, 0);
    lv_label_set_text(_lblBattery, LV_SYMBOL_BATTERY_FULL " 100%");
    lv_obj_align(_lblBattery, LV_ALIGN_LEFT_MID, MARGIN_SIDE, 0);

    // Time (Center)
    _lblUptime = lv_label_create(_container); // Reusing usage of uptime label var for time mainly
    lv_obj_add_style(_lblUptime, &style_label, 0);
    lv_obj_set_style_text_font(_lblUptime, FONT_TEXT_SEC, 0);
    lv_obj_set_style_text_color(_lblUptime, lv_color_white(), 0);
    lv_label_set_text(_lblUptime, "12:00");
    lv_obj_align(_lblUptime, LV_ALIGN_CENTER, 0, 0);

    // Wifi/BLE/Level Icons (Right) - Aligned to Margin (Tip 10)
    _lblMode = lv_label_create(_container);
    lv_obj_add_style(_lblMode, &style_label, 0);
    lv_label_set_text(_lblMode, LV_SYMBOL_WIFI " " LV_SYMBOL_BLUETOOTH);
    lv_obj_align(_lblMode, LV_ALIGN_RIGHT_MID, -MARGIN_SIDE, 0);

    // Hidden/Secondary stats (can be toggled or smaller)
    // For now keeping them initialized but hidden or small
    _lblChannel = lv_label_create(_container);
    lv_obj_add_flag(_lblChannel, LV_OBJ_FLAG_HIDDEN);

    _lblAPs = lv_label_create(_container);
    lv_obj_add_flag(_lblAPs, LV_OBJ_FLAG_HIDDEN);

    _lblPwnd = lv_label_create(_container);
    lv_obj_add_flag(_lblPwnd, LV_OBJ_FLAG_HIDDEN);

    _lblMem = lv_label_create(_container);
    lv_obj_add_flag(_lblMem, LV_OBJ_FLAG_HIDDEN);

    updateLabels();
}

void StatusBar::createLabel(lv_obj_t **label, const char *text, lv_coord_t x) {
    static lv_style_t style;
    static bool style_init = false;
    if (!style_init) {
        lv_style_init(&style);
        lv_style_set_text_font(&style, &lv_font_montserrat_12);
        lv_style_set_text_color(&style, lv_color_hex(0x00ff88));
        style_init = true;
    }

    *label = lv_label_create(_container);
    lv_obj_add_style(*label, &style, 0);
    lv_label_set_text(*label, text);
    lv_obj_set_pos(*label, x, 4);
}

void StatusBar::update() { updateLabels(); }

void StatusBar::setChannel(uint8_t channel) {
    _channel = channel;
    updateLabels();
}

void StatusBar::setAPs(uint16_t current, uint16_t total) {
    _apsChannel = current;
    _apsTotal = total;
    updateLabels();
}

void StatusBar::setUptime(uint32_t seconds) {
    _uptime = seconds;
    updateLabels();
}

void StatusBar::setPwnd(uint16_t session, uint16_t total, const char *lastSSID) {
    _pwndSession = session;
    _pwndTotal = total;
    if (lastSSID) { strncpy(_lastSSID, lastSSID, sizeof(_lastSSID) - 1); }
    updateLabels();
}

void StatusBar::setMode(OperationMode mode) {
    _mode = mode;
    updateLabels();
}

void StatusBar::setBattery(uint8_t percent, bool charging) {
    _battPercent = percent;
    _battCharging = charging;
    updateLabels();
}

void StatusBar::setMemory(uint32_t freeHeap, float tempC) {
    _freeHeap = freeHeap;
    _tempC = tempC;
    updateLabels();
}

void StatusBar::show() {
    if (_container) lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = true;
}

void StatusBar::hide() {
    if (_container) lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
    _visible = false;
}

void StatusBar::updateLabels() {
    if (!_container) return;

    char buf[32];

    // Bateria (Esquerda)
    const char *batSymbol = _battCharging ? LV_SYMBOL_CHARGE : LV_SYMBOL_BATTERY_FULL;
    if (_battCharging) {
        snprintf(buf, sizeof(buf), "%s %d%%", batSymbol,
                 _battPercent); // Charging icon
    } else {
        // Simple logic for icon based on percent could go here
        snprintf(buf, sizeof(buf), "%s %d%%", batSymbol, _battPercent);
    }
    lv_label_set_text(_lblBattery, buf);

    // Cor da bateria
    lv_color_t battColor;
    if (_battPercent > 50) {
        battColor = COLOR_NEON_GREEN;
    } else if (_battPercent > 20) {
        battColor = getTheme().warning;
    } else {
        battColor = getTheme().danger;
    }
    lv_obj_set_style_text_color(_lblBattery, battColor, 0);

    // Time (Center) - Using Uptime variable effectively as Clock for now
    // In real implementation, this should fetch actual RTC time
    // For now, let's just format uptime as HH:MM if it was just uptime,
    // but to follow Tip 2 (24h clock), we need real time.
    // Assuming external update loop sets this label or we simulate it.
    // For now, keeping uptime logic but formatting it nicely.
    uint32_t hours = (_uptime / 3600) % 24;
    uint32_t mins = (_uptime % 3600) / 60;
    snprintf(buf, sizeof(buf), "%02u:%02u", hours, mins);
    lv_label_set_text(_lblUptime, buf);

    // Mode/Connectivity (Right)
    // Tip 2: Wi-Fi + BLE icons | Canal atual
    // Format: "W B Lvl8 CH1"
    const char *wifiIcon = (g_state.wifi_enabled) ? LV_SYMBOL_WIFI : "";
    const char *bleIcon = (BLEConnected) ? LV_SYMBOL_BLUETOOTH : "";

    snprintf(buf, sizeof(buf), "%s %s CH%d", wifiIcon, bleIcon,
             _channel);
    lv_label_set_text(_lblMode, buf);
    lv_obj_set_style_text_color(_lblMode, getTheme().secondary, 0);
}
