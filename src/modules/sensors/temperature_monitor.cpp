#include "temperature_monitor.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/config.h"
#include <Preferences.h>

// Namespace para NVS
static Preferences tempPrefs;
static const char* NVS_NAMESPACE = "temp_config";

// Tentativa de incluir bibliotecas (podem não estar disponíveis)
#if __has_include(<OneWire.h>)
    #include <OneWire.h>
    #define HAS_ONEWIRE 1
#else
    #define HAS_ONEWIRE 0
#endif

#if __has_include(<DallasTemperature.h>)
    #include <DallasTemperature.h>
    #define HAS_DALLAS 1
#else
    #define HAS_DALLAS 0
#endif

// Estado interno
static TempConfig tempConfig = {
    .warningTemp = TEMP_WARNING_DEFAULT,
    .criticalTemp = TEMP_CRITICAL_DEFAULT,
    .shutdownTemp = TEMP_SHUTDOWN_DEFAULT,
    .alertType = TEMP_ALERT_SCREEN,
    .action = TEMP_ACTION_WARN,
    .showOnStatusBar = true,
    .readInterval = TEMP_READ_INTERVAL,
    .enabled = true
};

static TempSensorStatus sensorStatus = TEMP_SENSOR_NOT_FOUND;
static float currentTemp = -127.0f;
static unsigned long lastReadTime = 0;
static bool alertActive = false;

#if HAS_ONEWIRE && HAS_DALLAS
    static OneWire* oneWire = nullptr;
    static DallasTemperature* sensors = nullptr;
#endif

bool initTemperatureMonitor() {
    Serial.println("[Temp] Inicializando monitor de temperatura...");

    loadTempConfig();

    #if HAS_ONEWIRE && HAS_DALLAS
        // Tenta inicializar o sensor
        oneWire = new OneWire(DS18B20_PIN);
        sensors = new DallasTemperature(oneWire);

        sensors->begin();

        // Verifica se há sensores conectados
        int deviceCount = sensors->getDeviceCount();

        if (deviceCount > 0) {
            Serial.printf("[Temp] Encontrado(s) %d sensor(es) DS18B20\n", deviceCount);
            sensors->setResolution(12); // 12-bit = 0.0625°C
            sensors->setWaitForConversion(false); // Leitura assíncrona
            sensorStatus = TEMP_SENSOR_OK;

            // Primeira leitura
            sensors->requestTemperatures();
            delay(100);
            currentTemp = sensors->getTempCByIndex(0);

            if (currentTemp == DEVICE_DISCONNECTED_C || currentTemp == -127.0f) {
                Serial.println("[Temp] Sensor detectado mas leitura falhou");
                sensorStatus = TEMP_SENSOR_ERROR;
                return false;
            }

            Serial.printf("[Temp] Temperatura inicial: %.1f C\n", currentTemp);
            return true;
        } else {
            Serial.println("[Temp] Nenhum sensor DS18B20 encontrado");
            sensorStatus = TEMP_SENSOR_NOT_FOUND;

            // Libera memória se não encontrou sensor
            delete sensors;
            delete oneWire;
            sensors = nullptr;
            oneWire = nullptr;

            return false;
        }
    #else
        Serial.println("[Temp] Bibliotecas OneWire/DallasTemperature nao disponiveis");
        sensorStatus = TEMP_SENSOR_DISABLED;
        return false;
    #endif
}

void updateTemperatureMonitor() {
    if (!tempConfig.enabled) return;
    if (sensorStatus != TEMP_SENSOR_OK) return;

    unsigned long now = millis();
    if (now - lastReadTime < tempConfig.readInterval) return;
    lastReadTime = now;

    #if HAS_ONEWIRE && HAS_DALLAS
        if (sensors != nullptr) {
            // Solicita nova leitura
            sensors->requestTemperatures();

            float newTemp = sensors->getTempCByIndex(0);

            if (newTemp != DEVICE_DISCONNECTED_C && newTemp != -127.0f) {
                currentTemp = newTemp;

                // Verifica limites
                if (isTemperatureShutdown()) {
                    triggerTemperatureAlert();
                    if (tempConfig.action == TEMP_ACTION_SHUTDOWN) {
                        // Desligar módulos críticos
                        Serial.println("[Temp] CRITICO! Desligando modulos...");
                        WiFi.mode(WIFI_OFF);
                        btStop();
                        setCpuFrequencyMhz(80); // Reduz frequência da CPU
                    }
                } else if (isTemperatureCritical()) {
                    triggerTemperatureAlert();
                    if (tempConfig.action >= TEMP_ACTION_PAUSE) {
                        // Pausar operações pesadas
                        Serial.println("[Temp] Temperatura critica! Pausando...");
                        setCpuFrequencyMhz(160); // Reduz para 160MHz
                    }
                } else if (isTemperatureWarning()) {
                    if (!alertActive) {
                        triggerTemperatureAlert();
                    }
                } else {
                    clearTemperatureAlert();
                }
            } else {
                // Erro de leitura - não invalida leitura anterior
                Serial.println("[Temp] Erro de leitura, mantendo valor anterior");
            }
        }
    #endif
}

void disableTemperatureMonitor() {
    tempConfig.enabled = false;
    sensorStatus = TEMP_SENSOR_DISABLED;
}

float getCurrentTemperature() {
    return currentTemp;
}

bool isTemperatureValid() {
    return sensorStatus == TEMP_SENSOR_OK && currentTemp > -100.0f;
}

TempSensorStatus getSensorStatus() {
    return sensorStatus;
}

const char* getSensorStatusText() {
    switch (sensorStatus) {
        case TEMP_SENSOR_OK: return "OK";
        case TEMP_SENSOR_NOT_FOUND: return "Nao Encontrado";
        case TEMP_SENSOR_ERROR: return "Erro";
        case TEMP_SENSOR_DISABLED: return "Desativado";
        default: return "Desconhecido";
    }
}

bool isTemperatureWarning() {
    return isTemperatureValid() && currentTemp >= tempConfig.warningTemp;
}

bool isTemperatureCritical() {
    return isTemperatureValid() && currentTemp >= tempConfig.criticalTemp;
}

bool isTemperatureShutdown() {
    return isTemperatureValid() && currentTemp >= tempConfig.shutdownTemp;
}

void triggerTemperatureAlert() {
    if (alertActive) return;
    alertActive = true;

    String msg = "Temp: " + String(currentTemp, 1) + " C";

    if (tempConfig.alertType & TEMP_ALERT_LED) {
        // Integração com WS2812B
        #if __has_include("modules/lighting/ws2812b_control.h")
            extern void notifyWarning();
            extern void notifyError();
            if (isTemperatureCritical()) {
                notifyError();
            } else {
                notifyWarning();
            }
        #endif
        Serial.println("[Temp] Alerta LED ativado");
    }

    if (tempConfig.alertType & TEMP_ALERT_SCREEN) {
        // Exibe mensagem na tela
        if (isTemperatureCritical()) {
            displayError("TEMPERATURA ALTA!", true);
        } else {
            displayWarning("Atencao: " + msg, true);
        }
        Serial.println("[Temp] Alerta Tela: " + msg);
    }

    if (tempConfig.alertType & TEMP_ALERT_SOUND) {
        // Integração com sistema de som - beep de alerta
        #if __has_include("modules/others/audio.h")
            extern void playToneSimple(int freq, int duration);
            if (isTemperatureCritical()) {
                playToneSimple(2000, 500); // Tom alto e longo
            } else {
                playToneSimple(1000, 200); // Tom médio e curto
            }
        #endif
        Serial.println("[Temp] Alerta Som ativado");
    }
}

void clearTemperatureAlert() {
    alertActive = false;
}

TempConfig* getTempConfig() {
    return &tempConfig;
}

void setWarningTemp(float temp) {
    tempConfig.warningTemp = temp;
}

void setCriticalTemp(float temp) {
    tempConfig.criticalTemp = temp;
}

void setAlertType(uint8_t type) {
    tempConfig.alertType = type;
}

void setTempAction(TempAction action) {
    tempConfig.action = action;
}

void setShowOnStatusBar(bool show) {
    tempConfig.showOnStatusBar = show;
}

void saveTempConfig() {
    // Salvar em NVS
    tempPrefs.begin(NVS_NAMESPACE, false);
    tempPrefs.putFloat("warning", tempConfig.warningTemp);
    tempPrefs.putFloat("critical", tempConfig.criticalTemp);
    tempPrefs.putFloat("shutdown", tempConfig.shutdownTemp);
    tempPrefs.putUChar("alertType", tempConfig.alertType);
    tempPrefs.putUChar("action", (uint8_t)tempConfig.action);
    tempPrefs.putBool("statusBar", tempConfig.showOnStatusBar);
    tempPrefs.putULong("interval", tempConfig.readInterval);
    tempPrefs.putBool("enabled", tempConfig.enabled);
    tempPrefs.end();
    Serial.println("[Temp] Configuracao salva em NVS");
}

void loadTempConfig() {
    // Carregar de NVS
    tempPrefs.begin(NVS_NAMESPACE, true);
    if (tempPrefs.isKey("warning")) {
        tempConfig.warningTemp = tempPrefs.getFloat("warning", TEMP_WARNING_DEFAULT);
        tempConfig.criticalTemp = tempPrefs.getFloat("critical", TEMP_CRITICAL_DEFAULT);
        tempConfig.shutdownTemp = tempPrefs.getFloat("shutdown", TEMP_SHUTDOWN_DEFAULT);
        tempConfig.alertType = tempPrefs.getUChar("alertType", TEMP_ALERT_SCREEN);
        tempConfig.action = (TempAction)tempPrefs.getUChar("action", TEMP_ACTION_WARN);
        tempConfig.showOnStatusBar = tempPrefs.getBool("statusBar", true);
        tempConfig.readInterval = tempPrefs.getULong("interval", TEMP_READ_INTERVAL);
        tempConfig.enabled = tempPrefs.getBool("enabled", true);
        Serial.println("[Temp] Configuracao carregada de NVS");
    } else {
        Serial.println("[Temp] Usando configuracao padrao");
    }
    tempPrefs.end();
}

String getTemperatureString() {
    if (!isTemperatureValid()) {
        return "N/A";
    }
    return String(currentTemp, 1) + " C";
}

uint16_t getTemperatureColor() {
    if (!isTemperatureValid()) {
        return TFT_DARKGREY;
    }
    if (isTemperatureCritical()) {
        return TFT_RED;
    }
    if (isTemperatureWarning()) {
        return TFT_YELLOW;
    }
    return TFT_GREEN;
}
