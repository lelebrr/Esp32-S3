#include "ui_dice.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_DiceScreen;
static lv_obj_t *ui_ResultLabel;
static lv_obj_t *ui_TypeLabel;
static int current_die = 20;

static void roll_anim(lv_timer_t *t) {
  int *count = (int *)t->user_data;
  (*count)--;

  int val = (rand() % current_die) + 1;
  lv_label_set_text_fmt(ui_ResultLabel, "%d", val);

  if (*count <= 0) {
    delete count;
    lv_timer_del(t);
    // Clean final number style
    lv_obj_set_style_text_color(ui_ResultLabel, lv_color_hex(0x00FF00), 0);
  }
}

static void roll_click(lv_event_t *e) {
  lv_obj_set_style_text_color(ui_ResultLabel, lv_color_hex(0xFFFFFF), 0);
  int *count = new int(10); // 10 animation steps
  lv_timer_create(roll_anim, 50, count);
}

static void type_click(lv_event_t *e) {
  int val = (int)(intptr_t)lv_event_get_user_data(e);
  current_die = val;
  lv_label_set_text_fmt(ui_TypeLabel, "D%d", current_die);
  lv_label_set_text(ui_ResultLabel, "?");
}

void ui_dice_init(void) {
  ui_DiceScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_DiceScreen, lv_color_hex(0x101010), 0);

  ui_TypeLabel = lv_label_create(ui_DiceScreen);
  lv_label_set_text(ui_TypeLabel, "D20");
  lv_obj_align(ui_TypeLabel, LV_ALIGN_TOP_MID, 0, 20);
  lv_obj_set_style_text_font(ui_TypeLabel, &lv_font_montserrat_28, 0);

  ui_ResultLabel = lv_label_create(ui_DiceScreen);
  lv_label_set_text(ui_ResultLabel, "?");
  lv_obj_center(ui_ResultLabel);
  lv_obj_set_style_text_font(ui_ResultLabel, &lv_font_montserrat_48, 0);

  lv_obj_t *btn = lv_btn_create(ui_DiceScreen);
  lv_obj_set_size(btn, 120, 50);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, "ROLL");
  lv_obj_center(lbl);
  lv_obj_add_event_cb(btn, roll_click, LV_EVENT_CLICKED, NULL);

  // Type selector
  lv_obj_t *list = lv_obj_create(ui_DiceScreen);
  lv_obj_set_size(list, 220, 60);
  lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_ROW);

  int types[] = {4, 6, 8, 10, 12, 20, 100};
  for (int t : types) {
    lv_obj_t *b = lv_btn_create(list);
    lv_obj_set_size(b, 50, 40);
    lv_obj_add_event_cb(b, type_click, LV_EVENT_CLICKED, (void *)(intptr_t)t);
    lv_obj_t *l = lv_label_create(b);
    lv_label_set_text_fmt(l, "D%d", t);
    lv_obj_center(l);
  }

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_DiceScreen);
  lv_obj_set_size(backBtn, 50, 40);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);

  lv_scr_load_anim(ui_DiceScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
}
