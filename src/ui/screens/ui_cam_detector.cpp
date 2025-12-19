#include "ui_cam_detector.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_CamScreen;
static lv_obj_t *ui_CamList;
static lv_timer_t *cam_scan_timer;

static void cam_scan_task(lv_timer_t *t) {
  // Simulate finding RTSP ports
  if (rand() % 100 < 10) {
    lv_obj_t *btn =
        lv_list_add_btn(ui_CamList, LV_SYMBOL_VIDEO, "Cam 192.168.1.50");
    lv_obj_t *l = lv_label_create(btn);
    lv_label_set_text(l, "Port 554 Open");
    lv_obj_align(l, LV_ALIGN_RIGHT_MID, 0, 0);
  }
}

void ui_cam_detector_init(void) {
  ui_CamScreen = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(ui_CamScreen);
  lv_label_set_text(title, "IP CAM DETECTOR");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  ui_CamList = lv_list_create(ui_CamScreen);
  lv_obj_set_size(ui_CamList, LV_PCT(100), LV_PCT(70));
  lv_obj_align(ui_CamList, LV_ALIGN_CENTER, 0, 10);

  cam_scan_timer = lv_timer_create(cam_scan_task, 1000, NULL);

  // Back
  lv_obj_t *backBtn = lv_btn_create(ui_CamScreen);
  lv_obj_set_size(backBtn, 60, 40);
  lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "BACK");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (cam_scan_timer)
          lv_timer_del(cam_scan_timer);
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);
}
