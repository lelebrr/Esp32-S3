# 🔒 Lele Origin Security & Paranoid Mode Guide

## 1. Overview

The "Paranoid Mode" is a set of aggressive security features designed to prevent tampering, ensure firmware integrity, and maintain a "Zero Trace" operational footprint. This system is designed for high-risk environments where physical capture of the device is a threat.

## 2. Core Security Pillars

### 🛡️ 1. Aggressive SD Boot (Zero Persistence)

**Mechanism:**

- On every boot, the system mounts the SD card via low-level ESP-IDF VFS.
- It wipes the internal `LittleFS` partition (Flash memory).
- It loads configuration **ONLY** from the SD card.
- **Traceability:** Zero. No Wi-Fi credentials, API keys, or logs are stored on the internal chip. If the SD card is removed/destroyed, the device contains only the firmware, with no user data.

### 🔐 2. Firmware Integrity (Secure Boot)

**Mechanism:**

- **CRC32 Check:** Upon boot, the device calculates the CRC32 checksum of the running firmware partition.
- **Verification:** It compares this checksum against a `saved_hash.txt` stored on the SD card.
- **Action:** If the hashes mismatch (indicating tampered firmware or corruption), the device triggers a **Rollback** or refuses to boot (Panic Mode).

### 🌡️ 3. Physical Health Monitoring

- **Thermal Kill-Switch:** Real-time monitoring of ESP32 die temperature. If T > 70°C, the system enters deep sleep to prevent damage or heat-based side-channel attacks.
- **Voltage Check:** Monitors input voltage. If critical (< 3.3V), it triggers an emergency shutdown to prevent brownout-induced undefined behavior.

### 👻 4. Zero Trace Architecture

- **Config Enforcement:** The `saveFile()` function is hardcoded to **BLOCK** writing to internal Flash.
- **Volatile Execution:** Runtime data is kept in RAM/PSRAM.
- **Wipe-on-Boot:** As mentioned, any residue in Flash is formatted on next power-up.

## 3. Usage

### Activating Paranoid Mode

Paranoid features are active by default in the `main.cpp` boot sequence:

```cpp
aggressive_boot_logic(); // Wipes Flash
SecureBoot::run();       // Checks Integrity & Physical Health
```

### Hash Generation

To authorize a new firmware version:

1. Flash the new firmware.
2. The system may fail the first check if hash is missing.
3. Calculate the CRC32 of valid firmware.
4. Save it to `/sd/hash.txt` on the SD card.

### Emergency Interventions

- **Overheating:** System sleep. Cool down and reboot.
- **Tamper Detect:** If `Integrity Fail` occurs, re-flash known good firmware or verify SD card contents.
