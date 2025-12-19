// #include "../components/ir_remote/ir_blaster.h"
#include "lvgl.h"
#include <cstring>
#include <string>
#include <vector>

// Include IR nuke function
#include "../modules/ir/tv_nuke.h"

// Forward declare if we can't include easily or just rely on proper include
// #include "sd_card.h" // User prompt included this, assuming for future load
// feature? The prompt's screen code didn't actually use sd_card functions
// directly, mostly blaster. Prompt said: #include "sd_card.h" // seu código de
// SD já existente I will keep it commented out until I know the real path to
// avoid error.

static lv_obj_t *scr_ir;
static lv_obj_t *dragon_img;

static void btn_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    // Safe check for parent
    lv_obj_t *parent = lv_obj_get_parent(btn);
    // If it's a matrix we use get_btn_text, but this is a simple button with a
    // label child. The prompt code used lv_btnmatrix_get_btn_text on a button
    // event? That looks like a prompt error/mixup. "lv_obj_t *btn_nuke =
    // lv_btn_create(...)" -> it's a button. The callback provided:
    /*
    lv_obj_t *btn = lv_event_get_target(e);
    const char *txt = lv_btnmatrix_get_btn_text((lv_obj_t*)lv_obj_get_parent(btn),
    ...);
    */
    // That logic is for a button matrix. The btn_nuke is a standalone button.
    // For nuke, we just call nuke.

    // However, I should check if this callback is shared.
    // The prompt attaches it to `btn_nuke`.
    // And for `ir_send_by_name(txt)`, it implies this callback might be used for
    // a list of buttons. But in the `screen_ir_chaos` function provided, only
    // `btn_nuke` and `btn_learn` are created. I will implement `btn_nuke`
    // specifically for the nuke button.

    tv_nuke_init();
    tv_nuke_start();
    lv_obj_t *mbox = lv_msgbox_create(
        NULL,
        "TV NUKE",
        "1000+ protocolos enviados!\nTodas as TVs e aparelhos da vizinhança foram nukeados!",
        NULL,
        false
    );
    // lv_obj_add_flag(mbox, LV_OBJ_FLAG_AUTO_CLOSE); // LVGL 8/9 diffs? user
    // prompt used it. Assuming standard LVGL 8.x
    lv_obj_center(mbox);
}

static void learn_event_cb(lv_event_t *e) {
    lv_obj_t *mbox = lv_msgbox_create(
        NULL,
        "APRENDER CONTROLE",
        "Aponte o controle original para o LeleWatch\nPressione "
        "qualquer botão do controle...",
        NULL,
        false
    );
    lv_obj_center(mbox);
    // Instruction says "Aqui a mágica acontece (próximo bloco)" - meaning the
    // receiver task handles it.
}

void screen_ir_chaos() {
    scr_ir = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_ir, lv_color_make(0, 0, 0),
                              0); // lv_color_black() might be a macro

    // DRAGÃO GIGANTE SEGURANDO CONTROLE REMOTO
    // LV_IMG_DECLARE(dragon_ir_img); // User said "você vai criar esse PNG
    // depois". I don't have the image file. I will skip the image or use a
    // placeholder/label. I'll put a label "DRAGON IMG PLACEHOLDER" to avoid
    // compile error on missing symbol.

    lv_obj_t *label_img = lv_label_create(scr_ir);
    lv_label_set_text(label_img, "[ DRAGON IR IMAGE ]");
    lv_obj_align(label_img, LV_ALIGN_TOP_MID, 0, 40);

    // Título
    lv_obj_t *title = lv_label_create(scr_ir);
    lv_label_set_text(title, "IR CHAOS");
    // lv_obj_set_style_text_font(title, &lv_font_montserrat_32, 0); // Check if
    // font exists Default fonts usually available.
    lv_obj_set_style_text_color(title, lv_color_make(0, 255, 136),
                                0); // Hex 0x00FF88
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 80);

    // Botão gigante TV NUKE
    lv_obj_t *btn_nuke = lv_btn_create(scr_ir);
    lv_obj_set_size(btn_nuke, 200,
                    60); // Adjusted size to fit 240x240 or similar screen
    lv_obj_align(btn_nuke, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(btn_nuke, lv_color_make(68, 0, 0), 0);
    lv_obj_set_style_bg_grad_color(btn_nuke, lv_color_make(255, 0, 0), 0);
    lv_obj_set_style_bg_grad_dir(btn_nuke, LV_GRAD_DIR_VER, 0);

    lv_obj_t *lbl_nuke = lv_label_create(btn_nuke);
    lv_label_set_text(lbl_nuke, "TV NUKE");
    lv_obj_center(lbl_nuke);
    lv_obj_add_event_cb(btn_nuke, btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Botão aprender
    lv_obj_t *btn_learn = lv_btn_create(scr_ir);
    lv_obj_set_size(btn_learn, 200, 50);
    lv_obj_align(btn_learn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(btn_learn, lv_color_make(0, 51, 0), 0);
    lv_obj_t *lbl_learn = lv_label_create(btn_learn);
    lv_label_set_text(lbl_learn, "APRENDER");
    lv_obj_center(lbl_learn);
    lv_obj_add_event_cb(btn_learn, learn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_scr_load(scr_ir);
}
