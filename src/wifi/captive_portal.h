#pragma once
/**
 * @file captive_portal.h
 * @brief Captive portal stub
 */

#include <Arduino.h>

class CaptivePortal {
public:
    void start(const char* ssid) {}
    void stop() {}
    bool isRunning() { return false; }
    int getClientCount() { return 0; }
};

extern CaptivePortal captive_portal;
