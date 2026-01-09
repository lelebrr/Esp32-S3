/**
 * @file chameleon_logo.h
 * @brief MorphNode Chameleon Logo for LVGL Boot Screen
 *
 * Contains the chameleon mascot image data in RGB565 format for LVGL.
 * Logo size: 80x80 pixels (optimized for 240x320 display)
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#ifndef CHAMELEON_LOGO_H
#define CHAMELEON_LOGO_H

#include <stdint.h>

// Logo dimensions
#define CHAMELEON_LOGO_WIDTH 80
#define CHAMELEON_LOGO_HEIGHT 80

// Simple stylized chameleon logo in RGB565 format
// This is a simplified vector representation for embedded display
// Colors: Cyan (0x07FF), Magenta (0xF81F), Dark (0x0000)

// Logo data - placeholder array, actual conversion would be done with tools
// For now, we'll use LVGL's built-in drawing functions to render a stylized version

// Color definitions for the logo
#define CYBER_CYAN 0x07DF    // Bright cyan
#define CYBER_MAGENTA 0xF81F // Neon magenta
#define CYBER_DARK 0x1082    // Dark blue background
#define CYBER_GREEN 0x07E0   // Matrix green
#define CYBER_RED 0xF800     // Target red for eye

// Simple ASCII art representation for Serial debug
static const char *CHAMELEON_ASCII = R"(
    .--.     .--.
   ( o  )   (  o )
    '--'____'--'
   /   CYBER   \
  /    CLOAK    \
 |  ESP32-S3    |
  \  Security  /
   \__________/
)";

/**
 * @brief Draw stylized chameleon logo using LVGL primitives
 * @param parent LVGL parent object
 * @param x X position
 * @param y Y position
 * @return Created logo object
 */

// We'll implement the actual drawing in lvgl_menu.cpp using LVGL drawing functions
// This provides color and dimension constants for the logo

#endif // CHAMELEON_LOGO_H
