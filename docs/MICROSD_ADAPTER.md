# 💾 Micro SD Card Adapter Module

## 📋 Visão Geral

O módulo adaptador de cartão Micro SD é um componente crítico no projeto ESP32-S3, servindo como o **armazenamento primário** para o sistema. Graças à implementação do "Aggressive SD Boot", o dispositivo opera quase inteiramente a partir do SD, utilizando-o para logs, configurações, assets web, payloads e scripts de ataque, minimizando o uso do flash interno.

---

## ⚙️ Especificações de Hardware

### Pinagem (ESP32-S3 - Custom Config v2.0)

A configuração abaixo reflete a implementação atual no `pin_config.h`. O cartão SD utiliza um **barramento SPI dedicado (HSPI)** para evitar conflitos com o Display TFT.

| microSD Pin | Função | ESP32-S3 Pin | Notas |
| :--- | :--- | :---: | :--- |
| **CS** | Chip Select | **GPIO 39** | Dedicado para o SD Card. Ativo em nível BAIXO. |
| **SCK** | Serial Clock | **GPIO 40** | HSPI Clock. |
| **MOSI** | Master Out Slave In | **GPIO 41** | HSPI MOSI. |
| **MISO** | Master In Slave Out | **GPIO 42** | HSPI MISO. |
| **VCC** | Power (5V/3.3V) | **3.3V / 5V** | Módulos comuns possuem LDO 5V->3.3V. |
| **GND** | Ground | **GND** | Referência comum. |

### Diagrama de Conexão

```
  ┌───────────────┐              ┌───────────────┐
  │   ESP32-S3    │              │  MicroSD Mod  │
  │               │              │               │
  │   GPIO 39 ────┼──────────────┼── CS          │
  │   GPIO 40 ────┼──────────────┼── SCK         │
  │   GPIO 41 ────┼──────────────┼── MOSI        │
  │   GPIO 42 ────┼──────────────┼── MISO        │
  │   3.3V    ────┼──────────────┼── VCC         │
  │   GND     ────┼──────────────┼── GND         │
  └───────────────┘              └───────────────┘
```

### Arquitetura de Barramentos SPI

| Barramento | Função | Pinos (SCK/MOSI/MISO) | CS Devices |
|:-----------|:-------|:----------------------|:-----------|
| **SPI A (FSPI)** | Display + Touch | 14 / 13 / - | TFT (10), Touch (15) |
| **SPI B (HSPI)** | SD Card + RF | 40 / 41 / 42 | SD (39), CC1101 (46) |

---

## 💻 Implementação de Software

### Arquivos do Driver

- [aggressive_sd.h](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/src/core/aggressive_sd.h) - Header
- [aggressive_sd.cpp](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/src/core/aggressive_sd.cpp) - Implementação

### API do Driver

```cpp
#include "core/aggressive_sd.h"

// Inicialização (chamado automaticamente no boot)
aggressive_boot_logic();

// Verificar se SD está pronto
if (AggressiveSD::isReady()) { ... }

// Operações de arquivo
AggressiveSD::writeFile("/logs/attack.log", "Attack started\n");
AggressiveSD::appendFile("/logs/attack.log", "Target acquired\n");
String content = AggressiveSD::readFile("/config/settings.json");
bool exists = AggressiveSD::fileExists("/payloads/keyboard.txt");
AggressiveSD::deleteFile("/temp/data.bin");

// Informações do cartão
uint64_t total = AggressiveSD::getCardSize();
uint64_t free = AggressiveSD::getFreeSpace();
```

### Diretórios Padrão (Criados Automaticamente)

- `/logs/` - Arquivos de log do sistema
- `/config/` - Configurações persistentes
- `/payloads/` - Scripts de ataque (Duckyscript, etc.)
- `/captures/` - Handshakes, dumps de cartões, sinais RF
- `/web/` - Interface web (HTML/CSS/JS)

---

## ⚔️ Vetores de Ataque & Segurança (SD Related)

### 1. Payload Injection (BadUSB/Duckyscript)

- **Arquivos:** `/payloads/*.txt`
- **Descrição:** Scripts carregados do SD e executados via HID.

### 2. Data Exfiltration

- **Diretório:** `/captures/`
- **Descrição:** Handshakes WPA, dumps de cartões NFC, sinais RF gravados.

### 3. Configuration Tampering

- **Arquivo:** `/config/settings.json`
- **Descrição:** Modificar comportamento do dispositivo sem acesso ao firmware.

### 4. Evil Maid Firmware Update

- **Arquivo:** `/update.bin`
- **Descrição:** Possibilidade de flashear firmware via SD (se habilitado).

---

## 🛠️ Troubleshooting

| Problema | Solução |
|----------|---------|
| "Mount FAILED!" | Verifique conexões físicas e formatação (FAT32). |
| "No card attached" | Cartão pode estar mal encaixado ou defeituoso. |
| Leituras lentas | Reduza `SPI.setFrequency()` para 20MHz. |
| Conflito com Display | Certifique-se de usar pinos HSPI (39-42) para SD. |

---

## 📚 Referências

- [ElectronicWings - MicroSD Interfacing](https://www.electronicwings.com/esp32/microsd-card-interfacing-with-esp32)
- [Arduino Forum - SD on ESP32-S3 (pins 39-42)](https://forum.arduino.cc/t/sd-card-on-pins-39-42-on-the-esp32-s3-wroom-1u/1351193/5)
- [pin_config.h](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/include/pin_config.h)
