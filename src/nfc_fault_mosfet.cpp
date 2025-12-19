#include "nfc_fault_mosfet.h"
#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <globals.h>

// Pinos otimizados conforme especificação
#define MOSFET_CONTROL_PIN GPIO_NUM_42    // GPIO42 controla IRF520 MOSFET → PN532 VCC
#define FAULT_BUTTON_PIN GPIO_NUM_33      // GPIO33 push button fault injection
#define PN532_RESET_PIN GPIO_NUM_13       // GPIO13 PN532 RST (opcional)

// Configurações de pulso otimizadas
#define FAULT_PULSE_DURATION_MS 5         // 5ms pulso conforme especificação
#define FAULT_DEBOUNCE_MS 1000            // Debounce 1s para evitar spam
#define FAULT_LED_DURATION_MS 200         // LED acende por 200ms após fault

// Estados e controle
static volatile bool fault_active = false;
static volatile bool button_pressed = false;
static volatile uint32_t fault_count = 0;

// Timer para debounce
static esp_timer_handle_t fault_debounce_timer = NULL;

// Task para monitoramento do botão
static TaskHandle_t fault_monitor_task = NULL;

// Mutex para thread safety
static SemaphoreHandle_t fault_mutex = NULL;

// Função callback do timer de debounce
static void fault_debounce_callback(void* arg) {
    button_pressed = false;
}

// Função para executar fault injection
static void execute_nfc_fault() {
    if (fault_active) return;  // Já executando

    fault_active = true;
    fault_count++;

    log_w("[NFC_FAULT] Executing manual fault injection #%lu", fault_count);

    // Pulso no MOSFET: HIGH por 5ms corta VCC do PN532
    gpio_set_level(MOSFET_CONTROL_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(FAULT_PULSE_DURATION_MS));
    gpio_set_level(MOSFET_CONTROL_PIN, LOW);

    // Feedback LED (usando LED_ATTACK_RED_HIGH)
    extern bool fault_injection_active;  // Do main.cpp
    fault_injection_active = true;

    // TTS feedback
    extern void think_and_speak(const char* text);
    think_and_speak("Porta aberta. Falha injetada no NFC.");

    // Mantém LED aceso por tempo definido
    vTaskDelay(pdMS_TO_TICKS(FAULT_LED_DURATION_MS));
    fault_injection_active = false;

    fault_active = false;

    log_i("[NFC_FAULT] Fault injection completed");
}

// Task para monitorar botão de fault injection
static void fault_monitor_worker(void* pvParameters) {
    // Configura ISR para botão
    gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << FAULT_BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // Interrupt on falling edge
    };
    gpio_config(&button_config);

    // Instala ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(FAULT_BUTTON_PIN, [](void* arg) {
        if (!button_pressed) {
            button_pressed = true;
            // Reinicia timer de debounce
            esp_timer_stop(fault_debounce_timer);
            esp_timer_start_once(fault_debounce_timer, FAULT_DEBOUNCE_MS * 1000);
            // Executa fault
            execute_nfc_fault();
        }
    }, NULL);

    // Loop principal (mantém task viva)
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));  // Check a cada 100ms
    }
}

// Função para inicializar sistema de fault injection
bool init_nfc_fault_mosfet() {
    // Configura pino MOSFET como output
    gpio_config_t mosfet_config = {
        .pin_bit_mask = (1ULL << MOSFET_CONTROL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&mosfet_config);

    // Inicializa MOSFET em LOW (normal operation)
    gpio_set_level(MOSFET_CONTROL_PIN, LOW);

    // Configura PN532 RST se disponível
    if (PN532_RESET_PIN != GPIO_NUM_MAX) {
        gpio_config_t rst_config = {
            .pin_bit_mask = (1ULL << PN532_RESET_PIN),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&rst_config);
        gpio_set_level(PN532_RESET_PIN, HIGH);  // Normal state
    }

    // Cria timer de debounce
    esp_timer_create_args_t timer_args = {
        .callback = fault_debounce_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "fault_debounce"
    };

    if (esp_timer_create(&timer_args, &fault_debounce_timer) != ESP_OK) {
        log_e("[NFC_FAULT] Failed to create debounce timer");
        return false;
    }

    // Inicializa mutex
    fault_mutex = xSemaphoreCreateMutex();
    if (fault_mutex == NULL) {
        log_e("[NFC_FAULT] Failed to create mutex");
        return false;
    }

    // Cria task de monitoramento
    BaseType_t result = xTaskCreatePinnedToCore(
        fault_monitor_worker,
        "NFC_FAULT_MONITOR",
        2048,
        NULL,
        4,  // Prioridade média
        &fault_monitor_task,
        1   // Core 1
    );

    if (result != pdPASS) {
        log_e("[NFC_FAULT] Failed to create monitor task");
        return false;
    }

    log_i("[NFC_FAULT] NFC fault injection system initialized");
    return true;
}

// Função para desinicializar
void deinit_nfc_fault_mosfet() {
    if (fault_monitor_task != NULL) {
        vTaskDelete(fault_monitor_task);
        fault_monitor_task = NULL;
    }

    if (fault_debounce_timer != NULL) {
        esp_timer_stop(fault_debounce_timer);
        esp_timer_delete(fault_debounce_timer);
        fault_debounce_timer = NULL;
    }

    if (fault_mutex != NULL) {
        vSemaphoreDelete(fault_mutex);
        fault_mutex = NULL;
    }

    gpio_set_level(MOSFET_CONTROL_PIN, LOW);
}

// Funções de controle público
void trigger_nfc_fault_manual() {
    if (xSemaphoreTake(fault_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        execute_nfc_fault();
        xSemaphoreGive(fault_mutex);
    }
}

uint32_t get_nfc_fault_count() {
    return fault_count;
}

bool is_nfc_fault_active() {
    return fault_active;
}

// Função para reset do contador
void reset_nfc_fault_count() {
    if (xSemaphoreTake(fault_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        fault_count = 0;
        xSemaphoreGive(fault_mutex);
    }
}
