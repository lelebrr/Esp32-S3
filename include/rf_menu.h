#ifndef RF_MENU_H
#define RF_MENU_H

#include <Arduino.h>
#include "rf_core.h"
#include "attacks_manager.h"

// ============================================================================
// RF MENU - SubGHz Attack Interface
// ============================================================================

namespace RFMenu {

// Menu State
typedef enum {
    RF_STATE_IDLE = 0,
    RF_STATE_JAMMING,
    RF_STATE_CAPTURING,
    RF_STATE_REPLAYING,
    RF_STATE_SCANNING,
    RF_STATE_BRUTEFORCE
} RFMenuState;

// Current state
extern RFMenuState currentState;
extern float selectedFrequency;
extern RFProtocol selectedProtocol;

// ============================
// MENU ITEMS
// ============================
typedef struct {
    const char* name;
    const char* description;
    AttackType attackType;
    void (*callback)(void);
} RFMenuItem;

// Main RF Menu Items
static const RFMenuItem rfMainMenu[] = {
    {"Jammer 433MHz", "Flood 433MHz with noise", ATTACK_RF_JAMMER_433, nullptr},
    {"Jammer 315MHz", "Flood 315MHz with noise", ATTACK_RF_JAMMER_315, nullptr},
    {"Jammer 868MHz", "Flood 868/915MHz", ATTACK_RF_JAMMER_868, nullptr},
    {"Jammer Smart", "Detect & jam active TX", ATTACK_RF_JAMMER_SMART, nullptr},
    {"Capture Signal", "Record RF signal", ATTACK_RF_CAPTURE, nullptr},
    {"Replay Last", "Transmit captured signal", ATTACK_RF_REPLAY, nullptr},
    {"Ghost Replay", "Random timing replay", ATTACK_RF_GHOST_REPLAY, nullptr},
    {"Spectrum", "Frequency analyzer", ATTACK_RF_SPECTRUM, nullptr},
    {"Brute Force", "Try all codes", ATTACK_RF_BRUTE_FORCE, nullptr},
    {"De Bruijn", "Optimized brute", ATTACK_RF_DEBRUIJN, nullptr},
};
#define RF_MENU_COUNT (sizeof(rfMainMenu) / sizeof(RFMenuItem))

// Frequency presets
static const float frequencyPresets[] = {
    315.00f,   // US Garage
    433.92f,   // EU Garage, Car keys
    868.35f,   // EU IoT
    915.00f,   // US IoT
};
#define FREQ_PRESET_COUNT 4

// Protocol presets for brute force
static const struct {
    const char* name;
    RFProtocol proto;
    uint8_t bits;
} protocolPresets[] = {
    {"Princeton 24-bit", PROTO_PRINCETON, 24},
    {"CAME 12-bit", PROTO_CAME, 12},
    {"Nice FLO 12-bit", PROTO_NICE_FLO, 12},
    {"Holtek 12-bit", PROTO_HOLTEK, 12},
    {"Linear 10-bit", PROTO_LINEAR, 10},
};
#define PROTO_PRESET_COUNT 5

// ============================
// INITIALIZATION
// ============================
void init();

// ============================
// MENU ACTIONS
// ============================
void showMainMenu();
void showFrequencySelect();
void showProtocolSelect();
void showCapturedSignals();
void showStatus();

// ============================
// ATTACK CONTROLS
// ============================
void startSelectedAttack(uint8_t menuIndex);
void stopCurrentAttack();

// ============================
// FILE MANAGEMENT
// ============================
uint8_t listCapturedFiles(char filenames[][32], uint8_t maxFiles);
bool loadCapturedFile(const char* filename);
bool deleteCapturedFile(const char* filename);

// ============================
// UI HELPERS
// ============================
const char* getFrequencyName(float freq);
const char* getCurrentStatusText();
void updateStatusDisplay();

} // namespace RFMenu

#endif // RF_MENU_H
