# 🔊 Piezo Electric Transducer Module

> **Status**: Integrated
> **Pin**: GPIO 20 (Replaces Aux Button F)
> **Driver**: `PiezoDriver`

---

## 📋 Overview

The Piezo Transducer (20mm) is a simple audio output device used for alerts, feedback beeps, and acoustic "attacks" (high-frequency annoyance).

## 🔌 Connection

| Piezo Pin | ESP32-S3 GPIO | Notes |
|-----------|---------------|-------|
| **Red (+)** | **20** | Switched from Button F |
| **Black (-)** | **GND** | Common Ground |

> [!NOTE]
> Connected directly to GPIO. For louder volume, a transistor driver would be needed, but direct GPIO is sufficient for simple proximity alerts.

---

## 🎵 Features & Attacks

### 1. Standard Feedback

- **`tone(freq, dur)`**: Play specific frequency.
- **`beep()`**: Simple UI feedback beep.
- **`playBootSound()`**: melody played on startup.
- **`playErrorSound()`**: Low-frequency error buzz.

### 2. Acoustic Attacks ("Attacks" Menu)

- **🚨 Siren**: Cycles frequencies between 1kHz and 3kHz rapidly. Good for alerting or annoying.
- **🦟 Mosquito**: Emits a 17kHz high-frequency tone. Often inaudible to older adults but annoying to teenagers/young adults.
- **🎲 Noise**: Emits random chirps and beeps at efficient intervals to cause distraction.

---

## 💻 Code Usage

The module is available globally as `Piezo`.

```cpp
#include "s3_driver.h"

// Simple Beep
Piezo.beep();

// Siren Attack (Blocking unless orchestrated)
Piezo.siren(true); 
// ... wait ...
Piezo.siren(false);
```
