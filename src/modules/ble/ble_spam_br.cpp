#include "ble_spam_br.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_mac.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <globals.h>

// Nomes brasileiros para BLE spam otimizado
static const char* BR_NAMES[] = {
    "Oi Fibra", "Vivo Internet", "GVT", "TIM Live", "Claro Net", "NET Virtua",
    "Oi Velox", "Vivo Fibra", "TIM Fiber", "GVT Turbo", "Claro Fibra", "NET Ultra",
    "Oi WiFi", "Vivo WiFi", "TIM WiFi", "GVT WiFi", "Claro WiFi", "NET WiFi"
};
static const int BR_NAMES_COUNT = sizeof(BR_NAMES) / sizeof(BR_NAMES[0]);

// Buffer otimizado para reduzir alocações
static char device_name_buffer[32];
static uint8_t mac_buffer[6];

// Task handle para controle de spam
static TaskHandle_t ble_spam_task = NULL;
static volatile bool ble_spam_active = false;
static volatile uint32_t packets_sent = 0;

// Configurações otimizadas para máxima velocidade
#define BLE_SPAM_INTERVAL_MS 1  // ~1000pps (1ms entre pacotes)
#define BLE_ADV_DURATION_MS 20  // 20ms por anúncio
#define BLE_MAX_TX_POWER ESP_PWR_LVL_P21

// Mutex para thread safety
static SemaphoreHandle_t ble_mutex = NULL;

// Função otimizada para gerar MAC aleatória
static void generate_random_mac(uint8_t* mac) {
    esp_fill_random(mac, 6);
    mac[0] |= 0xF0;  // Set high bits para parecer válido
}

// Função otimizada para gerar nome brasileiro
static const char* get_random_br_name() {
    int idx = esp_random() % BR_NAMES_COUNT;
    return BR_NAMES[idx];
}

// Função otimizada para BLE spam
static void ble_spam_worker(void* pvParameters) {
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();

    while (ble_spam_active) {
        if (xSemaphoreTake(ble_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            // Gera nova identidade
            generate_random_mac(mac_buffer);
            esp_base_mac_addr_set(mac_buffer);

            // Reinicializa BLE com nova identidade
            BLEDevice::deinit();
            BLEDevice::init("");

            // Configura potência máxima
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, BLE_MAX_TX_POWER);

            // Cria anúncio otimizado
            BLEAdvertisementData advData;
            const char* name = get_random_br_name();
            advData.setName(name);
            advData.setFlags(0x06);  // General discoverable + BR/EDR

            // Adiciona serviço genérico para parecer dispositivo real
            NimBLEUUID serviceUUID((uint32_t)(esp_random() & 0xFFFFFF));
            advData.addServiceUUID(serviceUUID);

            pAdvertising = BLEDevice::getAdvertising();
            pAdvertising->setAdvertisementData(advData);

            // Anúncio rápido
            pAdvertising->start();
            vTaskDelay(pdMS_TO_TICKS(BLE_ADV_DURATION_MS));
            pAdvertising->stop();

            packets_sent++;

            xSemaphoreGive(ble_mutex);

            // Intervalo mínimo para ~800-1000pps
            vTaskDelay(pdMS_TO_TICKS(BLE_SPAM_INTERVAL_MS));
        } else {
            // Fallback se mutex falhar
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }

    // Cleanup
    BLEDevice::deinit();
    vTaskDelete(NULL);
}

// Função pública para iniciar BLE spam brasileiro
bool start_ble_spam_br() {
    if (ble_spam_active) return false;

    // Inicializa mutex
    if (ble_mutex == NULL) {
        ble_mutex = xSemaphoreCreateMutex();
        if (ble_mutex == NULL) return false;
    }

    ble_spam_active = true;
    packets_sent = 0;

    // Cria task de alta prioridade para máxima velocidade
    BaseType_t result = xTaskCreatePinnedToCore(
        ble_spam_worker,
        "BLE_SPAM_BR",
        4096,
        NULL,
        5,  // Alta prioridade
        &ble_spam_task,
        1   // Core 1
    );

    return (result == pdPASS);
}

// Função pública para parar BLE spam
void stop_ble_spam_br() {
    if (!ble_spam_active) return;

    ble_spam_active = false;

    if (ble_spam_task != NULL) {
        vTaskDelete(ble_spam_task);
        ble_spam_task = NULL;
    }

    BLEDevice::deinit();
}

// Função para obter estatísticas
uint32_t get_ble_spam_packets_sent() {
    return packets_sent;
}

// Função para verificar se está ativo
bool is_ble_spam_active() {
    return ble_spam_active;
}
