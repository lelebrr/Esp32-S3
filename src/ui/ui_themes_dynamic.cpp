#include "ui_themes.h"

ThemeColors g_dynamic_theme = {.bg = LV_COLOR_MAKE(0x05, 0x05, 0x05),
                               .panel = LV_COLOR_MAKE(0x10, 0x10, 0x10),
                               .primary = LV_COLOR_MAKE(0x88, 0x00, 0xFF),
                               .secondary = LV_COLOR_MAKE(0xFF, 0x00, 0x44),
                               .accent = LV_COLOR_MAKE(0xFF, 0x00, 0x88),
                               .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
                               .warning = LV_COLOR_MAKE(0xFF, 0x88, 0x00),
                               .success = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
                               .text = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
                               .text_dim = LV_COLOR_MAKE(0xAA, 0xAA, 0xAA),
                               .radius = 8,
                               .glass_opacity = 200};

void ui_update_theme_from_dragon_color(uint32_t color) {
  lv_color_t c = lv_color_hex(color);

  // Set Primary to Dragon Color
  g_dynamic_theme.primary = c;

  // Set Secondary to complimentary or darker shade
  // Simple logic: shift hue or just dim
  g_dynamic_theme.secondary = c;             // For now same
  g_dynamic_theme.accent = lv_color_white(); // Contrast

  // Background - very dark version of color
  // This is hard with just lv_color_t without HSL helper
  // Assuming black background for dragon focus
  g_dynamic_theme.bg = LV_COLOR_MAKE(0x00, 0x00, 0x00);
  g_dynamic_theme.panel = LV_COLOR_MAKE(0x10, 0x10, 0x10);
}
