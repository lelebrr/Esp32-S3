#include "ui_menu_offline.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

// Includes for sub-screens
#include "ui_cam_detector.h"
#include "ui_chat.h"
#include "ui_credits.h"
#include "ui_dead_drop.h"
#include "ui_dice.h"
#include "ui_flappy_dragon.h"
#include "ui_game_capture.h"
#include "ui_handshake_transfer.h"
#include "ui_hashcat_calc.h"
#include "ui_matrix.h"
#include "ui_party.h"
#include "ui_password_gen.h"
#include "ui_proximity.h"
#include "ui_radiation_meter.h"
#include "ui_snake.h"
#include "ui_thermometer.h"
#include "ui_walkie_talkie.h"
#include "ui_wps_brute.h"

static lv_obj_t *ui_MenuOffline;

static void menu_btn_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    intptr_t id = (intptr_t)lv_obj_get_user_data(obj);
    switch (id) {
    case 1:
      ui_walkie_talkie_screen_init();
      break;
    case 2:
      ui_chat_screen_init();
      break;
    case 3:
      ui_handshake_transfer_screen_init();
      break;
    case 4:
      ui_capture_handshake_game_init();
      break;
    case 5:
      ui_dead_drop_init();
      break;
    case 6:
      ui_proximity_screen_init();
      break;
    case 7:
      ui_password_gen_init();
      break;
    case 8:
      ui_hashcat_calc_init();
      break;
    case 9:
      ui_wps_brute_init();
      break;
    case 10:
      ui_cam_detector_init();
      break;
    case 11:
      ui_radiation_meter_init();
      break;
    case 12:
      ui_flappy_dragon_init();
      break;
    case 13:
      ui_snake_init();
      break;
    case 14:
      ui_dice_init();
      break;
    case 15:
      ui_party_init();
      break;
    case 16:
      ui_thermometer_init();
      break;
    case 17:
      ui_matrix_init();
      break;
    case 18:
      ui_credits_init();
      break;
    default:
      break;
    }
  }
}

void ui_menu_offline_init(void) {
  ui_MenuOffline = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(ui_MenuOffline);
  lv_label_set_text(title, "OFFLINE MODE");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);

  lv_obj_t *list = lv_list_create(ui_MenuOffline);
  lv_obj_set_size(list, LV_PCT(100), LV_PCT(80));
  lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex(0x000000), 0);

  // Helpers to add buttons
  auto add_btn = [&](const char *icon, const char *txt, int id) {
    lv_obj_t *btn = lv_list_add_btn(list, icon, txt);
    lv_obj_set_user_data(btn, (void *)(intptr_t)id);
    lv_obj_add_event_cb(btn, menu_btn_event_handler, LV_EVENT_CLICKED, NULL);
    return btn;
  };

  add_btn(LV_SYMBOL_AUDIO, "Walkie Talkie", 1);
  add_btn(LV_SYMBOL_ENVELOPE, "Mesh Chat", 2);
  add_btn(LV_SYMBOL_UPLOAD, "Transfer Handshake", 3);
  add_btn(LV_SYMBOL_GPS, "Global Game", 4);
  add_btn(LV_SYMBOL_DOWNLOAD, "Dead Drop", 5);
  add_btn(LV_SYMBOL_WIFI, "Proximity Radar", 6);
  add_btn(LV_SYMBOL_SHUFFLE, "Pass Generator", 7);
  add_btn(LV_SYMBOL_EDIT, "Hashcat Calc", 8);
  add_btn(LV_SYMBOL_WARNING, "WPS Brute", 9);
  add_btn(LV_SYMBOL_VIDEO, "Cam Detector", 10);
  add_btn(LV_SYMBOL_BELL, "Radiation Meter", 11);
  add_btn("🐉", "Flappy Dragon", 12);
  add_btn("🐍", "Snake", 13);
  add_btn("🎲", "Dice Roller", 14);
  add_btn("🎉", "Party Mode", 15);
  add_btn("🌡️", "Thermometer", 16);
  add_btn("💻", "Matrix Effect", 17);
  add_btn("ℹ️", "Credits", 18);

  lv_obj_t *backBtn = lv_btn_create(ui_MenuOffline);
  lv_obj_set_size(backBtn, 50, 30);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
