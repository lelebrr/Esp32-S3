# Paranoid Secure Boot Protocol

**Security Level:** PARANOID  
**Version:** 1.0  
**Implementation:** `src/core/secure_boot.cpp`

## Overview

This device is equipped with a high-security "Paranoid" bootloader sequence designed to prevent hardware damage, ensure firmware integrity, and maintain stealth reliability. This system runs *before* the main OS initializes.

## Boot Sequence

### 1. Hard-Reset Safe Mode

- **Trigger**: Hold `BOOT` button (GPIO 0) during power-on.
- **Action**:
  - Disables Overclocking (Forces 80MHz).
  - Disables advanced thermal checks.
  - Forces verbose logging.
  - Enters an infinite "Safe Mode" loop blinking status LED.

### 2. Thermal Defense

- **Check**: Reads internal ESP32 temperature sensor (using `driver/temp_sensor`).
- **Threshold**: **70°C**.
- **Response**:
  - Immediate throttle to **80MHz**.
  - Log event: `QUENTE PRA CARALHO. DESLIGA.`
  - Force **Deep Sleep**.

### 3. Aggressive SD Watchdog

- **Objective**: Prevent "hang on boot" due to faulty SD cards.
- **Mechanism**: Dedicated FreeRTOS task spawns to mount SD.
- **Timeout**: **500ms** strict.
- **Failure Response**:
  - Terminates mount task instantly.
  - Attempts fallback mount at **10MHz** (safe SPI).
  - If still dead, switches filesystem to Internal Flash (`LittleFS`) to allow boot (without external assets).

### 4. Firmware Integrity (Anti-Tamper)

- **Mechanism**: Calculates **CRC32** of the active firmware partition at runtime.
- **Verification**: Compares against `/sd/hash.txt`.
- **Failure Response**:
  - Logs `INTEGRITY FAIL`.
  - Triggers `esp_ota_mark_app_invalid_rollback_and_reboot()`.
  - Rolls back to the previous known good firmware slot.

### 5. Shielded Overclocking

- **Standard**: 160MHz boot.
- **Stress Test**: Runs 10 cycles of integer checksums.
- **Boost**: Only if stable, engages **240MHz** and acquires `esp_pm_lock` (Power Management Lock) to prevent frequency scaling drops.

### 6. Power Monitor

- **Input**: ADC on GPIO 34 (Battery Voltage Divider).
- **Threshold**: **< 3.3V** (Approx raw value < 2000).
- **Response**:
  - Saves critical state (logs).
  - Enters **Deep Sleep**.
  - Wake timer set to 3 seconds (simulates "long press to wake").

## Logging

- **Path**: `/sd/crash.log`
- **Rotation**: Auto-truncates if > 20KB to prevent storage exhaustion.
- **Format**: `Millis | Temp | Reason`

## Developer Notes

- **Watchdog**: The system feeds the task watchdog (`esp_task_wdt_reset`) every loop cycle via `SecureBoot::loopCheck()`.
- **Compilation**: `SecureBoot::run()` replaces the old `aggressive_boot_logic()`.
