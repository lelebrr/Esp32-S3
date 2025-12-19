# 🔧 ESP32-S3-WROOM-1 N8R8 Pinout Completo

> **Sincronizado com `pin_config.h`**
> Última atualização: 2025-12-19

---

## 📋 Especificações do Microcontrolador

| Característica | Valor |
|----------------|-------|
| **Modelo** | ESP32-S3-WROOM-1 (N8R8) |
| **Flash** | 8MB QIO 80MHz |
| **PSRAM** | 8MB Octal 120MHz (OPI) |
| **CPU** | Dual-core Xtensa LX7 @ 240MHz |
| **WiFi** | 2.4GHz 802.11 b/g/n |
| **Bluetooth** | BLE 5.0 |
| **USB** | Native USB-OTG (CDC) |
| **Deep Sleep** | <5µA |

---

## 📌 Tabela de Pinagem Completa

> [!IMPORTANT]
> Esta tabela reflete exatamente o arquivo `pin_config.h`

### Display TFT ILI9341 (SPI2)

| Função | GPIO | Notas |
|--------|:----:|-------|
| TFT CS | 10 | Chip Select |
| TFT RST | 11 | Reset |
| TFT DC | 12 | Data/Command |
| TFT MOSI | 13 | SPI Data |
| TFT SCLK | 14 | SPI Clock |
| TFT BL | 21 | Backlight PWM |
| TFT MISO | -1 | Não usado |

### Touch XPT2046 (SPI2 compartilhado)

| Função | GPIO | Notas |
|--------|:----:|-------|
| Touch CS | 15 | Chip Select |
| Touch IRQ | 16 | Interrupt |
| CLK/MOSI/MISO | 14/13/13 | Compartilhado com TFT |

### Joystick Shield (Funduino V1.A)

| Função | GPIO | Tipo |
|--------|:----:|------|
| Joystick X | 4 | ADC1_CH3 |
| Joystick Y | 5 | ADC1_CH4 |
| Joystick SW | 6 | Digital |
| Button A (↑) | 41 | Digital |
| Button B (→) | 42 | Digital |
| Button C (↓) | 1 | Digital |
| Button D (←) | 2 | Digital |
| Button E | 17 | Digital |
| Piezo (+) | 20 | Audio/PWM (Was Button F) |

### SD Card (SPI3)

| Função | GPIO | Notas |
|--------|:----:|-------|
| SD CS | 14 | Chip Select (compartilhado SCLK) |
| SD MOSI | 11 | Data Out |
| SD MISO | 13 | Data In |
| SD SCK | 12 | Clock |

### I2C Bus

| Função | GPIO | Dispositivos |
|--------|:----:|--------------|
| I2C SDA | 8 | PN532, DS3231, PAJ7620U2 |
| I2C SCL | 9 | 4.7K pull-up recomendado |

### NFC PN532

| Função | GPIO | Notas |
|--------|:----:|-------|
| PN532 EN | 7 | MOSFET Power Gate |
| PN532 IRQ | -1 | Não usado |
| SDA/SCL | 8/9 | I2C compartilhado |

### SubGHz CC1101 (SPI3)

| Função | GPIO | Notas |
|--------|:----:|-------|
| CC1101 CS | 46 | Chip Select |
| CC1101 GDO0 | 47 | Interrupt/Status |
| CC1101 EN | 48 | MOSFET Power Gate |
| MOSI/MISO/SCK | 11/13/12 | Compartilhado SD |

### I2S Audio (PCM5102A)

| Função | GPIO | Notas |
|--------|:----:|-------|
| I2S BCK | 40 | Bit Clock |
| I2S WS | 39 | Word Select (LRCK) |
| I2S DOUT | 41 | Data Out |

### Infravermelho

| Função | GPIO | Notas |
|--------|:----:|-------|
| IR TX | 3 | LED Transmissor |
| IR RX | -1 | Não usado nesta config |

### USB Serial

| Função | GPIO |
|--------|:----:|
| USB TX | 43 |
| USB RX | 44 |

### Status LEDs

| Função | GPIO | Notas |
|--------|:----:|-------|
| LED Attack High | RGB_BUILTIN | LED interno ESP32-S3 |
| LED Attack Low | RGB_BUILTIN | LED interno ESP32-S3 |
| LED AI | RGB_BUILTIN | LED interno ESP32-S3 |
| LED Sleep | RGB_BUILTIN | LED interno ESP32-S3 |

### Fault Injection

| Função | GPIO | Notas |
|--------|:----:|-------|
| MOSFET Gate | -1 | Não usado nesta config |

---

## 📊 Diagrama de Conexões

```text
┌─────────────────────────────────────────────────────────────────┐
│                    ESP32-S3-WROOM-1 N8R8                        │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │ DISPLAY     │    │ TOUCH       │    │ SD CARD     │         │
│  │ ILI9341     │    │ XPT2046     │    │ MicroSD     │         │
│  │ CS=10       │    │ CS=15       │    │ CS=14       │         │
│  │ RST=11      │    │ IRQ=16      │    │ MOSI=11     │         │
│  │ DC=12       │    └─────────────┘    │ MISO=13     │         │
│  │ MOSI=13     │                       │ SCK=12      │         │
│  │ SCLK=14     │                       └─────────────┘         │
│  │ BL=21       │                                                │
│  └─────────────┘                                                │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    I2C BUS (SDA=8, SCL=9)               │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────────┐              │   │
│  │  │ PN532   │  │ DS3231  │  │  PAJ7620U2  │              │   │
│  │  │ NFC     │  │  RTC    │  │   Gesture   │              │   │
│  │  │ EN=7    │  │ 0x68    │  │    0x73     │              │   │
│  │  └─────────┘  └─────────┘  └─────────────┘              │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │ JOYSTICK    │    │ CC1101      │    │ I2S AUDIO   │         │
│  │ X=4, Y=5    │    │ SubGHz      │    │ PCM5102A    │         │
│  │ SW=6        │    │ CS=46       │    │ BCK=40      │         │
│  │ A=41 B=42   │    │ GDO0=47     │    │ WS=39       │         │
│  │ C=1  D=2    │    │ EN=48       │    │ DOUT=41     │         │
│  │ E=17 F=18   │    └─────────────┘    └─────────────┘         │
│  └─────────────┘                                                │
│                                                                 │
│  ┌─────────────┐                                                │
│  │ IR TX=3     │                                                │
│  └─────────────┘                                                │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔗 Documentação Relacionada

| Documento | Módulo |
|-----------|--------|
| [MASTER_SYSTEM.md](MASTER_SYSTEM.md) | Visão geral do sistema |
| [PN532_NFC_MODULE_V3.md](PN532_NFC_MODULE_V3.md) | NFC/RFID |
| [CC1101_MODULE.md](CC1101_MODULE.md) | SubGHz RF |
| [PAJ7620_GESTURE_SENSOR.md](PAJ7620_GESTURE_SENSOR.md) | Gestos |
| [AUDIO_SYSTEM.md](AUDIO_SYSTEM.md) | Áudio I2S |
| [JOYSTICK_DISPLAY_PINOUT.md](JOYSTICK_DISPLAY_PINOUT.md) | Joystick + Display |
| [HARDWARE.md](HARDWARE.md) | Hardware geral |

---

> [!NOTE]
> Este documento reflete o arquivo `include/pin_config.h`.
> Qualquer alteração no código deve ser refletida aqui.
