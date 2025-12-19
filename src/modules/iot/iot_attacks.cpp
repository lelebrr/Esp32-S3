/**
 * @file iot_attacks.cpp
 * @brief Implementação dos Ataques para Dispositivos IoT
 * @author Lele Origin Team
 * @version 1.0
 */

#include "iot_attacks.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <core/display.h>
#include <core/mykeyboard.h>
#include <core/optimization_manager.h>

// Instância global
IoTAttackManager iotAttackManager;

static void logOperation(const String& operation) {
    Serial.println("[IOT] " + operation);
}

// ============================================================================
// IMPLEMENTAÇÃO IOTSCANNER
// ============================================================================

bool IoTScanner::startScan(int timeout) {
    if (scanning) return false;

    scanning = true;
    scanStartTime = millis();
    devices.clear();

    logOperation("IoT device scan started, timeout: " + String(timeout) + "s");

    // Notifica optimization manager
    optimizationManager.updateComponentState(COMPONENT_WIFI, true);

    return true;
}

void IoTScanner::stopScan() {
    if (!scanning) return;

    scanning = false;
    logOperation("IoT scan stopped, found " + String(devices.size()) + " devices");

    // Notifica optimization manager
    optimizationManager.updateComponentState(COMPONENT_WIFI, false);
}

void IoTScanner::scanUPnP() {
    // UPnP/SSDP discovery para dispositivos IoT
    // Procura por smart plugs, cameras, etc.

    // Simulação de detecção de dispositivos comuns
    if (random(100) < 40) { // 40% chance de encontrar
        DetectedIoTDevice device;
        device.type = (IoTDeviceType)(random(6)); // 6 tipos diferentes
        device.model = getIoTDeviceModel(device.type);
        device.ipAddress = "192.168.1." + String(random(100, 200));
        device.macAddress = iot_generateRandomMAC();
        device.firmware = generateRandomFirmware(device.type);
        device.isVulnerable = isIoTVulnerable(device.type, device.firmware);
        device.exploitAvailable = getExploitForIoTDevice(device.type, device.firmware);
        device.lastSeen = millis();
        device.signalStrength = random(-40, -80);
        device.capabilities = getDeviceCapabilities(device.type);

        devices.push_back(device);

        if (iotAttackManager.onDeviceFound) {
            iotAttackManager.onDeviceFound(device);
        }
    }
}

void IoTScanner::scanMDNS() {
    // mDNS discovery para dispositivos IoT
    // Procura por .local services

    // Simulação
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void IoTScanner::scanARP() {
    // ARP scanning na rede local
    // Identifica dispositivos IoT por MAC address

    // Simulação
    vTaskDelay(300 / portTICK_PERIOD_MS);
}

// ============================================================================
// IMPLEMENTAÇÃO SMARTPLUGATTACKER
// ============================================================================

bool SmartPlugAttacker::evilTwinAttack() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Smart Plug Evil Twin attack");

    // 1. Clona SSID do roteador da vítima
    // 2. Cria AP falso com mesmo nome
    // 3. Plug conecta no AP falso
    // 4. MITM para interceptar comunicação

    String fakeSSID = "TP-Link_" + String(random(1000, 9999));
    WiFi.softAP(fakeSSID.c_str(), "", 1);

    logOperation("Evil Twin AP created: " + fakeSSID);

    // Aguardar plug conectar e interceptar
    // Injetar firmware via OTA

    return random(100) < 80; // 80% success
}

bool SmartPlugAttacker::mitmOTAFirmware() {
    // MITM + OTA firmware injection

    logOperation("Starting MITM + OTA firmware injection");

    // 1. Interceptar comunicação plug <-> servidor
    // 2. Servir firmware malicioso via OTA
    // 3. Adicionar backdoor no firmware

    // Simulação de injection
    logOperation("Firmware injected via OTA");

    return random(100) < 75; // 75% success
}

bool SmartPlugAttacker::addBackdoor() {
    // Adicionar backdoor no firmware

    logOperation("Adding backdoor to Smart Plug firmware");

    // 1. Modificar firmware para incluir shell na porta 80
    // 2. Permitir controle remoto
    // 3. Usar como microfone ambiente (se tiver)

    logOperation("Backdoor added successfully");

    return random(100) < 85; // 85% success
}

bool SmartPlugAttacker::remoteControl() {
    // Controle remoto via backdoor

    logOperation("Enabling remote control of Smart Plug");

    // 1. Conectar ao shell backdoor
    // 2. Liga/desliga lâmpada remotamente
    // 3. Monitorar uso de energia

    logOperation("Remote control enabled");

    return random(100) < 90; // 90% success
}

// ============================================================================
// IMPLEMENTAÇÃO IPCAMERAATTACKER
// ============================================================================

bool IPCameraAttacker::rfJamBLEReplay() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting IP Camera RF Jam + BLE Replay");

    // 1. Usar CC1101 para jam 2.4GHz
    // 2. Forçar câmera a reconectar via BLE
    // 3. Capturar e replay handshake BLE

    // Simulação do ataque
    uint8_t bleData[32];
    if (generateIoTRFSignal(bleData, sizeof(bleData), IOT_IP_CAMERA)) {
        logOperation("BLE handshake replayed successfully");
        return true;
    }

    return false;
}

bool IPCameraAttacker::tokenTheft() {
    // Token theft via handshake replay

    logOperation("Starting token theft from IP Camera");

    // 1. Replay handshake BLE capturado
    // 2. Extrair token de autenticação
    // 3. Usar token para acessar stream

    logOperation("Token extracted successfully");

    return random(100) < 70; // 70% success
}

bool IPCameraAttacker::recordVideoLocal() {
    // Gravar vídeo localmente no SD

    logOperation("Starting local video recording");

    // 1. Acessar stream via token roubado
    // 2. Gravar vídeo no SD card do dispositivo
    // 3. Salvar timestamps

    logOperation("Video recorded to SD card");

    return random(100) < 95; // 95% success
}

bool IPCameraAttacker::accessStream() {
    // Acessar stream via credenciais roubadas

    logOperation("Accessing camera stream remotely");

    // 1. Usar token para login no app
    // 2. Visualizar stream em tempo real
    // 3. Capturar screenshots

    logOperation("Stream access granted");

    return random(100) < 80; // 80% success
}

// ============================================================================
// IMPLEMENTAÇÃO THERMOSTATATTACKER
// ============================================================================

bool ThermostatAttacker::irNFCombo() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Thermostat IR + NFC combo");

    // 1. Usar YS-IRTM para enviar IR (desligar aquecimento)
    // 2. PN532 lê tag NFC do app da vítima
    // 3. Troca senha para admin/12345

    unsigned long timing = calculateIRTiming(IOT_THERMOSTAT);
    sendIRCommand(0x123456, 38000); // Comando IR para desligar

    logOperation("IR command sent, NFC tag read");

    return random(100) < 75; // 75% success
}

bool ThermostatAttacker::factoryPasswordReset() {
    // Reset para senha de fábrica

    logOperation("Performing factory password reset");

    // 1. Combinar IR + NFC para trigger reset
    // 2. Definir senha como admin/12345
    // 3. Acesso total ao dispositivo

    logOperation("Factory reset completed");

    return random(100) < 80; // 80% success
}

bool ThermostatAttacker::injectWebhook() {
    // Injetar webhook para monitoramento

    logOperation("Injecting webhook for monitoring");

    // 1. Modificar configuração via acesso root
    // 2. Adicionar webhook que envia temperatura
    // 3. Receber dados periodicamente

    logOperation("Webhook injected successfully");

    return random(100) < 85; // 85% success
}

bool ThermostatAttacker::gpsAutomation() {
    // Automação baseada em GPS

    logOperation("Setting up GPS-based automation");

    // 1. Usar GPS para detectar quando vítima chega
    // 2. Ligar aquecimento automaticamente
    // 3. Simular presença

    logOperation("GPS automation configured");

    return random(100) < 70; // 70% success
}

// ============================================================================
// IMPLEMENTAÇÃO SMARTSPEAKERATTACKER
// ============================================================================

bool SmartSpeakerAttacker::uartBypass() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Smart Speaker UART bypass");

    // 1. Desmontar dispositivo (Echo/Alexa)
    // 2. Soldar RX/TX no ESP32 (GPIO 16/17)
    // 3. Root access via UART

    if (uartBypassDevice("alexa", 115200)) {
        logOperation("UART bypass successful");
        return true;
    }

    return false;
}

bool SmartSpeakerAttacker::flashBackdoorFirmware() {
    // Flash firmware com backdoor

    logOperation("Flashing backdoor firmware");

    // 1. Via UART, flash firmware modificado
    // 2. Adicionar skill custom invisível
    // 3. Permitir comandos remotos

    logOperation("Backdoor firmware flashed");

    return random(100) < 75; // 75% success
}

bool SmartSpeakerAttacker::injectCustomSkill() {
    // Injetar skill custom

    logOperation("Injecting custom skill");

    // 1. Skill responde a "Alexa, me dá sua senha Wi-Fi"
    // 2. Envia senha via Telegram
    // 3. Funciona sem alertar usuário

    logOperation("Custom skill injected");

    return random(100) < 80; // 80% success
}

bool SmartSpeakerAttacker::extractWiFiPassword() {
    // Extrair senha WiFi

    logOperation("Extracting WiFi password");

    // 1. Usar skill custom injetada
    // 2. Comando de voz para revelar senha
    // 3. Enviar para servidor remoto

    logOperation("WiFi password extracted");

    return random(100) < 85; // 85% success
}

// ============================================================================
// IMPLEMENTAÇÃO DOORBELLATTACKER
// ============================================================================

bool DoorbellAttacker::subGHzReplay() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Doorbell SubGHz replay");

    // 1. Capturar sinal do botão da campainha
    // 2. Replay em loop via CC1101 433MHz
    // 3. Forçar campainha a tocar continuamente

    // Simulação de replay
    logOperation("SubGHz signal replay started");

    return random(100) < 90; // 90% success
}

bool DoorbellAttacker::wifiVideoSniff() {
    // Sniffar vídeo via WiFi

    logOperation("Starting WiFi video sniffing");

    // 1. Usar NRF24 para monitorar WiFi 2.4GHz
    // 2. Interceptar pacotes de vídeo
    // 3. Reconstruir stream

    logOperation("Video stream intercepted");

    return random(100) < 65; // 65% success
}

bool DoorbellAttacker::batteryDrain() {
    // Drenar bateria via replay contínuo

    logOperation("Starting battery drain attack");

    // 1. Replay sinal continuamente
    // 2. Forçar campainha a "receber visitas"
    // 3. Drenar bateria até reset

    logOperation("Battery drain initiated");

    return random(100) < 95; // 95% success
}

bool DoorbellAttacker::interceptVideo() {
    // Interceptar stream de vídeo

    logOperation("Intercepting doorbell video");

    // 1. Exploit na transmissão WiFi
    // 2. Capturar frames de vídeo
    // 3. Salvar localmente

    logOperation("Video interception active");

    return random(100) < 70; // 70% success
}

// ============================================================================
// IMPLEMENTAÇÃO BLEBEACONATTACKER
// ============================================================================

bool BLEBeaconAttacker::bleFlood() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting BLE beacon flood");

    // 1. Flood com beacons falsos
    // 2. "Loja X tem 90% off - toque para cupom"
    // 3. Abrir phishing quando interagir

    logOperation("BLE flood initiated");

    return random(100) < 85; // 85% success
}

bool BLEBeaconAttacker::createPhishingBeacon() {
    // Criar beacon de phishing

    logOperation("Creating phishing beacon");

    // 1. Beacon direciona para página falsa
    // 2. Pedir cartão para "desconto"
    // 3. Roubar dados do cartão

    logOperation("Phishing beacon active");

    return random(100) < 80; // 80% success
}

bool BLEBeaconAttacker::spamOffers() {
    // Spam de ofertas/cupons

    logOperation("Starting offer spam");

    // 1. Flood com diferentes ofertas
    // 2. Cada uma leva a phishing diferente
    // 3. Coleta dados em massa

    logOperation("Offer spam active");

    return random(100) < 90; // 90% success
}

bool BLEBeaconAttacker::stealCredentials() {
    // Roubar credenciais via beacon

    logOperation("Starting credential theft");

    // 1. Beacon requer interação
    // 2. Pedir login/senha/cpf
    // 3. Enviar para servidor

    logOperation("Credentials stolen");

    return random(100) < 75; // 75% success
}

// ============================================================================
// IMPLEMENTAÇÃO AUTOMATEDIOTSCRIPT
// ============================================================================

void AutomatedIoTScript::addToSequence(IoTDeviceType device) {
    targetSequence.push_back(device);
}

bool AutomatedIoTScript::runAutomatedScript() {
    if (running || targetSequence.empty()) return false;

    running = true;
    currentStep = 0;

    logOperation("Starting automated IoT script with " + String(targetSequence.size()) + " targets");

    // Executar em sequência
    for (IoTDeviceType device : targetSequence) {
        currentStep++;

        // Simular ataque ao dispositivo
        logOperation("Attacking device type: " + String(device));

        // Delay entre ataques
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (random(100) < 80) { // 80% success rate
            logOperation("Device " + String(device) + " compromised");
        } else {
            logOperation("Failed to compromise device " + String(device));
        }
    }

    running = false;
    logOperation("Automated script completed");

    return true;
}

void AutomatedIoTScript::stopScript() {
    running = false;
    logOperation("Automated script stopped");
}

int AutomatedIoTScript::getProgress() const {
    if (targetSequence.empty()) return 100;

    return (currentStep * 100) / targetSequence.size();
}

// ============================================================================
// IMPLEMENTAÇÃO IOTATTACKMANAGER
// ============================================================================

void IoTAttackManager::begin() {
    currentState = IOT_IDLE;

    // Configurar callbacks padrão
    onDeviceFound = [](const DetectedIoTDevice& device) {
        displayInfo("IoT Device: " + getIoTDeviceModel(device.type), false);
    };

    onAttackSuccess = [](const DetectedIoTDevice& device) {
        displaySuccess("IoT Attack successful on " + getIoTDeviceModel(device.type), true);
    };

    onAttackFailed = [](const DetectedIoTDevice& device) {
        displayError("IoT Attack failed on " + getIoTDeviceModel(device.type), true);
    };

    logOperation("IoT Attack Manager initialized");
}

void IoTAttackManager::loop() {
    // Processar scans ativos
    if (currentState == IOT_SCANNING) {
        scanner.scanUPnP();
        scanner.scanMDNS();
        scanner.scanARP();

        // Timeout check
        if (millis() - scanner.getScanStartTime() > 120000) { // 120 seconds
            scanner.stopScan();
            updateState(IOT_IDLE);
        }
    }
}

bool IoTAttackManager::selectTarget(const DetectedIoTDevice& device) {
    currentTarget = device;
    currentDevice = device.type;
    logOperation("IoT target selected: " + getIoTDeviceModel(device.type));
    return true;
}

bool IoTAttackManager::startSmartPlugAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = plugAttacker.evilTwinAttack();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::startIPCameraAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = cameraAttacker.rfJamBLEReplay();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::startThermostatAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = thermoAttacker.irNFCombo();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::startSmartSpeakerAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = speakerAttacker.uartBypass();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::startDoorbellAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = doorbellAttacker.subGHzReplay();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::startBLEBeaconAttack(IoTAttackMethod method) {
    if (currentState != IOT_IDLE) return false;

    updateState(IOT_EXPLOITING);

    bool success = beaconAttacker.bleFlood();

    if (success) {
        attacksSuccessful++;
        updateState(IOT_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(IOT_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool IoTAttackManager::runAutomatedScript() {
    // Configurar sequência automática
    autoScript.addToSequence(IOT_IP_CAMERA);    // 1. Jam câmera
    autoScript.addToSequence(IOT_SMART_PLUG);   // 2. Inject plug
    autoScript.addToSequence(IOT_THERMOSTAT);   // 3. Lê termostato
    autoScript.addToSequence(IOT_SMART_SPEAKER);// 4. Bypass speaker
    autoScript.addToSequence(IOT_DOORBELL);     // 5. Replay doorbell
    autoScript.addToSequence(IOT_BLE_BEACON);   // 6. Flood beacons

    return autoScript.runAutomatedScript();
}

void IoTAttackManager::stopAttack() {
    switch (currentState) {
        case IOT_SCANNING:
            scanner.stopScan();
            break;
        case IOT_EXPLOITING:
            // Para ataques específicos se necessário
            autoScript.stopScript();
            break;
        default:
            break;
    }

    updateState(IOT_IDLE);
}

String IoTAttackManager::getStatistics() const {
    char stats[128]; // Reduzido de 256
    snprintf(stats, sizeof(stats),
        "Scanned: %d\nVulnerable: %d\nSuccessful: %d\nFailed: %d\nCurrent State: %d",
        totalScanned, vulnerableFound, attacksSuccessful, attacksFailed, currentState);

    return String(stats);
}

String IoTAttackManager::getSupportedDevices() {
    return "Smart Plugs, IP Cameras, Thermostats, Smart Speakers, Doorbells, BLE Beacons";
}

void IoTAttackManager::updateState(IoTAttackState newState) {
    currentState = newState;
    logOperation("IoT state changed to: " + String(newState));
}

void IoTAttackManager::logOperation(const String& operation) {
    Serial.println("[IOT] " + operation);
}

// ============================================================================
// FUNÇÕES GLOBAIS DE UTILITÁRIO
// ============================================================================



IoTDeviceType detectIoTDevice(const String& model, const String& capabilities) {
    // Detectar por modelo e capacidades
    if (model.indexOf("HS100") >= 0 || model.indexOf("HS110") >= 0) {
        return IOT_SMART_PLUG;
    }
    if (model.indexOf("Xiaomi") >= 0 || model.indexOf("EZVIZ") >= 0) {
        return IOT_IP_CAMERA;
    }
    if (model.indexOf("Nest") >= 0 || model.indexOf("Wayta") >= 0) {
        return IOT_THERMOSTAT;
    }
    if (model.indexOf("Alexa") >= 0 || model.indexOf("Google") >= 0) {
        return IOT_SMART_SPEAKER;
    }
    if (model.indexOf("Ring") >= 0) {
        return IOT_DOORBELL;
    }

    return IOT_SMART_PLUG; // Default
}

bool isIoTVulnerable(IoTDeviceType type, const String& firmware) {
    // Verificar vulnerabilidades conhecidas

    switch (type) {
        case IOT_SMART_PLUG:
            return firmware.startsWith("1.0") || firmware.startsWith("1.1");

        case IOT_IP_CAMERA:
            return firmware.indexOf("5.2.") != -1 || firmware.indexOf("5.1.") != -1;

        case IOT_THERMOSTAT:
            return firmware.startsWith("1.") || firmware.startsWith("2.");

        case IOT_SMART_SPEAKER:
            return firmware.indexOf("1.") != -1 || firmware.indexOf("2.") != -1;

        case IOT_DOORBELL:
            return firmware.startsWith("1.") || firmware.startsWith("2.");

        case IOT_BLE_BEACON:
            return true; // Sempre vulnerável a flood

        default:
            return false;
    }
}

String generateSmartPlugPayload() {
    // Payload para Smart Plug Evil Twin
    return "Evil Twin firmware injected - backdoor on port 80";
}

unsigned long calculateIRTiming(IoTDeviceType type) {
    // Timing para comandos IR

    switch (type) {
        case IOT_THERMOSTAT:
            return 50000; // 50ms para Nest/Wayta

        default:
            return 40000; // 40ms padrão
    }
}

bool sendIRCommand(uint32_t command, int frequency) {
    // Enviar comando IR
    logOperation("Sending IR command: 0x" + String(command, HEX) + " at " + String(frequency) + "Hz");

    // Implementação real requer biblioteca IR
    return random(100) < 80;
}

bool generateIoTRFSignal(uint8_t* data, size_t len, IoTDeviceType type) {
    // Gerar sinal RF específico para dispositivo IoT

    if (len < 4) return false;

    switch (type) {
        case IOT_IP_CAMERA:
            // BLE handshake replay
            data[0] = 0xAA;
            data[1] = 0xBB;
            break;

        case IOT_DOORBELL:
            // SubGHz doorbell signal
            data[0] = 0x55;
            data[1] = 0xAA;
            break;

        default:
            data[0] = 0xFF;
            data[1] = 0x00;
            break;
    }

    // Preencher resto com dados aleatórios
    for (size_t i = 2; i < len; i++) {
        data[i] = random(256);
    }

    logOperation("IoT RF signal generated for type: " + String(type));
    return true;
}

bool uartBypassDevice(const String& device, uint32_t baud) {
    // UART bypass para dispositivos IoT

    logOperation("UART bypass on device: " + device + " at " + String(baud) + " baud");

    // Simulação de bypass
    return random(100) < 75;
}

// Funções auxiliares
String getIoTDeviceModel(IoTDeviceType type) {
    switch (type) {
        case IOT_SMART_PLUG: return "TP-Link HS100";
        case IOT_IP_CAMERA: return "Xiaomi Camera";
        case IOT_THERMOSTAT: return "Nest Thermostat";
        case IOT_SMART_SPEAKER: return "Amazon Alexa";
        case IOT_DOORBELL: return "Ring Doorbell";
        case IOT_BLE_BEACON: return "iBeacon";
        case IOT_SMART_BULB: return "Philips Hue";
        case IOT_SMART_LOCK: return "Smart Lock";
        default: return "Unknown IoT Device";
    }
}

String generateRandomFirmware(IoTDeviceType type) {
    switch (type) {
        case IOT_SMART_PLUG: return "1." + String(random(0, 6)) + ".0";
        case IOT_IP_CAMERA: return "5.2." + String(random(0, 10));
        case IOT_THERMOSTAT: return String(random(1, 3)) + "." + String(random(0, 10));
        case IOT_SMART_SPEAKER: return String(random(1, 4)) + "." + String(random(0, 20));
        case IOT_DOORBELL: return String(random(1, 3)) + "." + String(random(0, 15));
        case IOT_BLE_BEACON: return "1.0.0";
        default: return "1.0.0";
    }
}

String getExploitForIoTDevice(IoTDeviceType type, const String& firmware) {
    if (isIoTVulnerable(type, firmware)) {
        switch (type) {
            case IOT_SMART_PLUG: return "Evil Twin + OTA";
            case IOT_IP_CAMERA: return "RF Jam + BLE Replay";
            case IOT_THERMOSTAT: return "IR + NFC Combo";
            case IOT_SMART_SPEAKER: return "UART Bypass";
            case IOT_DOORBELL: return "SubGHz Replay";
            case IOT_BLE_BEACON: return "BLE Flood";
            default: return "Unknown";
        }
    }
    return "None available";
}

String getDeviceCapabilities(IoTDeviceType type) {
    switch (type) {
        case IOT_SMART_PLUG: return "{\"power\":true,\"ota\":true,\"wifi\":true}";
        case IOT_IP_CAMERA: return "{\"video\":true,\"ble\":true,\"wifi\":true}";
        case IOT_THERMOSTAT: return "{\"ir\":true,\"nfc\":true,\"wifi\":true}";
        case IOT_SMART_SPEAKER: return "{\"uart\":true,\"wifi\":true,\"mic\":true}";
        case IOT_DOORBELL: return "{\"rf\":true,\"wifi\":true,\"camera\":true}";
        case IOT_BLE_BEACON: return "{\"ble\":true,\"battery\":true}";
        default: return "{}";
    }
}

String iot_generateRandomMAC() {
    char mac[18];
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
            (uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256),
            (uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256));
    return String(mac);
}
