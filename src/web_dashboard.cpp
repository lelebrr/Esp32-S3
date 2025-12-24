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
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "web_dashboard.h"
#include "attacks_manager.h"
#include "pin_config.h"
#include "core/aggressive_sd.h"
#include "nfc_driver.h"
#include "usb_driver.h"
#include "gps_driver.h"
#include "rf_core.h"
#include "module_manager.h"
#include "led_driver.h"
#include "ble_attacks.h"
#include "ir_protocols.h"
#include "wps_attacks.h"
#include "pmkid_capture.h"
#include "nfc_relay.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
#define AP_SSID     "Monster_S3"
#define AP_PASSWORD "lele2025"
#define AP_HIDDEN   true
#define WEB_USER    "admin"
#define WEB_PASS    "lele2025"

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
void sendJson(AsyncWebServerRequest *request, JsonDocument& doc, int code = 200) {
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
    doc["device"] = "Monster S3";
    doc["version"] = "1.0";
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
void handleAttackStart(AsyncWebServerRequest *request, const String& type) {
    if (!checkAuth(request)) return;
    
    AttackType attack = ATTACK_NONE;
    bool specialHandler = false;
    
    // BLE
    if (type == "ble_spam") attack = ATTACK_BLE_SPAM;
    else if (type == "ble_spam_brazil") {
        BLEAttacks::start(BLE_ATTACK_SPAM_BRAZIL);
        specialHandler = true;
    }
    else if (type == "sour_apple") attack = ATTACK_BLE_SOUR_APPLE;
    else if (type == "swift_pair") attack = ATTACK_BLE_SWIFT_PAIR;
    else if (type == "fast_pair") attack = ATTACK_BLE_FAST_PAIR;
    else if (type == "samsung_buds") {
        BLEAttacks::start(BLE_ATTACK_SAMSUNG_BUDS);
        specialHandler = true;
    }
    
    // WiFi
    else if (type == "deauth") attack = ATTACK_WIFI_DEAUTH;
    else if (type == "beacon_spam") attack = ATTACK_WIFI_BEACON_SPAM;
    else if (type == "evil_twin") attack = ATTACK_WIFI_EVIL_TWIN;
    else if (type == "pmkid") attack = ATTACK_WIFI_PMKID;
    else if (type == "pmkid_capture") {
        PMKIDCapture::start();
        specialHandler = true;
    }
    else if (type == "wps_pixie") {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) {
            WPSAttacks::startPixieDust(WPSAttacks::getNetwork(0));
        }
        specialHandler = true;
    }
    else if (type == "wps_brute") {
        WPSAttacks::scanWPS();
        if (WPSAttacks::getNetwork(0).wpsEnabled) {
            WPSAttacks::startBruteForce(WPSAttacks::getNetwork(0));
        }
        specialHandler = true;
    }
    else if (type == "wardrive") attack = ATTACK_WIFI_WARDRIVE;
    
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
    }
    else if (type == "relay_emulator") {
        NFCRelay::init();
        NFCRelay::setEmulatorMode();
        specialHandler = true;
    }
    
    // IR
    else if (type == "ir_brute") attack = ATTACK_IR_BRUTE;
    else if (type == "tv_off" || type == "tv_bgone") {
        IRProtocols::tvBGone();
        specialHandler = true;
    }
    else if (type == "ir_clone") attack = ATTACK_IR_CLONE;
    else if (type == "brute_gate") {
        IRProtocols::bruteGate12bit();
        specialHandler = true;
    }
    
    // USB
    else if (type == "badusb") attack = ATTACK_USB_BADUSB;
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
const char* DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Monster S3 Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, sans-serif; 
            background: linear-gradient(135deg, #1a1a2e, #16213e);
            color: #eee;
            min-height: 100vh;
            padding: 20px;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 { 
            color: #e94560; 
            text-align: center; 
            margin-bottom: 30px;
            text-shadow: 0 0 20px rgba(233, 69, 96, 0.5);
        }
        .status-bar {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin-bottom: 30px;
        }
        .status-card {
            background: rgba(255,255,255,0.1);
            padding: 15px;
            border-radius: 10px;
            text-align: center;
        }
        .status-card h3 { color: #4ecdc4; font-size: 14px; }
        .status-card p { font-size: 20px; margin-top: 5px; }
        .attack-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
        }
        .attack-category {
            background: rgba(255,255,255,0.05);
            padding: 20px;
            border-radius: 15px;
            border: 1px solid rgba(255,255,255,0.1);
        }
        .attack-category h2 {
            color: #4ecdc4;
            margin-bottom: 15px;
            border-bottom: 1px solid rgba(255,255,255,0.1);
            padding-bottom: 10px;
        }
        button {
            width: 100%;
            padding: 12px;
            margin: 5px 0;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 14px;
            transition: all 0.3s;
        }
        button.attack { background: #0f3460; color: white; }
        button.attack:hover { background: #e94560; transform: scale(1.02); }
        button.stop { background: #e94560; color: white; font-weight: bold; }
        button.stop:hover { background: #ff6b6b; }
        #log {
            margin-top: 20px;
            background: #000;
            padding: 15px;
            border-radius: 10px;
            font-family: monospace;
            font-size: 12px;
            max-height: 200px;
            overflow-y: auto;
            color: #0f0;
        }
        .active { animation: pulse 1s infinite; }
        @keyframes pulse {
            0%, 100% { box-shadow: 0 0 5px #e94560; }
            50% { box-shadow: 0 0 20px #e94560; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔥 MONSTER S3 🔥</h1>
        
        <div class="status-bar">
            <div class="status-card"><h3>HEAP</h3><p id="heap">-</p></div>
            <div class="status-card"><h3>UPTIME</h3><p id="uptime">-</p></div>
            <div class="status-card"><h3>ATTACK</h3><p id="attack">-</p></div>
            <div class="status-card"><h3>GPS</h3><p id="gps">-</p></div>
        </div>
        
        <div class="attack-grid">
            <div class="attack-category">
                <h2>🔵 BLE</h2>
                <button class="attack" onclick="start('ble_spam')">BLE Spam</button>
                <button class="attack" onclick="start('sour_apple')">Sour Apple</button>
                <button class="attack" onclick="start('swift_pair')">Swift Pair</button>
            </div>
            
            <div class="attack-category">
                <h2>📶 WiFi</h2>
                <button class="attack" onclick="start('deauth')">Deauth</button>
                <button class="attack" onclick="start('beacon_spam')">Beacon Spam</button>
                <button class="attack" onclick="start('wardrive')">Wardriving</button>
            </div>
            
            <div class="attack-category">
                <h2>📡 RF SubGHz</h2>
                <button class="attack" onclick="start('jammer_433')">Jammer 433</button>
                <button class="attack" onclick="start('capture')">Capture</button>
                <button class="attack" onclick="start('replay')">Replay</button>
                <button class="attack" onclick="start('ghost_replay')">Ghost Replay</button>
            </div>
            
            <div class="attack-category">
                <h2>💳 NFC</h2>
                <button class="attack" onclick="start('nfc_clone')">Clone Card</button>
                <button class="attack" onclick="start('nfc_fault')">Fault Inject</button>
                <button class="attack" onclick="start('nfc_phishing')">Phishing Tag</button>
            </div>
            
            <div class="attack-category">
                <h2>🔴 IR</h2>
                <button class="attack" onclick="start('tv_off')">TV-B-Gone</button>
                <button class="attack" onclick="start('ir_brute')">IR Brute</button>
                <button class="attack" onclick="start('ir_clone')">IR Clone</button>
            </div>
            
            <div class="attack-category">
                <h2>💾 USB</h2>
                <button class="attack" onclick="start('badusb')">BadUSB</button>
                <button class="attack" onclick="start('usb_exfil')">WiFi Exfil</button>
            </div>
        </div>
        
        <button class="stop" style="margin-top: 20px;" onclick="stop()">🛑 STOP ALL ATTACKS</button>
        
        <div id="log"></div>
    </div>
    
    <script>
        function log(msg) {
            const l = document.getElementById('log');
            l.innerHTML = '[' + new Date().toLocaleTimeString() + '] ' + msg + '<br>' + l.innerHTML;
        }
        
        function start(attack) {
            fetch('/api/attack/' + attack)
                .then(r => r.json())
                .then(d => log('Started: ' + d.attack))
                .catch(e => log('Error: ' + e));
        }
        
        function stop() {
            fetch('/api/stop')
                .then(r => r.json())
                .then(d => log('All attacks stopped'))
                .catch(e => log('Error: ' + e));
        }
        
        function updateStatus() {
            fetch('/api/status')
                .then(r => r.json())
                .then(d => {
                    document.getElementById('heap').textContent = Math.round(d.heap/1024) + 'KB';
                    document.getElementById('uptime').textContent = Math.round(d.uptime/60) + 'm';
                    document.getElementById('attack').textContent = d.attack.current;
                    document.getElementById('gps').textContent = d.gps.valid ? '✓' : '✗';
                });
        }
        
        setInterval(updateStatus, 2000);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

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
    server.begin();
    Serial.println("[WEB] Dashboard Server Started");
    Serial.println("[WEB] Connect to WiFi 'Monster_S3' password 'lele2025'");
    Serial.println("[WEB] Open http://192.168.4.1 in browser");
}
