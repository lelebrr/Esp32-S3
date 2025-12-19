#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Modulos
#define MOD_NONE 0
#define MOD_CC1101 1
#define MOD_PN532 2
#define MOD_BLE 3

// Shared Functions
void enable_module(int mod);

#endif
