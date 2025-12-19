#pragma once
/**
 * @file ble_driver.h
 * @brief BLE abstraction layer for scanning and spam functionality
 */

#include <Arduino.h>
#include <vector>

// BLE Spam Types
enum BLESpamType {
    BLE_SPAM_NONE = 0,
    BLE_SPAM_OFF = 0,
    BLE_SPAM_APPLE,
    BLE_SPAM_GOOGLE,
    BLE_SPAM_SAMSUNG,
    BLE_SPAM_MICROSOFT,
    BLE_SPAM_WINDOWS,
    BLE_SPAM_KITCHEN_SINK,
    BLE_SPAM_ALL
};

struct BLEDeviceData {
    String name;
    String address;
    int rssi;
    bool isConnectable;
};

class BLEDriver {
public:
    bool init() { return true; }
    void deinit() {}
    
    // Scanning
    void startScanAsync(int seconds) {}
    void stopScan() {}
    bool isScanRunning() { return false; }
    std::vector<BLEDeviceData> getScanResults() { return {}; }
    
    // Spam
    void startSpam(BLESpamType type) {}
    void stopSpam() {}
    bool isSpamming() { return false; }
    BLESpamType getSpamType() { return BLE_SPAM_NONE; }
};

extern BLEDriver ble_driver;
