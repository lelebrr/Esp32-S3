#include "ui_leaderboard.h"
#include "globals.h" // For g_state (stats)
#include "ui_helpers.h"
#include "ui_themes.h"

static void lb_close_cb(lv_event_t *e) {
  lv_obj_t *overlay = (lv_obj_t *)lv_event_get_user_data(e);
  if (overlay)
    lv_obj_del(overlay);
}

void ui_leaderboard_show() {
  lv_obj_t *act_scr = lv_scr_act();

  // Transparent dark overlay
  lv_obj_t *overlay =
      lv_obj_create(lv_layer_top()); // Use top layer for global overlay
  lv_obj_set_size(overlay, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(overlay, 200, 0); // High opacity to hide bg
  lv_obj_add_event_cb(overlay, lb_close_cb, LV_EVENT_CLICKED, overlay);

  // Modal Box
  lv_obj_t *modal = lv_obj_create(overlay);
  lv_obj_set_size(modal, 260, 240);
  lv_obj_center(modal);
  lv_obj_set_style_bg_color(modal, lv_color_hex(0x111111), 0);
  lv_obj_set_style_border_color(modal, lv_color_hex(0xFFD700), 0); // Gold
  lv_obj_set_style_border_width(modal, 2, 0);
  lv_obj_set_style_shadow_width(modal, 20, 0);
  lv_obj_set_style_shadow_color(modal, lv_color_hex(0xFFD700), 0);

  // Title
  lv_obj_t *lbl_title = lv_label_create(modal);
  lv_label_set_text(lbl_title, "LEADERBOARD 👑");
  lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xFFD700), 0);
  lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_20, 0);
  lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 5);

  // Stats Content
  lv_obj_t *cont = lv_obj_create(modal);
  lv_obj_set_size(cont, lv_pct(100), 120);
  lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 40);
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont, 0, 0);
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

  // Rows
  const char *titles[] = {"Handshakes Rec", "Clients Kick", "Uptime Record"};
  char vals[3][32];
  snprintf(vals[0], 32, "%d",
           g_state.handshakes_captured);        // Using current valid global
  snprintf(vals[1], 32, "42");                  // Mock
  snprintf(vals[2], 32, "%02dh %02dm", 12, 34); // Mock

  for (int i = 0; i < 3; i++) {
    lv_obj_t *row = lv_obj_create(cont);
    lv_obj_set_size(row, lv_pct(100), 30);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *l1 = lv_label_create(row);
    lv_label_set_text(l1, titles[i]);
    lv_obj_set_style_text_color(l1, lv_color_white(), 0);
    lv_obj_align(l1, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *l2 = lv_label_create(row);
    lv_label_set_text(l2, vals[i]);
    lv_obj_set_style_text_color(l2, COLOR_NEON_GREEN, 0);
    lv_obj_align(l2, LV_ALIGN_RIGHT_MID, 0, 0);
  }

  // Close Button
  lv_obj_t *btn_close = lv_btn_create(modal);
  lv_obj_set_size(btn_close, 100, 40);
  lv_obj_align(btn_close, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(btn_close, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(btn_close, lb_close_cb, LV_EVENT_CLICKED, overlay);

  lv_obj_t *lbl_close = lv_label_create(btn_close);
  lv_label_set_text(lbl_close, "FECHAR");
  lv_obj_center(lbl_close);
}
