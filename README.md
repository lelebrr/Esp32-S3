# 🦎 Monster S3 - Arsenal Completo de Pentesting

<p align="center">
  <strong>Firmware ESP32-S3 com arsenal completo de ferramentas de segurança ofensiva</strong><br>
  <em>Atualizado: 2025-12-21</em>
</p>

---

## 📋 Sobre

O **Monster S3** é um firmware ESP32-S3 com arsenal completo para operações de segurança ofensiva, incluindo ferramentas avançadas de pentesting. Badge Black Hat completo em um dispositivo ESP32-S3.

### 🔧 Hardware Principal: ESP32-S3-WROOM-1 N8R8

| Recurso | Especificação |
|---------|---------------|
| **MCU** | ESP32-S3 Dual-Core 240MHz Xtensa LX7 |
| **Flash** | 8MB QIO 80MHz |
| **PSRAM** | 8MB Octal SPI 120MHz |
| **Display** | ILI9341 2.8" 320x240 TFT + Touch XPT2046 |
| **Áudio** | PCM5102A DAC I2S |
| **RTC** | DS3231 com bateria CR2032 |
| **Sensores** | PAJ7620U2 (gestos 9 direções) |
| **RF** | CC1101 (315/433/868MHz) |
| **NFC** | PN532 I2C (MIFARE, NTAG) |
| **IR** | YS-IRTM UART (NEC Protocol) |
| **GPS** | NEO-6M UART2 |

---

## ✨ Arsenal de Ataques

| Categoria | Ataques Disponíveis |
|-----------|---------------------|
| 📡 **WiFi** | Deauth, Beacon Spam, Evil Twin, Wardriving |
| 📶 **BLE** | Spam, Sour Apple, Swift Pair, Fast Pair |
| 🔀 **RF SubGHz** | Jammer (433/315/868), Capture, Replay, Ghost Replay, Brute Force, Spectrum Analyzer |
| 💳 **NFC** | Clone, Phishing, Fault Injection |
| 🔴 **IR** | TV-B-Gone (NEC), Clone |
| 🔌 **USB** | BadUSB, Exfiltration |

**Total: 30+ vetores de ataque**

---

## 📂 Estrutura do Projeto

```
src/
├── main.cpp              # Ponto de entrada, FreeRTOS tasks
├── attacks_manager.cpp   # Gerenciador central de ataques
├── rf_core.cpp           # CC1101 RF completo (783 linhas)
├── gesture_sensor.cpp    # PAJ7620U2 9 gestos
├── gps_driver.cpp        # GPS NEO-6M + wardriving
├── lvgl_menu.cpp         # Interface LVGL com touch + gestos
├── s3_driver.cpp         # HAL do hardware
├── rf_menu.cpp           # Menu RF com seleção de frequência
├── gps_menu.cpp          # Interface GPS LVGL
├── wardriving.cpp        # Logger WiFi/BLE com GPS
├── q_learn_ia.cpp        # Q-Learning IA adaptativa
├── rtc_driver.cpp        # DS3231 RTC
├── YsIrtm.cpp            # Driver IR NEC
├── web_dashboard.cpp     # Dashboard HTTP
├── core/
│   └── aggressive_sd.cpp # Boot SD prioritário via HSPI
└── modules/
    └── piezo_driver.cpp  # Buzzer e sons

include/
├── pin_config.h          # Definições de pinos GPIO
├── rf_core.h             # API RF (jammers, capture, replay, brute)
├── gesture_sensor.h      # API gestos 9 direções + cursor mode
├── gps_driver.h          # API GPS + exportação KML
├── attacks_manager.h     # Enum AttackType com 30+ ataques
├── s3_driver.h           # HAL class MonsterDriver
├── globals.h             # Estado global e structs
├── precompiler_flags.h   # Flags de compilação
└── lv_conf.h             # Configuração LVGL
```

---

## 🔧 Instalação

### Compilando com PlatformIO

```bash
# Instalar PlatformIO
pip install platformio

# Compilar
cd "Esp32-S3"
pio run

# Upload
pio run -t upload

# Monitor Serial
pio device monitor
```

### Ambiente: `Monster_S3` (platformio.ini)

- **Board:** `esp32-s3-devkitc-1`
- **Partition:** `custom_8Mb_S3.csv`
- **PSRAM:** Octal 8MB
- **USB CDC:** Habilitado

---

## 📚 Documentação

| Documento | Descrição |
|-----------|-----------|
| [HARDWARE.md](docs/HARDWARE.md) | Diagrama de hardware |
| [ESP32_S3_COMPLETE_PINOUT.md](docs/ESP32_S3_COMPLETE_PINOUT.md) | Pinout completo |
| [FUNCIONALIDADES.md](docs/FUNCIONALIDADES.md) | Lista de funcionalidades |
| [GPS_MODULE.md](docs/GPS_MODULE.md) | Módulo GPS |
| [CC1101_MODULE.md](docs/CC1101_MODULE.md) | Módulo RF SubGHz |

---

## 🔌 Pinos GPIO (Resumo)

| Função | Pinos |
|--------|-------|
| **Display SPI** | CS=10, DC=12, RST=11, MOSI=13, SCK=14, BL=21 |
| **Touch** | CS=15, IRQ=16 |
| **SD Card (HSPI)** | CS=39, SCK=40, MOSI=41, MISO=42 |
| **CC1101** | CS=46, GDO0=47, EN=48 |
| **I2C** | SDA=8, SCL=9 |
| **GPS UART2** | RX=17, TX=18 |
| **IR YS-IRTM** | TX=3, RX=27 |
| **I2S Audio** | BCK=43, WS=44, DOUT=38 |
| **Joystick** | X=4, Y=5, SW=6 |

---

## ⚠️ Aviso Legal

O Monster S3 é uma ferramenta para **testes de segurança autorizados**.
O uso para atividades maliciosas ou não autorizadas é **estritamente proibido**.

Distribuído sob licença **AGPL**. Use por sua conta e risco.

---

**Versão:** Monster S3 v1.0  
**Data:** 2025-12-21

