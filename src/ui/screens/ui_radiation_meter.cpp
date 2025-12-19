#include "ui_radiation_meter.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_RadScreen;
static lv_obj_t *ui_RadGauge;
static lv_obj_t *ui_RadLabel;
static lv_timer_t *rad_timer;

static void rad_update(lv_timer_t *t) {
  // Simulate reading RSSI from environment
  // Higher RSSI (closer to 0) = More Radiation
  int rssi = -90 + (rand() % 60); // -90 to -30

  // Convert to uW/cm2 (very approx)
  float power = pow(10, (rssi / 10.0)); // mW
  // Just show a relative scale

  lv_bar_set_value(ui_RadGauge, (rssi + 100), LV_ANIM_ON);

  const char *status = "SAFE";
  if (rssi > -50)
    status = "HIGH";
  else if (rssi > -70)
    status = "MODERATE";

  lv_label_set_text_fmt(ui_RadLabel, "%d dBm\n%s", rssi, status);

  if (rssi > -40) {
    // Trigger melting dragon animation if exists
  }
}

void ui_radiation_meter_init(void) {
  ui_RadScreen = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(ui_RadScreen);
  lv_label_set_text(title, "RADIATION METER");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  ui_RadGauge = lv_bar_create(ui_RadScreen);
  lv_obj_set_size(ui_RadGauge, 20, 150);
  lv_obj_align(ui_RadGauge, LV_ALIGN_CENTER, -50, 0);
  lv_bar_set_range(ui_RadGauge, 0, 100); // -100 to 0 normalized

  ui_RadLabel = lv_label_create(ui_RadScreen);
  lv_label_set_text(ui_RadLabel, "Scanning...");
  lv_obj_align(ui_RadLabel, LV_ALIGN_CENTER, 30, 0);
  lv_obj_set_style_text_font(ui_RadLabel, &lv_font_montserrat_24, 0);

  rad_timer = lv_timer_create(rad_update, 200, NULL);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_RadScreen);
  lv_obj_set_size(backBtn, 60, 40);
  lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "BACK");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (rad_timer)
          lv_timer_del(rad_timer);
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);
}
