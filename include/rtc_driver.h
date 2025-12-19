#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

class RTCDriver {
public:
    static bool init();
    static String getDateTimeString();
    static DateTime now(); // Added for direct DateTime access
    static float getTemperature();
    static bool isRunning();
    static void adjust(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s);
    static void adjust(const DateTime& dt); // Helper overlay

    // Alarm Functions
    static void setAlarm1(uint8_t h, uint8_t m, uint8_t s, Ds3231Alarm1Mode mode);
    static void setAlarm2(uint8_t h, uint8_t m, Ds3231Alarm2Mode mode);
    static void clearAlarms();
    static bool checkIfAlarm(uint8_t alarmNum);

private:
    static RTC_DS3231 rtc;
    static bool _initialized;
};

#endif // RTC_DRIVER_H
