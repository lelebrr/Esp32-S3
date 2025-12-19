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
#include <esp_bt.h>
#include <esp_gap_ble_api.h>

// Nomes brasileiros para BLE spam otimizado
static const char* BR_NAMES[] = {
    "Oi Fibra", "Vivo Internet", "GVT", "TIM Live", "Claro Net", "NET Virtua",
    "Oi Velox", "Vivo Fibra", "TIM Fiber", "GVT Turbo", "Claro Fibra", "NET Ultra",
    "Oi WiFi", "Vivo WiFi", "TIM WiFi", "GVT WiFi", "Claro WiFi", "NET WiFi",
    "iPhone de Lele", "Samsung S24 Ultra", "AirPods Pro", "JBL Flip 5",
    "TV Sala", "Chromecast", "Roku Express", "Alexa Echo Dot"
};
static const int BR_NAMES_COUNT = sizeof(BR_NAMES) / sizeof(BR_NAMES[0]);

// Task handle
static TaskHandle_t ble_spam_task = NULL;
static volatile bool ble_spam_active = false;
static volatile uint32_t packets_sent = 0;

// Configurações
#define BLE_SPAM_PACKET_DELAY_MS 0  // 0 = Máxima velocidade (flood)

// Raw Advertising Packet Structure
static uint8_t raw_adv_data[31];
static uint8_t raw_adv_len = 0;

// Random MAC generator
static void random_mac(uint8_t* mac) {
    esp_fill_random(mac, 6);
    mac[0] |= 0xC0; // Random private address
}

// Build raw packet
static void build_random_adv_packet() {
    uint8_t flags_len = 2;
    uint8_t flags_type = 0x01;
    uint8_t flags_val = 0x06;

    const char* name = BR_NAMES[esp_random() % BR_NAMES_COUNT];
    uint8_t name_len = strlen(name);
    if (name_len > 26) name_len = 26;

    uint8_t idx = 0;
    // Flags
    raw_adv_data[idx++] = flags_len;
    raw_adv_data[idx++] = flags_type;
    raw_adv_data[idx++] = flags_val;

    // Complete Local Name
    raw_adv_data[idx++] = name_len + 1;
    raw_adv_data[idx++] = 0x09; // Complete Local Name
    memcpy(&raw_adv_data[idx], name, name_len);
    idx += name_len;

    raw_adv_len = idx;
}

// Worker task for High Speed Spam
static void ble_spam_worker(void* pvParameters) {
    // Inicializa BLE controller se necessario (geralmente já feito no main)
    if (!BLEDevice::getInitialized()) {
        BLEDevice::init("");
    }
    
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    // Stop any existing adv
    pAdvertising->stop(); 

    // Raw GAP parameters
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x20, // Min interval (20ms) - ignored in manual loop
        .adv_int_max = 0x40,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_RANDOM,
        .peer_addr = {0},
        .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };

    uint8_t rand_mac[6];

    while (ble_spam_active) {
        // Change Address
        random_mac(rand_mac);
        esp_ble_gap_config_local_privacy(true); // Enable privacy to use random address
        // Directly manipulating MAC is hard without deinit, 
        // but we can cycle identity addresses if using privacy features,
        // OR just rely on rapid payload changes which confuse scanners.
        // For "Invalid Packet" spam or "Ghost" devices:
        
        build_random_adv_packet();
        
        // Config Raw Data
        esp_ble_gap_config_adv_data_raw(raw_adv_data, raw_adv_len);
        
        // Start Advertising
        esp_ble_gap_start_advertising(&adv_params);
        
        // Wait extremely short time
        // vTaskDelay(1) is too slow (10ms). We need shorter delay or loop.
        // Yield to allow stack to process
        // ets_delay_us(500); // 0.5ms delay -> ~2000 pps theoretical
        vTaskDelay(pdMS_TO_TICKS(10)); // Give some time for the packet to go out
        
        // Stop
        esp_ble_gap_stop_advertising();
        
        packets_sent++;
        
        // Small delay to prevent watchdog starvation if pinned to core 1
        if (packets_sent % 50 == 0) vTaskDelay(1);
    }
    
    // Cleanup
    esp_ble_gap_stop_advertising();
    vTaskDelete(NULL);
}

// Start/Stop functions
bool start_ble_spam_br() {
    if (ble_spam_active) return false;
    ble_spam_active = true;
    packets_sent = 0;
    
    // Core 1 for RF
    xTaskCreatePinnedToCore(ble_spam_worker, "BLE_SPAM", 4096, NULL, 5, &ble_spam_task, 1);
    return true;
}

void stop_ble_spam_br() {
    ble_spam_active = false;
    // Allow task to finish
    vTaskDelay(100); 
    if (ble_spam_task) {
        vTaskDelete(ble_spam_task);
        ble_spam_task = NULL;
    }
    // Ensure advertising is stopped
    esp_ble_gap_stop_advertising();
}

uint32_t get_ble_spam_packets_sent() {
    return packets_sent;
}

bool is_ble_spam_active() {
    return ble_spam_active;
}
