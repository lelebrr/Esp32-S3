#include "ui_ota.h"

#include "ui_helpers.h"
#include "ui_main.h" // To force switch


static lv_obj_t *scr_ota = nullptr;
static lv_obj_t *bar_ota = nullptr;
static lv_obj_t *lbl_pct = nullptr;
static lv_obj_t *dragon_ota = nullptr;
static lv_timer_t *sweat_timer = nullptr;
static int sweat_frame = 0;

static void sweat_anim_cb(lv_timer_t *t) {
  if (!scr_ota)
    return;

  // Sweating Dragon Animation (ASCII)
  // (;;>_<;;) -> (;;>_<;;)
  const char *sweats[] = {"(;;>_<)", "( ;;>_<)", "(  ;;>_<)", "(   ;;>_<)"};
  if (dragon_ota)
    lv_label_set_text(dragon_ota, sweats[sweat_frame % 4]);
  sweat_frame++;
}

void ui_ota_show() {
  if (scr_ota) {
    lv_scr_load(scr_ota);
    return;
  }

  scr_ota = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_ota, lv_color_black(), 0);

  // Dragon
  dragon_ota = lv_label_create(scr_ota);
  lv_obj_set_style_text_font(dragon_ota, &lv_font_montserrat_28, 0);
  // Use Warning Color or Red
  lv_obj_set_style_text_color(dragon_ota, lv_color_hex(0xFF4500), 0);
  lv_obj_align(dragon_ota, LV_ALIGN_CENTER, 0, -30);

  // Warning Text
  lv_obj_t *warn = lv_label_create(scr_ota);
  lv_label_set_text(warn, "UPDATING...\nDON'T DISCONNECT!");
  lv_obj_set_style_text_align(warn, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(warn, lv_color_white(), 0);
  lv_obj_align(warn, LV_ALIGN_CENTER, 0, 20);

  // Progress Bar
  bar_ota = lv_bar_create(scr_ota);
  lv_obj_set_size(bar_ota, 260, 10);
  lv_obj_align(bar_ota, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_set_style_bg_color(bar_ota, lv_color_hex(0x222222), LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_ota, lv_color_hex(0x00FFFF), LV_PART_INDICATOR);

  lbl_pct = lv_label_create(scr_ota);
  lv_label_set_text(lbl_pct, "0%");
  lv_obj_align_to(lbl_pct, bar_ota, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

  sweat_timer = lv_timer_create(sweat_anim_cb, 300, nullptr);

  lv_scr_load(scr_ota);
}

void ui_ota_update_progress(int progress) {
  if (!scr_ota)
    ui_ota_show();
  if (bar_ota)
    lv_bar_set_value(bar_ota, progress, LV_ANIM_ON);
  if (lbl_pct)
    lv_label_set_text_fmt(lbl_pct, "%d%%", progress);
}
