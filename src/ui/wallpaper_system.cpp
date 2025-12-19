#include "wallpaper_system.h"
#include "globals.h"
#include <FS.h>
#include <PNGdec.h> // Include at top
#include <SD_MMC.h>
#include <debug_log.h>

WallpaperSystem wallpaper_system;

// Global/Static for PNGdec
static PNG png;
static File pngFile;

// Callbacks must match PNGdec signatures
void *pngOpen(const char *filename, int32_t *size) {
    pngFile = SD_MMC.open(filename, FILE_READ);
    if (!pngFile) return NULL;
    *size = pngFile.size();
    return &pngFile;
}

void pngClose(void *handle) {
    if (pngFile) pngFile.close();
}

int32_t pngRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
    if (!pngFile) return 0;
    return pngFile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *handle, int32_t position) {
    if (!pngFile) return 0;
    return pngFile.seek(position);
}

// Fixed signature: return int
int pngDraw(PNGDRAW *pDraw) {
    WallpaperSystem *ws = &wallpaper_system;
    if (!ws || !ws->_pixelBuffer) return 0;

    uint16_t *pixels = (uint16_t *)ws->_pixelBuffer;
    int width = ws->_width;

    // Check bounds
    if (pDraw->y >= ws->_height) return 0;

    // Convert line to RGB565 and store
    png.getLineAsRGB565(pDraw, &pixels[pDraw->y * width], PNG_RGB565_BIG_ENDIAN, 0xffffffff);

    return 1; // 1 = continue
}

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════
WallpaperSystem::WallpaperSystem()
    : _currentIndex(0), _loaded(false), _pixelBuffer(nullptr), _width(0), _height(0),
      _lastSlideshowChange(0) {

    // Default configuration
    _config.enabled = true;
    memset(_config.currentWallpaper, 0, sizeof(_config.currentWallpaper));
    _config.menuTransparency = 70; // 70% opaque
    _config.blurEnabled = true;
    _config.blurStrength = 5;
    _config.slideshow = false;
    _config.slideshowInterval = 60; // 1 minute
}

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════
void WallpaperSystem::begin() {
    LOG_UI("Initializing wallpaper system...");

    // Scan for wallpapers on SD
    int count = scanWallpapers();
    LOG_UI("Found %d wallpapers", count);

    // Load default or saved wallpaper
    if (strlen(_config.currentWallpaper) > 0) {
        loadWallpaper(_config.currentWallpaper);
    } else if (_gallery.size() > 0) {
        loadWallpaper(_gallery[0].filename);
    }
}

void WallpaperSystem::update() {
    // Handle slideshow
    if (_config.slideshow && _gallery.size() > 1) {
        uint32_t now = millis();
        if (now - _lastSlideshowChange >= _config.slideshowInterval * 1000) {
            nextWallpaper();
            _lastSlideshowChange = now;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// WALLPAPER MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════
bool WallpaperSystem::loadWallpaper(const char *filename) {
    if (!filename || strlen(filename) == 0) { return false; }

    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", WALLPAPER_DIR, filename);

    LOG_UI("Loading: %s", fullPath);

    // Mark as current config
    strncpy(_config.currentWallpaper, filename, sizeof(_config.currentWallpaper) - 1);

    // Load PNG
    return loadPNG(fullPath);
}

bool WallpaperSystem::setWallpaper(const char *filename) { return loadWallpaper(filename); }

bool WallpaperSystem::nextWallpaper() {
    if (_gallery.size() == 0) return false;

    _currentIndex = (_currentIndex + 1) % _gallery.size();
    return loadWallpaper(_gallery[_currentIndex].filename);
}

bool WallpaperSystem::prevWallpaper() {
    if (_gallery.size() == 0) return false;

    _currentIndex = (_currentIndex - 1 + _gallery.size()) % _gallery.size();
    return loadWallpaper(_gallery[_currentIndex].filename);
}

// ═══════════════════════════════════════════════════════════════════════════
// GALLERY SCANNING
// ═══════════════════════════════════════════════════════════════════════════
int WallpaperSystem::scanWallpapers() {
    _gallery.clear();

    if (!SD_MMC.exists(WALLPAPER_DIR)) {
        LOG_UI("Creating wallpapers directory...");
        SD_MMC.mkdir(WALLPAPER_DIR);
        return 0;
    }

    File dir = SD_MMC.open(WALLPAPER_DIR);
    if (!dir || !dir.isDirectory()) {
        LOG_ERROR("Failed to open wallpapers directory");
        return 0;
    }

    File file = dir.openNextFile();
    while (file) {
        String name = file.name();
        if (name.endsWith(".png") || name.endsWith(".PNG")) {
            WallpaperInfo info;
            strncpy(info.filename, name.c_str(), sizeof(info.filename) - 1);
            info.filename[sizeof(info.filename) - 1] = '\0';
            info.width = 128; // Assumed until loaded
            info.height = 160;
            info.filesize = file.size();
            info.isDefault = name.startsWith("default_");

            _gallery.push_back(info);
            LOG_UI("Found: %s (%u bytes)", info.filename, info.filesize);
        }
        file = dir.openNextFile();
    }

    dir.close();
    return _gallery.size();
}

// ═══════════════════════════════════════════════════════════════════════════
// TRANSPARENCY & BLUR
// ═══════════════════════════════════════════════════════════════════════════
void WallpaperSystem::setMenuTransparency(uint8_t percent) {
    _config.menuTransparency = constrain(percent, 0, 90);
    LOG_UI("Menu transparency: %d%%", _config.menuTransparency);
}

void WallpaperSystem::enableBlur(bool enable) {
    _config.blurEnabled = enable;
    LOG_UI("Blur: %s", enable ? "ON" : "OFF");
}

void WallpaperSystem::setBlurStrength(uint8_t strength) { _config.blurStrength = constrain(strength, 1, 10); }

// ═══════════════════════════════════════════════════════════════════════════
// SLIDESHOW
// ═══════════════════════════════════════════════════════════════════════════
void WallpaperSystem::startSlideshow(uint16_t intervalSeconds) {
    _config.slideshow = true;
    _config.slideshowInterval = intervalSeconds;
    _lastSlideshowChange = millis();
    LOG_UI("Slideshow started: %ds interval", intervalSeconds);
}

void WallpaperSystem::stopSlideshow() {
    _config.slideshow = false;
    LOG_UI("Slideshow stopped");
}

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════
void WallpaperSystem::setConfig(const WallpaperConfig &config) {
    _config = config;

    // Load wallpaper if changed
    if (strlen(config.currentWallpaper) > 0) { loadWallpaper(config.currentWallpaper); }
}

// ═══════════════════════════════════════════════════════════════════════════
// FILE OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════
bool WallpaperSystem::deleteWallpaper(const char *filename) {
    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", WALLPAPER_DIR, filename);

    if (SD_MMC.remove(fullPath)) {
        LOG_UI("Deleted: %s", filename);
        scanWallpapers(); // Refresh gallery
        return true;
    }
    return false;
}

bool WallpaperSystem::renameWallpaper(const char *oldName, const char *newName) {
    char oldPath[128], newPath[128];
    snprintf(oldPath, sizeof(oldPath), "%s/%s", WALLPAPER_DIR, oldName);
    snprintf(newPath, sizeof(newPath), "%s/%s", WALLPAPER_DIR, newName);

    if (SD_MMC.rename(oldPath, newPath)) {
        LOG_UI("Renamed: %s -> %s", oldName, newName);
        scanWallpapers(); // Refresh gallery
        return true;
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// BUFFER MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════
void WallpaperSystem::freeBuffer() {
    if (_pixelBuffer) {
        free(_pixelBuffer);
        _pixelBuffer = nullptr;
    }
    _loaded = false;
    _width = 0;
    _height = 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// CORE PNG LOADING
// ═══════════════════════════════════════════════════════════════════════════
bool WallpaperSystem::loadPNG(const char *path) {
    LOG_UI("Loading PNG: %s", path);

    // Free any existing buffer
    freeBuffer();

    int16_t rc = png.open(path, pngOpen, pngClose, pngRead, pngSeek, pngDraw);
    if (rc != PNG_SUCCESS) {
        LOG_ERROR("PNG Open failed: %d", rc);
        return false;
    }

    uint16_t pngWidth = png.getWidth();
    uint16_t pngHeight = png.getHeight();

    LOG_UI("PNG size: %dx%d", pngWidth, pngHeight);

    if (pngWidth > 1024 || pngHeight > 1024) { // Sanity check
        LOG_ERROR("Image too large");
        png.close();
        return false;
    }

    // Allocate buffer for RGB565 pixels (2 bytes per pixel)
    size_t bufferSize = pngWidth * pngHeight * 2;
    _pixelBuffer = (uint8_t *)heap_caps_malloc(bufferSize, MALLOC_CAP_SPIRAM);
    if (!_pixelBuffer) {
        // Try regular RAM if PSRAM not available or full
        _pixelBuffer = (uint8_t *)malloc(bufferSize);
    }

    if (!_pixelBuffer) {
        LOG_ERROR("Failed to allocate buffer");
        png.close();
        return false;
    }

    _width = pngWidth;
    _height = pngHeight;

    // Decode image
    rc = png.decode(NULL, 0); // 0 = no filter (auto)
    png.close();

    if (rc != PNG_SUCCESS) {
        LOG_ERROR("PNG Decode failed: %d", rc);
        freeBuffer();
        return false;
    }

    _loaded = true;
    LOG_UI("Loaded successfully (%zu bytes)", bufferSize);
    return true;
}

bool WallpaperSystem::generateThumbnail(
    const char *filename, uint8_t *outBuffer, uint16_t thumbW, uint16_t thumbH
) {
    // Load the full wallpaper first if not already loaded
    if (!_loaded || strcmp(_config.currentWallpaper, filename) != 0) {
        char fullPath[128];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", WALLPAPER_DIR, filename);
        if (!loadPNG(fullPath)) { return false; }
    }

    if (!_pixelBuffer || _width == 0 || _height == 0) { return false; }

    LOG_UI("Generating %dx%d thumbnail", thumbW, thumbH);

    float xRatio = (float)(_width - 1) / thumbW;
    float yRatio = (float)(_height - 1) / thumbH;

    uint16_t *thumbBuffer = (uint16_t *)outBuffer;

    for (int y = 0; y < thumbH; y++) {
        for (int x = 0; x < thumbW; x++) {
            // Source coordinates
            float srcX = x * xRatio;
            float srcY = y * yRatio;

            int x1 = (int)srcX;
            int y1 = (int)srcY;
            int x2 = min(x1 + 1, (int)_width - 1);
            int y2 = min(y1 + 1, (int)_height - 1);

            float xFrac = srcX - x1;
            float yFrac = srcY - y1;

            // Get the 4 neighbor pixels (cast uint8_t* to uint16_t* for RGB565)
            uint16_t *srcPixels = (uint16_t *)_pixelBuffer;
            uint16_t p11 = srcPixels[y1 * _width + x1];
            uint16_t p21 = srcPixels[y1 * _width + x2];
            uint16_t p12 = srcPixels[y2 * _width + x1];
            uint16_t p22 = srcPixels[y2 * _width + x2];

            // Extract RGB565 components callback
            auto extractRGB = [](uint16_t c, uint8_t &r, uint8_t &g, uint8_t &b) {
                r = (c >> 11) & 0x1F;
                g = (c >> 5) & 0x3F;
                b = c & 0x1F;
            };

            uint8_t r11, g11, b11, r21, g21, b21, r12, g12, b12, r22, g22, b22;
            extractRGB(p11, r11, g11, b11);
            extractRGB(p21, r21, g21, b21);
            extractRGB(p12, r12, g12, b12);
            extractRGB(p22, r22, g22, b22);

            // Bilinear interpolation
            float r = (1 - xFrac) * (1 - yFrac) * r11 + xFrac * (1 - yFrac) * r21 +
                      (1 - xFrac) * yFrac * r12 + xFrac * yFrac * r22;
            float g = (1 - xFrac) * (1 - yFrac) * g11 + xFrac * (1 - yFrac) * g21 +
                      (1 - xFrac) * yFrac * g12 + xFrac * yFrac * g22;
            float b = (1 - xFrac) * (1 - yFrac) * b11 + xFrac * (1 - yFrac) * b21 +
                      (1 - xFrac) * yFrac * b12 + xFrac * yFrac * b22;

            // Pack back to RGB565
            thumbBuffer[y * thumbW + x] = ((uint8_t)r << 11) | ((uint8_t)g << 5) | (uint8_t)b;
        }
    }

    LOG_UI("Thumbnail generated");
    return true;
}
