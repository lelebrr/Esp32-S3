# 📍 GPS GY-NEO6MV2 Module (u-blox NEO-6M)

## 📋 Visão Geral

O módulo GPS GY-NEO6MV2 fornece funcionalidades de posicionamento global para o projeto, permitindo **wardriving**, **logging de posições**, **sincronização de tempo UTC**, e **geofencing** para ataques baseados em localização.

---

## ⚙️ Especificações de Hardware

### Chip u-blox NEO-6M

| Especificação | Valor |
|---------------|-------|
| **Chip** | u-blox NEO-6M |
| **Frequência** | L1 (1575.42 MHz) |
| **Canais** | 50 (22 tracking) |
| **Precisão** | 2.5 metros CEP |
| **TTFF** | Cold: 27s, Hot: 1s |
| **Sensibilidade** | -161 dBm (tracking) |
| **Atualização** | 5 Hz max (1 Hz default) |
| **Protocolo** | NMEA-0183 |
| **Tensão** | 3.3V - 5V |
| **Consumo** | 45mA (acquisition), 35mA (tracking) |

### Pinagem (ESP32-S3)

| GPS Pin | Função | ESP32-S3 Pin | Notas |
| :--- | :--- | :---: | :--- |
| **VCC** | Power | **3.3V** | Pode usar 5V se módulo tiver regulador |
| **GND** | Ground | **GND** | Referência comum |
| **TX** | GPS Data Out | **GPIO 17** | ESP32 RX ← GPS TX |
| **RX** | GPS Data In | **GPIO 18** | ESP32 TX → GPS RX |

### Diagrama de Conexão

```
  ┌───────────────┐              ┌───────────────┐
  │   ESP32-S3    │              │  GY-NEO6MV2   │
  │               │              │               │
  │   GPIO 17 ────┼──────────────┼── TX (Data)   │
  │   GPIO 18 ────┼──────────────┼── RX (Cmd)    │
  │   3.3V    ────┼──────────────┼── VCC         │
  │   GND     ────┼──────────────┼── GND         │
  └───────────────┘              └───────────────┘
```

---

## 💻 Implementação de Software

### Arquivos do Driver

- [gps_driver.h](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/include/gps_driver.h) - Header
- [gps_driver.cpp](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/src/gps_driver.cpp) - Implementação

### API do Driver

```cpp
#include "gps_driver.h"

// Inicialização (chamada pelo s3_driver ou main)
GPSDriver::init();

// Atualização (chamar em loop/task)
GPSDriver::update();

// Verificações
bool available = GPSDriver::isAvailable();
bool valid = GPSDriver::isValid();

// Dados de Posição
double lat = GPSDriver::getLatitude();
double lon = GPSDriver::getLongitude();
double alt = GPSDriver::getAltitude();
String coords = GPSDriver::getCoordsString();  // "lat,lon"

// Dados de Movimento
double speed = GPSDriver::getSpeedKmh();
double course = GPSDriver::getCourse();

// Qualidade do Sinal
int sats = GPSDriver::getSatellites();
double hdop = GPSDriver::getHDOP();

// Tempo (UTC)
String date = GPSDriver::getDateString();  // "YYYY-MM-DD"
String time = GPSDriver::getTimeString();  // "HH:MM:SS"

// Cálculos
double dist = GPSDriver::distanceTo(targetLat, targetLon);  // metros
double bearing = GPSDriver::bearingTo(targetLat, targetLon);  // graus

// Logging (SD Card)
GPSDriver::logPosition("/logs/gps.csv");
GPSDriver::logPosition("/logs/wardrive.csv", ssid, bssid, rssi);

// Export
GPSDriver::exportToKML("/logs/wardrive.csv", "/exports/map.kml");
```

---

## ⚔️ Funcionalidades de Ataque

### 1. Wardriving

- **Descrição:** Captura redes WiFi com coordenadas GPS.
- **Arquivo:** `/logs/wardrive.csv`
- **Formato:** Kismet-compatible CSV
- **Exportação:** KML para Google Earth

### 2. Location Logging

- **Descrição:** Histórico de posições para tracking.
- **Arquivo:** `/logs/gps.csv`
- **Uso:** Análise de movimentação, timestamps de ataques.

### 3. Time Sync

- **Descrição:** Sincronização de hora UTC precisa.
- **Uso:** Timestamps corretos em logs mesmo sem RTC.

### 4. Geofencing

- **Descrição:** Triggers baseados em proximidade.
- **Uso:** Ativar ataques quando próximo a um alvo.

```cpp
// Exemplo: Trigger quando próximo ao alvo
if (GPSDriver::distanceTo(targetLat, targetLon) < 100) {  // 100 metros
    attacks_start(ATTACK_WIFI_DEAUTH);
}
```

---

## 📡 Protocolo NMEA

### Sentenças Suportadas

| Sentença | Descrição |
|----------|-----------|
| **$GPGGA** | Fix data, posição, qualidade |
| **$GPRMC** | Posição, velocidade, data/hora |
| **$GPGSA** | DOP e satélites ativos |
| **$GPGSV** | Satélites em vista |
| **$GPVTG** | Velocidade e curso |

### Exemplo de Saída NMEA

```
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,47.0,M,,*47
$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
```

---

## 🛠️ Troubleshooting

| Problema | Solução |
|----------|---------|
| Sem fix (0 satélites) | Posicione antena com visão do céu |
| Demora no primeiro fix | Cold start pode levar 27+ segundos |
| Dados inválidos | Verifique baud rate (padrão 9600) |
| HDOP alto | Mova para área com menos obstruções |
| Sem comunicação | Verifique TX/RX cruzados corretamente |

---

## 📚 Referências

- [u-blox NEO-6 Datasheet](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf)
- [TinyGPS++ Library](http://arduiniana.org/libraries/tinygpsplus/)
- [pin_config.h](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/include/pin_config.h)

---

**Última Atualização:** 2025-12-19
