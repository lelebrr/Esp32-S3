/**
 * @file sound_manager.h
 * @brief MorphNode Audio Feedback System
 * 
 * Provides high-level sound effects for UI and System events.
 * Wraps the PiezoDriver module.
 */

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <Arduino.h>
#include "modules/piezo_driver.h"
#include "pin_config.h"

class SoundManager {
public:
    static void init();
    
    // UI Sounds
    static void playClick();
    static void playBack();
    static void playSelect();
    static void playError();
    static void playBoot();
    static void playLevelUp();
    
    // Morphy Sounds
    static void playChirpHappy();
    static void playChirpEating();
    static void playChirpAngry();

    static void setMute(bool mute);
    static bool isMuted();

private:
    static PiezoDriver* piezo;
    static bool _muted;
};

#endif
