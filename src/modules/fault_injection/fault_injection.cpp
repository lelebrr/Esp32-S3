/**
 * @file fault_injection.cpp
 * @brief Pocket Glitcher Pro - Fault Injection System Implementation
 * @author Lele Origin Team
 */

#include "fault_injection.h"
#include <driver/gpio.h>
#include <driver/dac.h>
#include <esp_timer.h>

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

FaultInjectionManager* faultInjectionManager = nullptr;

// FreeRTOS Task Handles
TaskHandle_t lvglTaskHandle = nullptr;
TaskHandle_t triggerTaskHandle = nullptr;
TaskHandle_t glitchTaskHandle = nullptr;
TaskHandle_t loggerTaskHandle = nullptr;

// ============================================================================
// TARGET TIMING DATABASE
// ============================================================================

const TargetTiming FaultInjectionManager::targetTimings[] = {
    // Nintendo Switch RCM Glitch
    {
        "Nintendo Switch RCM",
        150000,     // 150µs delay
        100,        // 100ns pulse
        "RCM entry via Tegra fuse read glitch. Connect wire to Joy-Con pin 10."
    },

    // iPhone Checkm8
    {
        "iPhone Checkm8",
        500000,     // 500µs delay
        200000,     // 200ms laser + crowbar combo
        "SEP bootrom signature bypass. Connect to VCC test point + laser on A11."
    },

    // Car Keyless Entry
    {
        "Car Keyless Entry",
        25000,      // 25µs delay
        50,         // 50ns pulse
        "AES-128 decrypt glitch. Access under dashboard panel."
    },

    // Smart Meter
    {
        "Smart Meter",
        100000,     // 100µs delay
        150000,     // 150ms laser pulse
        "Consumption calculation glitch. Point laser through transparent cover."
    },

    // RFID HID Prox Card
    {
        "RFID HID Prox",
        50000,      // 50µs delay
        75,         // 75ns pulse
        "Anti-collision bypass. Laser on RFID chip during read."
    },

    // Custom Target
    {
        "Custom Target",
        100000,     // 100µs default
        100000,     // 100µs default
        "User-defined timing parameters."
    }
};

// ============================================================================
// CROWBAR GLITCHER IMPLEMENTATION
// ============================================================================

CrowbarGlitcher::CrowbarGlitcher(uint8_t trigger, uint8_t enable)
    : triggerPin(trigger), enablePin(enable), armed(false) {
    glitchMutex = xSemaphoreCreateMutex();
}

CrowbarGlitcher::~CrowbarGlitcher() {
    if (glitchMutex) vSemaphoreDelete(glitchMutex);
}

void CrowbarGlitcher::setupHardware() {
    // Configure trigger pin as output (high speed)
    gpio_config_t trigger_conf = {
        .pin_bit_mask = (1ULL << triggerPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&trigger_conf);

    // Configure enable pin as output
    gpio_config_t enable_conf = {
        .pin_bit_mask = (1ULL << enablePin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&enable_conf);

    // Start disarmed
    gpio_set_level((gpio_num_t)triggerPin, 0);
    gpio_set_level((gpio_num_t)enablePin, 0);
}

bool CrowbarGlitcher::initialize() {
    setupHardware();
    return true;
}

bool CrowbarGlitcher::glitch(uint32_t pulseWidthNs) {
    if (!armed || !glitchMutex) return false;

    if (xSemaphoreTake(glitchMutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return false;
    }

    // Validate pulse width
    if (pulseWidthNs < CROWBAR_PULSE_MIN_NS || pulseWidthNs > CROWBAR_PULSE_MAX_NS) {
        xSemaphoreGive(glitchMutex);
        return false;
    }

    // Enable crowbar circuit
    gpio_set_level((gpio_num_t)enablePin, 1);

    // Small delay for circuit stabilization
    esp_rom_delay_us(10);

    // Generate precise pulse using ESP32 timer
    uint64_t start_time = esp_timer_get_time();

    // Trigger glitch pulse
    gpio_set_level((gpio_num_t)triggerPin, 1);

    // Wait for pulse width (nanosecond precision)
    while ((esp_timer_get_time() - start_time) < (pulseWidthNs / 1000)) {
        // Busy wait for precision
        __asm__ __volatile__("nop");
    }

    // End pulse
    gpio_set_level((gpio_num_t)triggerPin, 0);

    // Disable crowbar circuit
    gpio_set_level((gpio_num_t)enablePin, 0);

    xSemaphoreGive(glitchMutex);
    return true;
}

// ============================================================================
// LASER GLITCHER IMPLEMENTATION
// ============================================================================

LaserGlitcher::LaserGlitcher(uint8_t control, uint8_t enable)
    : controlPin(control), enablePin(enable), armed(false) {
    laserMutex = xSemaphoreCreateMutex();
}

LaserGlitcher::~LaserGlitcher() {
    if (laserMutex) vSemaphoreDelete(laserMutex);
}

void LaserGlitcher::setupHardware() {
    // Configure control pin as output
    gpio_config_t control_conf = {
        .pin_bit_mask = (1ULL << controlPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&control_conf);

    // Configure enable pin as output
    gpio_config_t enable_conf = {
        .pin_bit_mask = (1ULL << enablePin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&enable_conf);

    // Start with laser off
    powerOff();
}

bool LaserGlitcher::initialize() {
    setupHardware();
    return true;
}

void LaserGlitcher::powerOn() {
    gpio_set_level((gpio_num_t)enablePin, 1);
    esp_rom_delay_us(1000); // Stabilization delay
}

void LaserGlitcher::powerOff() {
    gpio_set_level((gpio_num_t)controlPin, 0);
    gpio_set_level((gpio_num_t)enablePin, 0);
}

bool LaserGlitcher::glitch(uint32_t pulseWidthMs) {
    if (!armed || !laserMutex) return false;

    if (xSemaphoreTake(laserMutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return false;
    }

    // Validate pulse width
    if (pulseWidthMs < LASER_PULSE_MIN_MS || pulseWidthMs > LASER_PULSE_MAX_MS) {
        xSemaphoreGive(laserMutex);
        return false;
    }

    // Power on laser
    powerOn();

    // Generate laser pulse
    gpio_set_level((gpio_num_t)controlPin, 1);
    vTaskDelay(pdMS_TO_TICKS(pulseWidthMs));
    gpio_set_level((gpio_num_t)controlPin, 0);

    // Keep power on for cooldown
    esp_rom_delay_us(50000); // 50ms cooldown

    // Power off
    powerOff();

    xSemaphoreGive(laserMutex);
    return true;
}

// ============================================================================
// RF TRIGGER IMPLEMENTATION
// ============================================================================

RFTrigger::RFTrigger(uint8_t cc1101, uint8_t nrf24)
    : cc1101Pin(cc1101), nrf24Pin(nrf24), triggerDetected(false) {
    triggerMutex = xSemaphoreCreateMutex();
}

RFTrigger::~RFTrigger() {
    if (triggerMutex) vSemaphoreDelete(triggerMutex);
}

void RFTrigger::setupHardware() {
    // Configure CC1101 pin as input with interrupt
    gpio_config_t cc1101_conf = {
        .pin_bit_mask = (1ULL << cc1101Pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&cc1101_conf);

    // Configure NRF24 pin as input with interrupt
    gpio_config_t nrf24_conf = {
        .pin_bit_mask = (1ULL << nrf24Pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&nrf24_conf);
}

bool RFTrigger::initialize() {
    setupHardware();

    // Install GPIO ISR service
    gpio_install_isr_service(0);

    // Add ISR handlers
    gpio_isr_handler_add((gpio_num_t)cc1101Pin, [](void* arg) {
        RFTrigger* trigger = (RFTrigger*)arg;
        trigger->triggerDetected = true;
    }, this);

    gpio_isr_handler_add((gpio_num_t)nrf24Pin, [](void* arg) {
        RFTrigger* trigger = (RFTrigger*)arg;
        trigger->triggerDetected = true;
    }, this);

    return true;
}

bool RFTrigger::waitForTrigger(TriggerType type, uint32_t timeoutMs) {
    uint32_t startTime = millis();

    while ((millis() - startTime) < timeoutMs) {
        if (triggerDetected) {
            triggerDetected = false;
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    return false;
}

// ============================================================================
// FAULT INJECTION MANAGER IMPLEMENTATION
// ============================================================================

FaultInjectionManager::FaultInjectionManager()
    : crowbar(nullptr), laser(nullptr), rfTrigger(nullptr),
      systemArmed(false), glitchingActive(false) {
    systemMutex = xSemaphoreCreateMutex();
    resultQueue = xQueueCreate(5, sizeof(GlitchResult));
}

FaultInjectionManager::~FaultInjectionManager() {
    emergencyStop();
    if (crowbar) delete crowbar;
    if (laser) delete laser;
    if (rfTrigger) delete rfTrigger;
    if (systemMutex) vSemaphoreDelete(systemMutex);
    if (resultQueue) vQueueDelete(resultQueue);
}

void FaultInjectionManager::setupHardware() {
    // Initialize crowbar glitcher
    crowbar = new CrowbarGlitcher(CROWBAR_TRIGGER_PIN, CROWBAR_ENABLE_PIN);
    crowbar->initialize();

    // Initialize laser glitcher
    laser = new LaserGlitcher(LASER_CONTROL_PIN, LASER_ENABLE_PIN);
    laser->initialize();

    // Initialize RF trigger
    rfTrigger = new RFTrigger(TRIGGER_CC1101_PIN, TRIGGER_NRF24_PIN);
    rfTrigger->initialize();
}

bool FaultInjectionManager::initialize() {
    setupHardware();
    return true;
}

bool FaultInjectionManager::configureGlitch(const GlitchConfig& config) {
    if (xSemaphoreTake(systemMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return false;
    }

    if (!validateConfig(config)) {
        xSemaphoreGive(systemMutex);
        return false;
    }

    currentConfig = config;
    xSemaphoreGive(systemMutex);
    return true;
}

bool FaultInjectionManager::validateConfig(const GlitchConfig& config) {
    // Validate target
    if (config.target < TARGET_SWITCH_RCM || config.target > TARGET_CUSTOM) {
        return false;
    }

    // Validate glitch type
    if (config.type < GLITCH_CROWBAR || config.type > GLITCH_RF_TRIGGER) {
        return false;
    }

    // Validate trigger type
    if (config.trigger < TRIGGER_MANUAL || config.trigger > TRIGGER_AUTO) {
        return false;
    }

    // Validate timing parameters
    if (config.delay < GLITCH_DELAY_MIN_US || config.delay > GLITCH_DELAY_MAX_US) {
        return false;
    }

    // Validate pulse width based on glitch type
    if (config.type == GLITCH_CROWBAR || config.type == GLITCH_COMBO) {
        if (config.pulseWidth < CROWBAR_PULSE_MIN_NS || config.pulseWidth > CROWBAR_PULSE_MAX_NS) {
            return false;
        }
    } else if (config.type == GLITCH_LASER) {
        if (config.pulseWidth < LASER_PULSE_MIN_MS || config.pulseWidth > LASER_PULSE_MAX_MS) {
            return false;
        }
    }

    return true;
}

GlitchResult FaultInjectionManager::executeGlitch(const GlitchConfig& config) {
    GlitchResult result;
    result.success = false;
    result.executionTime = 0;
    result.pulseWidthUsed = config.pulseWidth;
    result.delayUsed = config.delay;
    result.attempts = 0;

    uint32_t startTime = millis();

    // Wait for trigger if needed
    if (config.trigger != TRIGGER_MANUAL) {
        if (!rfTrigger->waitForTrigger(config.trigger, 5000)) {
            result.errorMessage = "Trigger timeout";
            return result;
        }
    }

    // Apply delay
    if (config.delay > 0) {
        esp_rom_delay_us(config.delay);
    }

    // Execute glitch based on type
    switch (config.type) {
        case GLITCH_CROWBAR:
            if (crowbar && crowbar->isArmed()) {
                result.success = crowbar->glitch(config.pulseWidth);
                if (!result.success) {
                    result.errorMessage = "Crowbar glitch failed";
                }
            } else {
                result.errorMessage = "Crowbar not armed";
            }
            break;

        case GLITCH_LASER:
            if (laser && laser->isArmed()) {
                result.success = laser->glitch(config.pulseWidth);
                if (!result.success) {
                    result.errorMessage = "Laser glitch failed";
                }
            } else {
                result.errorMessage = "Laser not armed";
            }
            break;

        case GLITCH_COMBO:
            if (crowbar && laser && crowbar->isArmed() && laser->isArmed()) {
                // Execute laser first, then crowbar
                result.success = laser->glitch(config.pulseWidth / 1000); // Convert to ms
                if (result.success) {
                    esp_rom_delay_us(100); // Small delay between glitches
                    result.success = crowbar->glitch(SWITCH_RCM_GLITCH_TIME);
                }
                if (!result.success) {
                    result.errorMessage = "Combo glitch failed";
                }
            } else {
                result.errorMessage = "Crowbar or laser not armed";
            }
            break;

        default:
            result.errorMessage = "Unsupported glitch type";
            break;
    }

    result.executionTime = millis() - startTime;
    result.attempts = 1;

    return result;
}

GlitchResult FaultInjectionManager::performGlitch() {
    if (!systemArmed) {
        GlitchResult errorResult;
        errorResult.success = false;
        errorResult.errorMessage = "System not armed";
        return errorResult;
    }

    glitchingActive = true;

    GlitchResult result = executeGlitch(currentConfig);

    // Auto retry if enabled and failed
    if (!result.success && currentConfig.autoRetry && result.attempts < currentConfig.maxRetries) {
        for (uint8_t retry = 1; retry < currentConfig.maxRetries; retry++) {
            vTaskDelay(pdMS_TO_TICKS(100)); // Delay between retries
            result = executeGlitch(currentConfig);
            result.attempts++;
            if (result.success) break;
        }
    }

    glitchingActive = false;

    // Log result
    logResult(result);

    return result;
}

void FaultInjectionManager::logResult(const GlitchResult& result) {
    // Send result to queue for logging task
    if (resultQueue) {
        xQueueSend(resultQueue, &result, pdMS_TO_TICKS(100));
    }
}

// ============================================================================
// TARGET-SPECIFIC METHODS
// ============================================================================

bool FaultInjectionManager::glitchSwitchRCM() {
    GlitchConfig config;
    config.target = TARGET_SWITCH_RCM;
    config.type = GLITCH_CROWBAR;
    config.trigger = TRIGGER_AUTO;
    config.pulseWidth = SWITCH_RCM_GLITCH_TIME;
    config.delay = 150000; // 150µs
    config.intensity = 100;
    config.autoRetry = true;
    config.maxRetries = 3;

    if (!configureGlitch(config)) return false;

    GlitchResult result = performGlitch();
    return result.success;
}

bool FaultInjectionManager::glitchiPhoneCheckm8() {
    GlitchConfig config;
    config.target = TARGET_IPHONE_CHECKM8;
    config.type = GLITCH_COMBO;
    config.trigger = TRIGGER_AUTO;
    config.pulseWidth = IPHONE_CHECKM8_GLITCH_TIME * 1000; // Convert to µs
    config.delay = 500000; // 500µs
    config.intensity = 100;
    config.autoRetry = true;
    config.maxRetries = 5;

    if (!configureGlitch(config)) return false;

    GlitchResult result = performGlitch();
    return result.success;
}

bool FaultInjectionManager::glitchCarKeyless() {
    GlitchConfig config;
    config.target = TARGET_CAR_KEYLESS;
    config.type = GLITCH_CROWBAR;
    config.trigger = TRIGGER_CC1101;
    config.pulseWidth = CAR_KEYLESS_GLITCH_TIME;
    config.delay = 25000; // 25µs
    config.intensity = 100;
    config.autoRetry = true;
    config.maxRetries = 3;

    if (!configureGlitch(config)) return false;

    GlitchResult result = performGlitch();
    return result.success;
}

bool FaultInjectionManager::glitchSmartMeter() {
    GlitchConfig config;
    config.target = TARGET_SMART_METER;
    config.type = GLITCH_LASER;
    config.trigger = TRIGGER_CC1101;
    config.pulseWidth = METER_GLITCH_TIME / 1000; // Convert to ms
    config.delay = 100000; // 100µs
    config.intensity = 100;
    config.autoRetry = false;
    config.maxRetries = 1;

    if (!configureGlitch(config)) return false;

    GlitchResult result = performGlitch();
    return result.success;
}

bool FaultInjectionManager::glitchRFIDHID() {
    GlitchConfig config;
    config.target = TARGET_RFID_HID;
    config.type = GLITCH_CROWBAR;
    config.trigger = TRIGGER_AUTO;
    config.pulseWidth = RFID_GLITCH_TIME;
    config.delay = 50000; // 50µs
    config.intensity = 100;
    config.autoRetry = true;
    config.maxRetries = 5;

    if (!configureGlitch(config)) return false;

    GlitchResult result = performGlitch();
    return result.success;
}

const TargetTiming* FaultInjectionManager::getTargetTiming(GlitchTarget target) {
    if (target >= TARGET_SWITCH_RCM && target <= TARGET_CUSTOM) {
        return &targetTimings[target];
    }
    return nullptr;
}

String FaultInjectionManager::getSystemStatus() {
    char status[256];
    snprintf(status, sizeof(status),
        "Pocket Glitcher Pro Status:\n"
        "System Armed: %s\n"
        "Active Glitch: %s\n"
        "Crowbar: %s\n"
        "Laser: %s\n"
        "RF Trigger: %s\n"
        "Current Target: %s",
        systemArmed ? "YES" : "NO",
        glitchingActive ? "YES" : "NO",
        (crowbar && crowbar->isArmed()) ? "ARMED" : "SAFE",
        (laser && laser->isArmed()) ? "ARMED" : "SAFE",
        rfTrigger ? "READY" : "ERROR",
        getTargetTiming(currentConfig.target)->name
    );
    return String(status);
}

void FaultInjectionManager::emergencyStop() {
    systemArmed = false;
    glitchingActive = false;

    if (crowbar) {
        crowbar->setArmed(false);
    }
    if (laser) {
        laser->setArmed(false);
    }
}

// ============================================================================
// FREERTOS TASKS
// ============================================================================

void lvglMenuTask(void* parameter) {
    // LVGL menu task for touchscreen control
    while (true) {
        // Handle LVGL events and updates
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void triggerDetectorTask(void* parameter) {
    // RF trigger detection task
    while (true) {
        if (faultInjectionManager && faultInjectionManager->isSystemArmed()) {
            // Monitor RF triggers
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void glitchControllerTask(void* parameter) {
    // Main glitch control task
    while (true) {
        if (faultInjectionManager && faultInjectionManager->isSystemArmed()) {
            // Execute queued glitches
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void dataLoggerTask(void* parameter) {
    // Result logging task
    GlitchResult result;
    while (true) {
        if (faultInjectionManager && faultInjectionManager->getResultQueue()) {
            if (xQueueReceive(faultInjectionManager->getResultQueue(), &result, pdMS_TO_TICKS(1000)) == pdTRUE) {
                // Log to SD card
                Serial.printf("GLITCH RESULT: Success=%d, Time=%lums, Attempts=%d\n",
                    result.success, (unsigned long)result.executionTime, result.attempts);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ============================================================================
// GLOBAL FUNCTIONS
// ============================================================================

bool initializeFaultInjectionSystem() {
    if (faultInjectionManager) {
        return true; // Already initialized
    }

    faultInjectionManager = new FaultInjectionManager();
    if (!faultInjectionManager->initialize()) {
        delete faultInjectionManager;
        faultInjectionManager = nullptr;
        return false;
    }

    // Create FreeRTOS tasks
    xTaskCreate(lvglMenuTask, "LVGLMenu", 4096, NULL, 2, &lvglTaskHandle);
    xTaskCreate(triggerDetectorTask, "TriggerDetector", 2048, NULL, 3, &triggerTaskHandle);
    xTaskCreate(glitchControllerTask, "GlitchController", 4096, NULL, 4, &glitchTaskHandle);
    xTaskCreate(dataLoggerTask, "DataLogger", 2048, NULL, 1, &loggerTaskHandle);

    return true;
}

void deinitializeFaultInjectionSystem() {
    if (faultInjectionManager) {
        delete faultInjectionManager;
        faultInjectionManager = nullptr;
    }

    // Delete tasks
    if (lvglTaskHandle) vTaskDelete(lvglTaskHandle);
    if (triggerTaskHandle) vTaskDelete(triggerTaskHandle);
    if (glitchTaskHandle) vTaskDelete(glitchTaskHandle);
    if (loggerTaskHandle) vTaskDelete(loggerTaskHandle);
}

String getFaultInjectionSystemInfo() {
    if (!faultInjectionManager) {
        return "Fault Injection System: NOT INITIALIZED";
    }

    return faultInjectionManager->getSystemStatus();
}
