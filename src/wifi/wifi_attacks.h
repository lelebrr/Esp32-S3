#pragma once
/**
 * @file wifi_attacks.h
 * @brief WiFi attack functions stub
 */

#include <Arduino.h>
#include <cstdint>

class WiFiAttacks {
public:
    void setTarget(const uint8_t* bssid, const char* ssid, uint8_t channel) {}
    void startDeauth(int count) {}
    void startHandshakeCapture() {}
    void startBeaconFlood(const char* ssids, int count) {}
    void startEvilTwin(const char* ssid, int templateIdx) {}
    void startOneTapNuke() {}
    void stopAttack() {}
    bool isAttacking() { return false; }
};

extern WiFiAttacks wifi_attacks;
