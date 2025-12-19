/**
 * @file wardriving.cpp
 * Wardriving Logger Implementation
 * Monster S3 Firmware
 * 
 * Updated: 2025-12-19
 */

#include "wardriving.h"
#include "gps_driver.h"
#include "rtc_driver.h"
#include "sd_adapter.h" // Assuming this exists or using standard SD

uint32_t WardrivingLogger::_logCount = 0;
bool WardrivingLogger::_initialized = false;

// File paths
#define LOG_FILE_WIFI "/wardriving_wifi.csv"
#define LOG_FILE_BLE  "/wardriving_ble.csv"

bool WardrivingLogger::init() {
    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[WARDRIVING] SD Init Failed");
        return false;
    }
    
    // Create headers if files don't exist
    if (!SD.exists(LOG_FILE_WIFI)) {
        File f = SD.open(LOG_FILE_WIFI, FILE_WRITE);
        if (f) {
            f.println("SSID,MAC,RSSI,Channel,Encryption,Latitude,Longitude,Altitude,HDOP,Timestamp");
            f.close();
        }
    }
    
    if (!SD.exists(LOG_FILE_BLE)) {
        File f = SD.open(LOG_FILE_BLE, FILE_WRITE);
        if (f) {
            f.println("Name,Address,RSSI,Latitude,Longitude,Altitude,HDOP,Timestamp");
            f.close();
        }
    }
    
    _initialized = true;
    Serial.println("[WARDRIVING] System Ready");
    return true;
}

void WardrivingLogger::logWiFi(String ssid, String bssid, int32_t rssi, int32_t channel, String encrypt) {
    if (!_initialized) return;
    
    GPSData gps = GPSDriver::getData();
    if (!gps.isValid) return; // Only log with valid GPS
    
    // Format: SSID,MAC,RSSI,Channel,Encryption,Lat,Lon,Alt,HDOP,Time
    String timestamp;
    if (gps.isValid && gps.year > 2000) {
        timestamp = GPSDriver::getDateString() + " " + GPSDriver::getTimeString();
    } else {
        // Fallback to RTC
        timestamp = RTCDriver::getDateTimeString();
    }

    String line = ssid + "," + bssid + "," + String(rssi) + "," + String(channel) + "," + encrypt + ",";
    line += String(gps.latitude, 6) + "," + String(gps.longitude, 6) + "," + String(gps.altitude, 1) + ",";
    line += String(gps.hdop, 1) + "," + timestamp;
    
    appendToFile(LOG_FILE_WIFI, line);
    _logCount++;
}

void WardrivingLogger::logBLE(String name, String address, int32_t rssi) {
    if (!_initialized) return;
    
    GPSData gps = GPSDriver::getData();
    if (!gps.isValid) return;
    
    // Format: Name,Address,RSSI,Lat,Lon,Alt,HDOP,Time
    String timestamp;
    if (gps.isValid && gps.year > 2000) {
        timestamp = GPSDriver::getDateString() + " " + GPSDriver::getTimeString();
    } else {
        // Fallback to RTC
        timestamp = RTCDriver::getDateTimeString();
    }

    String line = name + "," + address + "," + String(rssi) + ",";
    line += String(gps.latitude, 6) + "," + String(gps.longitude, 6) + "," + String(gps.altitude, 1) + ",";
    line += String(gps.hdop, 1) + "," + timestamp;
    
    appendToFile(LOG_FILE_BLE, line);
    _logCount++;
}

void WardrivingLogger::appendToFile(String filename, String data) {
    File f = SD.open(filename, FILE_APPEND);
    if (f) {
        f.println(data);
        f.close();
        Serial.print("."); // Heartbeat
    }
}

uint32_t WardrivingLogger::getLogCount() {
    return _logCount;
}
