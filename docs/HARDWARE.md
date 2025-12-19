# Monster S3 - Hardware Documentation

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
| CS | Display Chip Select | GPIO 5 |
| RESET | Display Reset | GPIO 4 |
| DC | Data/Command | GPIO 6 |
| SDI/MOSI | SPI Data In | GPIO 36 |
| SCK | SPI Clock | GPIO 35 |
| LED | Backlight | GPIO 3 |
| SDO/MISO | SPI Data Out | GPIO 37 |

### Touch Pins (XPT2046)

| LCD Pin | Function | ESP32-S3 GPIO |
|---------|----------|---------------|
| T_CLK | Touch Clock | GPIO 35 (shared) |
| T_CS | Touch Chip Select | GPIO 38 |
| T_DIN | Touch Data In | GPIO 36 (shared) |
| T_DO | Touch Data Out | GPIO 37 (shared) |
| T_IRQ | Touch Interrupt | GPIO 39 |

---

## SD Card Connection

| SD Pin | ESP32-S3 GPIO |
|--------|---------------|
| CS | GPIO 10 |
| MOSI | GPIO 11 |
| SCK | GPIO 12 |
| MISO | GPIO 13 |

---

## I2C Bus

| Function | ESP32-S3 GPIO |
|----------|---------------|
| SDA | GPIO 8 |
| SCL | GPIO 9 |

---

## RF Modules

### NFC (PN532 - I2C)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| Power Enable | GPIO 7 |
| IRQ | GPIO 15 |

### SubGHz (CC1101 - SPI)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| CS | GPIO 1 |
| GDO0 | GPIO 2 |
| Power Enable | GPIO 14 |

---

## Audio (I2S)

| Function | ESP32-S3 GPIO |
|----------|---------------|
| BCK | GPIO 16 |
| WS/LRCK | GPIO 17 |
| DOUT | GPIO 18 |

---

## Infrared

| Function | ESP32-S3 GPIO |
|----------|---------------|
| TX | GPIO 21 |
| RX | GPIO 47 |

---

## Joystick Shield V1.A (Funduino)

> [!TIP]
> Documentação completa: [JOYSTICK_DISPLAY_PINOUT.md](JOYSTICK_DISPLAY_PINOUT.md)

### Joystick Analógico

| Function | ESP32-S3 GPIO | Type |
|----------|---------------|------|
| Eixo X (VRx) | GPIO 4 | ADC1_CH3 |
| Eixo Y (VRy) | GPIO 5 | ADC1_CH4 |
| Click (SW) | GPIO 0 | Digital |

> [!CAUTION]
> GPIO0 é o pino de Boot! Não pressionar durante upload.

### Botões Direcionais

| Button | ESP32-S3 GPIO | Position |
|--------|---------------|----------|
| A | GPIO 41 | Cima |
| B | GPIO 42 | Direita |
| C | GPIO 14 | Baixo |
| D | GPIO 15 | Esquerda |

### Botões Auxiliares

| Button | ESP32-S3 GPIO | Position |
|--------|---------------|----------|
| E | GPIO 16 | Centro-Esq |
| F | GPIO 17 | Centro-Dir |

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
| Attack Active (High) | GPIO 45 |
| Attack Standby (Low) | GPIO 46 |
| AI Processing | GPIO 48 |
| Deep Sleep | RGB_BUILTIN |

---

## Fault Injection

| Function | ESP32-S3 GPIO |
|----------|---------------|
| MOSFET Gate | GPIO 40 |

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

---

## Memory Usage

| Resource | Used | Available | Percentage |
|----------|------|-----------|------------|
| Flash | 1.4MB | 3MB | 46% |
| RAM | 64KB | 320KB | 20% |
| PSRAM | Available | 8MB | - |

---

*Updated: 2025-12-19*
