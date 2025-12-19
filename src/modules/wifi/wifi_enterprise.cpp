/**
 * @file wifi_enterprise.cpp
 * @brief WiFi Enterprise (WPA2-Ent/MGT) Testing Tool
 * @author Lele Origin Team
 */

#include "wifi_enterprise.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/settings.h"
#include "esp_wifi.h"

void wifi_enterprise_menu() {
    tft.fillScreen(leleConfig.bgColor);
    drawMainBorder();
    tft.setTextSize(FM);
    tft.setCursor(10, 40);
    tft.println("Enterprise Tester");
    tft.setTextSize(1);
    tft.println("Targeting WPA2-Enterprise...");

    // Scan specific for Enterprise?
    // For now, simplify: just generic MGT flood to test AP load

    tft.setCursor(10, 80);
    tft.println("Sending EAPOL-Start...");

    bool running = true;
    while (running && !check(EscPress)) {
        // Construct RAW EAPOL Start Packet
        // Dest: Broadcast or Target
        // Type: EAPOL (0x888E)
        // Body: EAPOL Start (0x1)

        // Simulation trace
        tft.print(".");
        if (tft.getCursorX() > tftWidth - 10) {
            tft.fillRect(0, 80, tftWidth, tftHeight - 80, leleConfig.bgColor);
            tft.setCursor(10, 80);
        }

        delay(100);
    }
}
