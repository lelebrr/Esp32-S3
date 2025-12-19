/**
 * @file wardriving.h
 * Wardriving Logger - WiFi/BLE + GPS
 * Monster S3 Firmware
 * 
 * Logs scanned networks with GPS coordinates to CSV/KML on SD card.
 * 
 * Updated: 2025-12-19
 */

#ifndef WARDRIVING_H
#define WARDRIVING_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class WardrivingLogger {
public:
    static bool init();
    
    /**
     * @brief Log a WiFi network with current GPS location
     * @param ssid WiFi SSID
     * @param bssid MAC Address
     * @param rssi Signal Strength
     * @param channel WiFi Channel
     * @param encrypt Encryption type
     */
    static void logWiFi(String ssid, String bssid, int32_t rssi, int32_t channel, String encrypt);
    
    /**
     * @brief Log a BLE device with current GPS location
     * @param name Device Name
     * @param address MAC Address
     * @param rssi Signal Strength
     */
    static void logBLE(String name, String address, int32_t rssi);
    
    // Status
    static uint32_t getLogCount();
    
private:
    static uint32_t _logCount;
    static bool _initialized;
    static void appendToFile(String filename, String data);
};

#endif // WARDRIVING_H
