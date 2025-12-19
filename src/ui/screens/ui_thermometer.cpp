#include "ui_thermometer.h"
#include "../ui_helpers.h"
#include "../ui_main.h"
#include <Arduino.h>

static lv_obj_t *ui_ThermoScreen;
static lv_obj_t *ui_TempLabel;
static lv_timer_t *temp_timer = nullptr;

static void update_temp(lv_timer_t *t) {
  float temp = temperatureRead(); // ESP32 internal temp
  // Calibration offset (ESP32 runs hot)
  temp -= 20.0;

  lv_label_set_text_fmt(ui_TempLabel, "%.1f°C", temp);

  // Color scale
  if (temp < 30)
    lv_obj_set_style_text_color(ui_TempLabel, lv_color_hex(0x00FFFF), 0);
  else if (temp < 45)
    lv_obj_set_style_text_color(ui_TempLabel, lv_color_hex(0x00FF00), 0);
  else
    lv_obj_set_style_text_color(ui_TempLabel, lv_color_hex(0xFF0000), 0);
}

void ui_thermometer_init(void) {
  if (temp_timer)
    lv_timer_del(temp_timer);

  ui_ThermoScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_ThermoScreen, lv_color_hex(0x000000), 0);

  lv_obj_t *title = lv_label_create(ui_ThermoScreen);
  lv_label_set_text(title, "CPU TEMP");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

  ui_TempLabel = lv_label_create(ui_ThermoScreen);
  lv_label_set_text(ui_TempLabel, "--.-°C");
  lv_obj_center(ui_TempLabel);
  lv_obj_set_style_text_font(ui_TempLabel, &lv_font_montserrat_48, 0);

  temp_timer = lv_timer_create(update_temp, 1000, NULL);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_ThermoScreen);
  lv_obj_set_size(backBtn, 50, 40);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (temp_timer)
          lv_timer_del(temp_timer);
        temp_timer = nullptr;
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);

  lv_scr_load_anim(ui_ThermoScreen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, true);
}
