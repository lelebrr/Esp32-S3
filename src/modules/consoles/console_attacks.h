/**
 * @file console_attacks.h
 * @brief Módulo de Ataques para Consoles Antigos - Lele Origin
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef CONSOLE_ATTACKS_H
#define CONSOLE_ATTACKS_H

#include <Arduino.h>
#include <vector>

// ============================================================================
// ENUMS E CONSTANTES DE CONSOLES
// ============================================================================

/**
 * @brief Tipos de consoles suportados
 */
enum ConsoleType {
    CONSOLE_PS3 = 0,      ///< PlayStation 3
    CONSOLE_XBOX360,      ///< Xbox 360
    CONSOLE_WIIU,         ///< Wii U
    CONSOLE_SWITCH,       ///< Nintendo Switch
    CONSOLE_PS4,          ///< PlayStation 4
    CONSOLE_XBOXONE       ///< Xbox One
};

/**
 * @brief Estados dos ataques de console
 */
enum ConsoleAttackState {
    CONSOLE_IDLE = 0,
    CONSOLE_SCANNING,
    CONSOLE_EXPLOITING,
    CONSOLE_PAYLOAD_INJECTING,
    CONSOLE_SUCCESS,
    CONSOLE_FAILED
};

/**
 * @brief Métodos de ataque para consoles
 */
enum ConsoleAttackMethod {
    METHOD_BADUSB = 0,       ///< BadUSB injection
    METHOD_JTAG,             ///< JTAG exploit
    METHOD_EVIL_TWIN,        ///< WiFi evil twin
    METHOD_GLITCHING,        ///< Hardware glitching
    METHOD_MICROPHONE_BLEED, ///< Audio injection
    METHOD_RF_REPLAY         ///< RF signal replay
};

/**
 * @brief Estrutura de console detectado
 */
struct DetectedConsole {
    ConsoleType type;
    String model;
    String firmware;
    bool isVulnerable;
    String exploitAvailable;
    uint32_t lastSeen;
    int signalStrength;
};

// ============================================================================
// CLASSES PRINCIPAIS
// ============================================================================

/**
 * @brief Scanner de consoles na rede
 */
class ConsoleScanner {
private:
    std::vector<DetectedConsole> consoles;
    bool scanning;
    unsigned long scanStartTime;

public:
    ConsoleScanner() : scanning(false), scanStartTime(0) {}

    /**
     * @brief Inicia scan de consoles
     */
    unsigned long getScanStartTime() const { return scanStartTime; }
    bool startScan(int timeout = 60);

    /**
     * @brief Para o scan
     */
    void stopScan();

    /**
     * @brief Detecta consoles via UPnP/SSDP
     */
    void scanUPnP();

    /**
     * @brief Detecta via broadcast
     */
    void scanBroadcast();

    /**
     * @brief Obtém lista de consoles detectados
     */
    std::vector<DetectedConsole>& getConsoles() { return consoles; }
};

/**
 * @brief Executor de ataques PS3
 */
class PS3Attacker {
private:
    DetectedConsole target;
    bool attacking;

public:
    PS3Attacker() : attacking(false) {}

    /**
     * @brief BadUSB CFW injection
     */
    bool injectCFW();

    /**
     * @brief FTP server exploit
     */
    bool exploitFTP();

    /**
     * @brief Real-time payload injection
     */
    bool injectRealTimePayload();

    /**
     * @brief Install Multiman
     */
    bool installMultiman();
};

/**
 * @brief Executor de ataques Xbox 360
 */
class Xbox360Attacker {
private:
    DetectedConsole target;
    bool attacking;

public:
    Xbox360Attacker() : attacking(false) {}

    /**
     * @brief JTAG exploit via NRF24
     */
    bool jtagViaNRF24();

    /**
     * @brief NAND dump via KingKong
     */
    bool nandDumpKingKong();

    /**
     * @brief Install freeboot
     */
    bool installFreeboot();

    /**
     * @brief Create pirate server
     */
    bool createPirateServer();
};

/**
 * @brief Executor de ataques Wii U
 */
class WiiUAttacker {
private:
    DetectedConsole target;
    bool attacking;

public:
    WiiUAttacker() : attacking(false) {}

    /**
     * @brief Evil Twin Nintendo Update
     */
    bool evilTwinUpdate();

    /**
     * @brief Save injection exploit
     */
    bool injectSaveData();

    /**
     * @brief NAND dump via FTP
     */
    bool nandDumpFTP();

    /**
     * @brief NNID account cloning
     */
    bool cloneNNID();
};

/**
 * @brief Executor de ataques Nintendo Switch
 */
class SwitchAttacker {
private:
    DetectedConsole target;
    bool attacking;

public:
    SwitchAttacker() : attacking(false) {}

    /**
     * @brief IR glitching for RCM entry
     */
    bool irGlitchingRCM();

    /**
     * @brief TegraRcmGUI payload injection
     */
    bool injectTegraRcm();

    /**
     * @brief Atmosphere CFW installation
     */
    bool installAtmosphere();

    /**
     * @brief NAND backup via USB
     */
    bool nandBackupUSB();
};

/**
 * @brief Executor de Microphone Bleed Attacks
 */
class MicrophoneBleedAttacker {
private:
    int frequency;  // Hz
    int duration;   // seconds
    String command;

public:
    MicrophoneBleedAttacker() : frequency(19000), duration(5) {}

    /**
     * @brief Generate ultrasonic audio command
     */
    bool generateAudioCommand(const String& cmd, int freq = 19000);

    /**
     * @brief Transmit via CC1101 433MHz
     */
    bool transmitViaRF();

    /**
     * @brief Transmit via speaker
     */
    bool transmitViaSpeaker();

    /**
     * @brief Voice command injection
     */
    bool injectVoiceCommand(const String& command);
};

/**
 * @brief Gerenciador principal de ataques para consoles
 */
class ConsoleAttackManager {
private:
    ConsoleScanner scanner;
    PS3Attacker ps3Attacker;
    Xbox360Attacker xboxAttacker;
    WiiUAttacker wiiuAttacker;
    SwitchAttacker switchAttacker;
    MicrophoneBleedAttacker micAttacker;

    ConsoleAttackState currentState;
    ConsoleType currentConsole;
    DetectedConsole currentTarget;

    // Estatísticas
    int totalScanned;
    int vulnerableFound;
    int attacksSuccessful;
    int attacksFailed;

public:
    ConsoleAttackManager() :
        currentState(CONSOLE_IDLE),
        currentConsole(CONSOLE_PS3),
        totalScanned(0), vulnerableFound(0),
        attacksSuccessful(0), attacksFailed(0) {}

    // Wrappers for Scanner
    bool startScan(int timeout = 60) { return scanner.startScan(timeout); }
    void stopScan() { scanner.stopScan(); }
    std::vector<DetectedConsole>& getConsoles() { return scanner.getConsoles(); }

    // ============================================================================
    // MÉTODOS PÚBLICOS
    // ============================================================================

    /**
     * @brief Inicializa o gerenciador de consoles
     */
    void begin();

    /**
     * @brief Loop principal
     */
    void loop();

    /**
     * @brief Seleciona console alvo
     */
    bool selectTarget(const DetectedConsole& console);

    /**
     * @brief Inicia ataque PS3
     */
    bool startPS3Attack(ConsoleAttackMethod method = METHOD_BADUSB);

    /**
     * @brief Inicia ataque Xbox 360
     */
    bool startXbox360Attack(ConsoleAttackMethod method = METHOD_JTAG);

    /**
     * @brief Inicia ataque Wii U
     */
    bool startWiiUAttack(ConsoleAttackMethod method = METHOD_EVIL_TWIN);

    /**
     * @brief Inicia ataque Switch
     */
    bool startSwitchAttack(ConsoleAttackMethod method = METHOD_GLITCHING);

    /**
     * @brief Inicia Microphone Bleed attack
     */
    bool startMicBleedAttack(const String& command);

    /**
     * @brief Para ataque atual
     */
    void stopAttack();

    /**
     * @brief Obtém estado atual
     */
    ConsoleAttackState getState() const { return currentState; }

    /**
     * @brief Obtém alvo atual
     */
    DetectedConsole getCurrentTarget() const { return currentTarget; }

    /**
     * @brief Obtém estatísticas
     */
    String getStatistics() const;

    /**
     * @brief Lista consoles suportados
     */
    String getSupportedConsoles();

    // ============================================================================
    // CALLBACKS E EVENTOS
    // ============================================================================

    void (*onConsoleFound)(const DetectedConsole& console) = nullptr;
    void (*onAttackSuccess)(const DetectedConsole& console) = nullptr;
    void (*onAttackFailed)(const DetectedConsole& console) = nullptr;

private:
    void updateState(ConsoleAttackState newState);
    void logOperation(const String& operation);
};

// ============================================================================
// FUNÇÕES GLOBAIS DE UTILITÁRIO
// ============================================================================

/**
 * @brief Detecta tipo de console por MAC address
 */
ConsoleType detectConsoleByMAC(const uint8_t* mac);

/**
 * @brief Verifica vulnerabilidade conhecida
 */
bool isConsoleVulnerable(ConsoleType type, const String& firmware);

/**
 * @brief Gera payload BadUSB para PS3
 */
String generatePS3BadUSBPayload();

/**
 * @brief Calcula timing para glitching
 */
unsigned long calculateGlitchTiming(ConsoleType type);

/**
 * @brief Envia comando IR
 */
bool sendIRCommand(uint32_t command, int frequency = 38000);

/**
 * @brief Gera sinal RF para Xbox 360
 */
bool generateXbox360RFSignal(uint8_t* data, size_t len);

/**
 * @brief Obtém nome do modelo do console
 */
String getConsoleModelName(ConsoleType type);

/**
 * @brief Obtém nome do console (alias)
 */
String getConsoleName(ConsoleType type);

/**
 * @brief Gera firmware aleatório para simulação
 */
String generateRandomFirmware(ConsoleType type);

/**
 * @brief Obtém exploit conhecido para o console
 */
String getExploitForConsole(ConsoleType type, const String& firmware);

// ============================================================================
// INSTÂNCIA GLOBAL
// ============================================================================

extern ConsoleAttackManager consoleAttackManager;

#endif // CONSOLE_ATTACKS_H
