#include "ui_ble_scan.h"
#include "globals.h"
#include "../hardware/ble_driver.h"
#include "ui_menu_ble.h"

static lv_obj_t *_screen = nullptr;
static lv_obj_t *_list = nullptr;
static lv_obj_t *_btn_scan = nullptr;
static lv_obj_t *_spinner = nullptr;
static lv_timer_t *_timer = nullptr;
static bool _active = false;

static void update_list() {
    if (!_list) return;

    // Check if scan complete
    // For now we just check if we have results
    std::vector<BLEDeviceData> devices = ble_driver.getScanResults();

    // Simplistic: Clear and rebuild (Virtual scrolling would involve recycling
    // widgets) For < 50 items, this is acceptable if not done every frame.
    lv_obj_clean(_list);

    for (const auto &dev : devices) {
        const char *label_txt = dev.name.isEmpty() ? "Unknown" : dev.name.c_str();

        char desc_buf[64];
        snprintf(desc_buf, sizeof(desc_buf), "%s (%d dBm)", dev.address.c_str(), dev.rssi);

        lv_obj_t *btn = lv_list_add_btn(_list, LV_SYMBOL_BLUETOOTH, label_txt);
        lv_obj_t *lbl_desc = lv_label_create(btn);
        lv_label_set_text(lbl_desc, desc_buf);
        lv_obj_set_style_text_font(lbl_desc, &lv_font_montserrat_10, 0);
        lv_obj_align(lbl_desc, LV_ALIGN_BOTTOM_LEFT, 30, 0);
    }

    if (devices.empty()) { lv_list_add_text(_list, "Nenhum dispositivo encontrado"); }
}

static void scan_timer_cb(lv_timer_t *t) {
    if (!_active) return;

    // Check if results ready?
    // Current ble_driver hack: getScanResults pulls from lib buffer
    std::vector<BLEDeviceData> devices = ble_driver.getScanResults();
    if (devices.size() > 0) {
        if (_spinner) {
            lv_obj_del(_spinner);
            _spinner = nullptr;
        }
        update_list();
        // Stop timer?
        // lv_timer_del(t);
        // _timer = nullptr;
    }
}

static void btn_scan_start_cb(lv_event_t *e) {
    // Start Async Scan
    if (_spinner) lv_obj_del(_spinner);
    _spinner = lv_spinner_create(_screen, 1000, 60);
    lv_obj_set_size(_spinner, 50, 50);
    lv_obj_center(_spinner);

    lv_obj_clean(_list);

    ble_driver.startScanAsync(5); // 5 seconds
}

static void btn_back_cb(lv_event_t *e) {
    ui_ble_scan_hide();
    ui_menu_ble_show();
}

void ui_ble_scan_init() {
    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);

    // Header
    lv_obj_t *lbl = lv_label_create(_screen);
    lv_label_set_text(lbl, "BLE Victim List");
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x00ff00), 0);

    // List
    _list = lv_list_create(_screen);
    lv_obj_set_size(_list, lv_pct(100), lv_pct(75));
    lv_obj_align(_list, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(_list, lv_color_hex(0x111111), 0);

    // Scan Button
    _btn_scan = lv_btn_create(_screen);
    lv_obj_align(_btn_scan, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_size(_btn_scan, 80, 40);
    lv_label_set_text(lv_label_create(_btn_scan), "SCAN");
    lv_obj_add_event_cb(_btn_scan, btn_scan_start_cb, LV_EVENT_CLICKED, nullptr);

    // Back Button
    lv_obj_t *btn_back = lv_btn_create(_screen);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_size(btn_back, 80, 40);
    lv_label_set_text(lv_label_create(btn_back), "BACK");
    lv_obj_add_event_cb(btn_back, btn_back_cb, LV_EVENT_CLICKED, nullptr);
}

void ui_ble_scan_show() {
    if (!_screen) ui_ble_scan_init();
    _active = true;
    lv_scr_load(_screen);

    // Auto start scan?
    // btn_scan_start_cb(nullptr);

    _timer = lv_timer_create(scan_timer_cb, 1000, nullptr);
}

void ui_ble_scan_hide() {
    _active = false;
    if (_timer) {
        lv_timer_del(_timer);
        _timer = nullptr;
    }
}

bool ui_ble_scan_is_active() { return _active; }
