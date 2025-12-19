#pragma once

/**
 * @file ui_files_screen.h
 * @brief Tela de gerenciamento de arquivos (Placeholder)
 */

#include <Arduino.h>
#include <lvgl.h>

class FilesScreen {
public:
  FilesScreen();

  void create(lv_obj_t *parent);
  void show();
  void hide();

private:
  lv_obj_t *_screen;
  lv_obj_t *_label;
  lv_obj_t *_btnBack;

  static void onBackClick(lv_event_t *e);
};

extern FilesScreen filesScreen;
