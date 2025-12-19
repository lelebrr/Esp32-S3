# 🔧 Hardware - ESP32-S3-WROOM-1 N16R8

> **Especificações técnicas, pinouts e guia completo do hardware ESP32-S3.**
> Última atualização: 2025-12-19

---

## 📋 Especificações do Microcontrolador

| Característica | Valor |
|----------------|-------|
| **Modelo** | ESP32-S3-WROOM-1 (N16R8) |
| **Flash** | 16MB QIO 80MHz |
| **PSRAM** | 8MB Octal 120MHz (OPI) |
| **CPU** | Dual-core Xtensa LX7 @ 240MHz |
| **WiFi** | 2.4GHz 802.11 b/g/n |
| **Bluetooth** | BLE 5.0 |
| **USB** | Native USB-OTG (CDC) |
| **Deep Sleep** | <5µA |

---

## 📌 Tabela de Pinagem Completa

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                        ESP32-S3-WROOM-1 N16R8 PINOUT                          ║
╠═══════╦══════════════════════╦════════════════════════════════════════════════╣
║ GPIO  ║ Função               ║ Módulo / Notas                                 ║
╠═══════╬══════════════════════╬════════════════════════════════════════════════╣
║   0   ║ Boot Button          ║ Joystick SW, Wake Source                       ║
║   1   ║ I2C SDA              ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)        ║
║   2   ║ I2C SCL              ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)        ║
║   3   ║ Touch CS             ║ XPT2046                                        ║
║   4   ║ Joystick VRX         ║ ADC Canal 1                                    ║
║   5   ║ CC1101 SS / Joy VRY  ║ SubGHz SPI + ADC                               ║
║   6   ║ NRF24 CE             ║ 2.4GHz Radio                                   ║
║   7   ║ NRF24 SS             ║ 2.4GHz Radio                                   ║
║   8   ║ Touch IRQ            ║ XPT2046                                        ║
║   9   ║ TFT RST              ║ ILI9488                                        ║
║  10   ║ TFT CS               ║ ILI9488                                        ║
║  11   ║ SPI2 MOSI            ║ TFT + Touch (compartilhado)                    ║
║  12   ║ SPI2 SCK             ║ TFT + Touch (compartilhado)                    ║
║  13   ║ SPI2 MISO            ║ TFT + Touch (compartilhado)                    ║
║  14   ║ IR RX Main / VBAT    ║ TSOP38238 #1 + Battery ADC                     ║
║  15   ║ GPS RX               ║ NEO-6M TX → ESP RX (UART1)                     ║
║  16   ║ GPS TX               ║ NEO-6M RX ← ESP TX (UART1)                     ║
║  17   ║ YS-IRTM TX           ║ IR Module (UART2, Level Shifter!)              ║
║  18   ║ YS-IRTM RX           ║ IR Module (UART2, Level Shifter!)              ║
║  19   ║ 1-Wire               ║ DS18B20 x2 (4.7K pull-up)                      ║
║  20   ║ PAM8403 EN / IR Z4   ║ Amp Enable + TSOP38238 #5                      ║
║  21   ║ IR RX Zone1          ║ TSOP38238 #2                                   ║
║  35   ║ SD MOSI              ║ SD Card (SPI3)                                 ║
║  36   ║ SD SCK               ║ SD Card (SPI3)                                 ║
║  37   ║ SD MISO              ║ SD Card (SPI3)                                 ║
║  38   ║ SD CS                ║ SD Card (SPI3)                                 ║
║  39   ║ I2S LRCK             ║ PCM5102A Word Select                           ║
║  40   ║ I2S BCLK             ║ PCM5102A Bit Clock                             ║
║  41   ║ I2S DOUT             ║ PCM5102A Data                                  ║
║  42   ║ IR RX Zone2 / MOSFET ║ TSOP38238 #3 + IRF520 Gate                     ║
║  43   ║ USB TX               ║ CDC Serial                                     ║
║  44   ║ USB RX               ║ CDC Serial                                     ║
║  45   ║ IR RX Zone3 / Buzzer ║ TSOP38238 #4 + PWM Beep                        ║
║  46   ║ TFT DC               ║ ILI9488 Data/Command                           ║
║  47   ║ IR TX                ║ LED IR Transmitter                             ║
║  48   ║ TFT BL / WS2812B     ║ Backlight PWM + RGB LEDs (10)                  ║
╚═══════╩══════════════════════╩════════════════════════════════════════════════╝
```

---

## 🔌 Módulos Integrados

### 🔊 PCM5102A DAC + PAM8403 Amp + Speaker

**Função:** Saída de áudio de alta qualidade para alertas, tons e reprodução de arquivos WAV.

| Pino PCM5102A | GPIO | Notas |
|---------------|------|-------|
| BCK | 40 | Bit Clock (I2S) |
| LRCK | 39 | Word Select (I2S) |
| DIN | 41 | Data Out (I2S) |
| VCC | 3.3V | --- |
| GND | GND | --- |

| Pino PAM8403 | GPIO | Notas |
|--------------|------|-------|
| SHDN | 20 | Enable (HIGH = ON) |
| VCC | 5V | Via Shield V3 |
| IN+ | PCM5102A OUT | --- |
| OUT+ | Speaker | 4Ω 3W |

> [!TIP]
> O PAM8403 é desabilitado durante deep sleep para economizar energia.

---

### ⏰ DS3231 RTC

**Função:** Relógio de tempo real com bateria backup para manter hora mesmo desligado.

| Pino DS3231 | GPIO | Notas |
|-------------|------|-------|
| SDA | 1 | I2C (4.7K pull-up) |
| SCL | 2 | I2C (4.7K pull-up) |
| VCC | 3.3V | --- |
| GND | GND | --- |
| SQW | NC | Não usado |

**Endereço I2C:** `0x68`

---

### 👋 PAJ7620U2 Gesture Sensor

**Função:** Wake-up do deep sleep via gesto, controle de UI sem toque.

| Pino PAJ7620U2 | GPIO | Notas |
|----------------|------|-------|
| SDA | 1 | I2C compartilhado |
| SCL | 2 | I2C compartilhado |
| INT | NC | Pode ser usado para wake |
| VCC | 3.3V | --- |
| GND | GND | --- |

**Endereço I2C:** `0x73`

**Gestos suportados:** Up, Down, Left, Right, Forward, Backward, Wave

---

### 📻 CC1101 SubGHz 433MHz

**Função:** Transmissão e recepção RF para clonagem de controles, jammer, replay attacks.

| Pino CC1101 | GPIO | Notas |
|-------------|------|-------|
| GDO0 | 4 | Interrupt/Data |
| SS | 5 | Chip Select |
| MOSI | 35 | SPI3 (SD Card bus) |
| MISO | 37 | SPI3 (SD Card bus) |
| SCK | 36 | SPI3 (SD Card bus) |
| VCC | 3.3V | Via MOSFET gate |
| GND | GND | --- |

> [!WARNING]
> CC1101 e NRF24 compartilham SPI3. Use apenas um de cada vez com power gating via IRF520.

---

### 🛰️ NEO-6M GPS

**Função:** Localização GPS para wardriving, logging de posição, timestamp preciso.

| Pino GPS | GPIO | Notas |
|----------|------|-------|
| RX | 16 | ESP TX → GPS RX |
| TX | 15 | GPS TX → ESP RX |
| VCC | 3.3V | Via MOSFET gate |
| GND | GND | --- |

**Baud Rate:** 9600

---

### 🌡️ DS18B20 Temp Sensors (x2)

**Função:** Monitoramento térmico de baterias e throttling de CPU.

| Pino DS18B20 | GPIO | Notas |
|--------------|------|-------|
| DQ | 19 | 1-Wire (4.7K pull-up) |
| VCC | 3.3V | --- |
| GND | GND | --- |

> [!IMPORTANT]
> Throttling automático: CPU reduz de 240MHz → 160MHz quando temperatura > 70°C.

---

### 🔴 IR Multi-Zone (5x TSOP38238 + YS-IRTM)

**Função:** Captura IR de 360°, replay, TV-B-Gone.

| Zona | GPIO | Posição |
|------|------|---------|
| Main | 14 | Frontal |
| Zone1 | 21 | Esquerda |
| Zone2 | 42 | Traseira |
| Zone3 | 45 | Direita |
| Zone4 | 20 | Topo |
| TX | 47 | LED Transmissor |

**YS-IRTM (UART2):**

| Pino | GPIO | Notas |
|------|------|-------|
| TX | 17 | Level Shifter 3.3V→5V |
| RX | 18 | Level Shifter 5V→3.3V |

---

### 🌈 WS2812B RGB LEDs (10x)

**Função:** Indicadores de status, feedback visual.

| Parâmetro | Valor |
|-----------|-------|
| GPIO | 48 |
| Quantidade | 10 LEDs |
| Driver | RMT (FastLED) |
| Corrente max | 60mA/LED (600mA total) |

---

## ⚡ Consumo de Energia

| Componente | Idle (mA) | Ativo (mA) | Pico (mA) |
|------------|-----------|------------|-----------|
| ESP32-S3 240MHz | 30 | 100 | 240 |
| PSRAM 120MHz | 5 | 20 | 40 |
| ILI9488 TFT | 20 | 50 | 80 |
| PCM5102A + PAM8403 | 1 | 115 | 630 |
| DS3231 RTC | 0.1 | 0.2 | 0.3 |
| CC1101 TX | 0.5 | 30 | 35 |
| PN532 NFC | 2 | 80 | 150 |
| NEO-6M GPS | 10 | 45 | 70 |
| WS2812B (10) | 1 | 60 | 600 |
| **TOTAL** | ~70 | ~500 | ~1845 |

### Autonomia com 4x 18650 (~10400mAh)

| Modo | Consumo | Autonomia |
|------|---------|-----------|
| Deep Sleep | <5µA | ~240 anos |
| Idle (tela dim) | 80mA | 130 horas |
| Uso Normal | 300mA | 35 horas |
| RF Ativo | 600mA | 17 horas |
| Ataque Máximo | 1500mA | 7 horas |

---

## 🔋 Shield V3 18650 4X

| Característica | Valor |
|----------------|-------|
| Baterias | 4x 18650 |
| Saída 5V | 3A (5 pares) |
| Saída 3.3V | 1A (5 pares) |
| Proteção | BMS integrado |

> [!CAUTION]
> Use baterias de mesma capacidade e marca. NUNCA inverta polaridade!

---

## 📊 Diagrama de Blocos

```
                    ┌─────────────────────────────────────────┐
                    │      ESP32-S3-WROOM-1 N16R8             │
                    │   16MB Flash + 8MB PSRAM @ 240MHz       │
                    │                                         │
    ┌───────────┐   │   ┌─────────┐  ┌───────────┐           │   ┌───────────┐
    │  NEO-6M   │◄──┼───┤ UART1   │  │  UART2    ├───────────┼──►│ YS-IRTM   │
    │   GPS     │   │   └─────────┘  └───────────┘           │   │    IR     │
    └───────────┘   │                                         │   └───────────┘
                    │   ┌─────────────────────────────┐       │
    ┌───────────┐   │   │ I2C Bus (400kHz)            │       │   ┌───────────┐
    │ DS3231    │◄──┼───┤ PN532 + DS3231 + PAJ7620    │       │   │ PCM5102A  │
    │   RTC     │   │   └─────────────────────────────┘       │   │  + PAM8403│
    └───────────┘   │                                         │   │  → Speaker│
                    │   ┌─────────────────────────────┐       │   └─────┬─────┘
    ┌───────────┐   │   │ SPI2 (TFT + Touch)          │       │         │
    │ ILI9488   │◄──┼───┤ MOSI=11, MISO=13, SCK=12    │       │   ┌─────┴─────┐
    │ 3.5" TFT  │   │   └─────────────────────────────┘       │   │   I2S     │
    └───────────┘   │                                         │   │ BCLK=40   │
                    │   ┌─────────────────────────────┐       │   │ LRCK=39   │
    ┌───────────┐   │   │ SPI3 (SD + RF)              │       │   │ DOUT=41   │
    │  SD Card  │◄──┼───┤ MOSI=35, MISO=37, SCK=36    │       │   └───────────┘
    │  128GB    │   │   │                             │       │
    └───────────┘   │   │ ┌────────┐  ┌─────────┐     │       │
                    │   │ │ CC1101 │  │ NRF24   │     │       │
    ┌───────────┐   │   │ │ 433MHz │  │ 2.4GHz  │     │       │   ┌───────────┐
    │ DS18B20   │◄──┼───┤ └────────┘  └─────────┘     │───────┼──►│ IRF520    │
    │  x2 Temp  │   │   └─────────────────────────────┘       │   │ Power Gate│
    └───────────┘   │                                         │   └───────────┘
                    │   ┌─────────────────────────────┐       │
    ┌───────────┐   │   │ IR Zones (5x TSOP38238)     │       │   ┌───────────┐
    │ PN532     │◄──┼───┤ Main=14, Z1=21, Z2=42,      │       │   │ WS2812B   │
    │ NFC/RFID  │   │   │ Z3=45, Z4=20, TX=47         │       │   │ 10 LEDs   │
    └───────────┘   │   └─────────────────────────────┘       │   └───────────┘
                    │                                         │
                    │   ┌─────────────────────────────┐       │
    ┌───────────┐   │   │ 4x 18650 Battery Shield V3  │       │
    │ PAJ7620U2 │◄──┼───┤ 10400mAh @ 3.7V             │       │
    │ Gesture   │   │   │ Saídas: 5V 3A + 3.3V 1A     │       │
    └───────────┘   │   └─────────────────────────────┘       │
                    └─────────────────────────────────────────┘
```

---

## 🛠️ Código de Inicialização

```cpp
#include "hardware/s3_hardware_init.h"

void setup() {
    Serial.begin(115200);
    
    // Initialize ALL hardware at once
    if (!init_all_s3_hardware()) {
        Serial.println("Hardware init failed!");
    }
    
    // Print status summary
    print_hardware_status();
}
```

---

> [!NOTE]
> Este documento substitui a documentação antiga baseada em CYD-2USB/ESP32-WROOM.
> Para referência histórica, consulte `docs/HARDWARE_LEGACY.md`.
