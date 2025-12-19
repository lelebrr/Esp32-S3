#pragma once

#include <Arduino.h>
#include <lvgl.h>

enum SettingsSubmenu : uint8_t {
  SUBMENU_DISPLAY = 0,
  SUBMENU_DRAGON,
  SUBMENU_SOUND,
  SUBMENU_NOTIFICATIONS,
  SUBMENU_NETWORK,
  SUBMENU_ATTACKS,
  SUBMENU_SYSTEM,
  SUBMENU_COUNT
};

extern const char *SUBMENU_TITLES[];
extern const char *SUBMENU_ICONS[];

class SettingsMenuV2 {
public:
  SettingsMenuV2();

  lv_obj_t *create(lv_obj_t *parent);
  void show();
  void hide();
  bool isVisible() const { return _visible; }

  void openSubmenu(SettingsSubmenu submenu);
  void closeSubmenu();
  void goBack();
  void refresh();
  void applyChanges();

private:
  lv_obj_t *_screen;
  lv_obj_t *_mainMenu;
  lv_obj_t *_submenuContainer;
  SettingsSubmenu _currentSubmenuId;
  bool _visible;

  lv_obj_t *createDisplaySubmenu(lv_obj_t *parent);
  lv_obj_t *createDragonSubmenu(lv_obj_t *parent);
  lv_obj_t *createSoundSubmenu(lv_obj_t *parent);
  lv_obj_t *createNotificationsSubmenu(lv_obj_t *parent);
  lv_obj_t *createNetworkSubmenu(lv_obj_t *parent);
  lv_obj_t *createAttacksSubmenu(lv_obj_t *parent);
  lv_obj_t *createSystemSubmenu(lv_obj_t *parent);

  lv_obj_t *createSlider(lv_obj_t *p, const char *lbl, int min, int max,
                         int val, lv_event_cb_t cb);
  lv_obj_t *createSwitch(lv_obj_t *p, const char *lbl, bool val,
                         lv_event_cb_t cb);
  lv_obj_t *createDropdown(lv_obj_t *p, const char *lbl, const char *opts,
                           uint8_t sel, lv_event_cb_t cb);
  lv_obj_t *createButton(lv_obj_t *p, const char *lbl, const char *btn,
                         lv_event_cb_t cb);

  static void onSubmenuClick(lv_event_t *e);
  static void onBackClick(lv_event_t *e);
};

extern SettingsMenuV2 settings_menu_v2;
