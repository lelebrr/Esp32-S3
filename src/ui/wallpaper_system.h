#pragma once

#include <Arduino.h>
#include <PNGdec.h>
#undef local  // PNGdec defines 'local' as 'static' which conflicts with Timezone.h
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════
// WALLPAPER INFO
// ═══════════════════════════════════════════════════════════════════════════
struct WallpaperInfo {
  char filename[64];
  uint16_t width;
  uint16_t height;
  uint32_t filesize;
  bool isDefault;
};

// ═══════════════════════════════════════════════════════════════════════════
// WALLPAPER CONFIG
// ═══════════════════════════════════════════════════════════════════════════
struct WallpaperConfig {
  bool enabled;               // Enable wallpaper
  char currentWallpaper[64];  // Current wallpaper filename
  uint8_t menuTransparency;   // Menu panel transparency (0-90%)
  bool blurEnabled;           // Enable blur behind menus
  uint8_t blurStrength;       // Blur strength (1-10)
  bool slideshow;             // Auto-rotate wallpapers
  uint16_t slideshowInterval; // Seconds between changes
};

// ═══════════════════════════════════════════════════════════════════════════
// WALLPAPER SYSTEM CLASS
// ═══════════════════════════════════════════════════════════════════════════
class WallpaperSystem {
public:
  WallpaperSystem();

  // Initialization
  void begin();
  void update(); // For slideshow

  // Wallpaper management
  bool loadWallpaper(const char *filename);
  bool setWallpaper(const char *filename);
  bool nextWallpaper();
  bool prevWallpaper();

  // Gallery
  int scanWallpapers();
  std::vector<WallpaperInfo> getGallery() const { return _gallery; }
  int getWallpaperCount() const { return _gallery.size(); }
  int getCurrentIndex() const { return _currentIndex; }

  // Transparency
  void setMenuTransparency(uint8_t percent);
  uint8_t getMenuTransparency() const { return _config.menuTransparency; }

  // Blur
  void enableBlur(bool enable);
  void setBlurStrength(uint8_t strength);
  bool isBlurEnabled() const { return _config.blurEnabled; }

  // Slideshow
  void startSlideshow(uint16_t intervalSeconds);
  void stopSlideshow();
  bool isSlideshowActive() const { return _config.slideshow; }

  // Config
  void setConfig(const WallpaperConfig &config);
  WallpaperConfig getConfig() const { return _config; }

  // File operations
  bool deleteWallpaper(const char *filename);
  bool renameWallpaper(const char *oldName, const char *newName);

  // For rendering
  const uint8_t *getCurrentPixels() const { return _pixelBuffer; }
  bool hasWallpaper() const { return _loaded; }

  // Thumbnail generation
  bool generateThumbnail(const char *filename, uint8_t *outBuffer,
                         uint16_t thumbW, uint16_t thumbH);

private:
  WallpaperConfig _config;
  std::vector<WallpaperInfo> _gallery;
  int _currentIndex;
  bool _loaded;
  uint8_t *_pixelBuffer; // Raw pixels for current wallpaper
  uint16_t _width;
  uint16_t _height;
  uint32_t _lastSlideshowChange;

  bool loadPNG(const char *path);
  void freeBuffer();
  friend int pngDraw(PNGDRAW *pDraw);
};

// ═══════════════════════════════════════════════════════════════════════════
// DEFAULT WALLPAPERS (embedded in flash)
// ═══════════════════════════════════════════════════════════════════════════
// These are minimal placeholder images; full images would be on SD
#define WALLPAPER_DIR "/wallpapers"
#define WALLPAPER_EXTENSION ".png"
#define WALLPAPER_MAX_SIZE (128 * 160 * 3) // 128x160 RGB

extern WallpaperSystem wallpaper_system;
