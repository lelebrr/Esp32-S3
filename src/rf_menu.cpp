#include "rf_menu.h"
#include <SD.h>

// ============================================================================
// RF MENU IMPLEMENTATION
// ============================================================================

namespace RFMenu {

// State variables
RFMenuState currentState = RF_STATE_IDLE;
float selectedFrequency = FREQ_433;
RFProtocol selectedProtocol = PROTO_PRINCETON;

// ============================
// INITIALIZATION
// ============================
void init() {
    currentState = RF_STATE_IDLE;
    selectedFrequency = FREQ_433;
    selectedProtocol = PROTO_PRINCETON;
    
    // Ensure SD card has subghz folder
    if (SD.exists("/subghz") == false) {
        SD.mkdir("/subghz");
    }
    
    Serial.println("[RF_MENU] Initialized");
}

// ============================
// ATTACK CONTROLS
// ============================
void startSelectedAttack(uint8_t menuIndex) {
    if (menuIndex >= RF_MENU_COUNT) return;
    
    AttackType attack = rfMainMenu[menuIndex].attackType;
    
    Serial.printf("[RF_MENU] Starting: %s\n", rfMainMenu[menuIndex].name);
    
    // Update state based on attack type
    switch (attack) {
        case ATTACK_RF_JAMMER_433:
        case ATTACK_RF_JAMMER_315:
        case ATTACK_RF_JAMMER_868:
        case ATTACK_RF_JAMMER_BURST:
        case ATTACK_RF_JAMMER_SMART:
            currentState = RF_STATE_JAMMING;
            break;
            
        case ATTACK_RF_CAPTURE:
        case ATTACK_RF_SCAN:
            currentState = RF_STATE_CAPTURING;
            break;
            
        case ATTACK_RF_REPLAY:
        case ATTACK_RF_GHOST_REPLAY:
            currentState = RF_STATE_REPLAYING;
            break;
            
        case ATTACK_RF_SPECTRUM:
            currentState = RF_STATE_SCANNING;
            break;
            
        case ATTACK_RF_BRUTE_FORCE:
        case ATTACK_RF_DEBRUIJN:
            currentState = RF_STATE_BRUTEFORCE;
            break;
            
        default:
            currentState = RF_STATE_IDLE;
            break;
    }
    
    attacks_start(attack);
}

void stopCurrentAttack() {
    attacks_stop();
    currentState = RF_STATE_IDLE;
    Serial.println("[RF_MENU] Attack stopped");
}

// ============================
// FILE MANAGEMENT
// ============================
uint8_t listCapturedFiles(char filenames[][32], uint8_t maxFiles) {
    File dir = SD.open("/subghz");
    if (!dir) return 0;
    
    uint8_t count = 0;
    while (count < maxFiles) {
        File entry = dir.openNextFile();
        if (!entry) break;
        
        if (!entry.isDirectory()) {
            strncpy(filenames[count], entry.name(), 31);
            filenames[count][31] = '\0';
            count++;
        }
        entry.close();
    }
    dir.close();
    
    return count;
}

bool loadCapturedFile(const char* filename) {
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "/subghz/%s", filename);
    
    CapturedSignal sig;
    
    // Try Flipper format first
    if (strstr(filename, ".sub") != nullptr) {
        if (RFCore::loadFlipperFormat(fullPath, &sig)) {
            Serial.printf("[RF_MENU] Loaded Flipper file: %s\n", filename);
            return true;
        }
    }
    
    // Try native format
    if (RFCore::loadSignal(fullPath, &sig)) {
        Serial.printf("[RF_MENU] Loaded signal: %s\n", filename);
        return true;
    }
    
    Serial.printf("[RF_MENU] Failed to load: %s\n", filename);
    return false;
}

bool deleteCapturedFile(const char* filename) {
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "/subghz/%s", filename);
    
    if (SD.remove(fullPath)) {
        Serial.printf("[RF_MENU] Deleted: %s\n", filename);
        return true;
    }
    return false;
}

// ============================
// UI HELPERS
// ============================
const char* getFrequencyName(float freq) {
    if (freq < 320.0f) return "315 MHz (US)";
    if (freq < 450.0f) return "433 MHz (EU)";
    if (freq < 900.0f) return "868 MHz (EU)";
    return "915 MHz (US)";
}

const char* getCurrentStatusText() {
    switch (currentState) {
        case RF_STATE_IDLE:
            return "Ready";
        case RF_STATE_JAMMING:
            return "JAMMING...";
        case RF_STATE_CAPTURING:
            return "Capturing...";
        case RF_STATE_REPLAYING:
            return "Replaying...";
        case RF_STATE_SCANNING:
            return "Scanning...";
        case RF_STATE_BRUTEFORCE:
            return "Brute Force...";
        default:
            return "Unknown";
    }
}

void updateStatusDisplay() {
    // Called from main loop to update UI
    // This would integrate with LVGL labels
    
    if (currentState == RF_STATE_BRUTEFORCE && RFCore::isBruteForcing()) {
        uint32_t progress = RFCore::getBruteProgress();
        uint32_t total = RFCore::getBruteTotal();
        float percent = (float)progress / total * 100.0f;
        Serial.printf("\r[RF] Brute: %.1f%% (%lu/%lu)   ", percent, progress, total);
    }
}

// ============================
// MENU DISPLAY (Serial Interface)
// ============================
void showMainMenu() {
    Serial.println("\n=== RF SubGHz Menu ===");
    for (int i = 0; i < RF_MENU_COUNT; i++) {
        Serial.printf("%d. %s\n", i + 1, rfMainMenu[i].name);
    }
    Serial.printf("Current Freq: %.2f MHz\n", selectedFrequency);
    Serial.printf("Status: %s\n", getCurrentStatusText());
    Serial.println("======================");
}

void showFrequencySelect() {
    Serial.println("\n=== Select Frequency ===");
    for (int i = 0; i < FREQ_PRESET_COUNT; i++) {
        Serial.printf("%d. %.2f MHz\n", i + 1, frequencyPresets[i]);
    }
    Serial.println("========================");
}

void showProtocolSelect() {
    Serial.println("\n=== Select Protocol ===");
    for (int i = 0; i < PROTO_PRESET_COUNT; i++) {
        Serial.printf("%d. %s\n", i + 1, protocolPresets[i].name);
    }
    Serial.println("=======================");
}

void showCapturedSignals() {
    Serial.println("\n=== Captured Signals ===");
    
    char files[16][32];
    uint8_t count = listCapturedFiles(files, 16);
    
    if (count == 0) {
        Serial.println("No captures found");
    } else {
        for (int i = 0; i < count; i++) {
            Serial.printf("%d. %s\n", i + 1, files[i]);
        }
    }
    Serial.println("========================");
}

void showStatus() {
    Serial.println("\n=== RF Status ===");
    RFCore::printStatus();
    Serial.printf("Menu State: %s\n", getCurrentStatusText());
    Serial.println("=================");
}

} // namespace RFMenu
