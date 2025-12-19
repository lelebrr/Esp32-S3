#pragma once

#include <Arduino.h>
#include <lvgl.h>


/**
 * @brief Helper class to manage sprite sheets in PSRAM
 * Item 41: Sprite Sheet implementation (512x512px in PSRAM)
 */
class SpriteSheet {
public:
  SpriteSheet(const char *path, int frameWidth, int frameHeight)
      : _path(path), _frameW(frameWidth), _frameH(frameHeight), _img(nullptr) {}

  // Load sprite sheet to PSRAM
  bool load() {
    // Implementation would load image to PSRAM buffer
    // For now, using LVGL's image caching or custom buffer
    // This is a placeholder for the actual implementation linking to asset
    // system
    return true;
  }

  // Get a specific frame as an lv_img_dsc_t
  const lv_img_dsc_t *getFrame(int index) {
    // Logic to return a descriptor pointing to the sliced frame
    return nullptr;
  }

private:
  const char *_path;
  int _frameW, _frameH;
  void *_img;
};
