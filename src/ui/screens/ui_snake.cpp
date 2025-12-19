#include "ui_snake.h"
#include "../ui_helpers.h"
#include "../ui_main.h"
#include <deque>
#include <vector>

struct Point {
  int x;
  int y;
};

static const int GRID_SIZE = 20;
static const int COLS = 12; // 240 / 20
static const int ROWS = 14; // 280 / 20

static lv_obj_t *ui_SnakeScreen;
static lv_obj_t *ui_GameArea;
static lv_obj_t *ui_ScoreLabel;
static lv_obj_t *ui_Controls;
static lv_timer_t *snake_timer = nullptr;

static std::deque<Point> snake;
static Point food;
static int direction = 0; // 0:Right, 1:Down, 2:Left, 3:Up
static int score = 0;
static bool game_over = false;

static void spawn_food() {
  bool valid = false;
  while (!valid) {
    food.x = rand() % COLS;
    food.y = rand() % ROWS;
    valid = true;
    for (const auto &p : snake) {
      if (p.x == food.x && p.y == food.y) {
        valid = false;
        break;
      }
    }
  }
}

static void draw_game() {
  lv_obj_clean(ui_GameArea);

  // Draw Food
  lv_obj_t *f = lv_obj_create(ui_GameArea);
  lv_obj_set_size(f, GRID_SIZE - 2, GRID_SIZE - 2);
  lv_obj_set_style_bg_color(f, lv_color_hex(0xFF0000), 0);
  lv_obj_set_style_radius(f, 50, 0); // Cricle
  lv_obj_set_pos(f, food.x * GRID_SIZE, food.y * GRID_SIZE);

  // Draw Snake
  for (const auto &p : snake) {
    lv_obj_t *s = lv_obj_create(ui_GameArea);
    lv_obj_set_size(s, GRID_SIZE - 2, GRID_SIZE - 2);
    lv_obj_set_style_bg_color(s, lv_color_hex(0x00FF00), 0);
    lv_obj_set_pos(s, p.x * GRID_SIZE, p.y * GRID_SIZE);
  }
}

static void game_loop(lv_timer_t *timer) {
  if (game_over)
    return;

  Point head = snake.front();
  Point new_head = head;

  switch (direction) {
  case 0:
    new_head.x++;
    break;
  case 1:
    new_head.y++;
    break;
  case 2:
    new_head.x--;
    break;
  case 3:
    new_head.y--;
    break;
  }

  // Collision with walls
  if (new_head.x < 0 || new_head.x >= COLS || new_head.y < 0 ||
      new_head.y >= ROWS) {
    game_over = true;
    lv_label_set_text(ui_ScoreLabel, "GAME OVER");
    return;
  }

  // Collision with self
  for (const auto &p : snake) {
    if (p.x == new_head.x && p.y == new_head.y) {
      game_over = true;
      lv_label_set_text(ui_ScoreLabel, "GAME OVER");
      return;
    }
  }

  snake.push_front(new_head);

  // Eat food
  if (new_head.x == food.x && new_head.y == food.y) {
    score += 10;
    lv_label_set_text_fmt(ui_ScoreLabel, "Score: %d", score);
    spawn_food();
  } else {
    snake.pop_back();
  }

  draw_game();
}

static void reset_snake() {
  snake.clear();
  snake.push_back({5, 5});
  snake.push_back({4, 5});
  snake.push_back({3, 5});
  direction = 0;
  score = 0;
  game_over = false;
  lv_label_set_text(ui_ScoreLabel, "Score: 0");
  spawn_food();
  draw_game();
}

static void control_btn(lv_event_t *e) {
  int dir = (int)(intptr_t)lv_event_get_user_data(e);
  // Prevent 180 turns
  if ((direction == 0 && dir == 2) || (direction == 2 && dir == 0) ||
      (direction == 1 && dir == 3) || (direction == 3 && dir == 1))
    return;
  direction = dir;
}

void ui_snake_init(void) {
  if (snake_timer)
    lv_timer_del(snake_timer);

  ui_SnakeScreen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_SnakeScreen, lv_color_hex(0x202020), 0);

  ui_ScoreLabel = lv_label_create(ui_SnakeScreen);
  lv_label_set_text(ui_ScoreLabel, "Score: 0");
  lv_obj_align(ui_ScoreLabel, LV_ALIGN_TOP_MID, 0, 5);

  ui_GameArea = lv_obj_create(ui_SnakeScreen);
  lv_obj_set_size(ui_GameArea, COLS * GRID_SIZE, ROWS * GRID_SIZE);
  lv_obj_align(ui_GameArea, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_style_bg_color(ui_GameArea, lv_color_hex(0x000000), 0);
  lv_obj_set_style_pad_all(ui_GameArea, 0, 0);

  // Controls
  ui_Controls = lv_obj_create(ui_SnakeScreen);
  lv_obj_set_size(ui_Controls, 240, 60);
  lv_obj_align(ui_Controls, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_opa(ui_Controls, 0, 0);

  const char *syms[] = {LV_SYMBOL_RIGHT, LV_SYMBOL_DOWN, LV_SYMBOL_LEFT,
                        LV_SYMBOL_UP};
  int dirs[] = {0, 1, 2, 3};

  // Simple D-Pad setup could be better, but let's do 4 horizontal buttons for
  // now
  for (int i = 0; i < 4; i++) {
    lv_obj_t *btn = lv_btn_create(ui_Controls);
    lv_obj_set_size(btn, 50, 40);
    lv_obj_set_x(btn, i * 55 + 5);
    lv_obj_set_y(btn, 10);
    lv_obj_add_event_cb(btn, control_btn, LV_EVENT_CLICKED,
                        (void *)(intptr_t)dirs[i]);
    lv_obj_t *l = lv_label_create(btn);
    lv_label_set_text(l, syms[i]);
    lv_obj_center(l);
  }

  // Back Button (Top Left)
  lv_obj_t *backBtn = lv_btn_create(ui_SnakeScreen);
  lv_obj_set_size(backBtn, 40, 30);
  lv_obj_align(backBtn, LV_ALIGN_TOP_LEFT, 5, 0);
  lv_obj_t *l = lv_label_create(backBtn);
  lv_label_set_text(l, "<");
  lv_obj_center(l);
  lv_obj_add_event_cb(
      backBtn,
      [](lv_event_t *e) {
        if (snake_timer)
          lv_timer_del(snake_timer);
        snake_timer = nullptr;
        ui_main_show();
      },
      LV_EVENT_CLICKED, NULL);

  // Tap game area to reset if game over
  lv_obj_add_event_cb(
      ui_GameArea,
      [](lv_event_t *e) {
        if (game_over)
          reset_snake();
      },
      LV_EVENT_CLICKED, NULL);

  reset_snake();
  snake_timer = lv_timer_create(game_loop, 300, NULL);

  lv_scr_load_anim(ui_SnakeScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
}
