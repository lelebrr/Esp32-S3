# Monster S3 - Hardware Documentation

> **Documentação sincronizada com `pin_config.h`**
> Para visão completa do sistema: [MASTER_SYSTEM.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/MASTER_SYSTEM.md)

## Hardware Specifications

| Component | Specification |
|-----------|---------------|
| MCU | ESP32-S3-WROOM-1 N8R8 |
| Flash | 8MB |
| PSRAM | 8MB Octal |
| Display | ILI9341 240x320 TFT SPI (MSP2402) |
| Touch | XPT2046 Resistive |
| Frequency | 240MHz Dual Core |

---

## Display Connection (ILI9341 MSP2402)

| LCD Pin | Function | ESP32-S3 GPIO |
|---------|----------|---------------|
| VCC | Power | 3.3V |
| GND | Ground | GND |
| CS | Display Chip Select | GPIO 10 |
| RESET | Display Reset | GPIO 11 |
| DC | Data/Command | GPIO 12 |
| SDI/MOSI | SPI Data In | GPIO 13 |
| SCK | SPI Clock | GPIO 14 |
| LED | Backlight | GPIO 21 |
| SDO/MISO | SPI Data Out | -1 (não usado) |

### Touch Pins (XPT2046)

| LCD Pin | Function | ESP32-S3 GPIO |
|---------|----------|---------------|
| T_CLK | Touch Clock | GPIO 14 (shared) |
| T_CS | Touch Chip Select | GPIO 15 |
| T_DIN | Touch Data In | GPIO 13 (shared) |
| T_DO | Touch Data Out | -1 (not used) |
| T_IRQ | Touch Interrupt | GPIO 16 |

---

## SD Card Connection (SPI B - HSPI)

> [!TIP]
> Documentação completa e vetores de ataque: [MICROSD_ADAPTER.md](MICROSD_ADAPTER.md)

| SD Pin | Function | ESP32-S3 GPIO |
|--------|----------|:-------------:|
| CS | Chip Select | GPIO 39 |
| SCK | Clock | GPIO 40 |
| MOSI | Master Out Slave In | GPIO 41 |
| MISO | Master In Slave Out | GPIO 42 |
| VCC | Power | 3.3V / 5V |
| GND | Ground | GND |

> [!NOTE]
> HSPI dedicated bus provides 40MHz+ throughput for aggressive boot and fast logging.

---

## I2C Bus

| Function | ESP32-S3 GPIO |
|----------|---------------|
| SDA | GPIO 8 |
| SCL | GPIO 9 |

---

## RF Modules

### NFC (PN532 V3 - I2C)

> Documentação completa: [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| SDA | GPIO 8 (shared) |
| SCL | GPIO 9 (shared) |
| Power Enable | GPIO 7 (MOSFET) |
| I2C Address | 0x24 |

### SubGHz (CC1101 - SPI)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| CS | GPIO 37 |
| GDO0 | GPIO 47 |
| Power Enable | GPIO 48 |
| MOSI/MISO/SCK | SD Card bus (shared) |

### Piezo Buzzer

| Function | ESP32-S3 GPIO |
|----------|---------------|
| Buzzer | GPIO 20 |

---

## Gesture Sensor (PAJ7620U2)

> [!TIP]
> Documentação completa: [PAJ7620_GESTURE_SENSOR.md](PAJ7620_GESTURE_SENSOR.md)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| SDA | GPIO 8 (I2C compartilhado) |
| SCL | GPIO 9 (I2C compartilhado) |
| INT | -1 (modo polling) |

**Características:**

- Interface I2C (400kHz max), Endereço: 0x73
- 9 gestos reconhecidos (Up/Down/Left/Right/Forward/Backward/CW/CCW/Wave)
- Distância de detecção: 5-15cm
- Usado para wake from deep sleep e controle touch-free

---

## Audio (I2S - PCM5102A DAC)

| Function | ESP32-S3 GPIO |
|----------|---------------:|
| BCK | GPIO 43 |
| WS/LRCK | GPIO 44 |
| DOUT | GPIO 38 |

---

## GPS (GY-NEO6MV2 - UART2)

> [!TIP]
> Documentação completa: [GPS_NEO6MV2.md](GPS_NEO6MV2.md)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| RX (ESP ← GPS TX) | GPIO 17 |
| TX (ESP → GPS RX) | GPIO 18 |
| Baud Rate | 9600 |

**Características:**

- Chip u-blox NEO-6M, protocolo NMEA 0183
- TTFF: Cold 27s, Warm 1s
- Precisão: 2.5m CEP
- Usado para wardriving (WiFi/BLE/RF com coordenadas GPS)

---

## Infrared (YS-IRTM Module)

> [!TIP]
> Documentação completa: [HARDWARE_YS_IRTM.md](HARDWARE_YS_IRTM.md)

| Function | ESP32-S3 GPIO | Note |
|----------|---------------:|------|
| TX (ESP -> Module) | GPIO 3 | UART TX |
| RX (ESP <- Module) | GPIO 27 | UART RX |

---

## Joystick Shield V1.A (Funduino)

> [!TIP]
> Documentação completa: [JOYSTICK_DISPLAY_PINOUT.md](JOYSTICK_DISPLAY_PINOUT.md)

### Joystick Analógico

| Function | ESP32-S3 GPIO | Type |
|----------|---------------|------|
| Eixo X (VRx) | GPIO 4 | ADC1_CH3 |
| Eixo Y (VRy) | GPIO 5 | ADC1_CH4 |
| Click (SW) | GPIO 6 | Digital |

### Botões Direcionais

| Button | ESP32-S3 GPIO | Position |
|--------|---------------|----------|
| A | GPIO 35 | Cima ↑ |
| B | GPIO 36 | Direita → |
| C | GPIO 1 | Baixo ↓ |
| D | GPIO 2 | Esquerda ← |

### Botões Auxiliares

| Button | ESP32-S3 GPIO | Position |
|--------|---------------|----------|
| E | GPIO 19 | Centro-Esq |
| ~~F~~ | ~~GPIO 20~~ | Removido → Piezo |

> [!IMPORTANT]
> **Button F was removed** - GPIO 20 reassigned to Piezo Buzzer for audible feedback.

### Conectores Externos

| Connector | Pinos Shield | ESP32-S3 GPIOs |
|-----------|--------------|----------------|
| I2C | SCL,SDA,GND,+5V | GPIO8,GPIO9 |
| Serial | RX,TX,GND,VCC | GPIO44,GPIO43 |
| nRF24L01 | - | Já configurado |

---

## Status LEDs

| Function | ESP32-S3 GPIO |
|----------|---------------|
| Attack Active (High) | RGB_BUILTIN |
| Attack Standby (Low) | RGB_BUILTIN |
| AI Processing | RGB_BUILTIN |
| Deep Sleep | RGB_BUILTIN |

---

## Fault Injection

| Function | ESP32-S3 GPIO |
|----------|---------------|
| MOSFET Gate | -1 (not used) |

---

## RTC (DS3231 - I2C)

> [!TIP]
> Real-Time Clock de alta precisão com bateria de backup e sensor de temperatura.
> Documentação completa: [DS3231_RTC.md](DS3231_RTC.md)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| SDA | GPIO 8 (shared) |
| SCL | GPIO 9 (shared) |
| SQW/INT | -1 (não usado) |
| I2C Address | 0x68 |
| EEPROM Addr | 0x57 (AT24C32) |

---

## Libraries Used

| Library | Version | Purpose |
|---------|---------|---------|
| TFT_eSPI | 2.5.43 | Display Driver |
| LVGL | 8.3.7 | UI Framework |
| NimBLE-Arduino | 1.4.3 | BLE |
| ESPAsyncWebServer | 1.2.4 | Web Dashboard |
| IRremoteESP8266 | 2.8.6 | IR Attacks |
| SmartRC-CC1101 | 2.5.7 | SubGHz |
| Adafruit PN532 | 1.3.4 | NFC |
| TinyGPSPlus | 1.1.0 | GPS |
| ArduinoJson | 7.4.2 | JSON |
| RevEng_PAJ7620 | 1.5.1 | Gesture Sensor |

---

## Memory Usage

| Resource | Used | Available | Percentage |
|----------|------|-----------|------------|
| Flash | 1.4MB | 3MB | 46% |
| RAM | 64KB | 320KB | 20% |
| PSRAM | Available | 8MB | - |

---

**Last Updated:** 2025-12-21
