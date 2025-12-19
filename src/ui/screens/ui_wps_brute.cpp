#include "ui_wps_brute.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_WpsScreen;
static lv_obj_t *ui_LogTA;
static lv_timer_t *wps_timer;

static void wps_attack_sim(lv_timer_t *t) {
  // Simulation of Pixie Dust or Online Brute Force
  if (rand() % 100 < 5) {
    lv_textarea_add_text(ui_LogTA, "[+] PIN FOUND: 12345670\n");
    lv_timer_del(t);
    return;
  }

  char buf[32];
  sprintf(buf, "[*] Try: %08d\n", rand() % 99999999);
  lv_textarea_add_text(ui_LogTA, buf);
}

void ui_wps_brute_init(void) {
  ui_WpsScreen = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(ui_WpsScreen);
  lv_label_set_text(title, "WPS BRUTE FORCE");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  ui_LogTA = lv_textarea_create(ui_WpsScreen);
  lv_obj_set_size(ui_LogTA, LV_PCT(95), LV_PCT(60));
  lv_obj_align(ui_LogTA, LV_ALIGN_CENTER, 0, 10);
  lv_obj_add_state(ui_LogTA, LV_STATE_DISABLED); // LVGL 8.x: disable editing
  lv_textarea_set_text(ui_LogTA, "Target: TP-Link_28A4\nStarting...\n");

  lv_obj_t *startBtn = lv_btn_create(ui_WpsScreen);
  lv_obj_align(startBtn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
  lv_obj_t *l = lv_label_create(startBtn);
  lv_label_set_text(l, "START");
  lv_obj_center(l);

  lv_obj_add_event_cb(
      startBtn,
      [](lv_event_t *e) {
        wps_timer = lv_timer_create(wps_attack_sim, 500, NULL);
      },
      LV_EVENT_CLICKED, NULL);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_WpsScreen);
  lv_obj_set_size(backBtn, 60, 40);
  lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
  l = lv_label_create(backBtn);
  lv_label_set_text(l, "BACK");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (wps_timer)
          lv_timer_del(wps_timer);
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);
}
