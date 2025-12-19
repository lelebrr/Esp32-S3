/**
 * @file ui_files_screen.cpp
 * @brief Implementação da tela de arquivos (Placeholder)
 */

#include "ui_files_screen.h"
#include "../ui_main.h"

FilesScreen filesScreen;

FilesScreen::FilesScreen() : _screen(nullptr), _label(nullptr) {}

void FilesScreen::create(lv_obj_t *parent) {
  _screen = lv_obj_create(parent);
  lv_obj_set_size(_screen, lv_pct(100), lv_pct(100));
  lv_obj_set_style_bg_opa(_screen, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);

  // Label Info
  _label = lv_label_create(_screen);
  lv_label_set_text(_label, "Gerenciador de Arquivos\ndisponível apenas no\nWeb Dashboard!\n\nUse o navegador:\nhttp://wavepwn.local");
  lv_obj_set_style_text_align(_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(_label);

  // Botão Voltar
  _btnBack = lv_btn_create(_screen);
  lv_obj_align(_btnBack, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_obj_set_size(_btnBack, 100, 40);
  lv_obj_set_style_bg_color(_btnBack, lv_color_hex(0xFF0055), 0);
  lv_obj_add_event_cb(_btnBack, onBackClick, LV_EVENT_CLICKED, this);

  lv_obj_t *lblBack = lv_label_create(_btnBack);
  lv_label_set_text(lblBack, "Voltar");
  lv_obj_center(lblBack);
}

void FilesScreen::show() {
  if (!_screen)
    create(ui_get_content_area());
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void FilesScreen::hide() {
  if (_screen)
    lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void FilesScreen::onBackClick(lv_event_t *e) {
  FilesScreen *self = (FilesScreen *)lv_event_get_user_data(e);
  self->hide();
  ui_set_screen(UI_SCREEN_MAIN);
}
