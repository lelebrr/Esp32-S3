#include "attacks_manager.h"
#include "core/aggressive_sd.h"
#include "sd_structure.h"
#include "gesture_sensor.h"
#include "gps_driver.h"
#include "hardware/audio_driver.h"
#include "lvgl_menu.h"
#include "pin_config.h"
#include "s3_driver.h"
#include <Arduino.h>

// Novos módulos
#include "module_manager.h"
#include "led_driver.h"
#include "gesture_actions.h"
#include "ble_attacks.h"
#include "ir_protocols.h"
#include "wps_attacks.h"
#include "pmkid_capture.h"
#include "nfc_relay.h"

// TTS, AI e Logging
#include "tts_espeak.h"
#include "q_learn_ia.h"
#include "logger.h"

// Resource Download e Brute Generator
#include "resource_downloader.h"
#include "brute_generator.h"

// Task Handles
TaskHandle_t hDisplayTask;
TaskHandle_t hNetTask;
TaskHandle_t hAttackTask;
TaskHandle_t hGestureTask;
TaskHandle_t hGPSTask;

// Forward declarations
void handleGestureAction(GestureAction action);

void taskSystemDisplay(void *p) {
    // Check if display is connected by testing SPI response
    // If no display connected, run in headless mode
    Serial.println("[DISPLAY] Checking for TFT display...");

    // Try to initialize - wrapped in safe check
    pinMode(PIN_TFT_CS, OUTPUT);
    digitalWrite(PIN_TFT_CS, HIGH);

    // Simple check: if TFT_RST pin is not connected, skip display init
    // In headless mode, we just log to Serial
    bool displayPresent = true; // Assume present for now

#ifdef HEADLESS_MODE
    displayPresent = false;
#endif

    if (displayPresent) {
        // Try display init with timeout protection
        Serial.println("[DISPLAY] Initializing TFT_eSPI...");
        setup_lvgl_menu();

        for (;;) {
            lv_task_handler();
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    } else {
        Serial.println("[DISPLAY] Running in HEADLESS mode - no display");
        vTaskDelete(NULL); // Kill this task if no display
    }
}

void taskNetworkManager(void *p) {
    for (;;) {
        // Web Server Handle
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void taskAttackEngine(void *p) {
    pinMode(PIN_BTN_FAULT, INPUT_PULLUP);
    for (;;) {
        // Check Hardware Trigger
        if (digitalRead(PIN_BTN_FAULT) == LOW) {
            Serial.println("[ATTACK] Hardware Triggered Fault Injection!");
            digitalWrite(PIN_LED_RED_ATTACK_HI, HIGH);
            delay(50); // Debounce
            digitalWrite(PIN_LED_RED_ATTACK_HI, LOW);
            // attacks_start(ATTACK_NFC_FAULT); // Example
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Gesture sensor polling task
 * Runs on Core 1, updates gesture sensor and processes actions
 */
void taskGestureSensor(void *p) {
    Serial.println("[GESTURE] Task started");

    // Wait for sensor to be ready
    vTaskDelay(100 / portTICK_PERIOD_MS);

    for (;;) {
        if (GestureSensor::isAvailable()) {
            // Poll for gestures
            GestureSensor::update();

            // Check if we got a gesture
            GestureType gesture = GestureSensor::getLastGesture();
            if (gesture != GESTURE_NONE) {
                // Convert to action and handle
                GestureAction action = GestureSensor::getActionForGesture(gesture);
                handleGestureAction(action);
            }
        }

        // Poll every 30ms for responsive gesture detection
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief GPS polling task
 * Runs on Core 0, continuously reads GPS data from UART2
 */
void taskGPS(void *p) {
    Serial.println("[GPS] Task started");

    for (;;) {
        if (GPSDriver::isAvailable()) {
            GPSDriver::update();

            // Log position every 10 seconds if valid
            static uint32_t lastLog = 0;
            if (GPSDriver::isValid() && millis() - lastLog > 10000) {
                Serial.printf(
                    "[GPS] Position: %s, Sats: %d, HDOP: %.1f\n",
                    GPSDriver::getCoordsString().c_str(),
                    GPSDriver::getSatellites(),
                    GPSDriver::getHDOP()
                );
                lastLog = millis();
            }
        }

        // Poll every 100ms for GPS data
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Handle gesture actions - map to navigation/UI events and attacks
 */
void handleGestureAction(GestureAction action) {
    // Get LVGL key for this gesture
    uint32_t lvgl_key = gesture_action_to_lvgl_key(action);

    // Send to LVGL if it's a navigation action
    if (lvgl_key != 0) { lvgl_send_gesture_key(lvgl_key); }

    // Handle specific gestures
    switch (action) {
        case ACTION_MENU_UP: Serial.println("[GESTURE->UI] Navigate UP"); break;

        case ACTION_MENU_DOWN: Serial.println("[GESTURE->UI] Navigate DOWN"); break;

        case ACTION_MENU_LEFT: Serial.println("[GESTURE->UI] Navigate LEFT"); break;

        case ACTION_MENU_RIGHT: Serial.println("[GESTURE->UI] Navigate RIGHT"); break;

        case ACTION_SELECT: Serial.println("[GESTURE->UI] SELECT/CONFIRM"); break;

        case ACTION_BACK:
            Serial.println("[GESTURE->UI] BACK/CANCEL");
            // Also stop any running attack when going back
            if (attacks_is_running()) {
                attacks_stop();
                Serial.println("[GESTURE] Attack stopped via BACK gesture");
            }
            break;

        case ACTION_SPECIAL_1:
            Serial.println("[GESTURE->UI] Clockwise - Next Attack");
            // Could cycle to next attack type
            break;

        case ACTION_SPECIAL_2:
            Serial.println("[GESTURE->UI] Anti-Clockwise - Prev Attack");
            // Could cycle to previous attack type
            break;

        case ACTION_WAKE:
            Serial.println("[GESTURE] WAVE - Emergency Stop All!");
            // Wave gesture = STOP ALL ATTACKS (emergency)
            if (attacks_is_running()) {
                attacks_stop();
                Serial.println("[GESTURE] All attacks stopped via WAVE gesture!");
            }
            // Also wake screen if dimmed
            ledcWrite(1, 255);
            break;

        default: break;
    }
}

void setup() {
    // --- EARLY SERIAL INIT FOR USB CDC ---
    // Increase timeout safety
    Serial.begin(115200);
    // Give valid delay for USB to catch up, but don't block forever with WDT risk
    delay(2000); 
    
    Serial.println("\n\n========================================");
    Serial.println("     MONSTER S3 DEBUG BOOT START");
    Serial.println("========================================");
    
    // --- SAFE MODE CHECK ---
    // If boot loop persists, we can debug here
    Serial.println("[BOOT] Checkpoint 1: Serial Init OK");
    
    // Setup backlight - Turn OFF initially to save power/avoid glitches
    ledcSetup(1, 2000, 8);
    ledcAttachPin(PIN_TFT_BL, 1);
    ledcWrite(1, 0); // Black screen
    
    Serial.printf("[BOOT] Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("[BOOT] PSRAM: %s\n", psramFound() ? "FOUND" : "NOT FOUND");
    
    // --- SD BOOT (DISABLED FOR DEBUGGING) ---
    // Serial.println("[BOOT] Starting SD init...");
    // aggressive_boot_logic();

    // --- SD STRUCTURE SETUP (DISABLED) ---
    // Serial.println("[BOOT] Setting up SD structure...");
    // setup_sd_structure();
    
    // Verify Driver Init safely
    Serial.println("[BOOT] Checkpoint 2: Drivers");
    
    // MonsterDriver::init(); // Commented out for isolation
    
    // --- Attacks Manager Init ---
    Serial.println("[BOOT] Starting Attacks manager init...");
    attacks_init();
    
    // --- Hardware Drivers ---
    Serial.println("[BOOT] Init Hardware modules...");
    ModuleManager::init();
    LEDDriver::init();
    LEDDriver::setBrightness(20);
    LEDDriver::setAll(LED_BLUE);
    LEDDriver::show();
    
    // --- Init Display ---
    Serial.println("[BOOT] Starting Display Task...");
    // Create Tasks
    xTaskCreatePinnedToCore(taskSystemDisplay, "Display", 8192, NULL, 2, &hDisplayTask, 1);
    xTaskCreatePinnedToCore(taskNetworkManager, "Net", 4096, NULL, 1, &hNetTask, 1);
    xTaskCreatePinnedToCore(taskAttackEngine, "Attacks", 8192, NULL, 5, &hAttackTask, 0);
    xTaskCreatePinnedToCore(taskGestureSensor, "Gesture", 4096, NULL, 3, &hGestureTask, 1);
    xTaskCreatePinnedToCore(taskGPS, "GPS", 4096, NULL, 1, &hGPSTask, 0);

    Serial.println("[SYSTEM] Boot sequence complete.");
    tts_speak("sistema_pronto");
}

void loop() {
    // AI loop step - runs every 30s in combat mode
    ai_loop_step();
    
    // TTS loop (running in own task)
    loop_tts();
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
