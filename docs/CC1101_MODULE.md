# 📡 CC1101 SubGHz Module - Complete Documentation

> **Status:** ✅ FULLY IMPLEMENTED  
> **Last Update:** 2025-12-19

## 📌 1. Hardware Overview

O **CC1101** é um transceptor sub-1GHz de baixo custo da Texas Instruments, projetado para aplicações wireless de ultra-baixo consumo.

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Chip** | TI CC1101 |
| **Frequências** | 300-348, 387-464, 779-928 MHz |
| **Modulações** | 2-FSK, 4-FSK, GFSK, MSK, ASK/OOK |
| **Sensibilidade** | -111 dBm @ 1.2 kBaud |
| **Potência TX** | -30 a +10 dBm |
| **Data Rate** | 0.6 a 600 kbps |
| **Interface** | SPI |
| **Tensão** | 1.8V - 3.6V (3.3V recomendado) |

---

## 🔌 2. Pinout ESP32-S3

| CC1101 Pin | ESP32-S3 GPIO | Função |
|:----------:|:-------------:|:-------|
| **VCC** | 3.3V | Alimentação |
| **GND** | GND | Terra |
| **CSN** | **GPIO 46** | SPI Chip Select |
| **SCK** | **GPIO 12** | SPI Clock (Shared SD) |
| **MOSI** | **GPIO 11** | SPI Data In (Shared SD) |
| **MISO** | **GPIO 13** | SPI Data Out (Shared SD) |
| **GDO0** | **GPIO 47** | Interrupt/Status |
| **GDO2** | NC | Não usado |

### Power Gating (MOSFET)

- **Enable Pin:** `GPIO 48`
- **Logic:** HIGH = ON, LOW = OFF

```cpp
// Ligar CC1101
digitalWrite(PIN_CC1101_EN, HIGH);
delay(100);

// Desligar CC1101
digitalWrite(PIN_CC1101_EN, LOW);
```

---

## ⚔️ 3. Ataques Implementados

### Jammers (Interferência)

| Ataque | Descrição | Frequência |
|--------|-----------|------------|
| **Jammer Contínuo** | Ruído constante | 315/433/868/915 MHz |
| **Jammer Burst** | Pulsos intermitentes | Configurável |
| **Jammer Smart** | Detecta e interfere | Adaptativo |

### Capture & Replay

| Ataque | Descrição |
|--------|-----------|
| **RF Capture** | Captura sinais (salva no SD) |
| **RF Replay** | Reproduz último sinal |
| **Ghost Replay** | Replay aleatório (8-45s delay) |

### Brute Force

| Ataque | Descrição |
|--------|-----------|
| **Brute Force** | Testa todos códigos (12-24 bits) |
| **De Bruijn** | Sequência otimizada |

### Análise

| Função | Descrição |
|--------|-----------|
| **Spectrum Analyzer** | Varredura de espectro |
| **Protocol Detection** | Identifica protocolo |

---

## 📋 4. Protocolos Suportados

| Protocolo | Bits | Uso |
|-----------|:----:|-----|
| Princeton | 24 | Controles genéricos |
| Nice FLO | 12 | Portões Nice |
| CAME | 12 | Portões CAME |
| Linear | 10 | Controles Linear |
| Holtek | 12 | Controles baratos |
| PT2262 | 24 | Sensores, alarmes |
| KeeLoq | var | Rolling code (detecção apenas) |

---

## 💻 5. API de Uso

### Inicialização

```cpp
#include "rf_core.h"

void setup() {
    RFCore::init();  // Inicializa CC1101
}
```

### Jammer

```cpp
// Jammer contínuo 433MHz
RFCore::startJammerContinuous(FREQ_433);

// No loop
RFCore::updateJammer();

// Para parar
RFCore::stopJammer();
```

### Captura e Replay

```cpp
// Iniciar captura
RFCore::startReceive(FREQ_433);

// Verificar sinal
if (RFCore::hasSignal()) {
    CapturedSignal sig = RFCore::getLastCapture();
    RFCore::saveSignal("/subghz/capture.txt", &sig);
}

// Replay
RFCore::replayLast();
```

### Spectrum Analyzer

```cpp
RFCore::startFrequencyScan(430.0f, 440.0f, 0.1f);

while(!done) {
    RFCore::updateScanner();
    SpectrumData data = RFCore::getSpectrumData();
    float strongest = RFCore::findStrongestFrequency();
}
```

### Brute Force

```cpp
RFCore::startBruteForce(FREQ_433, 12, PROTO_CAME);

while(RFCore::isBruteForcing()) {
    RFCore::updateBruteForce();
    Serial.printf("Progress: %lu/%lu\n", 
        RFCore::getBruteProgress(), 
        RFCore::getBruteTotal());
}
```

---

## 📁 6. Formato de Arquivos

### Formato Nativo

```text
# RF Signal Capture
Frequency: 433.92
Protocol: Princeton
Code: 0x123456
Bits: 24
RSSI: -45
RawLen: 12
RawData: AA 55 12 34 56 78 ...
```

### Flipper Zero (.sub)

```text
Filetype: Flipper SubGhz Key File
Version: 1
Frequency: 433920000
Preset: FuriHalSubGhzPresetOok650Async
Protocol: Princeton
Bit: 24
Key: 00 00 00 00 00 12 34 56
```

---

## 📊 7. Arquivos de Implementação

| Arquivo | Descrição |
|---------|-----------|
| `include/rf_core.h` | Header com 200+ linhas |
| `src/rf_core.cpp` | Implementação 700+ linhas |
| `src/attacks_manager.cpp` | Integração com sistema |
| `include/pin_config.h` | Definição de pinos |

---

## ⚠️ 8. Avisos Legais

> [!CAUTION]
> **Jammers são ilegais** na maioria dos países.
> Use apenas em ambientes controlados para testes educacionais.
> O uso indevido pode resultar em penalidades severas.

---

## 🔗 Documentação Relacionada

- [RF.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/RF.md) - Funcionalidades RF gerais
- [MASTER_SYSTEM.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/MASTER_SYSTEM.md) - Sistema Master
- [ATAQUES.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/ATAQUES.md) - Lista de ataques
