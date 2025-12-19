/**
 * @file wps_attacks.h
 * @brief Módulo de Ataques WPS para Lele Origin
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef WPS_ATTACKS_H
#define WPS_ATTACKS_H

#include <Arduino.h>
#include <vector>

// ============================================================================
// ENUMS E CONSTANTES WPS
// ============================================================================

/**
 * @brief Estados do ataque WPS
 */
enum WPSAttackState {
    WPS_IDLE = 0,
    WPS_SCANNING,
    WPS_PIXIE_DUST,
    WPS_REAVER_BRUTE,
    WPS_EVIL_TWIN,
    WPS_DEAUTH,
    WPS_SUCCESS,
    WPS_FAILED
};

/**
 * @brief Tipos de ataque WPS
 */
enum WPSAttackType {
    WPS_TYPE_PIXIE_DUST = 0,    ///< Pixie Dust (offline)
    WPS_TYPE_REAVER,            ///< Reaver brute force
    WPS_TYPE_ONLINE_BRUTE,      ///< Online PIN brute
    WPS_TYPE_NULL_PIN,          ///< Null PIN attack
    WPS_TYPE_EVIL_TWIN,         ///< Evil Twin + WPS
    WPS_TYPE_DEAUTH_FLOOD       ///< Deauth + WPS
};

/**
 * @brief Estrutura de rede WPS descoberta
 */
struct WPSNetwork {
    String ssid;
    String bssid;
    int channel;
    int rssi;
    bool wpsEnabled;
    bool wpsLocked;
    String manufacturer;
    String model;
    uint32_t lastSeen;
    uint8_t pixieHash[32];  // Para Pixie Dust
};

// ============================================================================
// CLASSES PRINCIPAIS
// ============================================================================

/**
 * @brief Scanner WPS - encontra redes vulneráveis
 */
// ... (enums and structs remain)

#define MAX_WPS_NETWORKS 30

/**
 * @brief Scanner WPS - encontra redes vulneráveis
 */
class WPSScanner {
private:
    WPSNetwork networks[MAX_WPS_NETWORKS];
    int networkCount;
    bool scanning;
    unsigned long scanStartTime;
    int scanTimeout;  // segundos

public:
    WPSScanner() : networkCount(0), scanning(false), scanStartTime(0), scanTimeout(30) {}

    /**
     * @brief Inicia scan de redes WPS
     */
    bool startScan(int timeout = 30);

    /**
     * @brief Para o scan
     */
    void stopScan();

    /**
     * @brief Obtém rede por índice
     */
    WPSNetwork* getNetwork(int index) {
        if (index >= 0 && index < networkCount) return &networks[index];
        return nullptr;
    }

    /**
     * @brief Obtém contagem de redes
     */
    int getNetworkCount() const { return networkCount; }

    /**
     * @brief Verifica se está escaneando
     */
    bool isScanning() const { return scanning; }

    /**
     * @brief Callback para quando uma rede é encontrada
     */
    void onNetworkFound(WPSNetwork network);

    /**
     * @brief Obtém progresso do scan (0-100)
     */
    int getScanProgress() const;
};

// ... (PixieDustAttacker remains similar)

enum ReaverPhase {
    PHASE_COMMON,
    PHASE_COMPUTED,
    PHASE_BRUTE,
    PHASE_DONE
};

/**
 * @brief Executor de ataques Reaver (Zero Alloc Version)
 */
class ReaverAttacker {
private:
    WPSNetwork target;
    bool attacking;
    unsigned long attackStartTime;
    String currentPIN;
    
    // State Machine logic
    ReaverPhase phase;
    int phaseIndex;
    uint32_t bruteCurrent;
    
    int attemptCount;
    String calculateBSSIDPins(const String& bssidStr);

public:
    ReaverAttacker() : attacking(false), attackStartTime(0), phase(PHASE_COMMON), phaseIndex(0), attemptCount(0) {}

    /**
     * @brief Inicia ataque Reaver brute force
     */
    bool startAttack(const WPSNetwork& network);

    /**
     * @brief Para o ataque
     */
    void stopAttack();

    /**
     * @brief Tenta próximo PIN
     */
    bool tryNextPIN();

    /**
     * @brief Verifica se PIN foi encontrado
     */
    bool checkPINResult(const String& pin);

    /**
     * @brief Obtém progresso do ataque (0-100)
     */
    int getAttackProgress() const;

    /**
     * @brief Verifica se está atacando
     */
    bool isAttacking() const { return attacking; }

    /**
     * @brief Obtém PIN atual
     */
    const String& getCurrentPIN() const { return currentPIN; }
};

/**
 * @brief Executor de Evil Twin + WPS
 */
class WPSEvilTwin {
private:
    WPSNetwork target;
    bool active;
    String fakeSSID;
    String fakeBSSID;

public:
    WPSEvilTwin() : active(false) {}

    /**
     * @brief Inicia Evil Twin
     */
    bool startEvilTwin(const WPSNetwork& network);

    /**
     * @brief Para Evil Twin
     */
    void stopEvilTwin();

    /**
     * @brief Manipula conexões de vítimas
     */
    void handleVictimConnection();

    /**
     * @brief Verifica se está ativo
     */
    bool isActive() const { return active; }

private:
    String generateFakeBSSID(const String& realBSSID);
};

/**
 * @brief Gerenciador principal de ataques WPS
 */
class WPSAttackManager {
private:
    WPSScanner scanner;
    PixieDustAttacker pixieAttacker;
    ReaverAttacker reaverAttacker;
    WPSEvilTwin evilTwin;

    WPSAttackState currentState;
    WPSAttackType currentAttackType;
    WPSNetwork currentTarget;

    // Estatísticas
    int totalScanned;
    int vulnerableFound;
    int attacksSuccessful;
    int attacksFailed;

public:
    WPSAttackManager() :
        currentState(WPS_IDLE),
        currentAttackType(WPS_TYPE_PIXIE_DUST),
        totalScanned(0), vulnerableFound(0),
        attacksSuccessful(0), attacksFailed(0) {}

    // ============================================================================
    // MÉTODOS PÚBLICOS
    // ============================================================================

    /**
     * @brief Inicializa o gerenciador WPS
     */
    void begin();

    /**
     * @brief Loop principal (chamar no loop())
     */
    void loop();

    /**
     * @brief Inicia scan de redes WPS
     */
    bool startScan(int timeout = 30);

    /**
     * @brief Seleciona alvo para ataque
     */
    bool selectTarget(const WPSNetwork& network);

    /**
     * @brief Inicia ataque Pixie Dust
     */
    bool startPixieDustAttack();

    /**
     * @brief Inicia ataque Reaver
     */
    bool startReaverAttack();

    /**
     * @brief Inicia Evil Twin
     */
    bool startEvilTwin();

    /**
     * @brief Para ataque atual
     */
    void stopAttack();

    /**
     * @brief Obtém estado atual
     */
    WPSAttackState getState() const { return currentState; }

    /**
     * @brief Obtém alvo atual
     */
    WPSNetwork getCurrentTarget() const { return currentTarget; }

    /**
     * @brief Obtém rede por índice
     */
    WPSNetwork* getNetwork(int index) { return scanner.getNetwork(index); }

    /**
     * @brief Obtém total de redes
     */
    int getNetworkCount() const { return scanner.getNetworkCount(); }

    /**
     * @brief Obtém estatísticas
     */
    String getStatistics() const;

    /**
     * @brief Salva dados capturados no SD
     */
    bool saveCapturedData();

    /**
     * @brief Carrega wordlist de PINs
     */
    bool loadPINWordlist(const char* filename);

    // ============================================================================
    // CALLBACKS E EVENTOS
    // ============================================================================

    /**
     * @brief Callback quando rede WPS é encontrada
     */
    void (*onNetworkFound)(const WPSNetwork& network) = nullptr;

    /**
     * @brief Callback quando PIN é encontrado
     */
    void (*onPinFound)(const String& pin, const String& password) = nullptr;

    /**
     * @brief Callback quando ataque falha
     */
    void (*onAttackFailed)(const WPSNetwork& network) = nullptr;

private:
    // ============================================================================
    // MÉTODOS PRIVADOS
    // ============================================================================

    /**
     * @brief Atualiza estado interno
     */
    void updateState(WPSAttackState newState);

    /**
     * @brief Processa ataques ativos
     */
    void processAttacks();

    /**
     * @brief Verifica timeout de operações
     */
    void checkTimeouts();

    /**
     * @brief Log de operações
     */
    void logOperation(const String& operation);
};

// ============================================================================
// FUNÇÕES GLOBAIS DE UTILITÁRIO
// ============================================================================

/**
 * @brief Calcula checksum WPS PIN
 */
uint32_t wpsPinChecksum(uint32_t pin);

/**
 * @brief Valida formato de PIN WPS
 */
bool validateWPSPin(const String& pin);

/**
 * @brief Converte BSSID para string formatada
 */
String formatBSSID(const uint8_t* bssid);

/**
 * @brief Calcula força do sinal (0-100)
 */
int calculateSignalStrength(int rssi);

/**
 * @brief Detecta fabricante pelo BSSID
 */
String detectManufacturer(const uint8_t* bssid);

// ============================================================================
// INSTÂNCIA GLOBAL
// ============================================================================

extern WPSAttackManager wpsAttackManager;

#endif // WPS_ATTACKS_H
