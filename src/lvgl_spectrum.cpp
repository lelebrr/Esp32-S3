#include "lvgl_spectrum.h"
#include "lvgl_menu.h"
#include "rf_core.h"
#include <WiFi.h>
#include <lvgl.h>

static lv_obj_t *chart_spectrum = NULL;
static lv_chart_series_t *ser1 = NULL;
static lv_timer_t *spectrum_timer = NULL;

// Real data generation from RFCore
static void spectrum_timer_cb(lv_timer_t *timer) {
    if (!chart_spectrum) return;

    // Drive the scanner from the UI thread (scanning chunk by chunk)
    // Perform 10 scan steps per UI update (approx 20ms blocking)
    for (int k = 0; k < 10; k++) { RFCore::updateScanner(); }

    // Get latest data from RFCore
    SpectrumData data = RFCore::getSpectrumData();

    // Check if we have valid data
    if (data.numSamples > 0) {
        // Map 128 samples to 14 bars (simplified)
        for (int i = 0; i < 14; i++) {
            // Average a few samples per bar
            int startIdx = i * (data.numSamples / 14);
            int endIdx = (i + 1) * (data.numSamples / 14);
            int sum = 0;
            int count = 0;

            for (int j = startIdx; j < endIdx && j < 128; j++) {
                sum += (data.rssiValues[j] + 130); // Normalize -130..0 to 0..130
                count++;
            }

            int val = 0;
            if (count > 0) val = sum / count;

            // Scaling for chart (0-100)
            if (val < 0) val = 0;
            if (val > 100) val = 100;

            lv_chart_set_next_value(chart_spectrum, ser1, val);
        }
    } else {
        // No data yet, show flat line
        for (int i = 0; i < 14; i++) { lv_chart_set_next_value(chart_spectrum, ser1, 0); }
    }

    lv_chart_refresh(chart_spectrum);
}

void setup_spectrum_ui() {
    // Basic setup if needed
}

void show_spectrum_view() {
    // Start RF Scan
    if (RFCore::isInitialized()) {
        RFCore::startFrequencyScan(2400.0, 2483.5, 1.0); // 2.4GHz WiFi Band scan
    }

    // Create a new screen or container
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);

    // Title
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "2.4GHz Spectrum");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    // Close button
    lv_obj_t *btn_close = lv_btn_create(cont);
    lv_obj_set_size(btn_close, 30, 30);
    lv_obj_align(btn_close, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(
        btn_close,
        [](lv_event_t *e) {
            RFCore::stopFrequencyScan();
            lv_obj_del(lv_obj_get_parent(lv_event_get_target(e)));
        },
        LV_EVENT_CLICKED,
        NULL
    );
    lv_obj_t *lbl_close = lv_label_create(btn_close);
    lv_label_set_text(lbl_close, "X");
    lv_obj_center(lbl_close);

    // Chart
    chart_spectrum = lv_chart_create(cont);
    lv_obj_set_size(chart_spectrum, 260, 160);
    lv_obj_align(chart_spectrum, LV_ALIGN_CENTER, 0, 10);
    lv_chart_set_type(chart_spectrum, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart_spectrum, 14);
    lv_chart_set_range(chart_spectrum, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

    // Add data series
    ser1 = lv_chart_add_series(chart_spectrum, lv_color_hex(0x00FF00), LV_CHART_AXIS_PRIMARY_Y);

    // Start update timer
    spectrum_timer = lv_timer_create(spectrum_timer_cb, 100, NULL);
}
