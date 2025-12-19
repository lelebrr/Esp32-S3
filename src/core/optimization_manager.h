/**
 * @file optimization_manager.h
 * @brief Gerenciador de Otimizações do Sistema Lele Origin
 * @author Lele Origin Team
 * @version 2.0
 */

#ifndef OPTIMIZATION_MANAGER_H
#define OPTIMIZATION_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <WiFi.h>
#include <esp_bt.h>
#include <esp_adc_cal.h>

// ============================================================================
// ENUMS E CONSTANTES DE OTIMIZAÇÃO
// ============================================================================

/**
 * @brief Modos de otimização do sistema
 */
enum OptimizationMode {
    MODE_PERFORMANCE = 0,  ///< Prioriza performance
    MODE_BALANCED,         ///< Equilibra performance e energia
    MODE_POWERSAVE,        ///< Prioriza economia de energia
    MODE_IDLE              ///< Modo ocioso máximo
};

/**
 * @brief Componentes otimizáveis
 */
enum OptimizableComponent {
    COMPONENT_CPU = 0,
    COMPONENT_WIFI,
    COMPONENT_BLE,
    COMPONENT_DISPLAY,
    COMPONENT_MEMORY,
    COMPONENT_SENSORS,
    COMPONENT_RF,
    COMPONENT_USB
};

/**
 * @brief Configurações de otimização
 */
struct OptimizationConfig {
    uint32_t cpuFrequency;      // Frequência CPU em MHz
    uint8_t wifiPower;          // Potência WiFi (0-20.5 dBm)
    bool wifiSleep;             // Sleep mode WiFi
    uint8_t blePower;           // Potência BLE
    bool bleSleep;              // Sleep mode BLE
    bool displayDim;            // Dim display
    uint16_t displayTimeout;    // Timeout display em segundos
    bool psramEnabled;          // Uso de PSRAM
    uint32_t sensorInterval;    // Intervalo sensores em ms
    bool rfLowPower;            // Modo baixa potência RF
    bool usbOptimized;          // Otimização USB
};

// ============================================================================
// CLASSE BUFFER POOL
// ============================================================================

/**
 * @brief Pool de buffers reutilizáveis para otimização de memória
 */
class BufferPool {
private:
    // Otimização Massiva: Reduzido de 10x1024 (10KB) para 2x256 (512B)
    static const size_t MAX_BUFFERS = 2;     // Reduzido de 10
    static const size_t BUFFER_SIZE = 256;   // Reduzido de 1024
    uint8_t buffers[MAX_BUFFERS][BUFFER_SIZE];
    bool used[MAX_BUFFERS];
    SemaphoreHandle_t mutex;

public:
    BufferPool();
    uint8_t* getBuffer();
    bool releaseBuffer(uint8_t* buffer);
    size_t getBufferSize() const { return BUFFER_SIZE; }
    size_t getAvailableBuffers() const;
    size_t getMaxBuffers() const { return MAX_BUFFERS; }
};

// ============================================================================
// CLASSE CONNECTION POOL
// ============================================================================

/**
 * @brief Pool de conexões WiFi reutilizáveis
 */
class ConnectionPool {
private:
    static const size_t MAX_CONNECTIONS = 5;
    WiFiClient clients[MAX_CONNECTIONS];
    bool inUse[MAX_CONNECTIONS];
    SemaphoreHandle_t mutex;

public:
    ConnectionPool();
    WiFiClient* getClient();
    bool releaseClient(WiFiClient* client);
};

// ============================================================================
// CLASSE PRINCIPAL OPTIMIZATION MANAGER
// ============================================================================

/**
 * @brief Gerenciador central de otimizações do sistema
 */
class OptimizationManager {
private:
    OptimizationMode currentMode;
    OptimizationConfig currentConfig;
    BufferPool bufferPool;
    ConnectionPool connectionPool;

    // Estado dos componentes
    bool bleActive;
    bool wifiActive;
    bool rfActive;
    bool usbActive;
    bool attackActive;

    // Timers para otimização
    unsigned long lastOptimizationCheck;
    unsigned long lastPowerCheck;

    // Métricas de performance
    uint32_t cpuTime;
    uint32_t wifiTime;
    uint32_t bleTime;
    size_t memoryUsed;
    size_t psramUsed;

    // ADC para monitoramento de bateria
    esp_adc_cal_characteristics_t *adc_chars;
    adc1_channel_t batChannel;

public:
    OptimizationManager();

    // ============================================================================
    // MÉTODOS PÚBLICOS
    // ============================================================================

    /**
     * @brief Inicializa o gerenciador de otimizações
     */
    void begin();

    /**
     * @brief Define o modo de otimização
     */
    void setMode(OptimizationMode mode);

    /**
     * @brief Atualiza estado dos componentes
     */
    void updateComponentState(OptimizableComponent component, bool active);

    /**
     * @brief Loop principal de otimização (chamar no loop principal)
     */
    /**
     * @brief Loop principal de otimização (chamar no loop principal)
     */
    void loop();

    /**
     * @brief Obtém buffer otimizado do pool
     */
    /**
     * @brief Obtém buffer otimizado do pool
     */
    uint8_t* getOptimizedBuffer();
    size_t getBufferSize() const { return bufferPool.getBufferSize(); }

    /**
     * @brief Libera buffer para o pool
     */
    /**
     * @brief Libera buffer para o pool
     */
    bool releaseOptimizedBuffer(uint8_t* buffer);

    /**
     * @brief Obtém cliente WiFi otimizado do pool
     */
    /**
     * @brief Obtém cliente WiFi otimizado do pool
     */
    WiFiClient* getOptimizedClient();

    /**
     * @brief Libera cliente WiFi para o pool
     */
    /**
     * @brief Libera cliente WiFi para o pool
     */
    bool releaseOptimizedClient(WiFiClient* client);

    /**
     * @brief Aloca memória otimizada (usa PSRAM se disponível)
     */
    /**
     * @brief Aloca memória otimizada (usa PSRAM se disponível)
     */
    void* allocateOptimized(size_t size);

    /**
     * @brief Obtém métricas de performance
     */
    /**
     * @brief Obtém métricas de performance
     */
    String getPerformanceMetrics();

    /**
     * @brief Obtém tensão da bateria
     */
    float getBatteryVoltage();

private:
    // ============================================================================
    // MÉTODOS PRIVADOS
    // ============================================================================

    /**
     * @brief Aplica todas as otimizações atuais
     */
    void applyOptimizations();

    /**
     * @brief Otimização dinâmica baseada no uso
     */
    /**
     * @brief Otimização dinâmica baseada no uso
     */
    void optimizeDynamically();

    /**
     * @brief Otimização de consumo de energia
     */
    /**
     * @brief Otimização de consumo de energia
     */
    void optimizePowerConsumption();

    /**
     * @brief Otimiza CPU
     */
    /**
     * @brief Otimiza CPU
     */
    void optimizeCPU();

    /**
     * @brief Otimiza WiFi
     */
    /**
     * @brief Otimiza WiFi
     */
    void optimizeWiFi();

    /**
     * @brief Otimiza BLE
     */
    /**
     * @brief Otimiza BLE
     */
    void optimizeBLE();

    /**
     * @brief Otimiza display
     */
    /**
     * @brief Otimiza display
     */
    void optimizeDisplay();

    /**
     * @brief Otimiza gerenciamento de memória
     */
    /**
     * @brief Otimiza gerenciamento de memória
     */
    void optimizeMemory();

    /**
     * @brief Otimiza sensores
     */
    /**
     * @brief Otimiza sensores
     */
    void optimizeSensors();

    /**
     * @brief Otimiza RF
     */
    /**
     * @brief Otimiza RF
     */
    void optimizeRF();

    /**
     * @brief Otimiza USB
     */
    void optimizeUSB();

    /**
     * @brief Atualiza métricas de performance
     */
    void updateMetrics();

    /**
     * @brief Log de otimizações
     */
    void logOptimization(const String& message);
};

// ============================================================================
// INSTÂNCIA GLOBAL
// ============================================================================

extern OptimizationManager optimizationManager;

#endif // OPTIMIZATION_MANAGER_H
