#pragma once

/**
 * @file ui_stats_screen.h
 * @brief Tela de estatísticas detalhadas
 */

#include <Arduino.h>
#include <lvgl.h>

class StatsScreen {
public:
  StatsScreen();

  void create(lv_obj_t *parent);
  void show();
  void hide();
  void update();

private:
  lv_obj_t *_screen;
  lv_obj_t *_lblSession;
  lv_obj_t *_lblNetworks;
  lv_obj_t *_lblHandshakes;
  lv_obj_t *_lblDeauths;
  lv_obj_t *_lblUptime;
  lv_obj_t *_lblMemory;
  lv_obj_t *_lblBattery;
  lv_obj_t *_btnBack;

  static void onBackClick(lv_event_t *e);
};

extern StatsScreen statsScreen;
