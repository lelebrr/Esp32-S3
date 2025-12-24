# Monster S3 Firmware

## ESP32-S3-DevKitC-1 N8R2 Pentest Device

Complete firmware for offensive security toolkit supporting 27+ attack vectors.

---

## Quick Start

```bash
# Clone and build
pio run

# Upload to device
pio run -t upload

# Monitor serial output
pio device monitor
```

---

## Hardware

| Component | Model | Interface |
|-----------|-------|-----------|
| MCU | ESP32-S3-WROOM-1 | - |
| Display | MSP2402 ILI9341 2.4" | SPI (FSPI) |
| Touch | XPT2046 | SPI (shared) |
| NFC | PN532 | I2C |
| RF | CC1101 | SPI (HSPI) |
| IR | YS-IRTM | UART |
| GPS | GY-NEO6MV2 | UART2 |
| RTC | DS3231 | I2C |
| Audio | PCM5102A | I2S |
| Gesture | PAJ7620U2 | I2C |

---

## Attack Categories

| Category | Count | Status |
|----------|-------|--------|
| BLE | 4 | ✅ Working |
| WiFi | 5 | ✅ Working |
| RF SubGHz | 10 | ⚠️ Stub (CC1101 conflict) |
| NFC | 3 | ✅ Real PN532 |
| IR | 3 | ✅ YS-IRTM |
| USB | 2 | ✅ Real HID |

---

## Web Dashboard

- **SSID**: `Monster_S3` (hidden)
- **Password**: `lele2025`
- **URL**: http://192.168.4.1

### API Endpoints

| Endpoint | Description |
|----------|-------------|
| `GET /api/status` | System status |
| `GET /api/attack/{type}` | Start attack |
| `GET /api/stop` | Stop all attacks |
| `GET /api/nfc/read` | Read NFC card |
| `POST /api/usb/payload` | Execute USB script |

---

## Pin Configuration

See [pin_config.h](include/pin_config.h) for complete pinout.

---

## License

Educational use only. Use responsibly.
