#include "ui_proximity.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Not implemented
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_ProximityScreen;
static lv_obj_t *ui_RadarArc;
static lv_obj_t *ui_DeviceList;
static lv_timer_t *ping_timer;

static void update_radar(lv_timer_t *t) {
    // TODO: MeshSystem not implemented
    // MeshSystem.sendPing();

    // Animate radar sweep
    static int val = 0;
    val = (val + 10) % 360;
    // lv_arc_set_end_angle(ui_RadarArc, val); // Visual effect
}

static void add_device_to_list(const char *name, int rssi) {
    // Check if simple list item or complex
    lv_obj_t *btn = lv_list_add_btn(ui_DeviceList, LV_SYMBOL_WIFI, name);

    // Add RSSI label
    lv_obj_t *l = lv_label_create(btn);
    lv_label_set_text_fmt(l, "%d dBm", rssi);
    lv_obj_align(l, LV_ALIGN_RIGHT_MID, 0, 0);
}

void ui_proximity_screen_init(void) {
    // TODO: MeshSystem not implemented
    // MeshSystem.begin();

    // MeshSystem.onPing([](const uint8_t *mac, int rssi, const char *name) {
    //   // Add to list (safely)
    //   // For now assumes thread safety or low frequency
    //   // add_device_to_list(name, rssi);
    // });

    ui_ProximityScreen = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(ui_ProximityScreen);
    lv_label_set_text(title, "PROXIMITY RADAR");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Radar visual
    ui_RadarArc = lv_spinner_create(ui_ProximityScreen, 1000, 60);
    lv_obj_set_size(ui_RadarArc, 100, 100);
    lv_obj_align(ui_RadarArc, LV_ALIGN_TOP_MID, 0, 40);

    // Device list
    ui_DeviceList = lv_list_create(ui_ProximityScreen);
    lv_obj_set_size(ui_DeviceList, LV_PCT(100), LV_PCT(50));
    lv_obj_align(ui_DeviceList, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Start pinging
    ping_timer = lv_timer_create(update_radar, 1000, NULL);

    // Back btn
    lv_obj_t *backBtn = lv_btn_create(ui_ProximityScreen);
    lv_obj_set_size(backBtn, 60, 40);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t *l = lv_label_create(backBtn);
    lv_label_set_text(l, "BACK");
    lv_obj_center(l);
    lv_obj_add_event_cb(
        backBtn,
        [](lv_event_t *e) {
            if (ping_timer) lv_timer_del(ping_timer);
            ping_timer = NULL;
            ui_main_show();
        },
        LV_EVENT_CLICKED,
        NULL
    );
}
