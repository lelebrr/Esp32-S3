/**
 * @file globals.h
 * @brief Legacy compatibility header - MorphNode Firmware
 *
 * Note: This file contains legacy definitions for compatibility.
 * The main firmware uses pin_config.h and individual driver headers.
 */

#ifndef __GLOBALS__
#define __GLOBALS__

#include <Arduino.h>

// ============================================================================
// LEGACY COMPATIBILITY - Keep for reference only
// ============================================================================

// Color definition
#define ALCOLOR TFT_RED

// Global state structure for system monitoring
struct GlobalState {
    bool wifi_enabled = true;
    long handshakes_captured = 0;
    bool ble_enabled = false;
    bool voice_enabled = false;
    int battery_percent = 100;
    bool is_charging = false;
    bool chatgpt_enabled = false;
    char openai_voice[64] = "";
    char openai_model[64] = "gpt-3.5-turbo";
    char openai_api_key[256] = "";
    bool wps_pixie_auto = false;
    bool wps_bruteforce = false;
    int wps_flood_intensity = 0;
    int rogue_ap_count = 0;
    bool auto_capture_new_only = false;
    bool auto_save_pcap = false;
    bool auto_attack_favorites = false;
    bool insane_mode_enabled = false;
    int current_theme = 0;
    bool mascot_enabled = false;
};

extern GlobalState g_state;

// Touch point structure
struct TouchPoint {
    bool pressed = false;
    uint16_t x;
    uint16_t y;

    void Clear(void) {
        pressed = false;
        x = 0;
        y = 0;
    }
};

extern TouchPoint touchPoint;

// Display dimensions (set by platformio.ini)
extern volatile int tftWidth;
extern volatile int tftHeight;

// Screen control
extern int currentScreenBrightness;
extern unsigned long previousMillis;
extern bool isSleeping;
extern bool isScreenOff;
extern bool dimmer;

// Common state flags
extern bool sdcardMounted;
extern bool wifiConnected;
extern bool isWebUIActive;
extern bool BLEConnected;
extern bool gpsConnected;
extern bool returnToMenu;

#endif // __GLOBALS__
