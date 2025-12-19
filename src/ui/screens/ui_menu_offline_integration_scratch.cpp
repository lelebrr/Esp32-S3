#include "../ui_helpers.h"
#include "../ui_main.h"
#include "../ui_menu_tools.h"
#include "ui_menu_offline.h"

// ... existing code ...

// In tools menu init or valid place:
// Add button for "Offline Mode"
/*
    lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "Offline Mode");
    lv_obj_add_event_cb(btn, [](lv_event_t* e){
        ui_menu_offline_init();
    }, LV_EVENT_CLICKED, NULL);
*/
