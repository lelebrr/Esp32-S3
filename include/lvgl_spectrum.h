#ifndef LVGL_SPECTRUM_H
#define LVGL_SPECTRUM_H

#include <Arduino.h>

void setup_spectrum_ui();
void show_spectrum_view();
void update_spectrum_data(int16_t *rssi_data, int len);

#endif
