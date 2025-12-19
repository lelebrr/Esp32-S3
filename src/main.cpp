#include <Arduino.h>
#include "s3_driver.h"
#include "pin_config.h"
#include "lvgl_menu.h"
#include "attacks_manager.h"

// Task Handles
TaskHandle_t hDisplayTask;
TaskHandle_t hNetTask;
TaskHandle_t hAttackTask;

void taskSystemDisplay(void *p) {
    // Init LVGL inside the task (it's safe usually, but better if called once. 
    // setup_lvgl_menu() calls st7789_init which uses SPI. SPI should be thread safe or used on one task.
    // We will assume Display Task OWNS the display SPI.)
    setup_lvgl_menu();
    
    for(;;) {
        lv_task_handler();
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void taskNetworkManager(void *p) {
    for(;;) {
        // Web Server Handle
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void taskAttackEngine(void *p) {
    attacks_init();
    pinMode(PIN_BTN_FAULT, INPUT_PULLUP);
    for(;;) {
        // Check Hardware Trigger
        if(digitalRead(PIN_BTN_FAULT) == LOW) {
            Serial.println("[ATTACK] Hardware Triggered Fault Injection!");
            digitalWrite(PIN_LED_RED_ATTACK_HI, HIGH);
            delay(50); // Debounce
            digitalWrite(PIN_LED_RED_ATTACK_HI, LOW);
            // attacks_start(ATTACK_NFC_FAULT); // Example
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {
    MonsterDriver::init();
    
    // Ready Signal
    digitalWrite(PIN_LED_RED_ATTACK_LO, HIGH); 

    // Create Tasks
    xTaskCreatePinnedToCore(taskSystemDisplay, "Display", 8192, NULL, 2, &hDisplayTask, 1);
    xTaskCreatePinnedToCore(taskNetworkManager, "Net",    4096, NULL, 1, &hNetTask,     1);
    xTaskCreatePinnedToCore(taskAttackEngine,  "Attacks", 8192, NULL, 5, &hAttackTask,  0);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
