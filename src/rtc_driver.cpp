#include "rtc_driver.h"
#include "pin_config.h"

RTC_DS3231 RTCDriver::rtc;
bool RTCDriver::_initialized = false;

bool RTCDriver::init() {
    if (_initialized) return true;
    
    // Wire.begin() is called in MonsterDriver::initBuses()
    // But we check if device is on bus
    Wire.beginTransmission(DS3231_I2C_ADDR); // 0x68
    if (Wire.endTransmission() != 0) {
        Serial.println("[RTC] DS3231 not found at 0x68");
        return false;
    }

    if (!rtc.begin()) {
        Serial.println("[RTC] Initialization failed!");
        return false;
    }

    if (rtc.lostPower()) {
        Serial.println("[RTC] Lost power, setting compilation time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    // Disable 32K Pin to save power if not used
    rtc.disable32K();
    
    // Clear any pending alarms
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    
    // Turn off alarm interrupts by default
    rtc.writeSqwPinMode(DS3231_OFF);

    Serial.printf("[RTC] Ready! Time: %s Temp: %.2fC\n", 
                  getDateTimeString().c_str(), 
                  getTemperature());
                  
    _initialized = true;
    return true;
}

String RTCDriver::getDateTimeString() {
    if (!_initialized) return "1970-01-01 00:00:00";
    DateTime now = rtc.now();
    char buf[20];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    return String(buf);
}

DateTime RTCDriver::now() {
    if (!_initialized) return DateTime((uint32_t)0);
    return rtc.now();
}

float RTCDriver::getTemperature() {
    if (!_initialized) return 0.0f;
    return rtc.getTemperature();
}

bool RTCDriver::isRunning() {
    return _initialized;
}

void RTCDriver::adjust(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s) {
    if (!_initialized) return;
    rtc.adjust(DateTime(y, m, d, h, min, s));
    Serial.println("[RTC] Time adjusted manually");
}

void RTCDriver::adjust(const DateTime& dt) {
    if (!_initialized) return;
    rtc.adjust(dt);
    Serial.println("[RTC] Time adjusted manually");
}

void RTCDriver::setAlarm1(uint8_t h, uint8_t m, uint8_t s, Ds3231Alarm1Mode mode) {
    if (!_initialized) return;
    rtc.setAlarm1(DateTime(0, 0, 0, h, m, s), mode);
    Serial.printf("[RTC] Alarm 1 set for %02d:%02d:%02d\n", h, m, s);
}

void RTCDriver::setAlarm2(uint8_t h, uint8_t m, Ds3231Alarm2Mode mode) {
    if (!_initialized) return;
    rtc.setAlarm2(DateTime(0, 0, 0, h, m, 0), mode);
    Serial.printf("[RTC] Alarm 2 set for %02d:%02d\n", h, m);
}

void RTCDriver::clearAlarms() {
    if (!_initialized) return;
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
}

bool RTCDriver::checkIfAlarm(uint8_t alarmNum) {
    if (!_initialized) return false;
    return rtc.alarmFired(alarmNum);
}
