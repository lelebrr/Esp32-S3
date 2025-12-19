# 📍 ESP32-S3-WROOM-1 N16R8 - Pinout Completo

> **Mapeamento completo de GPIO, buses e conexões de módulos**  
> **Atualizado:** 2025-12-19

---

## 📋 Especificações do Módulo

| Recurso | Valor |
|---------|-------|
| **MCU** | ESP32-S3 Dual-Core Xtensa LX7 @ 240MHz |
| **Flash** | 16MB QIO 80MHz |
| **PSRAM** | 8MB Octal SPI @ 120MHz |
| **GPIOs** | 45 disponíveis |
| **ADC** | 20 canais (ADC1: GPIO1-10, ADC2: GPIO11-20) |
| **Touch** | 14 canais capacitivos |
| **USB** | Nativo OTG (GPIO19/20) |

---

## 🔌 Alocação Completa de GPIOs

```
┌──────────────────────────────────────────────────────────────────┐
│                    ESP32-S3-WROOM-1 N16R8                        │
│                       GPIO Allocation                            │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  GPIO 0  ─── BTN_BOOT / IR_RX_ZONE4 (shared)                    │
│  GPIO 1  ─── I2C SDA (PN532, DS3231, PAJ7620U2)                 │
│  GPIO 2  ─── I2C SCL (PN532, DS3231, PAJ7620U2)                 │
│  GPIO 3  ─── TOUCH_CS (XPT2046)                                 │
│  GPIO 4  ─── CC1101_GDO0 (IRQ)                                  │
│  GPIO 5  ─── CC1101_CS                                          │
│  GPIO 6  ─── NRF24_CE                                           │
│  GPIO 7  ─── NRF24_CS                                           │
│  GPIO 8  ─── TOUCH_IRQ / WAKE_PIN                               │
│  GPIO 9  ─── TFT_RST                                            │
│  GPIO 10 ─── TFT_CS (SPI2)                                      │
│  GPIO 11 ─── TFT_MOSI (SPI2)                                    │
│  GPIO 12 ─── TFT_SCK (SPI2)                                     │
│  GPIO 13 ─── TFT_MISO (SPI2)                                    │
│  GPIO 14 ─── IR_RX_MAIN / VBAT_ADC                              │
│  GPIO 15 ─── GPS_RX (UART1)                                     │
│  GPIO 16 ─── GPS_TX (UART1)                                     │
│  GPIO 17 ─── IRM_TX (UART2 - YS-IRTM)                           │
│  GPIO 18 ─── IRM_RX (UART2 - YS-IRTM)                           │
│  GPIO 19 ─── USB_D- (Native)                                    │
│  GPIO 20 ─── USB_D+ (Native)                                    │
│  GPIO 21 ─── IR_RX_ZONE1                                        │
│  GPIO 35 ─── SD_MOSI (SPI3)                                     │
│  GPIO 36 ─── SD_SCK (SPI3)                                      │
│  GPIO 37 ─── SD_MISO (SPI3)                                     │
│  GPIO 38 ─── SD_CS (SPI3)                                       │
│  GPIO 39 ─── I2S_LRCK (PCM5102A)                                │
│  GPIO 40 ─── I2S_BCLK (PCM5102A)                                │
│  GPIO 41 ─── I2S_DOUT (PCM5102A)                                │
│  GPIO 42 ─── MOSFET_PIN (IRF520) / IR_RX_ZONE2                  │
│  GPIO 43 ─── UART0_TX (USB CDC)                                 │
│  GPIO 44 ─── UART0_RX (USB CDC)                                 │
│  GPIO 45 ─── BUZZER / IR_RX_ZONE3                               │
│  GPIO 46 ─── TFT_DC                                             │
│  GPIO 47 ─── IR_TX / DS18B20 (time-multiplexed)                 │
│  GPIO 48 ─── TFT_BL / WS2812B (RGB LEDs)                        │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

---

## 🚌 Buses e Interfaces

### SPI2 - Display + Touch

| Sinal | GPIO | Função |
|-------|------|--------|
| TFT_MOSI | 11 | Dados para display |
| TFT_MISO | 13 | Leitura display |
| TFT_SCK | 12 | Clock display |
| TFT_CS | 10 | Chip select display |
| TFT_DC | 46 | Data/Command |
| TFT_RST | 9 | Reset |
| TFT_BL | 48 | Backlight PWM |
| TOUCH_CS | 3 | Chip select touch |
| TOUCH_IRQ | 8 | Interrupção touch |

### SPI3 - SD Card + RF Modules

| Sinal | GPIO | Função |
|-------|------|--------|
| SD_MOSI | 35 | Dados saída |
| SD_MISO | 37 | Dados entrada |
| SD_SCK | 36 | Clock |
| SD_CS | 38 | SD chip select |
| CC1101_CS | 5 | CC1101 chip select |
| CC1101_GDO0 | 4 | CC1101 IRQ |
| NRF24_CE | 6 | NRF24 enable |
| NRF24_CS | 7 | NRF24 chip select |

> [!NOTE]
> CC1101 e NRF24 compartilham o bus SPI3. Use apenas um por vez ou implemente seleção via CS.

### I2C Bus (400kHz)

| Dispositivo | Endereço | GPIO SDA | GPIO SCL |
|-------------|----------|----------|----------|
| PN532 NFC | 0x24 | 1 | 2 |
| DS3231 RTC | 0x68 | 1 | 2 |
| PAJ7620U2 Gesture | 0x73 | 1 | 2 |

> [!TIP]
> Use resistores pull-up de 4.7kΩ em SDA e SCL para maior estabilidade.

### I2S - PCM5102A DAC

| Sinal | GPIO | Função |
|-------|------|--------|
| BCLK | 40 | Bit clock |
| LRCK (WS) | 39 | Word select |
| DOUT | 41 | Data out |

### UARTs

| UART | Função | TX | RX | Baud |
|------|--------|----|----|------|
| UART0 | USB CDC | 43 | 44 | 115200 |
| UART1 | GPS NEO-6M | 16 | 15 | 9600 |
| UART2 | YS-IRTM IR | 17 | 18 | 9600 |

> [!WARNING]
> YS-IRTM opera em 5V. Use level shifter (3.3V ↔ 5V) nas linhas TX/RX.

---

## 📡 IR Multi-Zone (5x TSOP38238)

```
┌──────────────────────────────────────────────────────────────┐
│                    IR Multi-Zone Layout                       │
│                                                              │
│    ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐      │
│    │ZONE4│    │ZONE1│    │MAIN │    │ZONE2│    │ZONE3│      │
│    │ G0  │    │ G21 │    │ G14 │    │ G42 │    │ G45 │      │
│    └──┬──┘    └──┬──┘    └──┬──┘    └──┬──┘    └──┬──┘      │
│       │          │          │          │          │          │
│       ▼          ▼          ▼          ▼          ▼          │
│   [TSOP38238] [TSOP38238] [TSOP38238] [TSOP38238] [TSOP38238]│
│                                                              │
│                     IR TX (GPIO 47)                          │
│                          │                                   │
│                     ┌────▼────┐                              │
│                     │ IR LED  │                              │
│                     └─────────┘                              │
└──────────────────────────────────────────────────────────────┘
```

| Zona | GPIO | Observação |
|------|------|------------|
| Main | 14 | Receptor principal (ADC compartilhado) |
| Zone 1 | 21 | Receptor auxiliar |
| Zone 2 | 42 | Compartilhado com MOSFET |
| Zone 3 | 45 | Compartilhado com buzzer |
| Zone 4 | 0 | Compartilhado com boot button |
| TX | 47 | Transmissor (compartilhado DS18B20) |

---

## 🔋 Power Management

| Função | GPIO | Descrição |
|--------|------|-----------|
| MOSFET_PIN | 42 | IRF520 gate - controla módulos de alta potência |
| VBAT_ADC | 14 | ADC para leitura de tensão da bateria |
| WAKE_PIN | 8 | Interrupção para acordar de deep sleep |

### Módulos Controlados pelo MOSFET

- PAM8403 (amplificador de áudio)
- CC1101 (RF SubGHz)
- NRF24L01 (RF 2.4GHz)
- GPS NEO-6M

---

## 🌡️ Sensores

| Sensor | GPIO | Interface | Função |
|--------|------|-----------|--------|
| DS18B20 | 47 | 1-Wire | Temperatura (thermal throttling) |
| PAJ7620U2 | 1, 2 | I2C | Gestos (wake from deep sleep) |
| VBAT | 14 | ADC1_CH3 | Tensão da bateria |

---

## 🔊 Áudio

```
┌─────────────────────────────────────────────────────────────┐
│                    Audio Signal Flow                         │
│                                                             │
│  ESP32-S3          PCM5102A          PAM8403        Speaker │
│  ┌──────┐         ┌────────┐        ┌───────┐      ┌─────┐ │
│  │ I2S  │──BCLK──→│        │──L/R──→│       │──────│ 4Ω  │ │
│  │ DMA  │──LRCK──→│  DAC   │        │ 3W    │      │ 3W  │ │
│  │      │──DOUT──→│        │        │ Amp   │      │     │ │
│  └──────┘         └────────┘        └───────┘      └─────┘ │
│   G40,39,41         Analog           Class D        Output  │
└─────────────────────────────────────────────────────────────┘
```

---

## 💡 LEDs

| LED | GPIO | Tipo | Quantidade |
|-----|------|------|------------|
| WS2812B RGB | 48 | Endereçável | 10 unidades |
| Backlight TFT | 48 | PWM | N/A (compartilhado) |

> [!NOTE]
> GPIO 48 é compartilhado entre WS2812B e backlight. O firmware multiplexa automaticamente.

---

## ⚡ Consumo de Energia

| Estado | Corrente | CPU | Duração (4x 18650) |
|--------|----------|-----|-------------------|
| Deep Sleep | <5µA | Off | 14+ dias |
| Light Sleep | ~800µA | Idle | Dias |
| Idle 80MHz | ~25mA | 80MHz | ~16h |
| Ativo 240MHz | ~80mA | 240MHz | ~8h |
| WiFi TX Burst | +100mA | 240MHz | Momentâneo |
| Todos RF Max | <300mA | 240MHz | ~3h |

---

## 📌 Notas Importantes

> [!WARNING]
> **Tensão:** Todos os GPIOs operam em 3.3V. Use level shifter para módulos 5V (YS-IRTM).

> [!CAUTION]
> **GPIOs Compartilhados:** Alguns pinos têm múltiplas funções. Verifique a tabela de alocação antes de conectar.

> [!TIP]
> **Pull-ups:** Use 4.7kΩ para I2C e 1-Wire. O firmware ativa pull-ups internos quando disponíveis.
