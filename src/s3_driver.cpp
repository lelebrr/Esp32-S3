#include "s3_driver.h"
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

void MonsterDriver::init() {
    Serial.begin(115200);
    // Wait for USB CDC to reconnect after upload/reset
    delay(1000); 
    
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
    
    initSD();
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
    // I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    // SPI for SD Card (SPI2 / HSPI)
    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    // Note: Display SPI is usually handled by the TFT driver init, not global SPI
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
