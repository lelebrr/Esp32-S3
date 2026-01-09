/**
 * @file web_dashboard.cpp
 * @brief Complete Web Dashboard HTTP Server
 *
 * Features:
 * - Hidden WiFi AP with authentication
 * - REST API for all attack types
 * - Status endpoints
 * - Real-time attack control
 * - System information
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#include "web_dashboard.h"
#include "web_static/chameleon_html.h" // Frontend
#include "web_static/ir_remote_html.h" // IR Frontend


#include "chameleon_anim.h" // Backend Logic
#include "attacks_manager.h"
#include "ble_attacks.h"
#include "core/aggressive_sd.h"
#include "gps_driver.h"
#include "ir_protocols.h"
#include "led_driver.h"
#include "module_manager.h"
#include "nfc_driver.h"
#include "nfc_relay.h"
#include "pin_config.h"
#include "pmkid_capture.h"
#include "rf_core.h"
#include "usb_driver.h"
#include "wps_attacks.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
#define AP_SSID "MorphNode"
#define AP_PASSWORD "morphnode"
#define AP_HIDDEN true
#define WEB_USER "admin"
#define WEB_PASS "lele2025"

AsyncWebServer server(80);

// ============================================================================
// HELPER: Check authentication
// ============================================================================
bool checkAuth(AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS)) {
        request->requestAuthentication();
        return false;
    }
    return true;
}

// ============================================================================
// HELPER: Send JSON response
// ============================================================================
void sendJson(AsyncWebServerRequest *request, JsonDocument &doc, int code = 200) {
    String output;
    serializeJson(doc, output);
    request->send(code, "application/json", output);
}

// ============================================================================
// API: System Status
// ============================================================================
void handleStatus(AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;

    JsonDocument doc;
    doc["device"] = "MorphNode";
    doc["version"] = "2.0";
    doc["uptime"] = millis() / 1000;
    doc["heap"] = ESP.getFreeHeap();
    doc["psram"] = ESP.getFreePsram();

    // Attack status
    doc["attack"]["running"] = attacks_is_running();
    doc["attack"]["current"] = attacks_get_name(attacks_get_current());

    // GPS status
    doc["gps"]["available"] = GPSDriver::isAvailable();
    doc["gps"]["valid"] = GPSDriver::isValid();
    if (GPSDriver::isValid()) {
        doc["gps"]["lat"] = GPSDriver::getLatitude();
        doc["gps"]["lon"] = GPSDriver::getLongitude();
        doc["gps"]["sats"] = GPSDriver::getSatellites();
    }

    // SD status
    doc["sd"]["ready"] = AggressiveSD::isReady();
    if (AggressiveSD::isReady()) {
        doc["sd"]["size_mb"] = AggressiveSD::getCardSize() / (1024 * 1024);
        doc["sd"]["free_mb"] = AggressiveSD::getFreeSpace() / (1024 * 1024);
    }

    // NFC status
    doc["nfc"]["ready"] = NFCDriver::isReady();

    // RF status
    doc["rf"]["initialized"] = RFCore::isInitialized();

    sendJson(request, doc);
}

// ============================================================================
// API: List all attacks
// ============================================================================
void handleAttacksList(AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;

    JsonDocument doc;
    JsonArray attacks = doc["attacks"].to<JsonArray>();

    // BLE Attacks
    JsonObject ble = attacks.add<JsonObject>();
    ble["category"] = "BLE";
    JsonArray bleList = ble["items"].to<JsonArray>();
    bleList.add("ble_spam");
    bleList.add("ble_spam_brazil");
    bleList.add("sour_apple");
    bleList.add("swift_pair");
    bleList.add("fast_pair");
    bleList.add("samsung_buds");

    // WiFi Attacks
    JsonObject wifi = attacks.add<JsonObject>();
    wifi["category"] = "WiFi";
    JsonArray wifiList = wifi["items"].to<JsonArray>();
    wifiList.add("deauth");
    wifiList.add("beacon_spam");
    wifiList.add("evil_twin");
    wifiList.add("pmkid");
    wifiList.add("pmkid_capture");
    wifiList.add("wps_pixie");
    wifiList.add("wps_brute");
    wifiList.add("wardrive");

    // RF Attacks
    JsonObject rf = attacks.add<JsonObject>();
    rf["category"] = "RF SubGHz";
    JsonArray rfList = rf["items"].to<JsonArray>();
    rfList.add("jammer_433");
    rfList.add("jammer_315");
    rfList.add("jammer_868");
    rfList.add("jammer_burst");
    rfList.add("jammer_smart");
    rfList.add("capture");
    rfList.add("replay");
    rfList.add("ghost_replay");
    rfList.add("brute_force");
    rfList.add("spectrum");

    // NFC Attacks
    JsonObject nfc = attacks.add<JsonObject>();
    nfc["category"] = "NFC";
    JsonArray nfcList = nfc["items"].to<JsonArray>();
    nfcList.add("clone");
    nfcList.add("fault");
    nfcList.add("phishing");
    nfcList.add("relay_reader");
    nfcList.add("relay_emulator");

    // IR Attacks
    JsonObject ir = attacks.add<JsonObject>();
    ir["category"] = "IR";
    JsonArray irList = ir["items"].to<JsonArray>();
    irList.add("brute");
    irList.add("tv_bgone");
    irList.add("clone");
    irList.add("brute_gate");

    // USB Attacks
    JsonObject usb = attacks.add<JsonObject>();
    usb["category"] = "USB";
    JsonArray usbList = usb["items"].to<JsonArray>();
    usbList.add("badusb");
    usbList.add("exfil");

    sendJson(request, doc);
}

// ============================================================================
// API: Start Attack
// ============================================================================
void handleAttackStart(AsyncWebServerRequest *request, const String &type) {
    if (!checkAuth(request)) return;

    AttackType attack = ATTACK_NONE;
    bool specialHandler = false;

    // BLE
    if (type == "ble_spam") attack = ATTACK_BLE_SPAM;
    else if (type == "ble_spam_brazil") {
        BLEAttacks::start(BLE_ATTACK_SPAM_BRAZIL);
        specialHandler = true;
    } else if (type == "sour_apple") attack = ATTACK_BLE_SOUR_APPLE;
    else if (type == "swift_pair") attack = ATTACK_BLE_SWIFT_PAIR;
    else if (type == "fast_pair") attack = ATTACK_BLE_FAST_PAIR;
    else if (type == "samsung_buds") {
        BLEAttacks::start(BLE_ATTACK_SAMSUNG_BUDS);
        specialHandler = true;
    }

    // WiFi
    else if (type == "deauth")
        attack = ATTACK_WIFI_DEAUTH;
    else if (type == "beacon_spam") attack = ATTACK_WIFI_BEACON_SPAM;
    else if (type == "evil_twin") attack = ATTACK_WIFI_EVIL_TWIN;
    else if (type == "pmkid") attack = ATTACK_WIFI_PMKID;
    else if (type == "pmkid_capture") {
        PMKIDCapture::start();
        specialHandler = true;
    } else if (type == "wps_pixie") {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) { WPSAttacks::startPixieDust(WPSAttacks::getNetwork(0)); }
        specialHandler = true;
    } else if (type == "wps_brute") {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) { WPSAttacks::startBruteForce(WPSAttacks::getNetwork(0)); }
        specialHandler = true;
    } else if (type == "wardrive") attack = ATTACK_WIFI_WARDRIVE;

    // RF
    else if (type == "jammer_433") attack = ATTACK_RF_JAMMER_433;
    else if (type == "jammer_315") attack = ATTACK_RF_JAMMER_315;
    else if (type == "jammer_868") attack = ATTACK_RF_JAMMER_868;
    else if (type == "jammer_burst") attack = ATTACK_RF_JAMMER_BURST;
    else if (type == "jammer_smart") attack = ATTACK_RF_JAMMER_SMART;
    else if (type == "capture") attack = ATTACK_RF_CAPTURE;
    else if (type == "replay") attack = ATTACK_RF_REPLAY;
    else if (type == "ghost_replay") attack = ATTACK_RF_GHOST_REPLAY;
    else if (type == "brute_force") attack = ATTACK_RF_BRUTE_FORCE;
    else if (type == "spectrum") attack = ATTACK_RF_SPECTRUM;

    // NFC
    else if (type == "nfc_clone") attack = ATTACK_NFC_CLONE;
    else if (type == "nfc_fault") attack = ATTACK_NFC_FAULT;
    else if (type == "nfc_phishing") attack = ATTACK_NFC_PHISHING;
    else if (type == "relay_reader") {
        NFCRelay::init();
        NFCRelay::setReaderMode();
        specialHandler = true;
    } else if (type == "relay_emulator") {
        NFCRelay::init();
        NFCRelay::setEmulatorMode();
        specialHandler = true;
    }

    // IR
    else if (type == "ir_brute")
        attack = ATTACK_IR_BRUTE;
    else if (type == "tv_off" || type == "tv_bgone") {
        IRProtocols::tvBGone();
        specialHandler = true;
    } else if (type == "ir_clone") attack = ATTACK_IR_CLONE;
    else if (type == "brute_gate") {
        IRProtocols::bruteGate12bit();
        specialHandler = true;
    }

    // USB
    else if (type == "badusb")
        attack = ATTACK_USB_BADUSB;
    else if (type == "usb_exfil") attack = ATTACK_USB_EXFIL;

    JsonDocument doc;

    if (specialHandler) {
        LEDDriver::blinkSuccess();
        doc["status"] = "started";
        doc["attack"] = type;
        doc["handler"] = "direct";
        sendJson(request, doc);
    } else if (attack != ATTACK_NONE) {
        attacks_start(attack);
        LEDDriver::blinkSuccess();
        doc["status"] = "started";
        doc["attack"] = attacks_get_name(attack);
        sendJson(request, doc);
    } else {
        LEDDriver::blinkError();
        doc["status"] = "error";
        doc["message"] = "Unknown attack type: " + type;
        sendJson(request, doc, 400);
    }
}

// ============================================================================
// API: Send specific IR Code
// ============================================================================
void handleIRSend(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!checkAuth(request)) return;

    JsonDocument doc;
    deserializeJson(doc, (const char*)data);

    String protoName = doc["protocol"];
    uint64_t code = doc["code"];
    int bits = doc["bits"];

    IRProtocolType proto = IR_PROTO_NEC;
    if (protoName == "NEC") proto = IR_PROTO_NEC;
    else if (protoName == "SAMSUNG") proto = IR_PROTO_SAMSUNG;
    else if (protoName == "SONY") proto = IR_PROTO_SONY;
    else if (protoName == "RC5") proto = IR_PROTO_RC5;
    else if (protoName == "RC6") proto = IR_PROTO_RC6;
    else if (protoName == "LG") proto = IR_PROTO_LG;
    else if (protoName == "PANASONIC") proto = IR_PROTO_PANASONIC;

    IRProtocols::send(proto, code, bits);

    JsonDocument resp;
    resp["status"] = "sent";
    resp["protocol"] = protoName;
    resp["code"] = code;
    sendJson(request, resp);
}

// ============================================================================
// API: Stop Attack
// ============================================================================
void handleAttackStop(AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;

    attacks_stop();

    JsonDocument doc;
    doc["status"] = "stopped";
    sendJson(request, doc);
}

// ============================================================================
// API: USB Payload Execution
// ============================================================================
void handleUSBPayload(AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;

    JsonDocument doc;

    if (request->hasParam("payload", true)) {
        String payload = request->getParam("payload", true)->value();
        USBDriver::executePayload(payload.c_str());
        doc["status"] = "executed";
        doc["length"] = payload.length();
    } else if (request->hasParam("file", true)) {
        String filename = request->getParam("file", true)->value();
        if (USBDriver::executePayloadFromFile(filename.c_str())) {
            doc["status"] = "executed";
            doc["file"] = filename;
        } else {
            doc["status"] = "error";
            doc["message"] = "File not found";
        }
    } else {
        doc["status"] = "error";
        doc["message"] = "No payload or file specified";
    }

    sendJson(request, doc);
}

// ============================================================================
// API: NFC Read
// ============================================================================
void handleNFCRead(AsyncWebServerRequest *request) {
    if (!checkAuth(request)) return;

    JsonDocument doc;

    NFCCard card = NFCDriver::readCard(2000);

    if (card.valid) {
        doc["status"] = "success";
        doc["uid"] = NFCDriver::uidToString(card.uid, card.uidLength);
        doc["type"] = NFCDriver::getTypeName(card.type);
        doc["uidLength"] = card.uidLength;
    } else {
        doc["status"] = "no_card";
        doc["message"] = "No card detected";
    }

    sendJson(request, doc);
}

// ============================================================================
// HTML Dashboard Page
// ============================================================================
#include "web_static/dashboard_html.h"

// ============================================================================
// SETUP WEB DASHBOARD
// ============================================================================
void setup_web_dashboard() {
    // Configure hidden AP
    WiFi.softAP(AP_SSID, AP_PASSWORD, 1, AP_HIDDEN, 4);
    Serial.printf("[WEB] AP Started: %s (Hidden: %s)\n", AP_SSID, AP_HIDDEN ? "Yes" : "No");
    Serial.printf("[WEB] IP: %s\n", WiFi.softAPIP().toString().c_str());

    // Main dashboard page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        request->send(200, "text/html", DASHBOARD_HTML);
    });

    // Status API
    server.on("/api/status", HTTP_GET, handleStatus);

    // List attacks
    server.on("/api/attacks", HTTP_GET, handleAttacksList);

    // Stop attack
    server.on("/api/stop", HTTP_GET, handleAttackStop);

    // NFC read
    server.on("/api/nfc/read", HTTP_GET, handleNFCRead);

    // USB payload (POST)
    server.on("/api/usb/payload", HTTP_POST, handleUSBPayload);

    // Dynamic attack routes
    server.on("^\\/api\\/attack\\/([a-z_]+)$", HTTP_GET, [](AsyncWebServerRequest *request) {
        String attackType = request->pathArg(0);
        handleAttackStart(request, attackType);
    });

    // Start server
    // ========================================================================
    // CHAMELEON STUDIO API
    // ========================================================================
    
    // API: WiFi Scan
    server.on("/api/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        
        int n = WiFi.scanNetworks();
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        
        for (int i = 0; i < n; ++i) {
            JsonObject net = array.add<JsonObject>();
            net["ssid"] = WiFi.SSID(i);
            net["rssi"] = WiFi.RSSI(i);
            net["ch"] = WiFi.channel(i);
            net["enc"] = WiFi.encryptionType(i);
        }
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: File System (SD)
    server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        
        File root = SD.open("/");
        if(!root){
            request->send(500, "application/json", "{\"error\":\"SD Error\"}");
            return;
        }

        File file = root.openNextFile();
        while(file){
            JsonObject f = array.add<JsonObject>();
            f["name"] = String(file.name());
            f["size"] = file.size();
            f["dir"] = file.isDirectory();
            file = root.openNextFile();
        }
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // API: LED Test
    server.on("/api/led_test", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        // Blink internal LED
        pinMode(2, OUTPUT);
        for(int i=0; i<5; i++) {
            digitalWrite(2, HIGH); delay(50);
            digitalWrite(2, LOW); delay(50);
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // Serve HTML
    server.on("/chameleon", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        request->send_P(200, "text/html", CHAMELEON_HTML);
    });

    server.on("/ir", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        request->send_P(200, "text/html", IR_REMOTE_HTML);
    });

    server.on("/api/ir/send", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleIRSend);

    // GET Config
    server.on("/api/chameleon", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!checkAuth(request)) return;
        JsonDocument doc;
        ChameleonConfig cfg = ChameleonAnim::getConfig();
        doc["idle"] = cfg.color_idle;
        doc["attack"] = cfg.color_attack;
        doc["success"] = cfg.color_success;
        doc["error"] = cfg.color_error;
        doc["sneaky"] = cfg.color_sneaky;
        doc["eating"] = cfg.color_eating;
        doc["party"] = cfg.color_party;
        doc["rainbow"] = cfg.rainbow_party;
        doc["speed"] = cfg.anim_speed;
        sendJson(request, doc);
    });

    // POST Config (Update)
    server.on("/api/chameleon", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!checkAuth(request)) return;
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data);
        if (!error) {
            ChameleonConfig cfg = ChameleonAnim::getConfig();
            if(doc.containsKey("idle")) cfg.color_idle = doc["idle"];
            if(doc.containsKey("attack")) cfg.color_attack = doc["attack"];
            if(doc.containsKey("success")) cfg.color_success = doc["success"];
            if(doc.containsKey("eating")) cfg.color_eating = doc["eating"];
            if(doc.containsKey("sneaky")) cfg.color_sneaky = doc["sneaky"];
            if(doc.containsKey("party")) cfg.color_party = doc["party"];
            if(doc.containsKey("rainbow")) cfg.rainbow_party = doc["rainbow"];
            if(doc.containsKey("speed")) cfg.anim_speed = doc["speed"];
            
            ChameleonAnim::setConfig(cfg);
            ChameleonAnim::saveConfig(); // Persist
            
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\"}");
        }
    });

    server.begin();
    Serial.println("[WEB] Dashboard Server Started");
    Serial.println("[WEB] Connect to WiFi 'MorphNode' password 'lele2025'");
    Serial.println("[WEB] Open http://192.168.4.1 in browser");
}
