#include "ui_matrix.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_MatrixScreen;
static lv_timer_t *matrix_timer = nullptr;

static const int COLS = 15;
static const int ROWS = 20;
static lv_obj_t *labels[COLS];
static int drops[COLS];

static void matrix_update(lv_timer_t *t) {
    for (int i = 0; i < COLS; i++) {
        if (drops[i] > 0) {
            drops[i]++;
            if (drops[i] > ROWS) drops[i] = 0; // Reset

            char buffer[ROWS + 10]; // Enough size
            int pos = 0;
            for (int j = 0; j < ROWS; j++) {
                if (pos >= sizeof(buffer) - 2) break; // Safety

                if (j == drops[i]) buffer[pos++] = (char)(rand() % 90 + 33); // Head
                else if (j < drops[i] && j > drops[i] - 5)
                    buffer[pos++] = (char)(rand() % 2 == 0 ? '1' : '0'); // Tail
                else buffer[pos++] = '\n';
            }
            buffer[pos] = '\0';
            // lv_label_set_text(labels[i], buffer);
        } else {
            if (rand() % 50 == 0) drops[i] = 1;
        }
    }
}

// Canvas based optimized approach
static lv_obj_t *canvas;
static lv_color_t cbuf[240 * 280];

static void matrix_canvas_update(lv_timer_t *t) {
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), 255);

    for (int i = 0; i < 240 / 10; i++) { // 24 cols
        if (drops[i] > 280) drops[i] = 0;

        if (drops[i] > 0) {
            lv_draw_label_dsc_t dsc;
            lv_draw_label_dsc_init(&dsc);
            dsc.color = lv_color_hex(0x00FF00);

            char c[2];
            c[0] = (rand() % 2) ? '1' : '0';
            c[1] = 0;

            lv_canvas_draw_text(canvas, i * 10, drops[i], 100, &dsc, c);

            drops[i] += 10;
        } else {
            if (rand() % 20 == 0) drops[i] = 1;
        }
    }
}

void ui_matrix_init(void) {
    if (matrix_timer) lv_timer_del(matrix_timer);

    ui_MatrixScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_MatrixScreen, lv_color_hex(0x000000), 0);

    // Using simple labels for text rain
    // Create one big label? No, multiple cols ok

    for (int i = 0; i < COLS; i++) {
        drops[i] = rand() % ROWS;
        // labels[i] = lv_label_create(ui_MatrixScreen);
        // lv_obj_set_pos(labels[i], i * 16, 0);
        // lv_obj_set_style_text_color(labels[i], lv_color_hex(0x00FF00), 0);
        // lv_obj_set_style_text_font(labels[i], &lv_font_montserrat_12, 0);
    }

    // Canvas approach is better but we lack buffer updates in some drivers.
    // Let's stick to a simple visual trick: Many floating labels moving down.
    // Actually, simple static text updated randomly is cooler.

    lv_obj_t *l = lv_label_create(ui_MatrixScreen);
    lv_label_set_text(l, "MATRIX MODE\n(Canvas Pending)");
    lv_obj_center(l);
    lv_obj_set_style_text_color(l, lv_color_hex(0x00FF00), 0);

    // Back
    lv_obj_t *backBtn = lv_btn_create(ui_MatrixScreen);
    lv_obj_set_size(backBtn, 50, 40);
    lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_t *lbl = lv_label_create(backBtn);
    lv_label_set_text(lbl, "<");
    lv_obj_center(lbl);
    lv_obj_add_event_cb(
        backBtn,
        [](lv_event_t *e) {
            if (matrix_timer) lv_timer_del(matrix_timer);
            matrix_timer = nullptr;
            ui_main_show();
        },
        LV_EVENT_CLICKED,
        NULL
    );

    lv_scr_load_anim(ui_MatrixScreen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, true);
}
