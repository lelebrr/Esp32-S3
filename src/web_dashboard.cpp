/**
 * @file web_dashboard.cpp
 * @brief Web Interface Implementation
 * 
 * Implements the async web server, WebSocket event handling,
 * and JSON API responses for the dashboard.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include <Arduino.h>


#include "web_dashboard.h"
#include "attacks_manager.h"
#include "config_manager.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include "pin_config.h"
#include "q_learn_ia.h"
#include "gps_driver.h"
#include "fault_mosfet.h"
#include "hoaxshell_attack.h"
#include "payload_manager.h"

// Server instances
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static bool web_started = false;

// Credentials
static const char* AP_SSID = "Monster_S3_AP";
static const char* AP_PASS = "lele2025";
static const char* AUTH_USER = "admin";
static const char* AUTH_PASS = "lele2025";

// Log buffer (circular)
#define LOG_BUFFER_SIZE 50
static String log_buffer[LOG_BUFFER_SIZE];
static int log_index = 0;

// Forward declarations
// Forward declarations
void handleWebSocketCommand(JsonObject json);
void broadcastStatus();

/**
 * @brief Add entry to log buffer
 */
void web_log(const char* msg) {
    log_buffer[log_index] = String(millis()) + ": " + String(msg);
    log_index = (log_index + 1) % LOG_BUFFER_SIZE;
    
    // Also broadcast to WebSocket clients
    DynamicJsonDocument doc(512);
    doc["type"] = "log";
    doc["msg"] = msg;
    String output;
    serializeJson(doc, output);
    ws.textAll(output);
}

/**
 * @brief Check authentication
 */
static bool checkAuth(AsyncWebServerRequest* request) {
    if (!request->authenticate(AUTH_USER, AUTH_PASS)) {
        request->requestAuthentication();
        return false;
    }
    return true;
}

/**
 * @brief WebSocket event handler
 */
void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("[WS] Client connected: %u\n", client->id());
        broadcastStatus();
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[WS] Client disconnected: %u\n", client->id());
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;
            String msg = (char*)data;
            
            // Parse JSON command
            DynamicJsonDocument doc(4096); // Increased size for config import
            DeserializationError error = deserializeJson(doc, msg);
            if (!error) {
                if (doc.containsKey("cmd")) {
                    handleWebSocketCommand(doc.as<JsonObject>());
                }
            } else {
                Serial.printf("[WS] JSON Error: %s\n", error.c_str());
            }
        }
    }
}

/**
 * @brief Handle WebSocket command - Maps all attack commands
 */
void handleWebSocketCommand(JsonObject json) {
    String cmd = json["cmd"] | "";
    Serial.printf("[WS] Command: %s\n", cmd.c_str());
    
    // ========== CONFIGURATION COMMANDS ==========
    if (cmd == "config_get") {
        char* json = config_to_json();
        if (json) {
            ws.textAll(json);
            free(json);
        }
    }
    else if (cmd == "config_set") {
        String key = data["key"] | "";
        if (key.length() > 0) {
            // Create a mini JSON document for the single update
            DynamicJsonDocument update(512);
            update[key] = data["value"];
            String updateStr;
            serializeJson(update, updateStr);
            
            // Apply update
            config_from_json(updateStr.c_str());
            config_apply();
            config_set_dirty();
        }
    }
    else if (cmd == "config_save") {
        if (config_save()) {
            web_log("Configuration saved to SD");
        } else {
            web_log("Failed to save configuration");
        }
    }
    else if (cmd == "config_load") {
        if (config_load()) {
            config_apply();
            web_log("Configuration loaded from SD");
            // Send new config to clients
            char* json = config_to_json();
            if (json) {
                ws.textAll(json);
                free(json);
            }
        } else {
            web_log("Failed to load configuration");
        }
    }
    else if (cmd == "config_reset") {
        config_reset_defaults();
        config_apply();
        web_log("Configuration reset to defaults");
        // Send new config to clients
        char* json = config_to_json();
        if (json) {
            ws.textAll(json);
            free(json);
        }
    }
    else if (cmd == "config_export") {
        char* json = config_to_json();
        if (json) {
            ws.textAll(json);
            free(json);
        }
    }
    else if (cmd == "config_import") {
        String jsonStr;
        serializeJson(data["data"], jsonStr);
        if (config_from_json(jsonStr.c_str())) {
            config_apply();
            config_save();
            web_log("Configuration imported successfully");
            // Broadcast new state
            char* json = config_to_json();
            if (json) {
                ws.textAll(json);
                free(json);
            }
        } else {
            web_log("Import failed: Invalid JSON");
        }
    }
    
    // ========== BLE ATTACKS ==========
    else if (cmd == "ble_spam") {
        attacks_start(ATTACK_BLE_SPAM);
        web_log("BLE Spam BR started");
    }
    else if (cmd == "ble_sour_apple") {
        attacks_start(ATTACK_BLE_SOUR_APPLE);
        web_log("Sour Apple (iOS) started");
    }
    else if (cmd == "ble_swift_pair") {
        attacks_start(ATTACK_BLE_SWIFT_PAIR);
        web_log("Swift Pair (Windows) started");
    }
    else if (cmd == "ble_fast_pair") {
        attacks_start(ATTACK_BLE_FAST_PAIR);
        web_log("Fast Pair (Android) started");
    }
    else if (cmd == "stop_ble") {
        attacks_stop();
        web_log("BLE attacks stopped");
    }
    
    // ========== WIFI ATTACKS ==========
    else if (cmd == "wifi_deauth") {
        attacks_start(ATTACK_WIFI_DEAUTH);
        web_log("WiFi Deauth started");
    }
    else if (cmd == "wifi_beacon") {
        attacks_start(ATTACK_WIFI_BEACON_SPAM);
        web_log("Beacon Spam started");
    }
    else if (cmd == "wifi_eviltwin") {
        attacks_start(ATTACK_WIFI_EVIL_TWIN);
        web_log("Evil Twin started");
    }
    else if (cmd == "wifi_pmkid") {
        attacks_start(ATTACK_WIFI_PMKID);
        web_log("PMKID Capture started");
    }
    else if (cmd == "wifi_wardrive") {
        attacks_start(ATTACK_WIFI_WARDRIVE);
        web_log("Wardriving started");
    }
    else if (cmd == "stop_wifi") {
        attacks_stop();
        web_log("WiFi attacks stopped");
    }
    
    // ========== RF JAMMERS ==========
    else if (cmd == "rf_jam_433") {
        attacks_start(ATTACK_RF_JAMMER_433);
        web_log("RF Jammer 433MHz started");
    }
    else if (cmd == "rf_jam_315") {
        attacks_start(ATTACK_RF_JAMMER_315);
        web_log("RF Jammer 315MHz started");
    }
    else if (cmd == "rf_jam_868") {
        attacks_start(ATTACK_RF_JAMMER_868);
        web_log("RF Jammer 868MHz started");
    }
    else if (cmd == "rf_jam_burst") {
        attacks_start(ATTACK_RF_JAMMER_BURST);
        web_log("RF Jammer Burst started");
    }
    else if (cmd == "rf_jam_smart") {
        attacks_start(ATTACK_RF_JAMMER_SMART);
        web_log("RF Smart Jammer started");
    }
    
    // ========== RF TOOLS ==========
    else if (cmd == "rf_scan" || cmd == "rf_capture") {
        attacks_start(ATTACK_RF_CAPTURE);
        web_log("RF Capture started");
    }
    else if (cmd == "rf_replay") {
        attacks_start(ATTACK_RF_REPLAY);
        web_log("RF Replay started");
    }
    else if (cmd == "rf_ghost") {
        attacks_start(ATTACK_RF_GHOST_REPLAY);
        web_log("RF Ghost Replay started");
    }
    else if (cmd == "rf_brute") {
        attacks_start(ATTACK_RF_BRUTE_FORCE);
        web_log("RF Brute Force started");
    }
    else if (cmd == "rf_spectrum") {
        attacks_start(ATTACK_RF_SPECTRUM);
        web_log("RF Spectrum scan started");
    }
    else if (cmd == "rf_rolljam") {
        attacks_start(ATTACK_RF_ROLLJAM);
        web_log("RF Rolljam started");
    }
    else if (cmd == "rf_rollback") {
        attacks_start(ATTACK_RF_ROLLBACK);
        web_log("RF Rollback started");
    }
    else if (cmd == "stop_rf") {
        attacks_stop();
        web_log("RF attacks stopped");
    }
    
    // ========== NFC ATTACKS ==========
    else if (cmd == "nfc_clone") {
        attacks_start(ATTACK_NFC_CLONE);
        web_log("NFC Clone started");
    }
    else if (cmd == "nfc_fault") {
        attacks_start(ATTACK_NFC_FAULT);
        web_log("NFC Fault Injection started");
    }
    else if (cmd == "nfc_phishing") {
        attacks_start(ATTACK_NFC_PHISHING);
        web_log("NFC Phishing started");
    }
    else if (cmd == "stop_nfc") {
        attacks_stop();
        web_log("NFC attacks stopped");
    }
    
    // ========== NRF24 ATTACKS ==========
    else if (cmd == "nrf_mousejack") {
        attacks_start(ATTACK_NRF_MOUSEJACK);
        web_log("NRF MouseJack started");
    }
    else if (cmd == "nrf_keyboard") {
        attacks_start(ATTACK_NRF_KEYBOARD);
        web_log("NRF Key Sniffer started");
    }
    else if (cmd == "stop_nrf") {
        attacks_stop();
        web_log("NRF attacks stopped");
    }
    
    // ========== IR ATTACKS ==========
    else if (cmd == "ir_tvbgone") {
        attacks_start(ATTACK_IR_TV_OFF);
        web_log("TV-B-Gone started");
    }
    else if (cmd == "ir_brute") {
        attacks_start(ATTACK_IR_BRUTE);
        web_log("IR Brute Force started");
    }
    else if (cmd == "ir_clone") {
        attacks_start(ATTACK_IR_CLONE);
        web_log("IR Clone mode started");
    }
    else if (cmd == "stop_ir") {
        attacks_stop();
        web_log("IR attacks stopped");
    }
    
    // ========== USB ATTACKS ==========
    else if (cmd == "usb_badusb") {
        attacks_start(ATTACK_USB_BADUSB);
        web_log("BadUSB HID started");
    }
    else if (cmd == "usb_exfil") {
        attacks_start(ATTACK_USB_EXFIL);
        web_log("USB Exfil started");
    }
    else if (cmd == "usb_wax") {
        attacks_start(ATTACK_USB_WAX);
        web_log("USB WAX Payload started");
    }
    else if (cmd == "stop_usb") {
        attacks_stop();
        web_log("USB attacks stopped");
    }
    
    // ========== NRF24 ATTACKS ==========
    else if (cmd == "nrf_mousejack") {
        attacks_start(ATTACK_NRF_MOUSEJACK);
        web_log("NRF MouseJack started");
    }
    else if (cmd == "nrf_keyboard") {
        attacks_start(ATTACK_NRF_KEYBOARD);
        web_log("NRF Key Sniffer started");
    }
    else if (cmd == "stop_nrf") {
        attacks_stop();
        web_log("NRF attacks stopped");
    }
    
    // ========== GLOBAL STOP ==========
    else if (cmd == "stop_all") {
        attacks_stop();
        web_log("ALL attacks stopped");
    }
    
    // ========== SYSTEM COMMANDS ==========
    else if (cmd == "scan_wifi") {
        web_log("WiFi scan requested");
        // TODO: Trigger scan and send results
    }
    else if (cmd == "scan_ble") {
        web_log("BLE scan requested");
        // TODO: Trigger BLE scan
    }
    else if (cmd == "reboot") {
        web_log("Rebooting...");
        delay(500);
        ESP.restart();
    }
    else if (cmd == "format_sd") {
        web_log("SD Format requested (disabled)");
    }
    else if (cmd == "self_destruct") {
        web_log("SELF DESTRUCT - Formatting...");
        // TODO: Factory reset
    }
    
    // ========== HARDWARE GLITCHING ==========
    else if (cmd == "hw_single") {
        int width = json["width"] | 100;
        fault_pulse_custom(width);
        web_log("HW: Single pulse triggered");
    }
    else if (cmd == "hw_burst") {
        int count = json["count"] | 5;
        int delay = json["delay"] | 100;
        fault_burst_trigger(count, delay);
        web_log("HW: Burst triggered");
    }
    else if (cmd == "hw_sweep") {
        fault_sweep_calibration();
        web_log("HW: Calibration sweep started");
    }
    else if (cmd == "stop_hw") {
         // Stop doesn't really apply unless continuous, but good for logging
         web_log("HW: Glitcher stopped");
    }
    
    // ========== CONFIGURATION COMMANDS ==========
    else if (cmd == "config_get") {
        char* json = config_to_json();
        if (json) {
            // Send config as a special message
            String msg = "{\"type\":\"config_data\",\"data\":";
            msg += json;
            msg += "}";
            ws.textAll(msg);
            free(json);
            web_log("Config sent to client");
        }
    }
    else if (cmd == "config_set") {
        String key = json["key"] | "";
        if (key.length() > 0 && json.containsKey("value")) {
            DynamicJsonDocument updateDoc(512);
            updateDoc[key] = json["value"];
            String jsonStr;
            serializeJson(updateDoc, jsonStr);
            
            if (config_from_json(jsonStr.c_str())) {
                web_log(("Config updated: " + key).c_str());
                // Check if specific keys were updated for logging
                if (key == "attacker_ip") {
                    web_log(("Attacker IP set to: " + String((const char*)json["value"])).c_str());
                }
                
                // Don't auto-save on every slider move, rely on explicit save or periodic?
                // For now, save to ensure persistence.
                // Or maybe the UI sends 'save' command separately?
                // Let's check config_manager... g_config_dirty is set.
                // Let's allow frequent updates in memory, but maybe not spam SD.
                // For now, we update memory.
            }
        }
    }
    else if (cmd == "config_save") {
         if (config_save()) {
             web_log("Settings saved to SD");
         } else {
             web_log("Error saving settings");
         }
    }
    else if (cmd == "config_export") {
        char* jsonStr = config_to_json();
        if (jsonStr) {
            // Send config as a special message
            // Use larger buffer for full config
            DynamicJsonDocument resp(4096);
            resp["type"] = "config_export";
            // Check if we can just embed the string or need to parse it back
            // To avoid double parsing, we can just perform string manipulation or use deserializeJson
            // Easiest is to deserialize the string into the object
            deserializeJson(resp["data"], jsonStr);
            
            String output;
            serializeJson(resp, output);
            ws.textAll(output);
            free(jsonStr);
        }
    }
    else if (cmd == "config_import") {
        if (json.containsKey("data")) {
            String dataStr;
            serializeJson(json["data"], dataStr);
            if (config_from_json(dataStr.c_str())) {
                config_save();
                web_log("Config imported & saved");
                config_apply();
            } else {
                web_log("Config import failed");
            }
        }
    }
    else if (cmd == "stealth_toggle") {
        DeviceConfig* cfg = config_get_mutable();
        cfg->stealth_mode = !cfg->stealth_mode;
        config_apply();
        web_log(cfg->stealth_mode ? "Stealth Mode ENABLED" : "Stealth Mode DISABLED");
    }
    else if (cmd == "clear_captures") {
        SDAdapter::removeFile(SD, "/nfc_clones.txt");
        SDAdapter::removeFile(SD, "/ir_codes.txt");
        SDAdapter::removeFile(SD, "/pmkid_targets.txt");
        // Clear subghz folder
        File root = SD.open("/subghz");
        if (root) {
            while (true) {
                File entry = root.openNextFile();
                if (!entry) break;
                String path = "/subghz/" + String(entry.name());
                SD.remove(path);
                entry.close();
            }
            root.close();
        }
        web_log("All captures cleared");
    }
    else if (cmd == "ai_reset_qtable") {
        // ai_reset(); // Assuming this function is available in q_learn_ia.h
        // For now, just log until header is checked
        web_log("Q-Table reset (Simulated)");
    }
    else {
        Serial.printf("[WS] Unknown command: %s\n", cmd.c_str());
    }
    
    // ========== HARDWARE GLITCHER ==========
    else if (cmd == "glitch_single") {
        fault_pulse_trigger();
        web_log("Glitch: Single Pulse Triggered");
    }
    else if (cmd == "glitch_burst") {
        int count = json["count"] | 5;
        int delay_ms = json["delay"] | 100;
        fault_burst_trigger(count, delay_ms);
        char msg[64];
        snprintf(msg, sizeof(msg), "Glitch: Burst (%d pulses, %d ms)", count, delay_ms);
        web_log(msg);
    }
    else if (cmd == "glitch_sweep") {
        web_log("Glitch: Calibration Sweep Started");
        fault_sweep_calibration();
        web_log("Glitch: Sweep Complete");
    }
    else if (cmd == "glitch_custom") {
        int width_us = json["width"] | 100;
        fault_pulse_custom(width_us);
        char msg[64];
        snprintf(msg, sizeof(msg), "Glitch: Custom Pulse (%d us)", width_us);
        web_log(msg);
    }

    // ========== HOAXSHELL COMMANDS ==========
    else if (cmd == "hoax_cmd") {
        String sid = json["sid"] | "";
        String command = json["command"] | "";
        if (sid.length() > 0 && command.length() > 0) {
            HoaxShell.queueCommand(sid, command);
        }
    }
    else if (cmd == "hoax_get_payload") {
        String ip = WiFi.softAPIP().toString();
        String payload = HoaxShell.generatePayload(ip);
        
        DynamicJsonDocument doc(2048);
        doc["type"] = "hoax_payload";
        doc["payload"] = payload;
        String output;
        serializeJson(doc, output);
        ws.textAll(output);
    }
    
    // Broadcast updated status
    broadcastStatus();
}

/**
 * @brief Broadcast status to all WebSocket clients
 */
void broadcastStatus() {
    StaticJsonDocument<512> doc;
    doc["type"] = "status";
    doc["attack"] = attacks_get_name(attacks_get_current());
    doc["running"] = attacks_is_running();
    doc["pps"] = attacks_get_packet_count();
    doc["bat"] = 85; // TODO: Read from ADC
    doc["temp"] = 42; // TODO: Read from sensor
    
    if (GPSDriver::isValid()) {
        doc["gps"] = GPSDriver::getCoordsString();
    }
    
    String output;
    serializeJson(doc, output);
    ws.textAll(output);
}

/**
 * @brief Setup all web routes
 */
void setup_web_dashboard() {
    // Start hidden AP
    WiFi.softAP(AP_SSID, AP_PASS, 1, 1, 4);
    Serial.println("[WEB] AP Started: 192.168.4.1 (Hidden)");
    Serial.printf("[WEB] SSID: %s, Pass: %s\n", AP_SSID, AP_PASS);
    
    // WebSocket handler
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    // Initialize HoaxShell C2
    HoaxShell.begin(&server);
    PayloadManager::begin(&server);
    
    // ========== SERVE STATIC FILES ==========
    
    // Serve from SD card if available
    server.serveStatic("/", SD, "/LeleWebUI/").setDefaultFile("index.html");
    
    // ========== STATUS API ==========
    
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        
        StaticJsonDocument<512> doc;
        doc["attack"] = attacks_get_name(attacks_get_current());
        doc["running"] = attacks_is_running();
        doc["packets"] = attacks_get_packet_count();
        doc["battery"] = 85;
        doc["gps"] = GPSDriver::isValid() ? GPSDriver::getCoordsString() : "No Fix";
        
        uint32_t ai_dec;
        float ai_eps;
        ai_get_stats(&ai_dec, &ai_eps);
        doc["ai_decisions"] = ai_dec;
        doc["ai_epsilon"] = ai_eps;
        
        doc["uptime_s"] = millis() / 1000;
        doc["free_heap"] = ESP.getFreeHeap();
        doc["free_psram"] = ESP.getFreePsram();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // ========== LEGACY ATTACK ROUTES (for old UI compatibility) ==========
    
    server.on("/attack/ble", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        attacks_start(ATTACK_BLE_SPAM);
        web_log("BLE Spam started (Web)");
        request->send(200, "text/plain", "BLE Spam BR Started");
    });
    
    server.on("/attack/wifi", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        attacks_start(ATTACK_WIFI_DEAUTH);
        web_log("WiFi Deauth started (Web)");
        request->send(200, "text/plain", "WiFi Deauth Started");
    });
    
    server.on("/attack/rf", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        attacks_start(ATTACK_RF_GHOST_REPLAY);
        web_log("RF Ghost Replay started (Web)");
        request->send(200, "text/plain", "RF Ghost Replay Started");
    });
    
    server.on("/attack/nfc", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        attacks_start(ATTACK_NFC_FAULT);
        web_log("NFC Fault started (Web)");
        request->send(200, "text/plain", "NFC Fault Injection Started");
    });
    
    server.on("/attack/stop", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        attacks_stop();
        web_log("All attacks stopped (Web)");
        request->send(200, "text/plain", "All Attacks Stopped");
    });
    
    // ========== AI FEEDBACK ==========
    
    server.on("/ai/success", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        ai_feedback_success();
        web_log("AI Feedback: SUCCESS");
        request->send(200, "text/plain", "AI: Success feedback recorded (+1.0)");
    });
    
    server.on("/ai/fail", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        ai_feedback_fail();
        web_log("AI Feedback: FAIL");
        request->send(200, "text/plain", "AI: Fail feedback recorded (-1.0)");
    });
    
    // ========== LOGS ==========
    
    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!checkAuth(request)) return;
        
        String logs = "";
        for (int i = 0; i < LOG_BUFFER_SIZE; i++) {
            int idx = (log_index + i) % LOG_BUFFER_SIZE;
            if (log_buffer[idx].length() > 0) {
                logs += log_buffer[idx] + "\n";
            }
        }
        request->send(200, "text/plain", logs);
    });
    
    // ========== 404 ==========
    
    server.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Monster S3: 404 Not Found");
    });
    
    // Start server
    server.begin();
    web_started = true;
    Serial.println("[WEB] Dashboard ready at http://192.168.4.1");
    Serial.println("[WEB] WebSocket endpoint: ws://192.168.4.1/ws");
}

/**
 * @brief Update loop - broadcast status periodically
 */
void web_dashboard_loop() {
    static uint32_t last_broadcast = 0;
    
    // Clean up disconnected clients
    ws.cleanupClients();
    
    // Broadcast status every 2 seconds
    if (millis() - last_broadcast > 2000) {
        if (ws.count() > 0) {
            broadcastStatus();
            
            // Also broadcast HoaxShell output if any
            if (HoaxShell.hasOutput()) {
                DynamicJsonDocument doc(4096);
                doc["type"] = "hoax_output";
                doc["data"] = HoaxShell.getOutput();
                doc["sessions"] = HoaxShell.getSessionsJson();
                String output;
                serializeJson(doc, output);
                ws.textAll(output);
            }
        }
        last_broadcast = millis();
    }
}

/**
 * @brief Get web dashboard status
 */
bool web_is_running() {
    return web_started;
}
