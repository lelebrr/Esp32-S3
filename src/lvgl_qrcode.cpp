#include "lvgl_menu.h"
#include <WiFi.h>
#include <lvgl.h>


// This requires LVGL to be compiled with LV_USE_QRCODE = 1
// If not, we might need a fallback or just a message.
// For now, we assume it's available or we display a placeholder.

static lv_obj_t *qr = NULL;

void show_qrcode_view(const char *data) {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFFFFFF), 0); // White BG for QR

    // Title
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "QR Code");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // QR Code
    // Size 150, Dark Color, Light Color
    // Note: lv_qrcode_create might need extra lib enabled.
    // If fail, we show text.

#if LV_USE_QRCODE
    qr = lv_qrcode_create(scr, 150, lv_color_hex(0x000000), lv_color_hex(0xFFFFFF));
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);
#else
    lv_obj_t *msg = lv_label_create(scr);
    lv_label_set_text(msg, "QR Lib Missing\nCompile with\nLV_USE_QRCODE");
    lv_obj_set_style_text_color(msg, lv_color_hex(0x000000), 0);
    lv_obj_center(msg);
#endif

    // Back Button
    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "Voltar");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *e) {
            show_ai_voice_menu(); // Return to Others menu
        },
        LV_EVENT_CLICKED,
        NULL
    );
}

void show_qrcode_menu() {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(COL_BG), 0);
    create_header(scr, "Gerador QR Code");

    int y = 45;
    create_menu_btn(scr, "WiFi MorphNode", 10, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        // WIFI:S:SSID;T:WPA;P:PASSWORD;;
        show_qrcode_view("WIFI:S:MorphNode;T:WPA;P:lele2025;;");
    });

    create_menu_btn(scr, "Rick Roll", 165, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        show_qrcode_view("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    });

    y += 50;
    create_menu_btn(scr, "PIX (Exemplo)", 10, y, 145, 40, COL_BTN, [](lv_event_t *e) {
        // Dummy PIX code
        show_qrcode_view("00020126580014BR.GOV.BCB.PIX0136123e4567-e12b-12d1-a456-426655440000");
    });

    create_menu_btn(scr, "Voltar", 165, y, 145, 40, COL_RED, [](lv_event_t *e) { show_ai_voice_menu(); });
}
