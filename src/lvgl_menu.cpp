#include "lvgl_menu.h"
#include "pin_config.h"
#include "attacks_manager.h"
#include "s3_driver.h" // For power control if needed
#include <SPI.h>

// Screen dimensions
#define SCR_W 240
#define SCR_H 240

// LVGL Global Definitions
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf; // Dynamic allocation in PSRAM

// ST7789 Specific Commands
#define ST7789_SWRESET 0x01
#define ST7789_SLPOUT  0x11
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_MADCTL  0x36
#define ST7789_COLMOD  0x3A

// Direct SPI Driver for ST7789 (Optimized)
void st7789_send_cmd(uint8_t cmd) {
    digitalWrite(PIN_TFT_DC, LOW);
    digitalWrite(PIN_TFT_CS, LOW);
    SPI.transfer(cmd);
    digitalWrite(PIN_TFT_CS, HIGH);
}

void st7789_send_data(uint8_t data) {
    digitalWrite(PIN_TFT_DC, HIGH);
    digitalWrite(PIN_TFT_CS, LOW);
    SPI.transfer(data);
    digitalWrite(PIN_TFT_CS, HIGH);
}

void st7789_init() {
    // Pins are already init in s3_driver if needed, but we ensure OUTPUT here
    pinMode(PIN_TFT_MOSI, OUTPUT);
    pinMode(PIN_TFT_SCLK, OUTPUT);
    pinMode(PIN_TFT_CS, OUTPUT);
    pinMode(PIN_TFT_DC, OUTPUT);
    pinMode(PIN_TFT_RST, OUTPUT);
    pinMode(PIN_TFT_BL, OUTPUT);

    // SPI Init (using global SPI instance or separate?)
    // Using standard SPI instance for simplicity, but re-configuring for TFT
    SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE2)); // 80MHz if possible
    
    // Hardware Reset
    digitalWrite(PIN_TFT_RST, HIGH);
    delay(5);
    digitalWrite(PIN_TFT_RST, LOW);
    delay(20);
    digitalWrite(PIN_TFT_RST, HIGH);
    delay(150);

    // Init Sequence
    st7789_send_cmd(ST7789_SWRESET); delay(150);
    st7789_send_cmd(ST7789_SLPOUT); delay(50);
    st7789_send_cmd(ST7789_COLMOD); st7789_send_data(0x55); // 16-bit 565
    st7789_send_cmd(ST7789_MADCTL); st7789_send_data(0x00); // RGB
    st7789_send_cmd(ST7789_INVON); // Most ST7789 need inversion
    st7789_send_cmd(ST7789_NORON);
    st7789_send_cmd(ST7789_DISPON);
    
    digitalWrite(PIN_TFT_BL, HIGH); // Turn on Backlight
    SPI.endTransaction();
}

// Flush Callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE2));

    st7789_send_cmd(ST7789_CASET);
    st7789_send_data(area->x1 >> 8); st7789_send_data(area->x1 & 0xFF);
    st7789_send_data(area->x2 >> 8); st7789_send_data(area->x2 & 0xFF);

    st7789_send_cmd(ST7789_RASET);
    st7789_send_data(area->y1 >> 8); st7789_send_data(area->y1 & 0xFF);
    st7789_send_data(area->y2 >> 8); st7789_send_data(area->y2 & 0xFF);

    st7789_send_cmd(ST7789_RAMWR);

    digitalWrite(PIN_TFT_DC, HIGH);
    digitalWrite(PIN_TFT_CS, LOW);
    
    SPI.writeBytes((uint8_t*)color_p, w * h * 2);
    
    digitalWrite(PIN_TFT_CS, HIGH);
    SPI.endTransaction();

    lv_disp_flush_ready(disp);
}

// UI Event Handler
static void event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        String btn_text = String(lv_list_get_btn_text(lv_obj_get_parent(obj), obj));
        Serial.println("Selected: " + btn_text);
        
        // Attack Launch Logic
        if (btn_text.indexOf("BLE Spam") >= 0) attacks_start(ATTACK_BLE_SPAM);
        else if (btn_text.indexOf("WiFi Deauth") >= 0) attacks_start(ATTACK_WIFI_DEAUTH);
        else if (btn_text.indexOf("Parar") >= 0) attacks_stop();
        else if (btn_text.indexOf("Stealth") >= 0) {
            // Toggle Stealth
            // MonsterDriver::toggleStealth();
        }
    }
}

void build_menu() {
    lv_obj_t * list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 240, 240);
    lv_obj_center(list1);

    lv_list_add_text(list1, "MONSTER S3");
    
    lv_obj_t * btn;
    btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, "WiFi Deauth");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "BLE Spam");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list1, LV_SYMBOL_WARNING, "Parar Tudo");
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xAA0000), 0);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
}

void setup_lvgl_menu() {
    st7789_init();
    lv_init();

    // Allocate buffer in PSRAM
    size_t buf_size = SCR_W * SCR_H / 5; // 1/5th screen buffer
    buf = (lv_color_t*) ps_malloc(buf_size * sizeof(lv_color_t));
    if(!buf) {
        Serial.println("PSRAM Alloc Failed for LVGL! Using Heap.");
        buf = (lv_color_t*) malloc(buf_size * sizeof(lv_color_t));
    }

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, buf_size);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    build_menu();
}
