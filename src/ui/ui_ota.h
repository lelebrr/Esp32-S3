#pragma once
#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Show OTA Update Screen (Tip 38)
 * Sweating Dragon + % + "Don't Disconnect"
 */
void ui_ota_show();

/**
 * @brief Update OTA Progress
 * @param progress 0-100
 */
void ui_ota_update_progress(int progress);
