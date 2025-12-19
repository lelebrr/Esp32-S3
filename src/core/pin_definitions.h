#pragma once
/**
 * @file pin_definitions.h
 * @brief LCD and hardware pin definitions
 *
 * This file provides LCD dimensions and basic hardware constants
 */

#include <Arduino.h>

// LCD Dimensions - Use TFT_eSPI definitions if available
#ifndef LCD_WIDTH
#ifdef TFT_WIDTH
#define LCD_WIDTH TFT_WIDTH
#else
#define LCD_WIDTH 320
#endif
#endif

#ifndef LCD_HEIGHT
#ifdef TFT_HEIGHT
#define LCD_HEIGHT TFT_HEIGHT
#else
#define LCD_HEIGHT 480
#endif
#endif

// Touch calibration (if needed)
#ifndef TOUCH_CS
#define TOUCH_CS -1
#endif

// Backlight pin (if controlled by software)
#ifndef TFT_BL
#define TFT_BL -1
#endif
