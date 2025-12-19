/**
 * @file ui_settings_widgets.cpp
 * @brief Implementação dos widgets reutilizáveis para settings
 */

#include "ui_settings_widgets.h"
#include "globals.h"
#include "../../core/pin_definitions.h"
#include "../ui_themes.h"


// ============================================================
// ESTRUTURAS INTERNAS PARA CALLBACKS
// ============================================================
struct SliderUserData {
  setting_slider_cb_t cb;
};

struct SwitchUserData {
  setting_switch_cb_t cb;
};

struct DropdownUserData {
  setting_dropdown_cb_t cb;
};

struct ButtonUserData {
  setting_button_cb_t cb;
};

// ============================================================
// CALLBACKS INTERNOS
// ============================================================
static void slider_event_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  SliderUserData *data = (SliderUserData *)lv_event_get_user_data(e);
  if (data && data->cb) {
    data->cb(lv_slider_get_value(slider));
  }
}

static void switch_event_cb(lv_event_t *e) {
  lv_obj_t *sw = lv_event_get_target(e);
  SwitchUserData *data = (SwitchUserData *)lv_event_get_user_data(e);
  if (data && data->cb) {
    data->cb(lv_obj_has_state(sw, LV_STATE_CHECKED));
  }
}

static void dropdown_event_cb(lv_event_t *e) {
  lv_obj_t *dd = lv_event_get_target(e);
  DropdownUserData *data = (DropdownUserData *)lv_event_get_user_data(e);
  if (data && data->cb) {
    data->cb(lv_dropdown_get_selected(dd));
  }
}

static void button_event_cb(lv_event_t *e) {
  ButtonUserData *data = (ButtonUserData *)lv_event_get_user_data(e);
  if (data && data->cb) {
    data->cb();
  }
}

// ============================================================
// CRIAÇÃO DE ROW BASE
// ============================================================
static lv_obj_t *create_base_row(lv_obj_t *parent, const char *label,
                                 const char *icon) {
  lv_obj_t *row = lv_obj_create(parent);
  lv_obj_set_size(row, lv_pct(100), 52);
  lv_obj_set_style_bg_color(row, THEME_PANEL, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  lv_obj_set_style_radius(row, 10, 0);
  lv_obj_set_style_pad_all(row, 8, 0);
  lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  // Container para ícone + label
  lv_obj_t *left = lv_obj_create(row);
  lv_obj_set_size(left, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(left, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(left, 0, 0);
  lv_obj_set_style_pad_all(left, 0, 0);
  lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(left, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  // Ícone
  if (icon && strlen(icon) > 0) {
    lv_obj_t *ico = lv_label_create(left);
    lv_label_set_text(ico, icon);
    lv_obj_set_style_text_color(ico, THEME_ACCENT, 0);
    lv_obj_set_style_text_font(ico, &lv_font_montserrat_16, 0);
  }

  // Label
  lv_obj_t *lbl = lv_label_create(left);
  lv_label_set_text(lbl, label);
  lv_obj_set_style_text_color(lbl, THEME_TEXT, 0);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
  if (icon && strlen(icon) > 0) {
    lv_obj_set_style_pad_left(lbl, 8, 0);
  }

  return row;
}

// ============================================================
// IMPLEMENTAÇÕES PÚBLICAS
// ============================================================
lv_obj_t *ui_create_slider_row(lv_obj_t *parent, const char *label,
                               const char *icon, int32_t min, int32_t max,
                               int32_t current, setting_slider_cb_t cb) {
  lv_obj_t *row = create_base_row(parent, label, icon);

  // Container direito com valor + slider
  lv_obj_t *right = lv_obj_create(row);
  lv_obj_set_size(right, 130, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(right, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(right, 0, 0);
  lv_obj_set_style_pad_all(right, 0, 0);
  lv_obj_clear_flag(right, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  // Slider
  lv_obj_t *slider = lv_slider_create(right);
  lv_obj_set_size(slider, 100, 8);
  lv_slider_set_range(slider, min, max);
  lv_slider_set_value(slider, current, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(slider, THEME_PRIMARY, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider, THEME_ACCENT, LV_PART_KNOB);

  // Callback
  if (cb) {
    SliderUserData *data =
        (SliderUserData *)lv_mem_alloc(sizeof(SliderUserData));
    data->cb = cb;
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, data);
  }

  return slider;
}

lv_obj_t *ui_create_switch_row(lv_obj_t *parent, const char *label,
                               const char *icon, bool checked,
                               setting_switch_cb_t cb) {
  lv_obj_t *row = create_base_row(parent, label, icon);

  // Switch
  lv_obj_t *sw = lv_switch_create(row);
  lv_obj_set_style_bg_color(sw, THEME_PRIMARY,
                            LV_PART_INDICATOR | LV_STATE_CHECKED);
  if (checked) {
    lv_obj_add_state(sw, LV_STATE_CHECKED);
  }

  // Callback
  if (cb) {
    SwitchUserData *data =
        (SwitchUserData *)lv_mem_alloc(sizeof(SwitchUserData));
    data->cb = cb;
    lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, data);
  }

  return sw;
}

lv_obj_t *ui_create_dropdown_row(lv_obj_t *parent, const char *label,
                                 const char *icon, const char *options,
                                 uint16_t selected, setting_dropdown_cb_t cb) {
  lv_obj_t *row = create_base_row(parent, label, icon);

  // Dropdown
  lv_obj_t *dd = lv_dropdown_create(row);
  lv_dropdown_set_options(dd, options);
  lv_dropdown_set_selected(dd, selected);
  lv_obj_set_width(dd, 120);
  lv_obj_set_style_bg_color(dd, THEME_PANEL, 0);
  lv_obj_set_style_text_color(dd, THEME_TEXT, 0);
  lv_obj_set_style_border_color(dd, THEME_PRIMARY, 0);
  lv_obj_set_style_border_width(dd, 1, 0);

  // Estilo da lista de opções
  lv_obj_t *list = lv_dropdown_get_list(dd);
  if (list) {
    lv_obj_set_style_bg_color(list, THEME_PANEL, 0);
    lv_obj_set_style_text_color(list, THEME_TEXT, 0);
    lv_obj_set_style_bg_color(list, THEME_PRIMARY, LV_PART_SELECTED);
  }

  // Callback
  if (cb) {
    DropdownUserData *data =
        (DropdownUserData *)lv_mem_alloc(sizeof(DropdownUserData));
    data->cb = cb;
    lv_obj_add_event_cb(dd, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, data);
  }

  return dd;
}

lv_obj_t *ui_create_button_row(lv_obj_t *parent, const char *label,
                               const char *icon, const char *btn_text,
                               setting_button_cb_t cb) {
  lv_obj_t *row = create_base_row(parent, label, icon);

  // Botão
  lv_obj_t *btn = lv_btn_create(row);
  lv_obj_set_size(btn, LV_SIZE_CONTENT, 32);
  lv_obj_set_style_bg_color(btn, THEME_PRIMARY, 0);
  lv_obj_set_style_bg_color(btn, THEME_ACCENT, LV_STATE_PRESSED);
  lv_obj_set_style_radius(btn, 8, 0);
  lv_obj_set_style_pad_hor(btn, 16, 0);

  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, btn_text);
  lv_obj_set_style_text_color(lbl, THEME_TEXT, 0);
  lv_obj_center(lbl);

  // Callback
  if (cb) {
    ButtonUserData *data =
        (ButtonUserData *)lv_mem_alloc(sizeof(ButtonUserData));
    data->cb = cb;
    lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, data);
  }

  return btn;
}

lv_obj_t *ui_create_color_picker(lv_obj_t *parent, const char *label,
                                 uint8_t current_idx,
                                 setting_dropdown_cb_t cb) {
  // Header
  lv_obj_t *header = lv_label_create(parent);
  lv_label_set_text(header, label);
  lv_obj_set_style_text_color(header, THEME_TEXT, 0);
  lv_obj_set_style_text_font(header, &lv_font_montserrat_14, 0);
  lv_obj_set_style_pad_bottom(header, 8, 0);

  // Grid de cores (8x4)
  lv_obj_t *grid = lv_obj_create(parent);
  lv_obj_set_size(grid, lv_pct(100), 80);
  lv_obj_set_style_bg_color(grid, THEME_PANEL, 0);
  lv_obj_set_style_border_width(grid, 0, 0);
  lv_obj_set_style_radius(grid, 10, 0);
  lv_obj_set_style_pad_all(grid, 8, 0);
  lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(grid, LV_OBJ_FLAG_SCROLLABLE);

  // Criar 32 botões de cor
  for (int i = 0; i < 32; i++) {
    lv_obj_t *btn = lv_btn_create(grid);
    lv_obj_set_size(btn, 16, 16);
    lv_obj_set_style_bg_color(btn, lv_color_hex(neon_colors[i]), 0);
    lv_obj_set_style_radius(btn, 4, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_width(btn, i == current_idx ? 2 : 0, 0);
    lv_obj_set_style_border_color(btn, lv_color_white(), 0);

    // Callback para seleção
    if (cb) {
      DropdownUserData *data =
          (DropdownUserData *)lv_mem_alloc(sizeof(DropdownUserData));
      data->cb = cb;
      lv_obj_add_event_cb(
          btn,
          [](lv_event_t *e) {
            lv_obj_t *clicked = lv_event_get_target(e);
            lv_obj_t *container = lv_obj_get_parent(clicked);

            // Remove borda de todos
            uint32_t child_cnt = lv_obj_get_child_cnt(container);
            for (uint32_t i = 0; i < child_cnt; i++) {
              lv_obj_t *child = lv_obj_get_child(container, i);
              lv_obj_set_style_border_width(child, 0, 0);
            }

            // Adiciona borda no selecionado
            lv_obj_set_style_border_width(clicked, 2, 0);

            // Callback
            DropdownUserData *data =
                (DropdownUserData *)lv_event_get_user_data(e);
            if (data && data->cb) {
              int idx = lv_obj_get_index(clicked);
              data->cb(idx);
            }
          },
          LV_EVENT_CLICKED, data);
    }
  }

  return grid;
}

lv_obj_t *ui_create_info_row(lv_obj_t *parent, const char *label,
                             const char *icon, const char *value) {
  lv_obj_t *row = create_base_row(parent, label, icon);

  // Valor (read-only)
  lv_obj_t *val = lv_label_create(row);
  lv_label_set_text(val, value);
  lv_obj_set_style_text_color(val, THEME_ACCENT, 0);
  lv_obj_set_style_text_font(val, &lv_font_montserrat_12, 0);

  return val;
}

lv_obj_t *ui_create_section_header(lv_obj_t *parent, const char *title) {
  lv_obj_t *header = lv_label_create(parent);
  lv_label_set_text(header, title);
  lv_obj_set_style_text_color(header, THEME_PRIMARY, 0);
  lv_obj_set_style_text_font(header, &lv_font_montserrat_16, 0);
  lv_obj_set_style_pad_top(header, 16, 0);
  lv_obj_set_style_pad_bottom(header, 8, 0);
  return header;
}

lv_obj_t *ui_create_settings_screen(lv_obj_t **screen_out, const char *title,
                                    lv_event_cb_t back_cb) {
  // Criar tela
  lv_obj_t *screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen, THEME_BG, 0);
  *screen_out = screen;

  // Header com título e botão voltar
  lv_obj_t *header = lv_obj_create(screen);
  lv_obj_set_size(header, LCD_WIDTH, 50);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header, THEME_PANEL, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_radius(header, 0, 0);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  // Botão voltar
  lv_obj_t *btn_back = lv_btn_create(header);
  lv_obj_set_size(btn_back, 40, 36);
  lv_obj_align(btn_back, LV_ALIGN_LEFT_MID, 5, 0);
  lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x333355), 0);
  lv_obj_set_style_radius(btn_back, 8, 0);
  lv_obj_add_event_cb(btn_back, back_cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *back_lbl = lv_label_create(btn_back);
  lv_label_set_text(back_lbl, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(back_lbl, THEME_TEXT, 0);
  lv_obj_center(back_lbl);

  // Título
  lv_obj_t *title_lbl = lv_label_create(header);
  lv_label_set_text(title_lbl, title);
  lv_obj_set_style_text_color(title_lbl, THEME_ACCENT, 0);
  lv_obj_set_style_text_font(title_lbl, &lv_font_montserrat_18, 0);
  lv_obj_align(title_lbl, LV_ALIGN_CENTER, 10, 0);

  // Container scrollável para conteúdo
  lv_obj_t *content = lv_obj_create(screen);
  lv_obj_set_size(content, LCD_WIDTH - 16, LCD_HEIGHT - 60);
  lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(content, 0, 0);
  lv_obj_set_style_pad_all(content, 0, 0);
  lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(content, 8, 0);

  return content;
}

void ui_update_info_row(lv_obj_t *label_obj, const char *value) {
  if (label_obj) {
    lv_label_set_text(label_obj, value);
  }
}
