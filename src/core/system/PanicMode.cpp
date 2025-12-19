#include "PanicMode.h"
#include <globals.h>
#include <core/display.h>

void PanicMode::init() {
    _active = false;
    _touchCount = 0;
}

void PanicMode::trigger() {
    if (_active) return;
    
    // STOP ALL ATTACKS IMMEDIATELY
    // wifi_stop_all(); // Hypothetical function
    // ble_stop_all();
    
    _active = true;
    
    // CREATE FAKE CLOCK UI
    drawFakeClock();
}

void PanicMode::drawFakeClock() {
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);
    lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    
    // Time Label
    _labelTime = lv_label_create(_screen);
    lv_label_set_text(_labelTime, "12:00");
    lv_obj_set_style_text_font(_labelTime, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(_labelTime, lv_color_white(), 0);
    lv_obj_center(_labelTime);
    
    // Date Label
    _labelDate = lv_label_create(_screen);
    lv_label_set_text(_labelDate, "Wed, Dec 17");
    lv_obj_set_style_text_font(_labelDate, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(_labelDate, lv_color_make(200, 200, 200), 0);
    lv_obj_align(_labelDate, LV_ALIGN_CENTER, 0, 40);
    
    // Simular "Lock Screen" behavior
    lv_obj_t* hint = lv_label_create(_screen);
    lv_label_set_text(hint, "Swipe up to unlock");
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(hint, lv_color_make(100, 100, 100), 0);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void PanicMode::update() {
    if (!_active) return;
    
    // Update Clock (Mock implementation)
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 60000) {
        lastUpdate = millis();
        // Here we would get real time if available
        lv_label_set_text(_labelTime, "12:01"); 
    }
}

void PanicMode::checkTrigger(int x, int y) {
    // Magic Zone: Top Right Corner (Double Tap)
    if (x > 300 && y < 40) {
        if (millis() - _lastTouch < 500) {
            trigger();
        }
        _lastTouch = millis();
    }
}
