#include "ui_charging.h"

#include "ui_helpers.h"
#include "../hardware/system_hardware.h" // Para sys_hw


static lv_obj_t *scr_charging = nullptr;
static lv_obj_t *bar_batt = nullptr;
static lv_obj_t *dragon = nullptr;
static lv_timer_t *anim_timer = nullptr;
static int fire_frame = 0;

static void fire_anim_cb(lv_timer_t *t) {
  if (!scr_charging)
    return;

  // Dragon Fire Animation (ASCII)
  const char *fires[] = {"( >o<) >", "( >o<) >>", "( >o<) >>>", "( >o<) >>>>"};
  if (dragon)
    lv_label_set_text(dragon, fires[fire_frame % 4]);
  fire_frame++;

  // Update Battery
  if (bar_batt) {
    int pct = sys_hw.getBatteryPercent();
    lv_bar_set_value(bar_batt, pct, LV_ANIM_ON);
  }
}

void ui_charging_show() {
  if (scr_charging) {
    lv_scr_load(scr_charging);
    return;
  }

  scr_charging = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_charging, lv_color_black(), 0);

  // Dragon
  dragon = lv_label_create(scr_charging);
  lv_obj_set_style_text_font(dragon, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(dragon, lv_color_hex(0xFF4500), 0); // Orange Red
  lv_obj_center(dragon);

  // Battery Bar
  bar_batt = lv_bar_create(scr_charging);
  lv_obj_set_size(bar_batt, 200, 20);
  lv_obj_align(bar_batt, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_set_style_bg_color(bar_batt, lv_color_hex(0x222222), LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_batt, lv_color_hex(0x00FF00),
                            LV_PART_INDICATOR);

  anim_timer = lv_timer_create(fire_anim_cb, 200, nullptr);

  lv_scr_load(scr_charging);
}
