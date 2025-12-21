// TFT_eSPI User_Setup.h
// Monster S3 Firmware - ILI9341 240x320 with XPT2046 Touch
// Place in include/ folder or lib/TFT_eSPI/

#define USER_SETUP_INFO "Monster S3 - ILI9341 240x320"

// ========================================
// DISPLAY DRIVER
// ========================================
#define ILI9341_DRIVER

// ========================================
// SCREEN DIMENSIONS
// ========================================
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// ========================================
// ESP32-S3 PIN DEFINITIONS (SPI A - FSPI for Display)
// Synced with pin_config.h
// ========================================
#define TFT_MISO 16 // Touch XPT2046 T_DO (IMPORTANT: Connect T_DO to GPIO 16!)
#define TFT_MOSI 13 // Display MOSI
#define TFT_SCLK 14 // Display Clock
#define TFT_CS 10   // Display Chip Select
#define TFT_DC 12   // Data/Command
#define TFT_RST 11  // Display Reset
#define TFT_BL 21   // Backlight (PWM)

// ========================================
// TOUCH SCREEN PINS (XPT2046)
// ========================================
#define TOUCH_CS 15 // Touch CS (SAFE)
// Touch uses same SPI bus as display

// ========================================
// COLOR CORRECTION
// ========================================
#define TFT_INVERSION_ON // Fix washed-out colors on some displays
// #define TFT_RGB_ORDER TFT_BGR    // Uncomment if red/blue are swapped

// ========================================
// SPI SETTINGS
// ========================================
#define SPI_FREQUENCY 27000000      // 27 MHz (more stable for generic displays)
#define SPI_READ_FREQUENCY 20000000 // 20 MHz
#define SPI_TOUCH_FREQUENCY 2500000 // 2.5 MHz (slower for touch accuracy)

// ========================================
// ESP32-S3 SPI BUS CONFIGURATION
// ========================================
// Use HSPI for ESP32-S3 with custom pins (TFT_eSPI will call spi.begin internally)
#define USE_HSPI_PORT

// ========================================
// FONTS
// ========================================
#define LOAD_GLCD  // Font 1
#define LOAD_FONT2 // Font 2
#define LOAD_FONT4 // Font 4
#define LOAD_FONT6 // Font 6
#define LOAD_FONT7 // Font 7
#define LOAD_FONT8 // Font 8
#define LOAD_GFXFF // FreeFonts

#define SMOOTH_FONT

// ========================================
// PERFORMANCE
// ========================================
// Use DMA for faster updates
#define TFT_SPI_MODE SPI_MODE0
