#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "pin_config.h"

// Hardware Abstraction Layer for Monster S3
class MonsterDriver {
public:
    static void init();
    static void powerOnModules(bool wifi, bool ble, bool nfc, bool cc1101);
    static void deepSleep(uint64_t time_ms);
    static bool isPsramReady();
    static float getBatteryVoltage();
    static float getTemperature(); // DS18B20 or Internal

private:
    static void initPowerPins();
    static void initBuses();
    static void initSD();
};
