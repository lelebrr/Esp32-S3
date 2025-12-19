#include "web_dashboard.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"

#include "attacks_manager.h"

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Monster S3 Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: courier, sans-serif; background-color: #000; color: #0f0; text-align: center; }
    h1 { color: #f00; }
    .card { background-color: #111; border: 1px solid #0f0; padding: 20px; margin: 10px; }
    .btn { background-color: #333; color: #fff; padding: 15px 32px; margin: 5px; border: 1px solid #f00; cursor: pointer; width: 100%; }
    .btn:hover { background-color: #f00; color: #000; }
    .btn-red { border-color: #f00; background-color: #500; }
  </style>
</head>
<body>
  <h1>MONSTER S3 CONTROL</h1>
  <div class="card">
    <h2>ATAQUES</h2>
    <button class="btn" onclick="attack('ble')">BLE SPAM</button>
    <button class="btn" onclick="attack('wifi')">WIFI DEAUTH</button>
    <button class="btn" onclick="attack('nfc')">NFC FAULT</button>
    <button class="btn" onclick="attack('subghz')">SUBGHZ REPLAY</button>
    <button class="btn" onclick="attack('ir')">IR BRUTE</button>
    <br><br>
    <button class="btn btn-red" onclick="attack('stop')">PARAR TUDO</button>
  </div>
  <div class="card">
    <h2>STATUS</h2>
    <p>Bateria: <span id="bat">98%</span></p>
    <p>Log: <span id="log">Aguardando...</span></p>
  </div>
<script>
function attack(type) {
  fetch('/api/attack?type=' + type)
    .then(response => response.text())
    .then(data => document.getElementById('log').innerText = data);
}
</script>
</body>
</html>
)rawliteral";

void setup_web_dashboard() {
    WiFi.softAP("lele2025");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    server.on("/api/attack", HTTP_GET, [](AsyncWebServerRequest *request){
        String type = "";
        if (request->hasParam("type")) {
            type = request->getParam("type")->value();
            Serial.println("Web Attack CMD: " + type);
            
            if (type == "ble") attacks_start(ATTACK_BLE_SPAM);
            else if (type == "wifi") attacks_start(ATTACK_WIFI_DEAUTH);
            else if (type == "nfc") attacks_start(ATTACK_NFC_FAULT);
            else if (type == "subghz") attacks_start(ATTACK_SUBGHZ_REPLAY);
            else if (type == "ir") attacks_start(ATTACK_IR_BRUTE);
            else if (type == "stop") attacks_stop();
        }
        request->send(200, "text/plain", "Comando Enviado: " + type);
    });

    server.begin();
    Serial.println("Web Dashboard Initialized");
}
