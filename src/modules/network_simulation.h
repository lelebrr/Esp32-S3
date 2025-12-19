#pragma once
/**
 * @file network_simulation.h
 * @brief High-performance ESP32 network simulation module
 * Absolute performance, zero compromises
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_adc_cal.h>
#include <driver/uart.h>
#include <driver/adc.h>
#include <SD.h>

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief Raw packet structure for lock-free queue
 */
typedef struct {
    uint8_t data[2048];  // Max payload size
    size_t len;
    uint8_t type;        // 0=BLE, 1=WiFi, 2=UART
    uint64_t timestamp;
} raw_pkt_t;

// ============================================================================
// IRAM HOT FUNCTIONS AND TABLES
// ============================================================================

/**
 * @brief MAC address table in IRAM for 12ns access
 */
extern uint8_t mac_table[300][6] __attribute__((section(".iram1.bss")));

/**
 * @brief Hot packet processing function in IRAM
 */
void ICACHE_FLASH_ATTR process_packet_hot(const raw_pkt_t* pkt);

/**
 * @brief Fast MAC lookup in IRAM
 */
bool ICACHE_FLASH_ATTR mac_lookup_fast(const uint8_t* mac);

// ============================================================================
// PERFORMANCE SIMULATION CLASS
// ============================================================================

class NetworkSimulation {
private:
    // Lock-free queue
    QueueHandle_t pkt_q;

    // Task handles
    TaskHandle_t ble_task_handle;
    TaskHandle_t wifi_task_handle;

    // Clock management
    esp_pm_config_t pm_config;
    bool high_perf_mode;
    unsigned long last_adc_check;
    float voltage_threshold;

    // DMA UART
    uart_config_t uart_config;
    QueueHandle_t uart_queue;

    // BLE HCI controller
    bool ble_initialized;

    // Metrics
    File metrics_file;
    uint64_t burst_start_time;
    uint32_t packet_count;

    // Berserk mode
    bool berserk_active;

    // ADC calibration
    esp_adc_cal_characteristics_t* adc_chars;
    adc1_channel_t adc_channel;

public:
    NetworkSimulation();
    ~NetworkSimulation();

    // Initialization
    bool begin();

    // Core tasks
    void ble_capture_task();
    void wifi_inject_task();

    // Clock management
    void adjust_clock_speed();
    float read_battery_voltage();

    // DMA setup
    bool setup_uart_dma();
    bool setup_ble_hci();

    // Burst transmission
    void wifi_burst_transmit();

    // Metrics
    void log_burst_metrics(double ms_per_packet);
    void check_performance_threshold(double ms_per_packet);

    // Berserk mode
    void enter_berserk_mode();
    void exit_berserk_mode();

    // Control
    void start_simulation();
    void stop_simulation();
    void restart_on_failure();

    // LED indicator
    void set_led_indicator(bool red_flash);

private:
    // Internal helpers
    void init_queue();
    void create_tasks();
    void cleanup();
};

// ============================================================================
// GLOBAL INSTANCE
// ============================================================================

extern NetworkSimulation networkSim;

// ============================================================================
// TASK FUNCTIONS (extern C for FreeRTOS)
// ============================================================================

extern "C" {
    void bleCaptureTask(void* param);
    void wifiInjectTask(void* param);
}

#endif // NETWORK_SIMULATION_H
