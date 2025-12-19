#include "ui_party.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_PartyScreen;
static lv_timer_t *strobe_timer = nullptr;
static bool color_state = false;

static void strobe_anim(lv_timer_t *t) {
  color_state = !color_state;
  // Flash Screen Background
  if (color_state) {
    lv_obj_set_style_bg_color(ui_PartyScreen, lv_color_hex(0xFFFFFF), 0);
  } else {
    lv_obj_set_style_bg_color(ui_PartyScreen,
                              lv_palette_main((lv_palette_t)(rand() % 10)), 0);
  }
}

void ui_party_init(void) {
  if (strobe_timer)
    lv_timer_del(strobe_timer);

  ui_PartyScreen = lv_obj_create(NULL);

  lv_obj_t *lbl = lv_label_create(ui_PartyScreen);
  lv_label_set_text(lbl, "PARTY MODE");
  lv_obj_center(lbl);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_28, 0);

  strobe_timer = lv_timer_create(strobe_anim, 100, NULL);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_PartyScreen);
  lv_obj_set_size(backBtn, 50, 40);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (strobe_timer)
          lv_timer_del(strobe_timer);
        strobe_timer = nullptr;
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);

  lv_scr_load_anim(ui_PartyScreen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, true);
}
