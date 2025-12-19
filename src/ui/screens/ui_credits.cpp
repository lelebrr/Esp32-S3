/*
 * 🐉 LELEWATCH CREDITS
 * 
 * Onde a mágica acontece.
 * Respeite os devs.
 */
#include "ui_credits.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_CreditsScreen;
static lv_obj_t *ui_Roller;

void ui_credits_init(void) {
  ui_CreditsScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_CreditsScreen, lv_color_hex(0x000000), 0);

  lv_obj_t *title = lv_label_create(ui_CreditsScreen);
  lv_label_set_text(title, "CREDITS");
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFD700), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);

  lv_obj_t *txt = lv_label_create(ui_CreditsScreen);
  lv_label_set_text(txt, "LeleWatch v2.1\n\n"
                         "Project based on ESP32-S3\n"
                         "Touch AMOLED 1.8\n\n"
                         "Developed by:\n"
                         "Lele\n\n"
                         "2025 (c) LeleWatch");
  lv_obj_set_style_text_align(txt, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(txt, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(txt, LV_ALIGN_CENTER, 0, 20);

  // Animation to scroll up?
  // Use lv_anim later.

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_CreditsScreen);
  lv_obj_set_size(backBtn, 50, 40);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);

  lv_scr_load_anim(ui_CreditsScreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
}
