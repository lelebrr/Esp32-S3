#include "stealth.h"
#include <globals.h>
#include <Arduino.h>
#include <driver/gpio.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_system.h>

// Stealth implementation
class StealthManager {
private:
    const int WAKE_BUTTON_PIN = 0;
    const int LED_PIN = 2; // Assuming standard LED pin
    bool stealth_mode = false; // Default to false until explicitly enabled

    void configure_deep_sleep() {
        // Configure wake sources
        esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BUTTON_PIN, 0); // Wake on GPIO 0 low

        // Timer wake up every 10 minutes (for 90% sleep time)
        // 90% sleep = 10% awake time
        // If awake for 1 minute, sleep for 9 minutes
        esp_sleep_enable_timer_wakeup(9 * 60 * 1000000ULL); // 9 minutes in microseconds
    }

    void blink_led_code(uint8_t code) {
        // Blink LED once for code 0x1
        if (code == 0x1) {
            pinMode(LED_PIN, OUTPUT);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
        }
    }

    void spoof_mac_address() {
        uint8_t random_mac[6];
        esp_read_mac(random_mac, ESP_MAC_WIFI_STA);

        // Randomize MAC
        for (int i = 3; i < 6; i++) { // Keep OUI, randomize rest
            random_mac[i] = esp_random() % 256;
        }

        esp_wifi_set_mac(WIFI_IF_STA, random_mac);
        if(Serial) Serial.println("MAC spoofed");
        // tft.println("MAC spoofed..."); // Reduced verbosity on screen
    }

    void disable_serial_debug() {
        // Disable serial output after boot if stealth is enabled
        if(stealth_mode) {
             Serial.end();
             // tft.println("Serial debug disabled");
        }
    }

public:
    void initialize_stealth() {
        // tft.println("Initializing stealth mode...");
        
        // Only enable if configured (logic to come from settings later)
        // For now, we just prepare it but don't force it unless enabled
        stealth_mode = false; 

        configure_deep_sleep();
        
        // If we wanted to force it:
        // disable_serial_debug();
        
        // Initial MAC spoof is good practice anyway for privacy, but maybe not strictly "stealth mode" dependent?
        // Let's do it if we are in a 'privacy' mindset.
        spoof_mac_address();

        // tft.println("Stealth prepared");
    }

    void enter_deep_sleep() {
        if (!stealth_mode) return;

        tft.println("Entering deep sleep (90% uptime)...");
        tft.println("Wake on button GPIO 0 or timer");

        delay(1000);
        esp_deep_sleep_start();
    }

    void on_attack_start() {
        if(stealth_mode) return; // In stealth, we don't blink? Or we do minimal?
        blink_led_code(0x1);
    }

    void periodic_mac_spoof() {
        static unsigned long last_spoof = 0;
        if (millis() - last_spoof > 5 * 60 * 1000) { // Every 5 minutes
            spoof_mac_address();
            last_spoof = millis();
        }
    }

    void check_wake_reason() {
        esp_sleep_wakeup_cause_t wake_reason = esp_sleep_get_wakeup_cause();

        switch (wake_reason) {
            case ESP_SLEEP_WAKEUP_EXT0:
                // tft.println("Woke up from GPIO 0 button");
                on_attack_start();
                break;
            case ESP_SLEEP_WAKEUP_TIMER:
                // tft.println("Woke up from timer");
                // Brief activity, then back to sleep
                if(stealth_mode) {
                    delay(10000); // 10 seconds of activity
                    enter_deep_sleep();
                }
                break;
            default:
                // tft.println("Normal boot");
                break;
        }
    }
    
    void set_mode(bool enabled) {
        stealth_mode = enabled;
        if(stealth_mode) disable_serial_debug();
        else Serial.begin(115200);
    }
};

// Global stealth manager
StealthManager stealth;

// Global functions
void init_stealth() {
    stealth.initialize_stealth();
}

void enter_stealth_sleep() {
    stealth.enter_deep_sleep();
}

void stealth_attack_start() {
    stealth.on_attack_start();
}

void stealth_periodic_tasks() {
    stealth.periodic_mac_spoof();
}

void check_stealth_wake() {
    stealth.check_wake_reason();
}
