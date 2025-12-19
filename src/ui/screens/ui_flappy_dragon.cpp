#include "ui_flappy_dragon.h"
#include "../ui_helpers.h"
#include "../ui_main.h"

static lv_obj_t *ui_FlappyScreen;
static lv_obj_t *ui_Dragon;
static lv_obj_t *ui_ScoreLabel;
static lv_obj_t *ui_GameOverPanel;
static lv_obj_t *ui_StartLabel;

// Game State
static bool game_running = false;
static float dragon_y = 0;
static float dragon_velocity = 0;
static int score = 0;
static lv_timer_t *game_timer = nullptr;

// Physics constants
const float GRAVITY = 0.6f;
const float JUMP_FORCE = -8.0f;
const float GAP_SIZE = 90.0f;
const float PIPE_SPEED = 3.0f;
const int SCREEN_WIDTH = 240;  // Approx
const int SCREEN_HEIGHT = 280; // Approx

struct Pipe {
  lv_obj_t *top_obj;
  lv_obj_t *bottom_obj;
  float x;
  bool passed;
};

static std::vector<Pipe> pipes;

static void game_over();
static void reset_game();

static void create_pipe() {
  int gap_y = rand() % (SCREEN_HEIGHT - (int)GAP_SIZE - 40) + 20;

  Pipe p;
  p.x = SCREEN_WIDTH;
  p.passed = false;

  // Top pipe
  p.top_obj = lv_obj_create(ui_FlappyScreen);
  lv_obj_set_style_bg_color(p.top_obj, lv_color_hex(0x00AA00), 0);
  lv_obj_set_style_border_width(p.top_obj, 2, 0);
  lv_obj_set_style_border_color(p.top_obj, lv_color_hex(0x000000), 0);
  lv_obj_set_size(p.top_obj, 40, gap_y);
  lv_obj_set_pos(p.top_obj, SCREEN_WIDTH, 0);

  // Bottom pipe
  p.bottom_obj = lv_obj_create(ui_FlappyScreen);
  lv_obj_set_style_bg_color(p.bottom_obj, lv_color_hex(0x00AA00), 0);
  lv_obj_set_style_border_width(p.bottom_obj, 2, 0);
  lv_obj_set_style_border_color(p.bottom_obj, lv_color_hex(0x000000), 0);
  lv_obj_set_size(p.bottom_obj, 40, SCREEN_HEIGHT - gap_y - GAP_SIZE);
  lv_obj_set_pos(p.bottom_obj, SCREEN_WIDTH, gap_y + GAP_SIZE);

  pipes.push_back(p);
}

static void game_loop(lv_timer_t *timer) {
  if (!game_running)
    return;

  // Physics
  dragon_velocity += GRAVITY;
  dragon_y += dragon_velocity;

  // Update Dragon
  lv_obj_set_y(ui_Dragon, (int)dragon_y);
  // Rotate dragon based on velocity
  int angle = (int)(dragon_velocity * 30);
  if (angle > 450)
    angle = 450;
  if (angle < -450)
    angle = -450;
  lv_obj_set_style_transform_angle(ui_Dragon, angle, 0);

  // Limits
  if (dragon_y > SCREEN_HEIGHT || dragon_y < -20) {
    game_over();
    return;
  }

  // Pipes
  // Add new pipe
  if (pipes.empty() || (SCREEN_WIDTH - pipes.back().x) > 140) {
    create_pipe();
  }

  for (int i = 0; i < pipes.size(); i++) {
    pipes[i].x -= PIPE_SPEED;
    lv_obj_set_x(pipes[i].top_obj, (int)pipes[i].x);
    lv_obj_set_x(pipes[i].bottom_obj, (int)pipes[i].x);

    // Collision
    int dragon_x = lv_obj_get_x(ui_Dragon);
    int dragon_w = lv_obj_get_width(ui_Dragon);
    int dragon_h = lv_obj_get_height(ui_Dragon);

    // Simple AABB
    if (pipes[i].x < dragon_x + dragon_w - 5 &&
        pipes[i].x + 40 > dragon_x + 5) {
      // Check heights
      int top_h = lv_obj_get_height(pipes[i].top_obj);
      if (dragon_y < top_h || dragon_y + dragon_h > top_h + GAP_SIZE) {
        game_over();
        return;
      }
    }

    // Score
    if (!pipes[i].passed && pipes[i].x + 40 < dragon_x) {
      score++;
      pipes[i].passed = true;
      lv_label_set_text_fmt(ui_ScoreLabel, "%d", score);
    }

    // Remove object if out of screen
    if (pipes[i].x < -50) {
      lv_obj_del(pipes[i].top_obj);
      lv_obj_del(pipes[i].bottom_obj);
      pipes.erase(pipes.begin() + i);
      i--;
    }
  }
}

static void jump() {
  if (!game_running) {
    // Start game
    reset_game();
    game_running = true;
    lv_obj_add_flag(ui_StartLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_GameOverPanel, LV_OBJ_FLAG_HIDDEN);
  }
  dragon_velocity = JUMP_FORCE;
}

static void input_cb(lv_event_t *e) { jump(); }

static void game_over() {
  game_running = false;
  lv_obj_clear_flag(ui_GameOverPanel, LV_OBJ_FLAG_HIDDEN);
}

static void reset_game() {
  dragon_y = SCREEN_HEIGHT / 2;
  dragon_velocity = 0;
  score = 0;
  lv_label_set_text(ui_ScoreLabel, "0");

  // Clear pipes
  for (auto &p : pipes) {
    lv_obj_del(p.top_obj);
    lv_obj_del(p.bottom_obj);
  }
  pipes.clear();

  lv_obj_set_y(ui_Dragon, (int)dragon_y);
  lv_obj_set_style_transform_angle(ui_Dragon, 0, 0);
}

void ui_flappy_dragon_init(void) {
  // Clean old
  if (game_timer) {
    lv_timer_del(game_timer);
    game_timer = nullptr;
  }
  pipes.clear();

  ui_FlappyScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_FlappyScreen, lv_color_hex(0x87CEEB),
                            0); // Sky blue

  // Start Label
  ui_StartLabel = lv_label_create(ui_FlappyScreen);
  lv_label_set_text(ui_StartLabel, "TAP TO FLY!");
  lv_obj_align(ui_StartLabel, LV_ALIGN_CENTER, 0, -50);
  lv_obj_set_style_text_font(ui_StartLabel, &lv_font_montserrat_24, 0);

  // Dragon (Yellow Box for now, maybe emoji?)
  ui_Dragon = lv_label_create(ui_FlappyScreen);
  lv_label_set_text(ui_Dragon, "🐲"); // Emoji dragon works nicely in fonts
  lv_obj_set_style_text_font(ui_Dragon, &lv_font_montserrat_28, 0);
  lv_obj_set_size(ui_Dragon, 30, 30);
  lv_obj_set_pos(ui_Dragon, 50, SCREEN_HEIGHT / 2);

  // Score
  ui_ScoreLabel = lv_label_create(ui_FlappyScreen);
  lv_label_set_text(ui_ScoreLabel, "0");
  lv_obj_align(ui_ScoreLabel, LV_ALIGN_TOP_MID, 0, 20);
  lv_obj_set_style_text_font(ui_ScoreLabel, &lv_font_montserrat_28, 0);

  // Game Over Panel
  ui_GameOverPanel = lv_obj_create(ui_FlappyScreen);
  lv_obj_set_size(ui_GameOverPanel, 180, 100);
  lv_obj_center(ui_GameOverPanel);
  lv_obj_add_flag(ui_GameOverPanel, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t *go_lbl = lv_label_create(ui_GameOverPanel);
  lv_label_set_text(go_lbl, "GAME OVER");
  lv_obj_align(go_lbl, LV_ALIGN_TOP_MID, 0, 10);

  lv_obj_t *retry_lbl = lv_label_create(ui_GameOverPanel);
  lv_label_set_text(retry_lbl, "Tap to retry");
  lv_obj_align(retry_lbl, LV_ALIGN_BOTTOM_MID, 0, -10);

  // Back Button
  lv_obj_t *backBtn = lv_btn_create(ui_FlappyScreen);
  lv_obj_set_size(backBtn, 50, 40);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        game_running = false;
        if (game_timer) {
          lv_timer_del(game_timer);
          game_timer = nullptr;
        }
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);

  // Click anywhere to jump
  lv_obj_add_event_cb(ui_FlappyScreen, input_cb, LV_EVENT_CLICKED, NULL);

  // Timer
  game_timer = lv_timer_create(game_loop, 20, NULL); // 50 FPS

  reset_game();
  game_running = false; // Wait for first tap

  lv_scr_load_anim(ui_FlappyScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
}
