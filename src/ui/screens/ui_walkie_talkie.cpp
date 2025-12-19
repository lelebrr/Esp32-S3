#include "ui_walkie_talkie.h"
#include "../../hardware/audio_driver.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Not implemented
#include "../ui_helpers.h"
#include "../ui_main.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static lv_obj_t *ui_WalkieTalkieScreen;
static lv_obj_t *ui_TalkButton;
static lv_obj_t *ui_StatusLabel;
static lv_obj_t *ui_ChannelRoller;
static lv_obj_t *ui_BackBtn;

static bool isTalking = false;
static TaskHandle_t audioTaskHandle = NULL;

// Audio processing task
static void audio_tx_task(void *parameter) {
    int16_t buffer[115]; // 230 bytes / 2 bytes per sample
    while (isTalking) {
        if (audioDriver.isRecording()) {
            size_t samplesRead = audioDriver.readAudioSamples(buffer, 115);
            if (samplesRead > 0) {
                // TODO: MeshSystem not implemented
                // MeshSystem.sendAudio((uint8_t *)buffer, samplesRead * 2);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // ~100Hz packet rate
    }
    audioTaskHandle = NULL;
    vTaskDelete(NULL);
}

static void talk_btn_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        if (!isTalking) {
            isTalking = true;
            // TODO: MeshSystem not implemented
            // MeshSystem.setWalkieTalkieMode(true);
            lv_label_set_text(ui_StatusLabel, "TRANSMITTING...");
            lv_obj_set_style_bg_color(ui_TalkButton, lv_color_hex(0xFF0000), LV_PART_MAIN);

            audioDriver.setMicGain(90);
            audioDriver.startRecording();

            xTaskCreate(audio_tx_task, "AudioTx", 4096, NULL, 5, &audioTaskHandle);
        }
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        if (isTalking) {
            isTalking = false;
            // TODO: MeshSystem not implemented
            // MeshSystem.setWalkieTalkieMode(false);
            lv_label_set_text(ui_StatusLabel, "HOLD TO TALK");
            lv_obj_set_style_bg_color(ui_TalkButton, lv_color_hex(0x00FF00), LV_PART_MAIN);

            audioDriver.stopRecording();
            // processing task will self dispose
        }
    }
}

void ui_walkie_talkie_screen_init(void) {
    // TODO: MeshSystem not implemented
    // MeshSystem.begin();

    // Register audio RX callback (disabled - MeshSystem not implemented)
    // MeshSystem.onAudio([](const uint8_t *mac, const uint8_t *data, size_t len) {
    //     if (!isTalking && !audioDriver.isRecording()) {
    //         if (len % 2 == 0) { audioDriver.playRaw((const int16_t *)data, len / 2); }
    //     }
    // });

    ui_WalkieTalkieScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_WalkieTalkieScreen, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    lv_obj_t *title = lv_label_create(ui_WalkieTalkieScreen);
    lv_label_set_text(title, "WALKIE TALKIE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    // Status
    ui_StatusLabel = lv_label_create(ui_WalkieTalkieScreen);
    lv_label_set_text(ui_StatusLabel, "HOLD TO TALK");
    lv_obj_align(ui_StatusLabel, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_text_font(ui_StatusLabel, &lv_font_montserrat_16, 0);

    // Talk Button (Big Circle)
    ui_TalkButton = lv_btn_create(ui_WalkieTalkieScreen);
    lv_obj_set_size(ui_TalkButton, 120, 120);
    lv_obj_align(ui_TalkButton, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_style_radius(ui_TalkButton, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui_TalkButton, lv_color_hex(0x00FF00), LV_PART_MAIN);

    lv_obj_t *btnLabel = lv_label_create(ui_TalkButton);
    lv_label_set_text(btnLabel, "PTT");
    lv_obj_center(btnLabel);

    lv_obj_add_event_cb(ui_TalkButton, talk_btn_event_handler, LV_EVENT_ALL, NULL);

    // Channel Selector (Roller)
    ui_ChannelRoller = lv_roller_create(ui_WalkieTalkieScreen);
    lv_roller_set_options(ui_ChannelRoller, "Ch 1\nCh 2\nCh 3\nCh 4", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(ui_ChannelRoller, 80, 40);
    lv_obj_align(ui_ChannelRoller, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    // Back Button
    ui_BackBtn = lv_btn_create(ui_WalkieTalkieScreen);
    lv_obj_set_size(ui_BackBtn, 60, 40);
    lv_obj_align(ui_BackBtn, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_t *backLabel = lv_label_create(ui_BackBtn);
    lv_label_set_text(backLabel, "BACK");
    lv_obj_center(backLabel);

    // Add event to go back
    lv_obj_add_event_cb(
        ui_BackBtn,
        [](lv_event_t *e) {
            // Cleanup
            // TODO: MeshSystem not implemented
            // MeshSystem.setWalkieTalkieMode(false);
            // MeshSystem.onAudio(NULL); // Unregister
            ui_main_show();
        },
        LV_EVENT_CLICKED,
        NULL
    );
}
