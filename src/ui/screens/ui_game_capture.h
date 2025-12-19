#pragma once
#include "lvgl.h"

void ui_capture_handshake_game_init(void);

// Called when a handshake is captured by the system
void game_report_handshake_capture(const char *ssid);
