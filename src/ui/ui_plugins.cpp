#include "ui_plugins.h"
#include "globals.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"


static lv_obj_t *scr_plugins = nullptr;

static void create_plugin_card(lv_obj_t *parent, const char *name,
                               const char *icon, bool active, int col,
                               int row) {
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_grid_cell(card, LV_GRID_ALIGN_STRETCH, col, 1,
                       LV_GRID_ALIGN_STRETCH, row, 1);

  ui_apply_glass_effect(card);

  if (active) {
    lv_obj_set_style_border_color(card, COLOR_NEON_GREEN, 0);
    lv_obj_set_style_shadow_color(card, COLOR_NEON_GREEN, 0);
    lv_obj_set_style_shadow_width(card, 15, 0);
  } else {
    lv_obj_set_style_border_color(card, lv_color_hex(0x444444), 0);
  }

  lv_obj_t *lbl_icon = lv_label_create(card);
  lv_label_set_text(lbl_icon, icon);
  lv_obj_set_style_text_font(lbl_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(lbl_icon, LV_ALIGN_CENTER, 0, -10);

  lv_obj_t *lbl_name = lv_label_create(card);
  lv_label_set_text(lbl_name, name);
  lv_obj_set_style_text_font(lbl_name, &lv_font_montserrat_10, 0);
  lv_obj_align(lbl_name, LV_ALIGN_BOTTOM_MID, 0, 5);
}

void ui_plugins_show() {
  scr_plugins = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_plugins, lv_color_hex(0x111111), 0);

  // Header
  lv_obj_t *header = lv_obj_create(scr_plugins);
  lv_obj_set_size(header, LCD_WIDTH, 50);
  lv_obj_set_style_bg_color(header, lv_color_hex(0x222222), 0);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "PLUGINS");
  lv_obj_center(title);

  ui_create_back_btn(scr_plugins, [](lv_event_t *e) { ui_main_show(); });

  // Grid Container
  lv_obj_t *cont = lv_obj_create(scr_plugins);
  lv_obj_set_size(cont, LCD_WIDTH, LCD_HEIGHT - 50);
  lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);

  static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};

  lv_obj_set_layout(cont, LV_LAYOUT_GRID);
  lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
  lv_obj_set_style_pad_all(cont, 10, 0);
  lv_obj_set_style_pad_gap(cont, 10, 0);

  // Mock Plugins
  create_plugin_card(cont, "MemTemp", LV_SYMBOL_SD_CARD, true, 0, 0);
  create_plugin_card(cont, "Clock", LV_SYMBOL_LOOP, true, 1, 0);
  create_plugin_card(cont, "Airmon", LV_SYMBOL_WIFI, false, 2, 0);
  create_plugin_card(cont, "GPS", LV_SYMBOL_GPS, false, 0, 1);
  create_plugin_card(cont, "Log", LV_SYMBOL_LIST, true, 1, 1);
  create_plugin_card(cont, "Game", LV_SYMBOL_PLAY, false, 2, 1);

  lv_scr_load(scr_plugins);
}
