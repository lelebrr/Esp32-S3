#include "ui_password_gen.h"
#include "../ui_helpers.h"
#include "../ui_main.h"
// #include "../../hal/imu_manager.h" // Assuming this exists or will exist

static lv_obj_t *ui_PassGenScreen;
static lv_obj_t *ui_PassLabel;
static lv_obj_t *ui_StrengthBar;

static void generate_pass_handler(lv_event_t *e) {
    // Entropia melhorada: combina millis, heap livre e ADC para aleatoriedade
    uint32_t entropy = millis();
    entropy ^= esp_random(); // Hardware RNG do ESP32
    entropy ^= esp_get_free_heap_size();

    srand(entropy);
    const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    char password[17];
    for (int i = 0; i < 16; i++) { password[i] = charset[rand() % strlen(charset)]; }
    password[16] = 0;

    lv_label_set_text(ui_PassLabel, password);
    lv_bar_set_value(ui_StrengthBar, 100, LV_ANIM_ON);
}

void ui_password_gen_init(void) {
    ui_PassGenScreen = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(ui_PassGenScreen);
    lv_label_set_text(title, "PASS GEN (IMU)");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    ui_PassLabel = lv_label_create(ui_PassGenScreen);
    lv_label_set_text(ui_PassLabel, "SHAKE TO GEN");
    lv_obj_set_style_text_font(ui_PassLabel, &lv_font_montserrat_24, 0);
    lv_obj_align(ui_PassLabel, LV_ALIGN_CENTER, 0, -20);

    ui_StrengthBar = lv_bar_create(ui_PassGenScreen);
    lv_obj_set_size(ui_StrengthBar, 200, 20);
    lv_obj_align(ui_StrengthBar, LV_ALIGN_CENTER, 0, 20);
    lv_bar_set_range(ui_StrengthBar, 0, 100);
    lv_bar_set_value(ui_StrengthBar, 0, LV_ANIM_OFF);

    lv_obj_t *btn = lv_btn_create(ui_PassGenScreen);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_t *l = lv_label_create(btn);
    lv_label_set_text(l, "GENERATE");
    lv_obj_center(l);
    lv_obj_add_event_cb(btn, generate_pass_handler, LV_EVENT_CLICKED, NULL);

    // Back
    lv_obj_t *backBtn = lv_btn_create(ui_PassGenScreen);
    lv_obj_set_size(backBtn, 60, 40);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    l = lv_label_create(backBtn);
    lv_label_set_text(l, "BACK");
    lv_obj_center(l);
    lv_obj_add_event_cb(backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
