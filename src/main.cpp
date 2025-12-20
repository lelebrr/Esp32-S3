/**
 * @file main.cpp
 * @brief System Entry Point and Task Scheduler
 * 
 * Initializes hardware drivers, mounts filesystems, and creates
 * FreeRTOS tasks for display, network, attack engine, and AI.
 * Functions as the central coordinator for the Monster S3 firmware.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include <Arduino.h>
#include "s3_driver.h"
#include "pin_config.h"
#include "lvgl_menu.h"
#include "attacks_manager.h"
#include "gesture_sensor.h"
#include "gps_driver.h"
#include "core/aggressive_sd.h"
#include "tts_espeak.h"
#include "q_learn_ia.h"
#include "fault_mosfet.h"
#include "web_dashboard.h"

// ============================================================================
// TASK HANDLES
// ============================================================================
TaskHandle_t hDisplayTask;
TaskHandle_t hNetTask;
TaskHandle_t hAttackTask;
TaskHandle_t hGestureTask;
TaskHandle_t hGPSTask;
TaskHandle_t hAITask;
TaskHandle_t hAudioTask;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
void handleGestureAction(GestureAction action);
void checkHardwareButtons();

// ============================================================================
// TASKS
// ============================================================================

/**
 * @brief Display/UI task - handles LVGL rendering
 * Runs on Core 1 for smooth UI
 */
void taskSystemDisplay(void *p) {
    setup_lvgl_menu();
    
    for(;;) {
        lv_task_handler();
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Network task - handles web server and WiFi
 * Runs on Core 1
 */
void taskNetworkManager(void *p) {
    // Small delay to let other systems init
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    setup_web_dashboard();
    
    for(;;) {
        // Process WebSocket clients and status broadcasts
        web_dashboard_loop();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Attack engine task - manages attack execution
 * Runs on Core 0 (dedicated)
 */
void taskAttackEngine(void *p) {
    attacks_init();
    setup_fault_mosfet();
    
    for(;;) {
        // Check hardware fault trigger button
        if (fault_button_pressed()) {
            Serial.println("[ATTACK] Hardware Triggered Fault Injection!");
            digitalWrite(PIN_LED_RED_ATTACK_HI, HIGH);
            fault_pulse_trigger();
            delay(50); // Debounce
            digitalWrite(PIN_LED_RED_ATTACK_HI, LOW);
            while(fault_button_pressed()) delay(10); // Wait release
        }
        
        // Update running attacks
        attacks_update();
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Gesture sensor task - polls PAJ7620U2
 * Runs on Core 1
 */
void taskGestureSensor(void *p) {
    Serial.println("[GESTURE] Task started");
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    for(;;) {
        if (GestureSensor::isAvailable()) {
            GestureSensor::update();
            
            GestureType gesture = GestureSensor::getLastGesture();
            if (gesture != GESTURE_NONE) {
                GestureAction action = GestureSensor::getActionForGesture(gesture);
                handleGestureAction(action);
            }
        }
        
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief GPS task - reads UART2 data
 * Runs on Core 0
 */
void taskGPS(void *p) {
    Serial.println("[GPS] Task started");
    
    for(;;) {
        if (GPSDriver::isAvailable()) {
            GPSDriver::update();
            
            static uint32_t lastLog = 0;
            if (GPSDriver::isValid() && millis() - lastLog > 10000) {
                Serial.printf("[GPS] Position: %s, Sats: %d\n",
                    GPSDriver::getCoordsString().c_str(),
                    GPSDriver::getSatellites());
                lastLog = millis();
            }
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief AI task - Q-learning decision making
 * Runs on Core 0
 */
void taskAI(void *p) {
    Serial.println("[AI] Task started");
    setup_q_learn();
    
    for(;;) {
        ai_loop_step();
        checkHardwareButtons(); // Check AI feedback buttons
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Audio task - TTS playback
 * Runs on Core 1
 */
void taskAudio(void *p) {
    Serial.println("[AUDIO] Task started");
    setup_tts();
    
    for(;;) {
        loop_tts();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// ============================================================================
// HELPERS
// ============================================================================

/**
 * @brief Handle gesture actions - UI navigation and attack control
 */
void handleGestureAction(GestureAction action) {
    uint32_t lvgl_key = gesture_action_to_lvgl_key(action);
    
    if (lvgl_key != 0) {
        lvgl_send_gesture_key(lvgl_key);
    }
    
    switch (action) {
        case ACTION_MENU_UP:
            Serial.println("[GESTURE->UI] Navigate UP");
            break;
        case ACTION_MENU_DOWN:
            Serial.println("[GESTURE->UI] Navigate DOWN");
            break;
        case ACTION_MENU_LEFT:
            Serial.println("[GESTURE->UI] Navigate LEFT");
            break;
        case ACTION_MENU_RIGHT:
            Serial.println("[GESTURE->UI] Navigate RIGHT");
            break;
        case ACTION_SELECT:
            Serial.println("[GESTURE->UI] SELECT");
            break;
        case ACTION_BACK:
            Serial.println("[GESTURE->UI] BACK");
            if (attacks_is_running()) {
                attacks_stop();
                speak(TTS_SUCCESS);
            }
            break;
        case ACTION_WAKE:
            Serial.println("[GESTURE] WAVE - Emergency Stop!");
            if (attacks_is_running()) {
                attacks_stop();
            }
            digitalWrite(PIN_TFT_BL, HIGH);
            break;
        default:
            break;
    }
}

/**
 * @brief Check hardware buttons for AI feedback
 * Button C = Success, Button D = Fail
 */
void checkHardwareButtons() {
    static uint32_t lastCheck = 0;
    if (millis() - lastCheck < 200) return; // Debounce
    
    // Button C (Down) = Success feedback
    if (digitalRead(PIN_BTN_C) == LOW) {
        ai_feedback_success();
        lastCheck = millis();
        while(digitalRead(PIN_BTN_C) == LOW) delay(10);
    }
    
    // Button D (Left) = Fail feedback
    if (digitalRead(PIN_BTN_D) == LOW) {
        ai_feedback_fail();
        lastCheck = millis();
        while(digitalRead(PIN_BTN_D) == LOW) delay(10);
    }
}

// ============================================================================
// SETUP & LOOP
// ============================================================================

void setup() {
    // --- SERIAL FIRST ---
    Serial.begin(115200);
    delay(100);
    
    // --- AGGRESSIVE SD BOOT ---
    aggressive_boot_logic();
    
    // --- HARDWARE INIT ---
    MonsterDriver::init();
    
    // --- BUTTON PINS ---
    pinMode(PIN_BTN_C, INPUT_PULLUP);
    pinMode(PIN_BTN_D, INPUT_PULLUP);
    
    // Check wake source
    if (GestureSensor::wasWakeupByGesture()) {
        Serial.println("[SYSTEM] Woke up by gesture!");
    }
    
    // Ready signal
    digitalWrite(PIN_LED_RED_ATTACK_LO, HIGH);
    delay(200);
    digitalWrite(PIN_LED_RED_ATTACK_LO, LOW);

    // --- CREATE TASKS ---
    // Core 1: Display, Network, Gesture, Audio (UI-related)
    // Core 0: Attacks, GPS, AI (processing)
    
    xTaskCreatePinnedToCore(taskSystemDisplay,   "Display",  8192,  NULL, 2, &hDisplayTask,  1);
    xTaskCreatePinnedToCore(taskNetworkManager,  "Net",      8192,  NULL, 1, &hNetTask,      1);
    xTaskCreatePinnedToCore(taskGestureSensor,   "Gesture",  4096,  NULL, 3, &hGestureTask,  1);
    xTaskCreatePinnedToCore(taskAudio,           "Audio",    8192,  NULL, 2, &hAudioTask,    1);
    xTaskCreatePinnedToCore(taskAttackEngine,    "Attacks",  8192,  NULL, 5, &hAttackTask,   0);
    xTaskCreatePinnedToCore(taskGPS,             "GPS",      4096,  NULL, 2, &hGPSTask,      0);
    xTaskCreatePinnedToCore(taskAI,              "AI",       8192,  NULL, 1, &hAITask,       0);
    
    Serial.println("[SYSTEM] Monster S3 Ready!");
    Serial.printf("[SYSTEM] Free Heap: %d, Free PSRAM: %d\n", 
                  ESP.getFreeHeap(), ESP.getFreePsram());
}

void loop() {
    // Main loop does nothing - all work in FreeRTOS tasks
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
