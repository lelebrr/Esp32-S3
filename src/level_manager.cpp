#include "level_manager.h"

PlayerStats LevelManager::stats = {0, 1, "Script Kiddie", 0, 0};
uint32_t LevelManager::last_xp_save = 0;
uint32_t LevelManager::last_uptime_check = 0;

void LevelManager::init() {
    load();
    last_uptime_check = millis();
    Serial.println("[LevelManager] Init. Current Level: " + String(stats.level));
}

void LevelManager::loop() {
    // Add 1 XP every minute of uptime
    if (millis() - last_uptime_check >= 60000) {
        addXP(1); 
        last_uptime_check = millis();
    }

    // Auto-save every 5 minutes if changed
    if (millis() - last_xp_save >= 300000) {
        save();
        last_xp_save = millis();
    }
}

void LevelManager::addXP(uint32_t amount) {
    stats.xp += amount;
    updateLevel();
}

void LevelManager::onAttackPerformed() {
    stats.attacks_performed++;
    addXP(10);
    save(); // Save on significant events
}

void LevelManager::onWifiScan() {
    stats.wifi_scans++;
    addXP(5);
}

void LevelManager::updateLevel() {
    // Formula: Level = sqrt(XP / 50) + 1
    // Level 1: 0-49 XP
    // Level 2: 50-199 XP
    // Level 3: 200-449 XP
    // ...
    uint32_t new_level = (uint32_t)sqrt(stats.xp / 50) + 1;
    
    if (new_level > stats.level) {
        stats.level = new_level;
        Serial.println("[LevelManager] LEVEL UP! New Level: " + String(stats.level));
        // TODO: Trigger UI prompt or Sound?
        save();
    }
    
    // Update Title based on Level
    const char* t = calculateTitle(stats.level);
    strncpy(stats.title, t, sizeof(stats.title)-1);
}

const char* LevelManager::calculateTitle(uint32_t level) {
    if (level < 5) return "Script Kiddie";
    if (level < 10) return "Cyber-Lizard";
    if (level < 20) return "Net-Stalker";
    if (level < 30) return "Code-Breaker";
    if (level < 50) return "Sys-Admin";
    if (level < 75) return "Root-Access";
    if (level < 99) return "Null-Pointer";
    return "The Architect";
}

uint32_t LevelManager::getXP() { return stats.xp; }
uint32_t LevelManager::getLevel() { return stats.level; }
const char* LevelManager::getTitle() { return stats.title; }

float LevelManager::getProgressToNextLevel() {
    // Current Level Min XP
    uint32_t current_lvl_xp = (stats.level - 1) * (stats.level - 1) * 50;
    // Next Level Min XP
    uint32_t next_lvl_xp = (stats.level) * (stats.level) * 50;
    
    // Prevent div by zero
    if (next_lvl_xp == current_lvl_xp) return 0.0f;

    return (float)(stats.xp - current_lvl_xp) / (float)(next_lvl_xp - current_lvl_xp);
}

void LevelManager::save() {
    if (SD.cardType() == CARD_NONE) return;
    
    File file = SD.open("/morph/xp.json", FILE_WRITE);
    if (!file) return;

    JsonDocument doc;
    doc["xp"] = stats.xp;
    doc["lvl"] = stats.level;
    doc["atk"] = stats.attacks_performed;
    doc["scan"] = stats.wifi_scans;
    
    serializeJson(doc, file);
    file.close();
    Serial.println("[LevelManager] Stats saved.");
}

void LevelManager::load() {
    if (SD.cardType() == CARD_NONE) return;
    
    if (!SD.exists("/morph/xp.json")) return;
    
    File file = SD.open("/morph/xp.json", FILE_READ);
    if (!file) return;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (!error) {
        stats.xp = doc["xp"] | 0;
        stats.level = doc["lvl"] | 1;
        stats.attacks_performed = doc["atk"] | 0;
        stats.wifi_scans = doc["scan"] | 0;
        updateLevel(); // Recalculate title
    }
    file.close();
    Serial.println("[LevelManager] Stats loaded.");
}
