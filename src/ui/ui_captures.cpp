#include "ui_captures.h"
#include "globals.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"

static lv_obj_t *scr_captures = nullptr;

static void create_capture_item(lv_obj_t *parent, const char *ssid,
                                const char *bssid, int pwr) {
  lv_obj_t *item = lv_obj_create(parent);
  lv_obj_set_size(item, lv_pct(100), 70);
  lv_obj_set_style_bg_color(item, lv_color_hex(0x202020), 0);
  lv_obj_set_style_border_width(item, 0, 0);
  lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *lbl_ssid = lv_label_create(item);
  lv_label_set_text(lbl_ssid, ssid);
  lv_obj_set_style_text_font(lbl_ssid, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(lbl_ssid, lv_color_white(), 0);
  lv_obj_align(lbl_ssid, LV_ALIGN_TOP_LEFT, 5, 5);

  lv_obj_t *lbl_bssid = lv_label_create(item);
  lv_label_set_text_fmt(lbl_bssid, "MAC: %s", bssid);
  lv_obj_set_style_text_font(lbl_bssid, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lbl_bssid, lv_color_hex(0xaaaaaa), 0);
  lv_obj_align(lbl_bssid, LV_ALIGN_BOTTOM_LEFT, 5, -5);

  // Icon type
  lv_obj_t *icon = lv_label_create(item);
  lv_label_set_text(icon, LV_SYMBOL_FILE);
  lv_obj_set_style_text_color(icon, COLOR_NEON_GREEN, 0);
  lv_obj_align(icon, LV_ALIGN_RIGHT_MID, -10, 0);
}

void ui_captures_show() {
  scr_captures = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_captures, lv_color_hex(0x111111), 0);

  // Header
  lv_obj_t *header = lv_obj_create(scr_captures);
  lv_obj_set_size(header, LCD_WIDTH, 50);
  lv_obj_set_style_bg_color(header, lv_color_hex(0x222222), 0);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "CAPTURES");
  lv_obj_center(title);

  // Back Button (Use standard helper if available, redefining simple for now or
  // use include)
  ui_create_back_btn(scr_captures, [](lv_event_t *e) {
    // ui_switch_screen(ui_get_previous_screen_obj(),
    // LV_SCR_LOAD_ANIM_MOVE_RIGHT);
    ui_main_show();
  });
  // Wait, ui_create_back_btn calls a callback. I need to handle navigation
  // properly. Ideally ui_captures should be part of the flow. For now simple
  // back to Main.

  // List
  lv_obj_t *list = lv_obj_create(scr_captures);
  lv_obj_set_size(list, LCD_WIDTH, LCD_HEIGHT - 50);
  lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

  // Fake Data for UI Demo (Tip 39)
  create_capture_item(list, "Neighbor_WiFi", "AA:BB:CC:DD:EE:FF", -60);
  create_capture_item(list, "Coffee_Free", "11:22:33:44:55:66", -75);
  create_capture_item(list, "Enterprise_Corp", "99:88:77:66:55:44", -50);

  lv_scr_load(scr_captures);
}
