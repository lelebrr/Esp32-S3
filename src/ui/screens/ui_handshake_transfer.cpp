#include "ui_handshake_transfer.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Not implemented yet
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_TransferScreen;
static lv_obj_t *ui_StatusText;
static lv_obj_t *ui_SendBtn;
static lv_obj_t *ui_ReceiveBtn;
static lv_obj_t *ui_DragonImage;

static void send_btn_handler(lv_event_t *e) {
    lv_label_set_text(ui_StatusText, "Searching for devices...");
    // Mock logic for now
    lv_timer_t *t = lv_timer_create(
        [](lv_timer_t *t) {
            lv_label_set_text(ui_StatusText, "Sending 'home_wifi.pcap'...");
            // In real impl: MeshSystem.sendHandshake(...)
            lv_timer_del(t);

            // Simulate completion
            lv_timer_t *t2 = lv_timer_create(
                [](lv_timer_t *t2) {
                    lv_label_set_text(ui_StatusText, "Transfer Complete!");
                    lv_timer_del(t2);
                },
                2000,
                NULL
            );
        },
        1000,
        NULL
    );
}

static void receive_btn_handler(lv_event_t *e) {
    lv_label_set_text(ui_StatusText, "Waiting for handshake...");
    // Logic to listen for handshake packets
}

void ui_handshake_transfer_screen_init(void) {
    ui_TransferScreen = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(ui_TransferScreen);
    lv_label_set_text(title, "HANDSHAKE SEND");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);

    ui_StatusText = lv_label_create(ui_TransferScreen);
    lv_label_set_text(ui_StatusText, "Ready to transfer PCAPs.");
    lv_obj_align(ui_StatusText, LV_ALIGN_CENTER, 0, -20);

    ui_SendBtn = lv_btn_create(ui_TransferScreen);
    lv_obj_set_size(ui_SendBtn, 100, 40);
    lv_obj_align(ui_SendBtn, LV_ALIGN_CENTER, 0, 30);
    lv_obj_t *l1 = lv_label_create(ui_SendBtn);
    lv_label_set_text(l1, "SEND");
    lv_obj_center(l1);
    lv_obj_add_event_cb(ui_SendBtn, send_btn_handler, LV_EVENT_CLICKED, NULL);

    ui_ReceiveBtn = lv_btn_create(ui_TransferScreen);
    lv_obj_set_size(ui_ReceiveBtn, 100, 40);
    lv_obj_align(ui_ReceiveBtn, LV_ALIGN_CENTER, 0, 80);
    lv_obj_t *l2 = lv_label_create(ui_ReceiveBtn);
    lv_label_set_text(l2, "RECEIVE");
    lv_obj_center(l2);
    lv_obj_add_event_cb(ui_ReceiveBtn, receive_btn_handler, LV_EVENT_CLICKED, NULL);

    // Back Button
    lv_obj_t *backBtn = lv_btn_create(ui_TransferScreen);
    lv_obj_set_size(backBtn, 60, 40);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t *backLabel = lv_label_create(backBtn);
    lv_label_set_text(backLabel, "BACK");
    lv_obj_center(backLabel);
    lv_obj_add_event_cb(backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
