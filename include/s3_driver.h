#pragma once

#include "modules/piezo_driver.h"
#include <Arduino.h>

// External Objects for global access
extern PiezoDriver Piezo;
#include "pin_config.h"
#include <SPI.h>
#include <Wire.h>


// Hardware Abstraction Layer for MorphNode
class MorphDriver {
public:
    static void init();
    static void powerOnModules(bool wifi, bool ble, bool nfc, bool cc1101);
    static void deepSleep(uint64_t time_ms);
    static bool isPsramReady();
    static float getBatteryVoltage();
    static float getTemperature(); // DS3231 or Internal

private:
    static void initPowerPins();
    static void initBuses();
    static void initSD();
    static void initGesture();
    static void initGPS();
    static void initRTC();
};
