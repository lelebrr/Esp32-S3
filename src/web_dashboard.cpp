#include <Arduino.h>
#include "web_dashboard.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "pin_config.h"
#include "attacks_manager.h"

AsyncWebServer server(80);

void setup_web_dashboard() {
    // Configura AP Stealth (ssid, pass, channel, hidden, max_conn)
    WiFi.softAP("Monster_S3_AP", "lele2025", 1, 1, 4);
    
    Serial.println("[WEB] AP Started: 192.168.4.1 (Hidden)");

    // Rota Principal
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate("admin", "lele2025")) // Basic Auth
            return request->requestAuthentication();
            
        request->send(200, "text/plain", "Monster S3 Dashboard\nStatus: Online\nBattery: 98%\n\nCommands:\n/attack/ble\n/attack/wifi\n/stop");
    });

    // API de Ataques
    server.on("/attack/ble", HTTP_GET, [](AsyncWebServerRequest *request){
        attacks_start(ATTACK_BLE_SPAM);
        request->send(200, "text/plain", "BLE Spam Started");
    });

    server.on("/attack/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
        attacks_start(ATTACK_WIFI_DEAUTH);
        request->send(200, "text/plain", "WiFi Deauth Started");
    });

    server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
        attacks_stop();
        request->send(200, "text/plain", "All Attacks Stopped");
    });

    server.begin();
    Serial.println("[WEB] Server Started");
}
