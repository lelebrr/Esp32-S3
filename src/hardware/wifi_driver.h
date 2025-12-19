#pragma once
/**
 * @file wifi_driver.h
 * @brief WiFi abstraction layer for scanning and network management
 */

#include <Arduino.h>
#include <vector>

struct WiFiNetwork {
    char ssid[33];
    uint8_t bssid[6];
    int32_t rssi;
    uint8_t channel;
    uint8_t encryptionType;
    bool wps;
};

class WiFiDriver {
public:
    bool init() { return true; }
    void startScan() {}
    bool isScanComplete() { return true; }
    int getNetworkCount() { return 0; }
    WiFiNetwork getNetwork(int index) { 
        WiFiNetwork net = {};
        return net; 
    }
    void setChannel(uint8_t channel) {}
    uint8_t getChannel() { return 1; }
    bool isConnected() { return false; }
    String getIP() { return "0.0.0.0"; }
};

extern WiFiDriver wifi_driver;
