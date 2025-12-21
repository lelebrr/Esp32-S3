/**
 * @file lvgl_menu.cpp
 * Monster S3 Firmware - LVGL Display with TFT_eSPI
 * ILI9341 240x320 with XPT2046 Touch + PAJ7620 Gesture Support
 */

#include "lvgl_menu.h"
#include "attacks_manager.h"
#include "gesture_sensor.h"
#include "pin_config.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// TFT_eSPI instance (includes touch support)
TFT_eSPI tft = TFT_eSPI();

// Screen dimensions
#define SCR_W TFT_WIDTH  // 240
#define SCR_H TFT_HEIGHT // 320

// LVGL Buffers
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

// LVGL Input Device for Gestures
static lv_indev_t *gesture_indev = NULL;
static lv_group_t *main_group = NULL;
static volatile uint32_t last_gesture_key = 0;

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
    tft.pushColors((uint16_t *)color_p, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// ========================================
// LVGL Touch Read Callback
// ========================================
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t tx, ty;

    if (tft.getTouch(&tx, &ty)) {
        Serial.printf("[TOUCH] x=%d y=%d\n", tx, ty);
        // Map touch coordinates
        data->point.x = tx;
        data->point.y = ty;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ========================================
// LVGL Gesture Keypad Read Callback
// ========================================
void gesture_keypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (last_gesture_key != 0) {
        data->key = last_gesture_key;
        data->state = LV_INDEV_STATE_PR;
        last_gesture_key = 0; // Clear after reading
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/**
 * @brief Send a gesture event to LVGL
 * Called from the gesture task when a gesture is detected
 */
void lvgl_send_gesture_key(uint32_t key) { last_gesture_key = key; }

/**
 * @brief Convert GestureAction to LVGL key
 */
uint32_t gesture_action_to_lvgl_key(GestureAction action) {
    switch (action) {
        case ACTION_MENU_UP: return LV_KEY_UP;
        case ACTION_MENU_DOWN: return LV_KEY_DOWN;
        case ACTION_MENU_LEFT: return LV_KEY_LEFT;
        case ACTION_MENU_RIGHT: return LV_KEY_RIGHT;
        case ACTION_SELECT: return LV_KEY_ENTER;
        case ACTION_BACK: return LV_KEY_ESC;
        case ACTION_SPECIAL_1: return LV_KEY_NEXT;
        case ACTION_SPECIAL_2: return LV_KEY_PREV;
        default: return 0;
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

    if (strcmp(txt, "BLE Spam") == 0) {
        attacks_start(ATTACK_BLE_SPAM);
    } else if (strcmp(txt, "WiFi Deauth") == 0) {
        attacks_start(ATTACK_WIFI_DEAUTH);
    } else if (strcmp(txt, "NFC Fault") == 0) {
        attacks_start(ATTACK_NFC_FAULT);
    } else if (strcmp(txt, "IR Brute") == 0) {
        attacks_start(ATTACK_IR_BRUTE);
    } else if (strcmp(txt, "SubGHz") == 0) {
        attacks_start(ATTACK_RF_GHOST_REPLAY);
    } else if (strcmp(txt, "STOP ALL") == 0) {
        attacks_stop();
    }
}

// ========================================
// Create Menu Button Helper
// ========================================
lv_obj_t *create_btn(
    lv_obj_t *parent, const char *text, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
    lv_color_t color
) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Add focus style for gesture navigation
    lv_obj_set_style_outline_width(btn, 3, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btn, lv_color_hex(0x00ff00), LV_STATE_FOCUSED);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    // Add to focus group for gesture navigation
    if (main_group) { lv_group_add_obj(main_group, btn); }

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

    // Subtitle with gesture status
    lv_obj_t *sub = lv_label_create(scr);
    if (GestureSensor::isAvailable()) {
        lv_label_set_text(sub, "Pentest v1.0 | Gesture: ON");
    } else {
        lv_label_set_text(sub, "Pentest Toolkit v1.0");
    }
    lv_obj_set_style_text_color(sub, lv_color_hex(0x888888), 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 35);

    // Attack Buttons Grid
    lv_color_t btn_blue = lv_color_hex(0x0f3460);
    lv_color_t btn_red = lv_color_hex(0xe94560);

    create_btn(scr, "BLE Spam", 15, 70, 100, 45, btn_blue);
    create_btn(scr, "WiFi Deauth", 125, 70, 100, 45, btn_blue);
    create_btn(scr, "NFC Fault", 15, 125, 100, 45, btn_blue);
    create_btn(scr, "IR Brute", 125, 125, 100, 45, btn_blue);
    create_btn(scr, "SubGHz", 15, 180, 100, 45, btn_blue);
    create_btn(scr, "RF Jammer", 125, 180, 100, 45, btn_blue);
    create_btn(scr, "Wardriving", 70, 235, 100, 45, btn_blue);

    // Stop Button (full width, red)
    create_btn(scr, "STOP ALL", 15, 290, 210, 50, btn_red);

    // Gesture hint
    lv_obj_t *hint = lv_label_create(scr);
    lv_label_set_text(hint, "Swipe to navigate, Push to select");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x555555), 0);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_16, 0);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -5);

    Serial.println("[UI] Menu created with gesture support");
}

// ========================================
// Public API
// ========================================
void setup_lvgl_menu() {
    Serial.println("[DISPLAY] Initializing TFT_eSPI...");

    // CRITICAL: Initialize SPI FSPI bus BEFORE TFT_eSPI
    // Without this, TFT_eSPI crashes with StoreProhibited
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    Serial.println("[SPI] FSPI bus initialized");

    // Initialize GPIO pins
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH); // Deselect TFT

    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);

    // Reset the display
    digitalWrite(TFT_RST, LOW);
    delay(10);
    digitalWrite(TFT_RST, HIGH);
    delay(120);

    // Backlight ON first (helps with debugging)
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);

    // Now initialize TFT_eSPI
    tft.init();
    tft.setRotation(2); // Try rotation 2 (inverted portrait)
    tft.fillScreen(TFT_BLACK);

    // Quick test - draw colored rectangles
    tft.fillRect(0, 0, 80, 80, TFT_RED);
    tft.fillRect(80, 0, 80, 80, TFT_GREEN);
    tft.fillRect(160, 0, 80, 80, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 100);
    tft.println("Monster S3");
    tft.setCursor(10, 130);
    tft.println("Touch to test...");
    delay(1000);

    tft.fillScreen(TFT_BLACK);

    Serial.println("[DISPLAY] ILI9341 initialized");

    // Touch calibration (adjust if needed)
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
    Serial.println("[TOUCH] XPT2046 calibrated");

    // Initialize LVGL
    lv_init();

    // Create default focus group for gesture navigation
    main_group = lv_group_create();
    lv_group_set_default(main_group);

    // Allocate buffer in PSRAM (40 lines)
    size_t buf_size = SCR_W * 40;
    buf = (lv_color_t *)ps_malloc(buf_size * sizeof(lv_color_t));
    if (!buf) {
        Serial.println("[LVGL] PSRAM failed, using heap");
        buf = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
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

    // Register gesture keypad driver
    static lv_indev_drv_t gesture_drv;
    lv_indev_drv_init(&gesture_drv);
    gesture_drv.type = LV_INDEV_TYPE_KEYPAD;
    gesture_drv.read_cb = gesture_keypad_read;
    gesture_indev = lv_indev_drv_register(&gesture_drv);

    // Assign gesture input to the focus group
    lv_indev_set_group(gesture_indev, main_group);
    Serial.println("[GESTURE] LVGL keypad input registered");

    // Create UI
    create_menu_ui();

    Serial.println("[LVGL] System ready with gesture navigation!");
}

void lvgl_loop() { lv_task_handler(); }

// ========================================
// Get Focus Group (for external access)
// ========================================
lv_group_t *lvgl_get_main_group() { return main_group; }
