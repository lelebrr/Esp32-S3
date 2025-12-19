#include "ui_helpers.h"
#include "globals.h"
#include "ui_main.h"
#include "ui_themes.h"

void ui_switch_screen(lv_obj_t *target_screen, lv_scr_load_anim_t anim_type,
                      uint32_t time, uint32_t delay) {
  if (!target_screen)
    return;
  lv_scr_load_anim(target_screen, anim_type, time, delay, false);
}

void ui_apply_glass_effect(lv_obj_t *obj) {
  if (!obj)
    return;

  // Obtém opacidade do tema
  uint8_t opa = getTheme().glass_opacity;
  if (opa == 0)
    opa = 200; // Fallback se não definido

  lv_obj_set_style_bg_opa(obj, opa, 0);
  lv_obj_set_style_bg_color(obj, getTheme().panel, 0);

  // Borda fina semi-transparente para efeito de vidro
  lv_obj_set_style_border_width(obj, 1, 0);
  lv_obj_set_style_border_color(obj, lv_color_white(), 0);
  lv_obj_set_style_border_opa(obj, 50, 0);

  // Sombra suave
  lv_obj_set_style_shadow_width(obj, 20, 0);
  lv_obj_set_style_shadow_color(obj, lv_color_black(), 0);
  lv_obj_set_style_shadow_opa(obj, 60, 0);
}

// Global Styles instances
lv_style_t style_card_neon;
lv_style_t style_text_large;
lv_style_t style_text_title;
lv_style_t style_nav_btn;

void ui_init_global_styles() {
  // Configura estilo de card Neon (Tip 44)
  lv_style_init(&style_card_neon);
  lv_style_set_radius(&style_card_neon, 12);
  lv_style_set_bg_color(&style_card_neon, COLOR_AMOLED_BLACK);
  lv_style_set_bg_opa(&style_card_neon, LV_OPA_COVER);
  lv_style_set_border_width(&style_card_neon, 2);
  lv_style_set_border_color(&style_card_neon, COLOR_NEON_GREEN);
  lv_style_set_shadow_color(&style_card_neon, COLOR_NEON_GREEN);
  lv_style_set_shadow_width(&style_card_neon, 15);
  lv_style_set_shadow_spread(&style_card_neon, 1);
  lv_style_set_pad_all(&style_card_neon, 5);

  // Typography Styles (Tips 13, 14, 15)
  lv_style_init(&style_text_large);
  lv_style_set_text_font(&style_text_large, FONT_LARGE_NUM);
  lv_style_set_text_color(&style_text_large, lv_color_white());

  lv_style_init(&style_text_title);
  lv_style_set_text_font(&style_text_title, FONT_TITLE);
  lv_style_set_text_color(&style_text_title, COLOR_NEON_GREEN);

  // Navigation Button Style
  lv_style_init(&style_nav_btn);
  lv_style_set_bg_opa(&style_nav_btn, LV_OPA_TRANSP);
  lv_style_set_text_color(&style_nav_btn, lv_color_hex(0x808080)); // Dimmed
  lv_style_set_img_recolor(&style_nav_btn, lv_color_hex(0x808080));
  lv_style_set_img_recolor_opa(&style_nav_btn, LV_OPA_COVER);
}

void ui_create_3_zone_layout(lv_obj_t *parent, lv_obj_t **content_area) {
  // 1. Status Bar (Fixed Top 40px)
  // Criado externamente pelo status_bar.cpp, mas reservamos o espaço
  // O content area começa em y=40

  // 2. Navigation Bar (Fixed Bottom 40px)
  lv_obj_t *nav_bar = lv_obj_create(parent);
  lv_obj_set_size(nav_bar, LCD_WIDTH, NAV_BAR_HEIGHT);
  lv_obj_align(nav_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(nav_bar, getTheme().panel, 0);
  lv_obj_set_style_bg_opa(nav_bar, LV_OPA_COVER, 0);
  lv_obj_set_style_border_side(nav_bar, LV_BORDER_SIDE_TOP, 0);
  lv_obj_set_style_border_width(nav_bar, 1, 0);
  lv_obj_set_style_border_color(nav_bar, COLOR_NEON_BLUE, 0);
  lv_obj_clear_flag(nav_bar, LV_OBJ_FLAG_SCROLLABLE);

  // Navigation Items (Home, Wifi, BLE, Plugins, Config) (Tip 3)
  const char *icons[] = {LV_SYMBOL_HOME, LV_SYMBOL_WIFI, LV_SYMBOL_BLUETOOTH,
                         LV_SYMBOL_CHARGE, // Plugins = Charge/Lightning
                         LV_SYMBOL_SETTINGS};

  lv_obj_set_flex_flow(nav_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(nav_bar, LV_FLEX_ALIGN_SPACE_EVENLY,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  for (int i = 0; i < 5; i++) {
    lv_obj_t *btn = lv_btn_create(nav_bar);
    lv_obj_set_size(btn, 60, MIN_TOUCH_TARGET); // Altura minima 48px para toque
    lv_obj_add_style(btn, &style_nav_btn, 0);
    lv_obj_add_style(btn, &style_nav_btn, LV_STATE_DEFAULT);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, icons[i]);
    lv_obj_center(lbl);

    // Add Event Callback
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *e) {
          int id = (int)(intptr_t)lv_event_get_user_data(e);
          switch (id) {
          case 0: // Home
            ui_set_screen(UI_SCREEN_MAIN);
            break;
          case 1:                                // Wi-Fi
            ui_set_screen(UI_SCREEN_WIFI_CHAOS); // Assuming we have chaos
                                                 // screen logic ready or mapped
            break;
          case 2:                               // BLE
            ui_set_screen(UI_SCREEN_BLE_CHAOS); // Mapped to BLE Chaos
            break;
          case 3: // Plugins
                  // Navigate to Plugins Screen (to be created/linked)
            break;
          case 4: // Config
            ui_set_screen(UI_SCREEN_SETTINGS);
            break;
          }
        },
        LV_EVENT_CLICKED, (void *)(intptr_t)i);
  }

  // 3. Content Area (Middle ~368px)
  *content_area = lv_obj_create(parent);
  lv_obj_set_size(*content_area, LCD_WIDTH, CONTENT_HEIGHT);
  lv_obj_align(*content_area, LV_ALIGN_TOP_MID, 0, STATUS_BAR_HEIGHT);
  lv_obj_set_style_bg_color(*content_area, COLOR_AMOLED_BLACK, 0); // Tip 45
  lv_obj_set_style_pad_all(*content_area, 0, 0);
  lv_obj_set_style_border_width(*content_area, 0, 0);
}

void ui_create_back_btn(lv_obj_t *parent, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 32, 32);               // Tip 7: 32x32px
  lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 5, 5); // Corner padding
  lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
  lv_obj_set_style_shadow_width(btn, 0, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_20,
                             0); // Sufficient for icon
  lv_obj_set_style_text_color(lbl, COLOR_NEON_BLUE, 0);
  lv_obj_center(lbl);
}
