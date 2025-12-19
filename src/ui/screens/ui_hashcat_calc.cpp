#include "ui_hashcat_calc.h"
#include "../ui_helpers.h"
#include "../ui_main.h"
#include <cstdio>

static lv_obj_t *ui_HashcatScreen;
static lv_obj_t *ui_SsidInput;
static lv_obj_t *ui_ResultLabel;

static void analyze_mask(const char *ssid) {
  // Mock logic for mask calculation
  // "NETGEAR23" -> ?u?u?u?u?u?u?u?d?d
  // "Vivo-2948" -> ?u?l?l?l?s?d?d?d?d

  char mask[64] = "";
  int len = strlen(ssid);
  bool mixed = false;

  for (int i = 0; i < len; i++) {
    if (ssid[i] >= '0' && ssid[i] <= '9')
      strcat(mask, "?d");
    else if (ssid[i] >= 'a' && ssid[i] <= 'z')
      strcat(mask, "?l");
    else if (ssid[i] >= 'A' && ssid[i] <= 'Z')
      strcat(mask, "?u");
    else
      strcat(mask, "?s");
  }

  lv_label_set_text_fmt(ui_ResultLabel,
                        "Mask:\n%s\n\nEst. Time (RTX 4090):\n< 1 second", mask);
}

static void calc_event_handler(lv_event_t *e) {
  const char *txt = lv_textarea_get_text(ui_SsidInput);
  analyze_mask(txt);
}

void ui_hashcat_calc_init(void) {
  ui_HashcatScreen = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(ui_HashcatScreen);
  lv_label_set_text(title, "HASHCAT CALC");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  ui_SsidInput = lv_textarea_create(ui_HashcatScreen);
  lv_textarea_set_placeholder_text(ui_SsidInput, "Enter SSID e.g. Vivo-5G");
  lv_textarea_set_one_line(ui_SsidInput, true);
  lv_obj_set_width(ui_SsidInput, LV_PCT(90));
  lv_obj_align(ui_SsidInput, LV_ALIGN_TOP_MID, 0, 40);

  lv_obj_t *btn = lv_btn_create(ui_HashcatScreen);
  lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 90);
  lv_obj_t *l = lv_label_create(btn);
  lv_label_set_text(l, "ANALYZE");
  lv_obj_center(l);
  lv_obj_add_event_cb(btn, calc_event_handler, LV_EVENT_CLICKED, NULL);

  ui_ResultLabel = lv_label_create(ui_HashcatScreen);
  lv_label_set_text(ui_ResultLabel, "Result will appear here.");
  lv_obj_set_width(ui_ResultLabel, LV_PCT(90));
  lv_label_set_long_mode(ui_ResultLabel, LV_LABEL_LONG_WRAP);
  lv_obj_align(ui_ResultLabel, LV_ALIGN_BOTTOM_MID, 0, -60);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_HashcatScreen);
  lv_obj_set_size(backBtn, 60, 40);
  lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
  l = lv_label_create(backBtn);
  lv_label_set_text(l, "BACK");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
