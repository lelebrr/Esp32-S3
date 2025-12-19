/**
 * @file iot_attacks.h
 * @brief Módulo de Ataques para Dispositivos IoT - Lele Origin
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef IOT_ATTACKS_H
#define IOT_ATTACKS_H

#include <Arduino.h>
#include <vector>

// ============================================================================
// ENUMS E CONSTANTES IOT
// ============================================================================

/**
 * @brief Tipos de dispositivos IoT suportados
 */
enum IoTDeviceType {
    IOT_SMART_PLUG = 0,    ///< Smart Plug (TP-Link HS100)
    IOT_IP_CAMERA,         ///< IP Camera (Xiaomi/EZVIZ)
    IOT_THERMOSTAT,        ///< Thermostat (Nest/Wayta)
    IOT_SMART_SPEAKER,     ///< Smart Speaker (Alexa/Google)
    IOT_DOORBELL,          ///< Smart Doorbell (Ring)
    IOT_BLE_BEACON,        ///< Bluetooth Beacon
    IOT_SMART_BULB,        ///< Smart Bulb (Philips Hue)
    IOT_SMART_LOCK         ///< Smart Lock
};

/**
 * @brief Estados dos ataques IoT
 */
enum IoTAttackState {
    IOT_IDLE = 0,
    IOT_SCANNING,
    IOT_EXPLOITING,
    IOT_PAYLOAD_INJECTING,
    IOT_SUCCESS,
    IOT_FAILED
};

/**
 * @brief Métodos de ataque IoT
 */
enum IoTAttackMethod {
    METHOD_EVIL_TWIN = 0,    ///< WiFi evil twin
    METHOD_RF_JAM,          ///< RF jamming
    METHOD_BLE_REPLAY,      ///< BLE replay
    METHOD_IR_CONTROL,      ///< IR commands
    METHOD_NFC_SPOOF,       ///< NFC spoofing
    METHOD_UART_BYPASS,     ///< UART bypass
    METHOD_SUBGHZ_REPLAY,   ///< SubGHz replay
    METHOD_BLE_FLOOD        ///< BLE flooding
};

/**
 * @brief Estrutura de dispositivo IoT detectado
 */
struct DetectedIoTDevice {
    IoTDeviceType type;
    String model;
    String ipAddress;
    String macAddress;
    String firmware;
    bool isVulnerable;
    String exploitAvailable;
    uint32_t lastSeen;
    int signalStrength;
    String capabilities;     // JSON-like capabilities
};

// ============================================================================
// CLASSES PRINCIPAIS
// ============================================================================

/**
 * @brief Scanner de dispositivos IoT na rede
 */
class IoTScanner {
private:
    std::vector<DetectedIoTDevice> devices;
    bool scanning;
    unsigned long scanStartTime;

public:
    IoTScanner() : scanning(false), scanStartTime(0) {}

    /**
     * @brief Inicia scan de dispositivos IoT
     */
    bool startScan(int timeout = 120);

    /**
     * @brief Para o scan
     */
    void stopScan();

    /**
     * @brief UPnP device discovery
     */
    void scanUPnP();

    /**
     * @brief mDNS device discovery
     */
    void scanMDNS();

    /**
     * @brief ARP scanning for IoT devices
     */
    void scanARP();

    /**
     * @brief Obtém lista de dispositivos detectados
     */
    std::vector<DetectedIoTDevice>& getDevices() { return devices; }
    unsigned long getScanStartTime() const { return scanStartTime; }
};

/**
 * @brief Executor de ataques Smart Plug
 */
class SmartPlugAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    SmartPlugAttacker() : attacking(false) {}

    /**
     * @brief Evil Twin WiFi attack
     */
    bool evilTwinAttack();

    /**
     * @brief MITM + OTA firmware injection
     */
    bool mitmOTAFirmware();

    /**
     * @brief Add backdoor via firmware
     */
    bool addBackdoor();

    /**
     * @brief Remote control via shell
     */
    bool remoteControl();
};

/**
 * @brief Executor de ataques IP Camera
 */
class IPCameraAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    IPCameraAttacker() : attacking(false) {}

    /**
     * @brief RF Jam 2.4GHz + BLE replay
     */
    bool rfJamBLEReplay();

    /**
     * @brief Token theft via handshake replay
     */
    bool tokenTheft();

    /**
     * @brief Local video recording
     */
    bool recordVideoLocal();

    /**
     * @brief Stream access via stolen credentials
     */
    bool accessStream();
};

/**
 * @brief Executor de ataques Thermostat
 */
class ThermostatAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    ThermostatAttacker() : attacking(false) {}

    /**
     * @brief IR + NFC combo attack
     */
    bool irNFCombo();

    /**
     * @brief Factory password reset
     */
    bool factoryPasswordReset();

    /**
     * @brief Webhook injection
     */
    bool injectWebhook();

    /**
     * @brief GPS-based automation
     */
    bool gpsAutomation();
};

/**
 * @brief Executor de ataques Smart Speaker
 */
class SmartSpeakerAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    SmartSpeakerAttacker() : attacking(false) {}

    /**
     * @brief UART bypass hardware attack
     */
    bool uartBypass();

    /**
     * @brief Firmware flash with backdoor
     */
    bool flashBackdoorFirmware();

    /**
     * @brief Custom skill injection
     */
    bool injectCustomSkill();

    /**
     * @brief WiFi password extraction
     */
    bool extractWiFiPassword();
};

/**
 * @brief Executor de ataques Smart Doorbell
 */
class DoorbellAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    DoorbellAttacker() : attacking(false) {}

    /**
     * @brief SubGHz replay attack
     */
    bool subGHzReplay();

    /**
     * @brief WiFi video sniffing
     */
    bool wifiVideoSniff();

    /**
     * @brief Battery drain via replay
     */
    bool batteryDrain();

    /**
     * @brief Video stream interception
     */
    bool interceptVideo();
};

/**
 * @brief Executor de ataques BLE Beacon
 */
class BLEBeaconAttacker {
private:
    DetectedIoTDevice target;
    bool attacking;

public:
    BLEBeaconAttacker() : attacking(false) {}

    /**
     * @brief BLE flood with fake beacons
     */
    bool bleFlood();

    /**
     * @brief Phishing beacon creation
     */
    bool createPhishingBeacon();

    /**
     * @brief Coupon/offer spam
     */
    bool spamOffers();

    /**
     * @brief Credential theft via beacon interaction
     */
    bool stealCredentials();
};

/**
 * @brief Script automatizado de ataque sequencial
 */
class AutomatedIoTScript {
private:
    std::vector<IoTDeviceType> targetSequence;
    bool running;
    int currentStep;

public:
    AutomatedIoTScript() : running(false), currentStep(0) {}

    /**
     * @brief Adiciona dispositivo na sequência
     */
    void addToSequence(IoTDeviceType device);

    /**
     * @brief Executa script automatizado
     */
    bool runAutomatedScript();

    /**
     * @brief Para execução
     */
    void stopScript();

    /**
     * @brief Verifica progresso
     */
    int getProgress() const;
};

/**
 * @brief Gerenciador principal de ataques IoT
 */
class IoTAttackManager {
private:
    IoTScanner scanner;
    SmartPlugAttacker plugAttacker;
    IPCameraAttacker cameraAttacker;
    ThermostatAttacker thermoAttacker;
    SmartSpeakerAttacker speakerAttacker;
    DoorbellAttacker doorbellAttacker;
    BLEBeaconAttacker beaconAttacker;
    AutomatedIoTScript autoScript;

    IoTAttackState currentState;
    IoTDeviceType currentDevice;
    DetectedIoTDevice currentTarget;

    // Estatísticas
    int totalScanned;
    int vulnerableFound;
    int attacksSuccessful;
    int attacksFailed;

public:
    IoTAttackManager() :
        currentState(IOT_IDLE),
        currentDevice(IOT_SMART_PLUG),
        totalScanned(0), vulnerableFound(0),
        attacksSuccessful(0), attacksFailed(0) {}

    // Wrappers for Scanner
    bool startScan(int timeout = 120) { return scanner.startScan(timeout); }
    void stopScan() { scanner.stopScan(); }
    std::vector<DetectedIoTDevice>& getDevices() { return scanner.getDevices(); }

    // ============================================================================
    // MÉTODOS PÚBLICOS
    // ============================================================================

    /**
     * @brief Inicializa o gerenciador IoT
     */
    void begin();

    /**
     * @brief Loop principal
     */
    void loop();

    /**
     * @brief Seleciona alvo IoT
     */
    bool selectTarget(const DetectedIoTDevice& device);

    /**
     * @brief Inicia ataque Smart Plug
     */
    bool startSmartPlugAttack(IoTAttackMethod method = METHOD_EVIL_TWIN);

    /**
     * @brief Inicia ataque IP Camera
     */
    bool startIPCameraAttack(IoTAttackMethod method = METHOD_RF_JAM);

    /**
     * @brief Inicia ataque Thermostat
     */
    bool startThermostatAttack(IoTAttackMethod method = METHOD_IR_CONTROL);

    /**
     * @brief Inicia ataque Smart Speaker
     */
    bool startSmartSpeakerAttack(IoTAttackMethod method = METHOD_UART_BYPASS);

    /**
     * @brief Inicia ataque Doorbell
     */
    bool startDoorbellAttack(IoTAttackMethod method = METHOD_SUBGHZ_REPLAY);

    /**
     * @brief Inicia ataque BLE Beacon
     */
    bool startBLEBeaconAttack(IoTAttackMethod method = METHOD_BLE_FLOOD);

    /**
     * @brief Executa script automatizado
     */
    bool runAutomatedScript();

    /**
     * @brief Para ataque atual
     */
    void stopAttack();

    /**
     * @brief Obtém estado atual
     */
    IoTAttackState getState() const { return currentState; }

    /**
     * @brief Obtém alvo atual
     */
    DetectedIoTDevice getCurrentTarget() const { return currentTarget; }

    /**
     * @brief Obtém estatísticas
     */
    String getStatistics() const;

    /**
     * @brief Lista dispositivos suportados
     */
    String getSupportedDevices();

    // ============================================================================
    // CALLBACKS E EVENTOS
    // ============================================================================

    void (*onDeviceFound)(const DetectedIoTDevice& device) = nullptr;
    void (*onAttackSuccess)(const DetectedIoTDevice& device) = nullptr;
    void (*onAttackFailed)(const DetectedIoTDevice& device) = nullptr;

private:
    void updateState(IoTAttackState newState);
    void logOperation(const String& operation);
};

// ============================================================================
// FUNÇÕES GLOBAIS DE UTILITÁRIO
// ============================================================================

/**
 * @brief Detecta tipo de dispositivo IoT por características
 */
IoTDeviceType detectIoTDevice(const String& model, const String& capabilities);

/**
 * @brief Verifica vulnerabilidade conhecida
 */
bool isIoTVulnerable(IoTDeviceType type, const String& firmware);

/**
 * @brief Gera payload Evil Twin para Smart Plug
 */
String generateSmartPlugPayload();

/**
 * @brief Calcula timing para IR commands
 */
unsigned long calculateIRTiming(IoTDeviceType type);

/**
 * @brief Envia comando IR
 */
bool sendIRCommand(uint32_t command, int frequency = 38000);

/**
 * @brief Gera sinal RF para dispositivos IoT
 */
bool generateIoTRFSignal(uint8_t* data, size_t len, IoTDeviceType type);

/**
 * @brief UART bypass utilities
 */
bool uartBypassDevice(const String& device, uint32_t baud = 115200);

String getIoTDeviceModel(IoTDeviceType type);
String generateRandomFirmware(IoTDeviceType type);
String getExploitForIoTDevice(IoTDeviceType type, const String& firmware);
String getDeviceCapabilities(IoTDeviceType type);
String iot_generateRandomMAC();

// ============================================================================
// INSTÂNCIA GLOBAL
// ============================================================================

extern IoTAttackManager iotAttackManager;

#endif // IOT_ATTACKS_H
