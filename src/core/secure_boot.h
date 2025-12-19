/**
 * @file secure_boot.h
 * @brief Paranoid Secure Boot implementation
 */

#pragma once

#include <Arduino.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class SecureBoot {
public:
    static void run();
    static void checkTemperature();
    static bool mountSDSafe();
    static void checkIntegrity();
    static void checkPower();
    static void applySafeOverclock();
    static void logCrash(const char* reason);
    static void loopCheck(); // Call this in loop()

private:
    static void safeMode();
    static uint32_t calculateFirmwareCRC();
    static void taskMountSD(void* param);
    static volatile bool sdMounted;
    static volatile bool sdMountFinished;
};
