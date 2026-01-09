/**
 * @file level_manager.h
 * @brief MorphNode Gamification Engine (XP & Levels)
 * 
 * Handles player progression:
 * - Tracks XP from uptime, attacks, scans
 * - Calculates Level based on XP
 * - Persists data to SD Card
 */

#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>

struct PlayerStats {
    uint32_t xp;
    uint32_t level;
    char title[32];
    uint32_t attacks_performed;
    uint32_t wifi_scans;
};

class LevelManager {
public:
    static void init();
    static void loop(); // Call periodically to add uptime XP

    // XP Actions
    static void addXP(uint32_t amount);
    static void onAttackPerformed(); // +10 XP
    static void onWifiScan();        // +5 XP
    
    // Getters
    static uint32_t getXP();
    static uint32_t getLevel();
    static float getProgressToNextLevel(); // 0.0 to 1.0
    static const char* getTitle();
    
    // Persistence
    static void save();
    static void load();

private:
    static PlayerStats stats;
    static uint32_t last_xp_save;
    static uint32_t last_uptime_check;
    
    static void updateLevel();
    static const char* calculateTitle(uint32_t level);
};

#endif
