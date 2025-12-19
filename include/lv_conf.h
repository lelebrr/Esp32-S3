/**
 * @file lv_conf.h
 * Configuration file for v9.0.0-dev
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to the `lvgl` folder
 * 2. or any other place and
 *    - define `LV_CONF_INCLUDE_SIMPLE`
 *    - add the path as include path
 */

/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>


/*====================
   COLOR SETTINGS
 *====================*/

/** Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 24 (RGB888), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

#define LV_HOR_RES_MAX 480

/*====================
   MEMORY SETTINGS
 *====================*/

/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc` or `lv_malloc` and `lv_mem_free` or `lv_free`*/
#define LV_USE_CUSTOM_MALLOC 0
#if LV_USE_CUSTOM_MALLOC == 0
    /*Size of the memory available for `lv_mem_alloc` or `lv_malloc` in bytes (>= 2kB)*/
    #define LV_MEM_SIZE (32U * 1024U)          /*[bytes]*/

    /*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
    #define LV_MEM_ADR 0     /*0: unused*/
#endif

/*====================
   HAL SETTINGS
 *====================*/

/*1: use a custom tick source (e.g. non-standard OS functionality).
 *If 0: `lv_tick_inc` function must be called periodically (e.g. from a timer interrupt or task)*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())    /*Expression evaluating to current system time in ms*/
#endif

/* Fonts */
#define LV_FONT_MONTSERRAT_8  1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_38 1
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_42 1
#define LV_FONT_MONTSERRAT_44 1
#define LV_FONT_MONTSERRAT_46 1
#define LV_FONT_MONTSERRAT_48 1

/*-------------
 * Drawing
 *-----------*/

/*Enable complex draw engine*/
#define LV_USE_DRAW_MASKS 1

#define LV_USE_DRAW_BUF 1
#define LV_DRAW_BUF_SIZE (LV_HOR_RES_MAX * 40) // só 40 linhas por vez

/*------------------
 * Monitor
 *-----------------*/
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

/*------------------
 * Log
 *-----------------*/
#define LV_USE_LOG 0

/*------------------
 * Font
 *-----------------*/
#define LV_USE_FONT_COMPRESSED 1
#define LV_FONT_FMT_TXT_LARGE 0

/*------------------
 * Examples
 *-----------------*/
#define LV_BUILD_EXAMPLES 1

/*------------------
 * Demos
 *-----------------*/
#define LV_BUILD_DEMOS 1

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/
