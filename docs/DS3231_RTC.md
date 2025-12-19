# DS3231 Real-Time Clock Module

## Overview

The **DS3231** is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature-compensated crystal oscillator (TCXO) and crystal. The device incorporates a battery input, and maintains accurate timekeeping when main power to the device is interrupted.

> [!IMPORTANT]
> **Battery Safety Warning:** Many cheap DS3231 modules (like the ZS-042) come with a charging circuit designed for LIR2032 rechargeable batteries. If you use a standard CR2032 non-rechargeable battery, **you MUST disable the charging circuit** by removing the diode or resistor (often labeled D2 or R5) to prevent the battery from swelling or exploding.

## Specifications

- **Accuracy:** ±2ppm from 0°C to +40°C (approx. ±1 minute per year)
- **Interface:** I2C (Address `0x68`)
- **Voltage:** 3.3V to 5.5V (Compatible with ESP32 3.3V logic)
- **Memory:** Integrated AT24C32 EEPROM (32K bits) on address `0x57` (default)
- **Functions:**
  - Seconds, Minutes, Hours, Day, Date, Month, Year
  - Leap-year compensation up to 2100
  - Two integrated alarms
  - Programmable Square-Wave Output
  - Integrated Temperature Sensor (±3°C accuracy)

## Pinout and Wiring

| DS3231 Pin | ESP32-S3 Pin | Function | Notes |
| :--- | :--- | :--- | :--- |
| **VCC** | 3.3V | Power | Supply Voltage |
| **GND** | GND | Ground | |
| **SDA** | 8 | I2C Data | Shared Bus |
| **SCL** | 9 | I2C Clock | Shared Bus |
| **SQW** | -1 (NC) | Interrupt/Square Wave | Can be connected to wake ESP32 |
| **32K** | NC | 32kHz Output | Usually unused |

## Integration in Monster S3

### Driver Implementation

The driver is implemented in `src/rtc_driver.cpp` and `include/rtc_driver.h`, utilizing the standard `RTClib`.

- **Initialization:** Checks I2C presence on address `0x68`.
- **Timekeeping:** Provides `getDateTimeString()` for logs.
- **Temperature:** Exposes internal temperature sensor via `getTemperature()`.
- **Alarms:** Supports setting Alarm 1 and Alarm 2.

### Use Cases & "Attacks"

1. **Precision Timestamping (Wardriving Fallback)**
    - *Mechanism:* When GPS is cold or indoor (invalid fix), the system falls back to RTC time for logging WiFi/BLE packets.
    - *Benefit:* Ensures `wardriving_wifi.csv` logs always have valid timestamps for correlation analysis.

2. **Scheduled Wake/Attacks (Sleep Mode)**
    - *Mechanism:* The DS3231 can pull the **SQW** pin LOW when an alarm triggers. This can be wired to an ESP32 GPIO to wake the system from Deep Sleep.
    - *Implementation:*

        ```cpp
        // Set alarm for 03:00 AM
        RTCDriver::setAlarm1(3, 0, 0, DS3231_A1_Hour);
        // Enter Deep Sleep with Ext1 wakeup on SQW pin
        ```

3. **Redundant Time Authority**
    - The RTC acts as the "Guardian of Time", correcting the ESP32's internal `millis()` drift over long uptime periods.

## Component Image

![DS3231 Module](https://cdn.awsli.com.br/78/78150/produto/6899793/bcad33985a.jpg)

## Troubleshooting

- **Device Not Found:** Check wiring (SDA/SCL swapped?) and ensure pulling resistors (usually built-in to the module).
- **Time Resets on Power Loss:** Battery is dead or missing.
- **High Temperature Reading:** Module might be heating up if charging circuit is active with non-rechargeable battery.
