#pragma once
#include "lvgl.h"

void ui_chat_screen_init(void);
void ui_chat_add_message(const char *sender, const char *text, bool isMe);
