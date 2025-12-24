# Monster S3 - System Reference

> **ATTENTION**: This document replaces all old documentation files.
> All specs updated for ESP32-S3-DevKitC-1 N8R2 (Dec 2025).

---

## Hardware Specifications

| Component | Model | Pins | Status |
|-----------|-------|------|--------|
| MCU | ESP32-S3-WROOM-1 | - | ✅ |
| Flash | 8MB QD | internal | ✅ |
| PSRAM | 2MB QD | internal | ✅ |
| Display | MSP2402 ILI9341 | GPIO 10-14, 21 | ✅ |
| Touch | XPT2046 | GPIO 15-16 | ✅ |
| NFC | PN532 | I2C (8, 9) | ✅ |
| RF | CC1101 | SPI (38-42, 47) | ⚠️ Disabled |
| IR | YS-IRTM | UART (17, 18) | ✅ |
| GPS | GY-NEO6MV2 | UART2 (1, 2) | ✅ |
| RTC | DS3231 | I2C (8, 9) | ✅ |
| Gesture | PAJ7620U2 | I2C (8, 9) | ✅ |
| Audio | PCM5102A | I2S (5, 43, 44) | ⚠️ Disabled |
| SD Card | MicroSD | SPI (39-42) | ✅ |

---

## Complete Pinout

```
GPIO 0  : Boot Button (DO NOT USE)
GPIO 1  : GPS TX → GPS RX
GPIO 2  : GPS RX ← GPS TX  
GPIO 4  : Joystick X (ADC)
GPIO 5  : I2S DOUT
GPIO 6  : Joystick Y (ADC)
GPIO 7  : PN532 Enable (MOSFET)
GPIO 8  : I2C SDA (shared)
GPIO 9  : I2C SCL (shared)
GPIO 10 : TFT CS
GPIO 11 : TFT RST
GPIO 12 : TFT DC
GPIO 13 : TFT MOSI
GPIO 14 : TFT SCLK
GPIO 15 : Touch CS
GPIO 16 : Touch IRQ
GPIO 17 : IR TX
GPIO 18 : IR RX
GPIO 19 : USB D- (RESERVED)
GPIO 20 : USB D+ (RESERVED)
GPIO 21 : TFT Backlight
GPIO 38 : CC1101 CS
GPIO 39 : SD CS
GPIO 40 : SD SCK
GPIO 41 : SD MOSI
GPIO 42 : SD MISO
GPIO 43 : I2S BCK
GPIO 44 : I2S WS
GPIO 47 : CC1101 GDO0
GPIO 48 : RGB LED / Piezo
```

---

## Attack Catalog (27 total)

### BLE (4)
- BLE Spam - Flood BLE advertisements
- Sour Apple - iOS crash packets
- Swift Pair - Windows pairing spam
- Fast Pair - Android pairing spam

### WiFi (5)
- Deauth - 802.11 deauthentication
- Beacon Spam - Fake AP flood
- Evil Twin - Clone AP attack
- PMKID - WPA3 hash capture
- Wardrive - GPS-tagged scanning

### RF SubGHz (10)
- Jammer 433/315/868 MHz
- Burst/Smart jamming
- Signal Capture
- Replay Attack
- Ghost Replay (delayed)
- Brute Force
- De Bruijn sequence
- Spectrum Analyzer

### NFC (3)
- Clone Card - Copy Mifare to file
- Fault Injection - Crash cards
- Phishing - NDEF URL injection

### IR (3)
- TV-B-Gone - Universal TV off
- IR Brute - Brute force codes
- IR Clone - Learn & replay

### USB (2)
- BadUSB - Keyboard injection
- WiFi Exfil - Steal credentials

---

## Web Dashboard

**Connection**: WiFi `Monster_S3` / Pass: `lele2025`
**URL**: http://192.168.4.1
**Auth**: admin / lele2025

---

## DuckyScript Commands

```
DELAY <ms>      - Wait milliseconds
STRING <text>   - Type text
ENTER           - Press Enter
TAB             - Press Tab
GUI r           - Win+R
CTRL c          - Ctrl+C
ALT F4          - Alt+F4
F1-F12          - Function keys
```

---

## SD Card Structure

```
/payloads/
  badusb_default.txt  - Default BadUSB script
  reverse_shell.txt   - Reverse shell
  wifi_exfil.txt      - WiFi dump
  
/nfc/
  clone_*.json        - Cloned cards
  
/captures/
  *.sub               - RF captures (Flipper format)
  
/ir/
  *.ir                - IR signals
```

---

## Known Issues

1. **CC1101 Disabled** - SPI symbol conflict with TFT_eSPI
2. **Audio Disabled** - Library requires C++20
3. **GPS Task** - Commented out in main.cpp

---

## Build Info

```
RAM:   20.2% (66KB / 320KB)
Flash: 41.0% (1.4MB / 3.3MB)
```

---

*Last Updated: 2025-12-23*
