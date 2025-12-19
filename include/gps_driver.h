/**
 * @file gps_driver.h
 * @brief GPS Driver for GY-NEO6MV2 (u-blox NEO-6M)
 * 
 * Uses UART2 for communication with TinyGPS++ library.
 * Provides wardriving, location logging, and time sync features.
 */

#ifndef GPS_DRIVER_H
#define GPS_DRIVER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "pin_config.h"

// GPS UART Configuration
#define GPS_UART_NUM    2
#define GPS_BAUD_RATE   9600

/**
 * @brief GPS Driver Class
 * 
 * Wrapper around TinyGPS++ with additional features for
 * wardriving, logging, and attack correlation.
 */
class GPSDriver {
public:
    /**
     * @brief Initialize GPS module on UART2
     * @return true if UART initialized successfully
     */
    static bool init();

    /**
     * @brief Update GPS data from serial buffer
     * Call this frequently from a task (every 100ms recommended)
     */
    static void update();

    /**
     * @brief Check if GPS module is responding
     */
    static bool isAvailable();

    /**
     * @brief Check if we have a valid GPS fix
     */
    static bool isValid();

    /**
     * @brief Get latitude in degrees
     */
    static double getLatitude();

    /**
     * @brief Get longitude in degrees
     */
    static double getLongitude();

    /**
     * @brief Get altitude in meters
     */
    static double getAltitude();

    /**
     * @brief Get speed in km/h
     */
    static double getSpeedKmh();

    /**
     * @brief Get course/heading in degrees
     */
    static double getCourse();

    /**
     * @brief Get number of satellites in view
     */
    static int getSatellites();

    /**
     * @brief Get HDOP (Horizontal Dilution of Precision)
     * Lower is better. <1 = excellent, 1-2 = good, 2-5 = moderate
     */
    static double getHDOP();

    /**
     * @brief Get formatted coordinates string
     * @return String in format "lat,lon"
     */
    static String getCoordsString();

    /**
     * @brief Get formatted date string
     * @return String in format "YYYY-MM-DD"
     */
    static String getDateString();

    /**
     * @brief Get formatted time string (UTC)
     * @return String in format "HH:MM:SS"
     */
    static String getTimeString();

    /**
     * @brief Get distance to a point in meters
     */
    static double distanceTo(double lat, double lon);

    /**
     * @brief Get bearing to a point in degrees
     */
    static double bearingTo(double lat, double lon);

    /**
     * @brief Log current position to SD card
     * @param filename Path on SD card
     * @param ssid Optional WiFi SSID for wardriving
     * @param bssid Optional WiFi BSSID
     * @param rssi Optional signal strength
     */
    static bool logPosition(const char* filename, 
                           const char* ssid = nullptr,
                           const char* bssid = nullptr,
                           int rssi = 0);

    /**
     * @brief Export logged positions to KML format
     * @param inputFile Source log file
     * @param outputFile KML output file
     */
    static bool exportToKML(const char* inputFile, const char* outputFile);

    /**
     * @brief Get underlying TinyGPSPlus object for advanced use
     */
    static TinyGPSPlus& getGPS();

private:
    static TinyGPSPlus _gps;
    static HardwareSerial _serial;
    static bool _initialized;
    static uint32_t _lastUpdate;
    static uint32_t _charsProcessed;
};

#endif // GPS_DRIVER_H
