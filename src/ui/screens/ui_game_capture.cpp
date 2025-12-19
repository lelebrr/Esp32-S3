#include "ui_game_capture.h"
// #include "../../wifi/esp_now_mesh.h" // TODO: Not implemented yet
#include "../ui_helpers.h"
#include "../ui_main.h"
#include <algorithm>
#include <globals.h>
#include <vector>

struct PlayerScore {
    char name[17];
    int score;
    uint32_t lastUpdate;
};

static std::vector<PlayerScore> players;
static int myScore = 0;
static lv_obj_t *ui_GameScreen;
static lv_obj_t *ui_ScoreList;
static lv_obj_t *ui_MyScoreLabel;

static void update_leaderboard_ui() {
    if (!ui_ScoreList) return;

    lv_obj_clean(ui_ScoreList);

    // Sort by score
    std::sort(players.begin(), players.end(), [](const PlayerScore &a, const PlayerScore &b) {
        return a.score > b.score;
    });

    for (const auto &p : players) {
        lv_obj_t *item = lv_list_add_btn(ui_ScoreList, NULL, NULL);

        lv_obj_t *name = lv_label_create(item);
        lv_label_set_text(name, p.name);
        lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, 0);

        lv_obj_t *score = lv_label_create(item);
        lv_label_set_text_fmt(score, "%d pts", p.score);
        lv_obj_align(score, LV_ALIGN_RIGHT_MID, -10, 0);
    }
}

// Struct for mesh packet
struct GameSyncPacket {
    char name[16];
    int score;
};

void game_report_handshake_capture(const char *ssid) {
    myScore += 100; // 100 pontos por handshake
    if (ui_MyScoreLabel) { lv_label_set_text_fmt(ui_MyScoreLabel, "PONTOS: %d", myScore); }

    // Broadcast new score
    GameSyncPacket pkt;
    strncpy(pkt.name, leleConfig.bleName.c_str(), 16);
    pkt.score = myScore;

    // TODO: MeshSystem not implemented yet
    // MeshSystem.broadcast(&pkt, sizeof(pkt));
}

void ui_capture_handshake_game_init(void) {
    // TODO: MeshSystem.begin() - not implemented yet

    ui_GameScreen = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(ui_GameScreen);
    lv_label_set_text(title, "CAPTURE THE HANDSHAKE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    ui_MyScoreLabel = lv_label_create(ui_GameScreen);
    lv_label_set_text_fmt(ui_MyScoreLabel, "PONTOS: %d", myScore);
    lv_obj_align(ui_MyScoreLabel, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_text_color(ui_MyScoreLabel, lv_color_hex(0x00FF00), 0);

    ui_ScoreList = lv_list_create(ui_GameScreen);
    lv_obj_set_size(ui_ScoreList, LV_PCT(90), LV_PCT(60));
    lv_obj_align(ui_ScoreList, LV_ALIGN_CENTER, 0, 10);

    update_leaderboard_ui();

    lv_obj_t *backBtn = lv_btn_create(ui_GameScreen);
    lv_obj_set_size(backBtn, 60, 40);
    lv_obj_align(backBtn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t *l = lv_label_create(backBtn);
    lv_label_set_text(l, "BACK");
    lv_obj_center(l);
    lv_obj_add_event_cb(backBtn, [](lv_event_t *e) { ui_main_show(); }, LV_EVENT_CLICKED, NULL);
}
