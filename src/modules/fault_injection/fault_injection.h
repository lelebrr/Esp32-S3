/**
 * @file fault_injection.h
 * @brief Pocket Glitcher Pro - Fault Injection System Header
 * @author Lele Origin Team
 */

#ifndef __FAULT_INJECTION_H__
#define __FAULT_INJECTION_H__

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <Wire.h>
#include <SD.h>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// GPIO Pins for Glitch Hardware
#define CROWBAR_TRIGGER_PIN    25    // MOSFET gate for voltage crowbar
#define CROWBAR_ENABLE_PIN     26    // Crowbar enable/disable
#define LASER_CONTROL_PIN      27    // Laser diode control
#define LASER_ENABLE_PIN       14    // Laser power enable
#define TRIGGER_CC1101_PIN     32    // CC1101 trigger input
#define TRIGGER_NRF24_PIN      33    // NRF24 trigger input

// Glitch Parameters
#define CROWBAR_PULSE_MIN_NS   50    // Minimum pulse width (50ns)
#define CROWBAR_PULSE_MAX_NS   500   // Maximum pulse width (500ns)
#define LASER_PULSE_MIN_MS     50    // Minimum laser pulse (50ms)
#define LASER_PULSE_MAX_MS     500   // Maximum laser pulse (500ms)
#define GLITCH_DELAY_MIN_US    1     // Minimum delay (1µs)
#define GLITCH_DELAY_MAX_US    1000  // Maximum delay (1ms)

// Target Timing Constants
#define SWITCH_RCM_GLITCH_TIME     100   // 100ns for Switch RCM
#define IPHONE_CHECKM8_GLITCH_TIME 200   // 200ms laser + 100ns crowbar
#define CAR_KEYLESS_GLITCH_TIME    50    // 50ns for car modules
#define METER_GLITCH_TIME          150   // 150ms for smart meters
#define RFID_GLITCH_TIME           75    // 75ns for RFID chips

// ============================================================================
// TARGET ENUMERATIONS
// ============================================================================

enum GlitchTarget {
    TARGET_SWITCH_RCM = 0,
    TARGET_IPHONE_CHECKM8,
    TARGET_CAR_KEYLESS,
    TARGET_SMART_METER,
    TARGET_RFID_HID,
    TARGET_CUSTOM
};

enum GlitchType {
    GLITCH_CROWBAR = 0,      // Voltage crowbar glitch
    GLITCH_LASER,           // Laser diode glitch
    GLITCH_COMBO,           // Crowbar + Laser combo
    GLITCH_RF_TRIGGER       // RF-triggered glitch
};

enum TriggerType {
    TRIGGER_MANUAL = 0,     // Manual trigger via button
    TRIGGER_CC1101,         // CC1101 RF trigger
    TRIGGER_NRF24,          // NRF24 RF trigger
    TRIGGER_AUTO            // Automatic timing
};

// ============================================================================
// STRUCTURES
// ============================================================================

struct GlitchConfig {
    GlitchTarget target;
    GlitchType type;
    TriggerType trigger;
    uint32_t pulseWidth;        // Pulse width in ns/ms
    uint32_t delay;             // Delay before glitch in µs
    uint8_t intensity;          // Glitch intensity 0-100%
    bool autoRetry;             // Auto retry on failure
    uint8_t maxRetries;         // Maximum retry attempts
};

struct GlitchResult {
    bool success;
    uint32_t executionTime;
    uint32_t pulseWidthUsed;
    uint32_t delayUsed;
    uint8_t attempts;
    String errorMessage;
};

struct TargetTiming {
    const char* name;
    uint32_t optimalDelay;
    uint32_t optimalPulse;
    const char* description;
};

// ============================================================================
// CLASS DEFINITIONS
// ============================================================================

class CrowbarGlitcher {
private:
    uint8_t triggerPin;
    uint8_t enablePin;
    volatile bool armed;
    SemaphoreHandle_t glitchMutex;

    void setupHardware();
    void armCrowbar();
    void disarmCrowbar();

public:
    CrowbarGlitcher(uint8_t trigger, uint8_t enable);
    ~CrowbarGlitcher();

    bool initialize();
    bool glitch(uint32_t pulseWidthNs);
    void setArmed(bool state) { armed = state; }
    bool isArmed() { return armed; }
};

class LaserGlitcher {
private:
    uint8_t controlPin;
    uint8_t enablePin;
    volatile bool armed;
    SemaphoreHandle_t laserMutex;

    void setupHardware();
    void powerOn();
    void powerOff();

public:
    LaserGlitcher(uint8_t control, uint8_t enable);
    ~LaserGlitcher();

    bool initialize();
    bool glitch(uint32_t pulseWidthMs);
    void setArmed(bool state) { armed = state; }
    bool isArmed() { return armed; }
};

class RFTrigger {
private:
    uint8_t cc1101Pin;
    uint8_t nrf24Pin;
    volatile bool triggerDetected;
    SemaphoreHandle_t triggerMutex;

    void setupHardware();
    bool detectCC1101Signal();
    bool detectNRF24Signal();

public:
    RFTrigger(uint8_t cc1101, uint8_t nrf24);
    ~RFTrigger();

    bool initialize();
    bool waitForTrigger(TriggerType type, uint32_t timeoutMs = 5000);
    void resetTrigger() { triggerDetected = false; }
};

class FaultInjectionManager {
private:
    CrowbarGlitcher* crowbar;
    LaserGlitcher* laser;
    RFTrigger* rfTrigger;

    GlitchConfig currentConfig;
    volatile bool systemArmed;
    volatile bool glitchingActive;

    SemaphoreHandle_t systemMutex;
    QueueHandle_t resultQueue;

    // Target-specific timing databases
    static const TargetTiming targetTimings[];

    void setupHardware();
    bool validateConfig(const GlitchConfig& config);
    GlitchResult executeGlitch(const GlitchConfig& config);
    void logResult(const GlitchResult& result);

public:
    FaultInjectionManager();
    ~FaultInjectionManager();

    bool initialize();
    bool configureGlitch(const GlitchConfig& config);
    GlitchResult performGlitch();
    void armSystem(bool state) { systemArmed = state; }
    bool isSystemArmed() { return systemArmed; }
    
    LaserGlitcher* getLaser() { return laser; }
    CrowbarGlitcher* getCrowbar() { return crowbar; }
    RFTrigger* getRFTrigger() { return rfTrigger; }
    QueueHandle_t getResultQueue() { return resultQueue; }

    // Target-specific methods
    bool glitchSwitchRCM();
    bool glitchiPhoneCheckm8();
    bool glitchCarKeyless();
    bool glitchSmartMeter();
    bool glitchRFIDHID();

    // Utility methods
    const TargetTiming* getTargetTiming(GlitchTarget target);
    String getSystemStatus();
    void emergencyStop();
};

// ============================================================================
// TASK DEFINITIONS
// ============================================================================

// FreeRTOS Task Handles
extern TaskHandle_t lvglTaskHandle;
extern TaskHandle_t triggerTaskHandle;
extern TaskHandle_t glitchTaskHandle;
extern TaskHandle_t loggerTaskHandle;

// Task Functions
void lvglMenuTask(void* parameter);
void triggerDetectorTask(void* parameter);
void glitchControllerTask(void* parameter);
void dataLoggerTask(void* parameter);

// ============================================================================
// GLOBAL FUNCTIONS
// ============================================================================

bool initializeFaultInjectionSystem();
void deinitializeFaultInjectionSystem();
String getFaultInjectionSystemInfo();

extern FaultInjectionManager* faultInjectionManager;

#endif // __FAULT_INJECTION_H__
