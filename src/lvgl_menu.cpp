#include "lvgl_menu.h"
#include <SPI.h>

// Configuração ST7789
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  4
#define TFT_BL   48

#define SCR_W 240
#define SCR_H 240

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCR_W * 20]; // Buffer pequeno para PSRAM salvar memoria

// Driver de baixo nível SPI ST7789
void st7789_send_cmd(uint8_t cmd) {
    digitalWrite(TFT_DC, LOW);
    digitalWrite(TFT_CS, LOW);
    SPI.transfer(cmd);
    digitalWrite(TFT_CS, HIGH);
}

void st7789_send_data(uint8_t data) {
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    SPI.transfer(data);
    digitalWrite(TFT_CS, HIGH);
}

void st7789_init() {
    pinMode(TFT_MOSI, OUTPUT);
    pinMode(TFT_SCLK, OUTPUT);
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_BL, OUTPUT);

    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    SPI.setFrequency(40000000); // 40MHz
    SPI.setDataMode(SPI_MODE2); // ST7789 usa Mode 2 ou 3 geralmente

    // Hard Reset
    digitalWrite(TFT_RST, LOW); delay(100);
    digitalWrite(TFT_RST, HIGH); delay(100);

    // Init Commands (Standard ST7789)
    st7789_send_cmd(0x01); delay(150); // SWRESET
    st7789_send_cmd(0x11); delay(50);  // SLPOUT
    st7789_send_cmd(0x3A); st7789_send_data(0x55); // COLMOD 16bit
    st7789_send_cmd(0x36); st7789_send_data(0x00); // MADCTL (Orientation)
    st7789_send_cmd(0x21); // INVON (Muitos ST7789 precisam disso)
    st7789_send_cmd(0x13); // NORON
    st7789_send_cmd(0x29); // DISPON
    
    digitalWrite(TFT_BL, HIGH); // Backlight
}

// Flush Callback para LVGL
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    st7789_send_cmd(0x2A); // CASET
    st7789_send_data(area->x1 >> 8); st7789_send_data(area->x1 & 0xFF);
    st7789_send_data(area->x2 >> 8); st7789_send_data(area->x2 & 0xFF);

    st7789_send_cmd(0x2B); // RASET
    st7789_send_data(area->y1 >> 8); st7789_send_data(area->y1 & 0xFF);
    st7789_send_data(area->y2 >> 8); st7789_send_data(area->y2 & 0xFF);

    st7789_send_cmd(0x2C); // RAMWR

    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    SPI.writeBytes((uint8_t*)color_p, w * h * 2);
    digitalWrite(TFT_CS, HIGH);

    lv_disp_flush_ready(disp);
}

#include "attacks_manager.h"

// --- UI Logic ---
static void event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        String btn_text = String(lv_list_get_btn_text(lv_obj_get_parent(obj), obj));
        Serial.println("Clicked: " + btn_text);

        if (btn_text == "BLE Spam") attacks_start(ATTACK_BLE_SPAM);
        else if (btn_text == "WiFi Deauth") attacks_start(ATTACK_WIFI_DEAUTH);
        else if (btn_text == "NFC Fault") attacks_start(ATTACK_NFC_FAULT);
        else if (btn_text == "SubGHz Replay") attacks_start(ATTACK_SUBGHZ_REPLAY);
        else if (btn_text == "IR Brute") attacks_start(ATTACK_IR_BRUTE);
        else if (btn_text == "Parar Ataques") attacks_stop();
        else if (btn_text == "Modo Stealth") {
            // Implement stealth logic (LEDs off)
             extern void enable_module(int); 
             // Toggle variable? For now just stop attacks.
             attacks_stop();
        }
    }
}

void build_menu() {
    lv_obj_t * list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 220, 220);
    lv_obj_align(list1, LV_ALIGN_CENTER, 0, 0);

    lv_list_add_text(list1, "ATAQUE");
    lv_obj_t * btn;
    btn = lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "BLE Spam");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, "WiFi Deauth");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_WARNING, "NFC Fault");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_UPLOAD, "SubGHz Replay");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_CHARGE, "IR Brute");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    
    btn = lv_list_add_btn(list1, LV_SYMBOL_STOP, "Parar Ataques");
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), 0); // Red button
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(list1, "IA & MODOS");
    btn = lv_list_add_btn(list1, LV_SYMBOL_REFRESH, "IA Atualizar");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_EYE_CLOSE, "Modo Stealth");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    
    lv_list_add_text(list1, "VOZ");
    btn = lv_list_add_btn(list1, LV_SYMBOL_VOLUME_MAX, "Voz Testar");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    
    lv_list_add_text(list1, "STATUS");
    btn = lv_list_add_btn(list1, LV_SYMBOL_BATTERY_FULL, "Bateria 98%");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
}

void setup_lvgl_menu() {
    attacks_init(); // Garantir init
    st7789_init();
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCR_W * 20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    build_menu();
}
