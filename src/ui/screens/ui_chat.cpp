#include "ui_chat.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Create or remove
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_ChatScreen;
static lv_obj_t *ui_ChatContainer;
static lv_obj_t *ui_MessageInput;
static lv_obj_t *ui_Keyboard;

static void keyboard_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        if (code == LV_EVENT_READY) {
            const char *txt = lv_textarea_get_text(ui_MessageInput);
            if (strlen(txt) > 0) {
                // TODO: MeshSystem not implemented yet
                // MeshSystem.sendChat(txt, true);
                ui_chat_add_message("Me", txt, true);
                lv_textarea_set_text(ui_MessageInput, "");
            }
        }
        lv_obj_add_flag(ui_Keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ui_MessageInput, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ui_MessageInput);
    }
}

static void ta_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
        if (lv_obj_has_flag(ui_Keyboard, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(ui_Keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_textarea(ui_Keyboard, ta);
        }
    }
}

void ui_chat_add_message(const char *sender, const char *text, bool isMe) {
    if (!ui_ChatContainer) return;

    lv_obj_t *obj = lv_obj_create(ui_ChatContainer);
    lv_obj_set_width(obj, LV_PCT(90));
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    lv_obj_get_style_bg_color(obj, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text_fmt(label, "%s: %s", sender, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, LV_PCT(95));
    lv_obj_center(label);

    if (isMe) {
        lv_obj_align(obj, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x00A000), 0);
    } else {
        lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x404040), 0);
    }

    lv_obj_scroll_to_view(obj, LV_ANIM_ON);
}

void ui_chat_screen_init(void) {
    ui_ChatScreen = lv_obj_create(NULL);

    // Header
    lv_obj_t *title = lv_label_create(ui_ChatScreen);
    lv_label_set_text(title, "MESH CHAT");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Messages Container
    ui_ChatContainer = lv_obj_create(ui_ChatScreen);
    lv_obj_set_size(ui_ChatContainer, LV_PCT(100), 160);
    lv_obj_align(ui_ChatContainer, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_flex_flow(ui_ChatContainer, LV_FLEX_FLOW_COLUMN);

    // Input
    ui_MessageInput = lv_textarea_create(ui_ChatScreen);
    lv_textarea_set_placeholder_text(ui_MessageInput, "Message...");
    lv_textarea_set_one_line(ui_MessageInput, true);
    lv_obj_set_size(ui_MessageInput, LV_PCT(100), 40);
    lv_obj_align(ui_MessageInput, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(ui_MessageInput, ta_event_cb, LV_EVENT_ALL, NULL);

    // Keyboard (Hidden by default)
    ui_Keyboard = lv_keyboard_create(ui_ChatScreen);
    lv_obj_set_size(ui_Keyboard, LV_PCT(100), 120);
    lv_obj_align(ui_Keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(ui_Keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_Keyboard, keyboard_event_cb, LV_EVENT_ALL, NULL);

    // TODO: MeshSystem not implemented yet
    // Register callback for incoming messages
    // MeshSystem.onChat([](const uint8_t *mac, const char *sender, const char *text) {
    //     ui_chat_add_message(sender, text, false);
    // });
}
