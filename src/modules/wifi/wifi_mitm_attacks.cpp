#include "wifi_mitm_attacks.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/optimization_manager.h"

static bool optimizedTimer(unsigned long *lastUpdate, unsigned long interval) {
    if (millis() - *lastUpdate >= interval) {
        *lastUpdate = millis();
        return true;
    }
    return false;
}

// WiFi MITM and Portal Attacks

// MITM with dnsspoof + ettercap simulation
void wifi_mitm_dnsspoof() {
    drawMainBorderWithTitle("MITM DNS Spoof");
    padprintln("");
    padprintln("Intercepting DNS queries");
    padprintln("Redirecting to fake sites");
    padprintln("");
    padprintln("Requires: ettercap + dnsspoof");
    padprintln("Run on external device");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// HTTPS Downgrade with sslstrip2/BetterCAP5
void wifi_https_downgrade() {
    drawMainBorderWithTitle("HTTPS Downgrade");
    padprintln("");
    padprintln("Stripping SSL/TLS");
    padprintln("Downgrading to HTTP");
    padprintln("");
    padprintln("Capturing credentials");
    padprintln("from bank sites, etc.");
    padprintln("");
    padprintln("Requires: BetterCAP5");
    padprintln("Run on external device");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Captive portal survey phishing
void wifi_captive_portal_survey() {
    // Notify optimization manager that WiFi is active
    optimizationManager.updateComponentState(COMPONENT_WIFI, true);

    // Get optimized buffer for AP name
    uint8_t* buffer = optimizationManager.getOptimizedBuffer();
    if (!buffer) {
        displayError("Memory allocation failed", true);
        return;
    }

    // Create fake captive portal AP with optimized naming
    const char* baseName = "McFreeWiFi";
    snprintf((char*)buffer, optimizationManager.getBufferSize(), "%s_%d", baseName, esp_random() % 1000);

    WiFi.softAP((char*)buffer, "", 1);

    drawMainBorderWithTitle("Fake Captive Portal");
    padprintln("");
    padprintln("AP: " + String((char*)buffer));
    padprintln("Survey: Win coffee!");
    padprintln("Steals CPF + phone");
    padprintln("");
    padprintln("Clients connected: " + String(WiFi.softAPgetStationNum()));
    padprintln("");
    padprintln("Press Any key to STOP.");

    unsigned long lastUpdate = millis();
    while (!check(AnyKeyPress)) {
        // Use optimized timer instead of delay
        if (optimizedTimer(&lastUpdate, 1000)) {
            padprintln("Clients: " + String(WiFi.softAPgetStationNum()));
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Reduced delay
    }

    WiFi.softAPdisconnect(true);

    // Release optimized buffer
    optimizationManager.releaseOptimizedBuffer(buffer);

    // Notify optimization manager that WiFi is inactive
    optimizationManager.updateComponentState(COMPONENT_WIFI, false);
}

// Beacon spam battery drain
void wifi_beacon_spam() {
    drawMainBorderWithTitle("Beacon Spam");
    padprintln("");
    padprintln("Spamming fake networks:");
    padprintln("FreeNetflix");
    padprintln("GratisDisney+");
    padprintln("");
    padprintln("Drains victim battery");
    padprintln("Forces device replacement");
    padprintln("Captures new device data");
    padprintln("");
    padprintln("Press Any key to STOP.");

    // Would need to implement beacon frame sending
    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Fake Spotify OAuth portal
void wifi_spotify_oauth_portal() {
    // Create AP with Spotify login portal
    String apName = "SpotifyFree";
    WiFi.softAP(apName.c_str(), "", 6);

    // Simple web server for OAuth phishing
    // Would serve fake Spotify login that captures credentials

    drawMainBorderWithTitle("Spotify OAuth Portal");
    padprintln("");
    padprintln("AP: " + apName);
    padprintln("Fake Spotify login");
    padprintln("Steals OAuth tokens");
    padprintln("");
    padprintln("Clients: " + String(WiFi.softAPgetStationNum()));
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        padprintln("Clients: " + String(WiFi.softAPgetStationNum()));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    WiFi.softAPdisconnect(true);
}
