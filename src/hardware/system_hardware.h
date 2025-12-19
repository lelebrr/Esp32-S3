#pragma once
/**
 * @file system_hardware.h
 * @brief Hardware abstraction for system-level functions
 */

#include <Arduino.h>

class SystemHardware {
public:
    int getBatteryPercent() { return 100; }
    float getBatteryVoltage() { return 4.2f; }
    bool isCharging() { return false; }
    bool isUSBConnected() { return false; }
    void setBrightness(int level) {}
    void setDisplayBrightness(int level) { setBrightness(level); }
    int getBrightness() { return 128; }
    void sleep() {}
    void wakeup() {}
    void vibratorOn(int durationMs) {}
    void vibratorOff() {}
};

extern SystemHardware sys_hw;
