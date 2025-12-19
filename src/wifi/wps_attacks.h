#pragma once
/**
 * @file wps_attacks.h
 * @brief WPS attack functions stub
 */

#include <Arduino.h>
#include <cstdint>

class WPSAttacks {
public:
    void setTarget(const uint8_t* bssid, const char* ssid, uint8_t channel) {}
    void startPixieDust() {}
    void startBruteforce() {}
    void stopAttack() {}
    bool isAttacking() { return false; }
    int getProgress() { return 0; }
};

extern WPSAttacks wps_attacks;
