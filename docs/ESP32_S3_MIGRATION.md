# ESP32-S3 Migration Guide

Complete migration documentation from ESP32 WROOM-32 to ESP32-S3-WROOM-1 N16R8.

**Generated:** 2025-12-19  
**Target:** ESP32-S3-WROOM-1 N16R8 (16MB Flash + 8MB PSRAM Octal)

---

## Overview

This migration upgrades the Cyd_Lele project from ESP32 classic to ESP32-S3, adding:

- **4x more Flash** (4MB → 16MB)
- **PSRAM Octal** (8MB @ 80MHz)
- **Native USB** (no FTDI chip)
- **CPU temperature sensor** (thermal throttling)
- **New peripherals:** I2S DAC, RTC, gesture sensor, multi-zone IR

---

## Hardware Comparison

| Feature | ESP32 Classic | ESP32-S3 N16R8 |
|---------|---------------|----------------|
| Flash | 4MB | 16MB |
| PSRAM | None | 8MB OPI |
| CPU | 240MHz dual | 240MHz dual |
| USB | UART bridge | Native CDC |
| Temp sensor | ❌ | ✅ |
| Touch pins | 10 | 14 |
| GPIO count | 34 | 45 |

---

## Complete Pinout Table

### SPI2 - Display + Touch

| Signal | GPIO | Function |
|--------|------|----------|
| TFT_MOSI | 11 | Display data |
| TFT_MISO | 13 | Display read |
| TFT_SCK | 12 | Display clock |
| TFT_CS | 10 | Display select |
| TFT_DC | 46 | Data/Command |
| TFT_RST | 9 | Reset |
| TFT_BL | 48 | Backlight PWM |
| TOUCH_CS | 3 | Touch select |
| TOUCH_IRQ | 8 | Touch interrupt |

### SPI3 - SD Card + RF

| Signal | GPIO | Function |
|--------|------|----------|
| SD_MOSI | 35 | SD data out |
| SD_MISO | 37 | SD data in |
| SD_SCK | 36 | SD clock |
| SD_CS | 38 | SD select |
| CC1101_GDO0 | 4 | CC1101 IRQ |
| CC1101_CS | 5 | CC1101 select |
| NRF24_CE | 6 | NRF24 enable |
| NRF24_CS | 7 | NRF24 select |

### I2C Bus (400kHz)

| Device | Address | GPIO |
|--------|---------|------|
| PN532 NFC | 0x24 | SDA=1, SCL=2 |
| DS3231 RTC | 0x68 | SDA=1, SCL=2 |
| PAJ7620U2 | 0x73 | SDA=1, SCL=2 |

### I2S DAC - PCM5102A

| Signal | GPIO |
|--------|------|
| BCLK | 40 |
| LRCK (WS) | 39 |
| DOUT | 41 |

### UARTs

| UART | Function | TX | RX |
|------|----------|----|----|
| UART0 | USB CDC | 43 | 44 |
| UART1 | GPS NEO-6M | 16 | 15 |
| UART2 | YS-IRTM IR | 17 | 18 |

### IR Multi-Zone (5x TSOP38238)

| Zone | GPIO |
|------|------|
| Main | 14 |
| Zone 1 | 21 |
| Zone 2 | 42 |
| Zone 3 | 45 |
| Zone 4 | 0 (boot) |
| TX | 47 |

### Sensors & Control

| Function | GPIO |
|----------|------|
| DS18B20 1-Wire | 47 |
| WS2812B LEDs | 48 |
| Buzzer | 45 |
| MOSFET Driver | 42 |
| Battery ADC | 14 |
| Boot Button | 0 |

---

## Power Consumption

| State | Current | Duration |
|-------|---------|----------|
| Deep Sleep | <5µA | 14+ days (4x 18650) |
| Light Sleep | ~800µA | Days |
| Idle (80MHz) | ~25mA | Hours |
| Active (240MHz) | ~80mA | ~8 hours |
| WiFi TX burst | +100mA | ms |
| BLE advertising | +30mA | Varies |
| All RF max | <300mA | Budget OK |

---

## Build Instructions

```bash
# Clean build
pio run -t clean -e CYD-S3-N16R8

# Build
pio run -e CYD-S3-N16R8

# Upload
pio run -t upload -e CYD-S3-N16R8

# Monitor
pio device monitor -e CYD-S3-N16R8
```

---

## Expected Boot Log

```text
[S3] PSRAM: 8388608 bytes OK
[S3] Temperature sensor initialized
[I2C] Bus initialized at 400kHz (SDA=1, SCL=2)
[RTC] DS3231 detected at 0x68
[GESTURE] PAJ7620U2 detected at 0x73
[NFC] PN532 detected at 0x24
[I2S] PCM5102A DAC initialized (44.1kHz 16-bit stereo)
[TEMP] DS18B20: 2 sensors found on GPIO47
[BUZZER] Initialized on GPIO45
[MOSFET] Initialized on GPIO42

=== Hardware Detection Summary ===
[CPU] ESP32-S3 @ 240MHz
[PSRAM] 8300000 / 8388608 bytes free
[RTC] DS3231: OK
[GESTURE] PAJ7620: OK
[NFC] PN532: OK
[TEMP] DS18B20: 2 sensors
```

---

## Troubleshooting

### PSRAM Not Detected

- Check `sdkconfig.defaults` has `CONFIG_SPIRAM_MODE_OCT=y`
- Verify N16R8 module (not N8R2 or N4)
- Clean build: `pio run -t clean`

### Touch Not Working

- Check SPI2 connections (GPIO 10-13, 46, 9)
- Verify TOUCH_CS (GPIO 3) is wired

### RTC Not Detected

- Check I2C wiring (SDA=1, SCL=2)
- Verify CR2032 battery installed
- Use 4.7kΩ pull-ups if needed

### I2S Audio Issues

- Verify PCM5102A wiring (BCLK=40, LRCK=39, DOUT=41)
- Check FLT and DEMP pins are grounded
- Verify SCK tied to GND (internal PLL)
