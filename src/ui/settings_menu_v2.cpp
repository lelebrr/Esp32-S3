#include "settings_menu_v2.h"
#include "globals.h"
#include "notifications_engine.h"
#include "sounds_manager.h"
#include "wallpaper_system.h"
#include <debug_log.h>

SettingsMenuV2 settings_menu_v2;

const char *SUBMENU_TITLES[] = {
    "Display & Interface",
    "Dragão & Personalidade",
    "Som & Vibração",
    "Notificações Visuais",
    "Rede & Wireless",
    "Ataques & Automação",
    "Sistema & Energia"
};

const char *SUBMENU_ICONS[] = {
    LV_SYMBOL_IMAGE,
    LV_SYMBOL_EYE_OPEN,
    LV_SYMBOL_AUDIO,
    LV_SYMBOL_BELL,
    LV_SYMBOL_WIFI,
    LV_SYMBOL_CHARGE,
    LV_SYMBOL_SETTINGS
};

// Style
static lv_style_t style_menu;
static lv_style_t style_btn;
static bool styles_inited = false;

static void init_styles() {
    if (styles_inited) return;

    lv_style_init(&style_menu);
    lv_style_set_bg_color(&style_menu, lv_color_hex(0x12121A));
    lv_style_set_bg_opa(&style_menu, LV_OPA_90);
    lv_style_set_border_color(&style_menu, lv_color_hex(0x00FFF5));
    lv_style_set_border_width(&style_menu, 1);
    lv_style_set_radius(&style_menu, 8);
    lv_style_set_pad_all(&style_menu, 10);

    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_color_hex(0x1A1A24));
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn, lv_color_hex(0x00FFF5));

    styles_inited = true;
}

SettingsMenuV2::SettingsMenuV2()
    : _screen(nullptr), _mainMenu(nullptr), _submenuContainer(nullptr), _currentSubmenuId(SUBMENU_DISPLAY),
      _visible(false) {}

lv_obj_t *SettingsMenuV2::create(lv_obj_t *parent) {
    init_styles();

    _screen = lv_obj_create(parent);
    lv_obj_set_size(_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(_screen, &style_menu, 0);

    // Header
    lv_obj_t *header = lv_label_create(_screen);
    lv_label_set_text(header, "⚙️ CONFIGURAÇÕES");
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(header, lv_color_hex(0x00FFF5), 0);

    // Main menu list
    _mainMenu = lv_list_create(_screen);
    lv_obj_set_size(_mainMenu, LV_PCT(100), LV_PCT(85));
    lv_obj_align(_mainMenu, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_add_style(_mainMenu, &style_menu, 0);

    // Add submenu buttons
    for (int i = 0; i < SUBMENU_COUNT; i++) {
        lv_obj_t *btn = lv_list_add_btn(_mainMenu, SUBMENU_ICONS[i], SUBMENU_TITLES[i]);
        lv_obj_add_style(btn, &style_btn, 0);
        lv_obj_set_user_data(btn, (void *)(intptr_t)i);
        lv_obj_add_event_cb(btn, onSubmenuClick, LV_EVENT_CLICKED, this);
    }

    // Submenu container (hidden initially)
    _submenuContainer = lv_obj_create(_screen);
    lv_obj_set_size(_submenuContainer, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(_submenuContainer, &style_menu, 0);
    lv_obj_add_flag(_submenuContainer, LV_OBJ_FLAG_HIDDEN);

    return _screen;
}

void SettingsMenuV2::show() {
    if (_screen) {
        lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
        _visible = true;
    }
}

void SettingsMenuV2::hide() {
    if (_screen) {
        lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
        _visible = false;
    }
}

void SettingsMenuV2::openSubmenu(SettingsSubmenu submenu) {
    // Clear previous submenu content
    lv_obj_clean(_submenuContainer);

    // Back button
    lv_obj_t *backBtn = lv_btn_create(_submenuContainer);
    lv_obj_set_size(backBtn, 40, 30);
    lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_add_event_cb(backBtn, onBackClick, LV_EVENT_CLICKED, this);
    lv_obj_t *backLbl = lv_label_create(backBtn);
    lv_label_set_text(backLbl, LV_SYMBOL_LEFT);
    lv_obj_center(backLbl);

    // Title
    lv_obj_t *title = lv_label_create(_submenuContainer);
    lv_label_set_text(title, SUBMENU_TITLES[submenu]);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00FFF5), 0);

    // Create content based on submenu
    lv_obj_t *content = lv_obj_create(_submenuContainer);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(85));
    lv_obj_align(content, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(content, 5, 0);
    lv_obj_add_style(content, &style_menu, 0);

    switch (submenu) {
        case SUBMENU_DISPLAY: createDisplaySubmenu(content); break;
        case SUBMENU_DRAGON: createDragonSubmenu(content); break;
        case SUBMENU_SOUND: createSoundSubmenu(content); break;
        case SUBMENU_NOTIFICATIONS: createNotificationsSubmenu(content); break;
        case SUBMENU_NETWORK: createNetworkSubmenu(content); break;
        case SUBMENU_ATTACKS: createAttacksSubmenu(content); break;
        case SUBMENU_SYSTEM: createSystemSubmenu(content); break;
        default: break;
    }

    // Show submenu, hide main
    lv_obj_add_flag(_mainMenu, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_submenuContainer, LV_OBJ_FLAG_HIDDEN);
    _currentSubmenuId = submenu;
}

void SettingsMenuV2::closeSubmenu() {
    lv_obj_add_flag(_submenuContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_mainMenu, LV_OBJ_FLAG_HIDDEN);
}

void SettingsMenuV2::goBack() { closeSubmenu(); }

void SettingsMenuV2::refresh() {
    // Refresh current submenu if visible
}

void SettingsMenuV2::applyChanges() {
    // Save to NVS
    LOG_UI("Applying changes...");
}

// Callbacks
void SettingsMenuV2::onSubmenuClick(lv_event_t *e) {
    SettingsMenuV2 *self = (SettingsMenuV2 *)lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    self->openSubmenu((SettingsSubmenu)idx);
}

void SettingsMenuV2::onBackClick(lv_event_t *e) {
    SettingsMenuV2 *self = (SettingsMenuV2 *)lv_event_get_user_data(e);
    self->goBack();
}
