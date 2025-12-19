#ifndef __TEMPERATURE_MONITOR_H__
#define __TEMPERATURE_MONITOR_H__

#include <Arduino.h>

// Configuração do sensor DS18B20
#define DS18B20_PIN         4       // GPIO para 1-Wire
#define TEMP_READ_INTERVAL  5000    // Intervalo de leitura em ms

// Limites de temperatura (em Celsius)
#define TEMP_WARNING_DEFAULT  40.0f
#define TEMP_CRITICAL_DEFAULT 45.0f
#define TEMP_SHUTDOWN_DEFAULT 50.0f

// Tipos de alerta
enum TempAlertType {
    TEMP_ALERT_NONE = 0,
    TEMP_ALERT_LED = 1,
    TEMP_ALERT_SCREEN = 2,
    TEMP_ALERT_SOUND = 4,
    TEMP_ALERT_ALL = 7
};

// Ações ao atingir limite
enum TempAction {
    TEMP_ACTION_WARN = 0,       // Apenas avisar
    TEMP_ACTION_PAUSE = 1,      // Pausar operações pesadas
    TEMP_ACTION_SHUTDOWN = 2    // Desligar módulos
};

// Status do sensor
enum TempSensorStatus {
    TEMP_SENSOR_OK = 0,
    TEMP_SENSOR_NOT_FOUND,
    TEMP_SENSOR_ERROR,
    TEMP_SENSOR_DISABLED
};

// Estrutura de configuração
struct TempConfig {
    float warningTemp;      // Temperatura de aviso (amarelo)
    float criticalTemp;     // Temperatura crítica (vermelho)
    float shutdownTemp;     // Temperatura de desligamento
    uint8_t alertType;      // Combinação de TempAlertType
    TempAction action;      // Ação ao atingir limite
    bool showOnStatusBar;   // Exibir na barra superior
    uint16_t readInterval;  // Intervalo de leitura (ms)
    bool enabled;           // Monitoramento ativo
};

// Funções principais
bool initTemperatureMonitor();      // Retorna false se sensor não encontrado
void updateTemperatureMonitor();    // Chamado no loop principal
void disableTemperatureMonitor();

// Leitura
float getCurrentTemperature();      // Retorna -127 se erro
bool isTemperatureValid();          // True se última leitura foi válida
TempSensorStatus getSensorStatus();
const char* getSensorStatusText();

// Verificação de limites
bool isTemperatureWarning();        // Acima do limite de aviso
bool isTemperatureCritical();       // Acima do limite crítico
bool isTemperatureShutdown();       // Acima do limite de desligamento

// Alertas
void triggerTemperatureAlert();     // Dispara alerta conforme config
void clearTemperatureAlert();

// Configuração
TempConfig* getTempConfig();
void setWarningTemp(float temp);
void setCriticalTemp(float temp);
void setAlertType(uint8_t type);
void setTempAction(TempAction action);
void setShowOnStatusBar(bool show);

// Persistência
void saveTempConfig();
void loadTempConfig();

// Para exibição na UI
String getTemperatureString();      // "25.5°C" ou "N/A"
uint16_t getTemperatureColor();     // Cor baseada no status (verde/amarelo/vermelho)

#endif // __TEMPERATURE_MONITOR_H__
