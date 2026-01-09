/**
 * @file lv_conf.h
 * Configuration file for v8.3.11
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
#include <stdlib.h> // Required for malloc

// ==========================
// COLOR SETTINGS
// ==========================
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1 

// ==========================
// MEMORY SETTINGS
// ==========================
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (64U * 1024U) 
#define LV_MEM_ADR 0
#define LV_MEM_POOL_INCLUDE <stdint.h>
#define LV_MEM_POOL_ALLOC malloc
#define LV_MEM_POOL_FREE free

// ==========================
// HAL SETTINGS
// ==========================
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

// ==========================
// FEATURE CONFIGURATION
// ==========================
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1

// ==========================
// FONT USAGE
// ==========================
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

// ==========================
// WIDGET USAGE
// ==========================
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_IMG 1
#define LV_USE_LINE 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LIST 1
#define LV_USE_MSGBOX 1

// ==========================
// COMPILER SETTINGS
// ==========================
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_FAST_MEM
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning 

#endif /*LV_CONF_H*/
