#include "s3_driver.h"
#include "sd_adapter.h"
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "gesture_sensor.h"
#include "gps_driver.h"
#include "rtc_driver.h"
#include "modules/piezo_driver.h"

// Global Piezo Object
PiezoDriver Piezo(PIN_PIEZO);

void MonsterDriver::init() {
    // Note: Serial.begin() now called in main.cpp before this
    // Wait for USB CDC to reconnect after upload/reset
    delay(500); 
    
    Serial.println("\n========================================");
    Serial.println("        MONSTER S3 FIRMWARE v1.0");
    Serial.println("========================================");
    Serial.println("[MONSTER] Booting specialized hardware...");

    initPowerPins();
    initBuses();
    
    // Check PSRAM
    if(psramFound()){
        Serial.printf("[SYSTEM] Octal PSRAM Active: %d bytes\n", ESP.getFreePsram());
    } else {
        Serial.println("[ERROR] PSRAM NOT FOUND! Performance compromised.");
    }
    
    // SD is now initialized by AggressiveSD in main.cpp
    // initSD(); // REMOVED - handled by aggressive_boot_logic()
    initRTC();
    initGesture();
    initGPS();
    
    // Piezo
    Piezo.begin();
    Piezo.playBootSound();
}

void MonsterDriver::initPowerPins() {
    pinMode(PIN_CC1101_EN, OUTPUT);
    pinMode(PIN_PN532_EN, OUTPUT);
    pinMode(PIN_LED_RED_ATTACK_HI, OUTPUT);
    pinMode(PIN_LED_RED_ATTACK_LO, OUTPUT);
    pinMode(PIN_LED_BLUE_AI, OUTPUT);
    pinMode(PIN_LED_BLUE_SLEEP, OUTPUT);
    
    // Default OFF
    digitalWrite(PIN_CC1101_EN, LOW);
    digitalWrite(PIN_PN532_EN, LOW);
    
    // LEDs OFF
    digitalWrite(PIN_LED_RED_ATTACK_HI, LOW);
    digitalWrite(PIN_LED_RED_ATTACK_LO, LOW);
    digitalWrite(PIN_LED_BLUE_AI, LOW);
    digitalWrite(PIN_LED_BLUE_SLEEP, LOW);
}

void MonsterDriver::initBuses() {
    // SD Card is now handled by AggressiveSD early boot
    // if (!SDAdapter::init()) {
    //     Serial.println("SD Init Failed! Checking connections...");
    // } else {
    //     Serial.println("SD Intialized!");
    // }

    // I2C for sensors (PN532, PAJ7620, DS3231)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Serial.println("[BUS] I2C initialized");
    
    // Note: SD SPI (HSPI) is initialized by AggressiveSD
    // Display SPI (FSPI) is handled by TFT_eSPI library
}

void MonsterDriver::initSD() {
    if(!SD.begin(PIN_SD_CS, SPI, 20000000)){ // 20MHz SD
        Serial.println("[ERROR] SD Card Mount Failed!");
    } else {
        Serial.printf("[SYSTEM] SD Card Mounted. Type: %d\n", SD.cardType());
    }
}

void MonsterDriver::powerOnModules(bool wifi, bool ble, bool nfc, bool cc1101) {
    // Hardware Mutex Logic handled here
    // Turn ON requested
    digitalWrite(PIN_PN532_EN, nfc ? HIGH : LOW);
    digitalWrite(PIN_CC1101_EN, cc1101 ? HIGH : LOW);
    
    // Wifi/BLE are handled by internal PHY management usually, 
    // but we can track state here if needed for GUI
}

void MonsterDriver::initGesture() {
#if PAJ7620_ENABLED
    if (GestureSensor::init()) {
        Serial.println("[SYSTEM] Gesture sensor ready");
    } else {
        Serial.println("[WARNING] Gesture sensor not available");
    }
#else
    Serial.println("[GESTURE] Sensor disabled in config");
#endif
}

void MonsterDriver::initGPS() {
#if GPS_ENABLED
    if (GPSDriver::init()) {
        Serial.println("[SYSTEM] GPS module ready");
    } else {
        Serial.println("[WARNING] GPS module not available");
    }
#else
    Serial.println("[GPS] Module disabled in config");
#endif
}

void MonsterDriver::initRTC() {
#if DS3231_ENABLED
    if (RTCDriver::init()) {
        Serial.println("[SYSTEM] RTC module ready");
    } else {
        Serial.println("[WARNING] RTC module not available");
    }
#else
    Serial.println("[RTC] Module disabled in config");
#endif
}
