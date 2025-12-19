#include "ui_dead_drop.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Create or remove
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_DeadDropScreen;
static lv_obj_t *ui_DropMessageTA;
static lv_obj_t *ui_PasswordTA;
static lv_obj_t *ui_StatusLabel;

static void create_drop_handler(lv_event_t *e) {
    const char *msg = lv_textarea_get_text(ui_DropMessageTA);
    const char *pass = lv_textarea_get_text(ui_PasswordTA);

    if (strlen(msg) > 0 && strlen(pass) > 0) {
        // Encrypt msg with pass
        // Save to SD / spiffs as .drop file
        // Start broadcasting availability
        lv_label_set_text(ui_StatusLabel, "DROP CREATED! Active.");
    }
}

static void search_drop_handler(lv_event_t *e) {
    lv_label_set_text(ui_StatusLabel, "Scanning for drops...");
    // TODO: MeshSystem not implemented yet
    // MeshSystem.sendPing();
    // Listen for specific drop advertisements
}

void ui_dead_drop_init(void) {
    ui_DeadDropScreen = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(ui_DeadDropScreen);
    lv_label_set_text(title, "DEAD DROP");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    ui_StatusLabel = lv_label_create(ui_DeadDropScreen);
    lv_label_set_text(ui_StatusLabel, "Create or Find");
    lv_obj_align(ui_StatusLabel, LV_ALIGN_TOP_MID, 0, 35);

    // Create Drop Section
    lv_obj_t *tabview = lv_tabview_create(ui_DeadDropScreen, LV_DIR_TOP, 30);
    lv_obj_set_size(tabview, LV_PCT(100), LV_PCT(80));
    lv_obj_align(tabview, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "CREATE");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "FIND");

    // Tab 1: Create
    ui_DropMessageTA = lv_textarea_create(tab1);
    lv_textarea_set_placeholder_text(ui_DropMessageTA, "Secret Message");
    lv_obj_set_width(ui_DropMessageTA, LV_PCT(90));
    lv_obj_align(ui_DropMessageTA, LV_ALIGN_TOP_MID, 0, 10);

    ui_PasswordTA = lv_textarea_create(tab1);
    lv_textarea_set_placeholder_text(ui_PasswordTA, "Password");
    lv_textarea_set_one_line(ui_PasswordTA, true);
    lv_obj_set_width(ui_PasswordTA, LV_PCT(90));
    lv_obj_align(ui_PasswordTA, LV_ALIGN_TOP_MID, 0, 60);

    lv_obj_t *createBtn = lv_btn_create(tab1);
    lv_obj_align(createBtn, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_t *l = lv_label_create(createBtn);
    lv_label_set_text(l, "DROP IT");
    lv_obj_center(l);
    lv_obj_add_event_cb(createBtn, create_drop_handler, LV_EVENT_CLICKED, NULL);

    // Tab 2: Find
    lv_obj_t *findBtn = lv_btn_create(tab2);
    lv_obj_align(findBtn, LV_ALIGN_CENTER, 0, 0);
    l = lv_label_create(findBtn);
    lv_label_set_text(l, "SCAN DROPS");
    lv_obj_center(l);
    lv_obj_add_event_cb(findBtn, search_drop_handler, LV_EVENT_CLICKED, NULL);

    // Back Btn
    lv_obj_t *backBtn = lv_btn_create(ui_DeadDropScreen);
    lv_obj_set_size(backBtn, 40, 30);
    lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
    l = lv_label_create(backBtn);
    lv_label_set_text(l, "<");
    lv_obj_center(l);
    lv_obj_add_event_cb(backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
