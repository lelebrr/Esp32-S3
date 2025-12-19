/**
 * @file lvgl_menu.cpp
 * Monster S3 Firmware - LVGL Display with TFT_eSPI
 * ILI9341 240x320 with XPT2046 Touch (built-in TFT_eSPI support)
 */

#include "lvgl_menu.h"
#include "pin_config.h"
#include "attacks_manager.h"
#include <TFT_eSPI.h>
#include <lvgl.h>

// TFT_eSPI instance (includes touch support)
TFT_eSPI tft = TFT_eSPI();

// Screen dimensions
#define SCR_W TFT_WIDTH   // 240
#define SCR_H TFT_HEIGHT  // 320

// LVGL Buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

// Touch calibration for MSP2402
#define TOUCH_X_MIN 200
#define TOUCH_X_MAX 3900
#define TOUCH_Y_MIN 200
#define TOUCH_Y_MAX 3900

// ========================================
// LVGL Display Flush Callback
// ========================================
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)color_p, w * h, true);
    tft.endWrite();
    
    lv_disp_flush_ready(disp);
}

// ========================================
// LVGL Touch Read Callback
// ========================================
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t tx, ty;
    
    if(tft.getTouch(&tx, &ty)) {
        // Map touch coordinates
        data->point.x = tx;
        data->point.y = ty;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ========================================
// UI Event Handler
// ========================================
static void btn_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    const char *txt = lv_label_get_text(label);
    
    Serial.printf("[UI] Button: %s\n", txt);
    
    if(strcmp(txt, "BLE Spam") == 0) {
        attacks_start(ATTACK_BLE_SPAM);
    } else if(strcmp(txt, "WiFi Deauth") == 0) {
        attacks_start(ATTACK_WIFI_DEAUTH);
    } else if(strcmp(txt, "NFC Fault") == 0) {
        attacks_start(ATTACK_NFC_FAULT);
    } else if(strcmp(txt, "IR Brute") == 0) {
        attacks_start(ATTACK_IR_BRUTE);
    } else if(strcmp(txt, "SubGHz") == 0) {
        attacks_start(ATTACK_SUBGHZ_REPLAY);
    } else if(strcmp(txt, "STOP") == 0) {
        attacks_stop();
    }
}

// ========================================
// Create Menu Button Helper
// ========================================
lv_obj_t* create_btn(lv_obj_t *parent, const char *text, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, lv_color_t color) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    
    return btn;
}

// ========================================
// Create Main Menu UI
// ========================================
void create_menu_ui() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);
    
    // Title
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "MONSTER S3");
    lv_obj_set_style_text_color(title, lv_color_hex(0xe94560), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Subtitle
    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "Pentest Toolkit v1.0");
    lv_obj_set_style_text_color(sub, lv_color_hex(0x888888), 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 35);
    
    // Attack Buttons Grid
    lv_color_t btn_blue = lv_color_hex(0x0f3460);
    lv_color_t btn_red = lv_color_hex(0xe94560);
    
    create_btn(scr, "BLE Spam",    15, 70,  100, 45, btn_blue);
    create_btn(scr, "WiFi Deauth", 125, 70, 100, 45, btn_blue);
    create_btn(scr, "NFC Fault",   15, 125, 100, 45, btn_blue);
    create_btn(scr, "IR Brute",   125, 125, 100, 45, btn_blue);
    create_btn(scr, "SubGHz",      15, 180, 100, 45, btn_blue);
    
    // Stop Button (full width, red)
    create_btn(scr, "STOP ALL", 15, 250, 210, 50, btn_red);
    
    Serial.println("[UI] Menu created");
}

// ========================================
// Public API
// ========================================
void setup_lvgl_menu() {
    Serial.println("[DISPLAY] Initializing TFT_eSPI...");
    
    // Initialize TFT
    tft.init();
    tft.setRotation(0); // Portrait
    tft.fillScreen(TFT_BLACK);
    
    // Backlight ON
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    Serial.println("[DISPLAY] ILI9341 initialized (240x320)");
    
    // Touch calibration (adjust if needed)
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
    Serial.println("[TOUCH] XPT2046 calibrated");
    
    // Initialize LVGL
    lv_init();
    
    // Allocate buffer in PSRAM (40 lines)
    size_t buf_size = SCR_W * 40;
    buf = (lv_color_t*) ps_malloc(buf_size * sizeof(lv_color_t));
    if(!buf) {
        Serial.println("[LVGL] PSRAM failed, using heap");
        buf = (lv_color_t*) malloc(buf_size * sizeof(lv_color_t));
    }
    
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, buf_size);
    
    // Register display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Register touch driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);
    
    // Create UI
    create_menu_ui();
    
    Serial.println("[LVGL] System ready!");
}

void lvgl_loop() {
    lv_task_handler();
}
