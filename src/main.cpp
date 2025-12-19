#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Headers dos módulos
#include "lvgl_menu.h"
#include "web_dashboard.h"
#include "q_learn_ia.h"
#include "tts_espeak.h"
#include "fault_mosfet.h"
#include "attacks_manager.h"

// Definições de Hardware
#define CC1101_EN 14
#define PN532_EN 17
#define BLE_EN 18  // Software flag ou pino físico se houver
#define LED_ATTACK_RED_HIGH 4
#define LED_ATTACK_RED_LOW 0
#define FAULT_BUTTON 33

// Mutex de Hardware
SemaphoreHandle_t hw_mutex;

// Estados
enum ModuleType { NONE = 0, MOD_CC1101, MOD_PN532, MOD_BLE };
int current_module = NONE;

// Declaração antecipada
void think_and_speak(const char* text);

void enable_module(int mod) {
    if (xSemaphoreTake(hw_mutex, 100 / portTICK_PERIOD_MS) == pdTRUE) {
        // 1. Desliga tudo (Logica Inversa ou Direta dependendo do MOSFET, assumindo HIGH = LIGA)
        digitalWrite(CC1101_EN, LOW);
        digitalWrite(PN532_EN, LOW);
        // BLE é interno, mas podemos "desligar" o controller
        if (current_module == MOD_BLE && mod != MOD_BLE) {
             esp_bt_controller_disable();
        }

        // 2. Espera descarga
        delay(10); 

        // 3. Liga só o pedido
        if (mod == MOD_CC1101) {
            digitalWrite(CC1101_EN, HIGH);
            current_module = MOD_CC1101;
        } else if (mod == MOD_PN532) {
            digitalWrite(PN532_EN, HIGH);
            current_module = MOD_PN532;
        } else if (mod == MOD_BLE) {
            if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
                esp_bt_controller_enable(ESP_BT_MODE_BLE);
            }
            current_module = MOD_BLE;
        } else {
            current_module = NONE;
        }

        // 4. Estabiliza
        delay(50);
        xSemaphoreGive(hw_mutex);
    } else {
        Serial.println("Hardware ocupado!");
        think_and_speak("Hardware ocupado. Aguarde.");
    }
}

void setup() {
    // 1. Hardware Básico
    Serial.begin(115200);
    
    // Configura Pinos de Power
    pinMode(CC1101_EN, OUTPUT);
    pinMode(PN532_EN, OUTPUT);
    digitalWrite(CC1101_EN, LOW); // Começa desligado
    digitalWrite(PN532_EN, LOW);

    pinMode(LED_ATTACK_RED_HIGH, OUTPUT);
    pinMode(LED_ATTACK_RED_LOW, OUTPUT);
    pinMode(FAULT_BUTTON, INPUT_PULLUP);

    // Cria Mutex
    hw_mutex = xSemaphoreCreateMutex();

    // 2. Init PSRAM e Otimizações
    if(psramInit()){
        Serial.printf("PSRAM Inicializada: %d bytes livres\n", ESP.getFreePsram());
    } else {
        Serial.println("ERRO: PSRAM FALHOU!");
    }

    // 3. Inicializa Módulos
    setup_lvgl_menu();     // UI e Display
    setup_web_dashboard(); // Web Server
    setup_q_learn();       // AI
    setup_tts();           // Voz
    setup_fault_mosfet();  // Fault Injection Trigger

    // 4. State Inicial
    think_and_speak("Sistema Monster Ativado");
    enable_module(NONE); // Garante tudo desligado
}

void loop() {
    // 1. LVGL Handler (Interface Gráfica)
    lv_task_handler(); 

    // 2. Checagem de Botão de Fault (Fora do Menu, direto no Loop para velocidade)
    if (digitalRead(FAULT_BUTTON) == LOW) {
        // Injeção Rápida
        Serial.println("FAULT BUTTON! INJECTING...");
        enable_module(MOD_PN532); // Liga NFC
        fault_pulse_trigger();    // Pulso MOSFET
        delay(500); // Debounce
    }

    // 3. Gerenciamento de Energia (Deep Sleep se ocioso)
    // Se não estiver em ataque, nem aprendendo, nem com web ativa
    static unsigned long last_activity = millis();
    if (lv_disp_get_inactive_time(NULL) < 1000) last_activity = millis();
    
    // Oversimplified sleep logic for demo:
    // if (millis() - last_activity > 30000) {
    //     Serial.println("Entrando em Deep Sleep...");
    //     esp_deep_sleep_start();
    // }

    delay(5); // Yield para WDT
}
