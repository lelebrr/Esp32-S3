# YS-IRTM Infrared Module Documentation

## Overview

The YS-IRTM is a compact infrared transmitter and receiver module that uses a dedicated MCU to handle NEC infrared protocol encoding and decoding. It communicates with the host microcontroller via a standard TTL serial interface (UART).

## Specifications

- **Voltage**: 3.3V - 5.5V (Recommended 5V for better range)
- **Current**: >100mA (Tx), ~17.5mA (Standby)
- **Frequency**: 38 KHz
- **Wavelength**: 940nm
- **Communication**: UART (TTL)
- **Protocol**: NEC Format (PD6121/6122 compatible)
- **Default Baud Rate**: 9600 bps
- **Default Address**: 0xA1

## Pin Configuration

| YS-IRTM Pin | Function | ESP32-S3 GPIO | Description |
|---|---|---|---|
| **VCC** | Power | 5V | Power Supply (Direct from 5V rail) |
| **GND** | Ground | GND | Common Ground |
| **RXD** | UART RX | GPIO 3 (Tx) | Receive Data from MCU (Commands) |
| **TXD** | UART TX | GPIO 27 (Rx) | Transmit Data to MCU (IR Codes) |

> [!WARNING]
> The module's RXD pin connects to ESP32 TX, and TXD connects to ESP32 RX.

## UART Communication Protocol

The module uses 5-byte command packets for transmission and control.

### Packet Format

`[Address] [Operation] [Data1] [Data2] [Data3]`

### Commands (Operation Byte)

#### 1. Transmit IR Code (0xF1)

Sends an NEC formatted IR signal.

- **Address**: User Device Address (Default 0xA1) or Universal 0xFA
- **Op**: `0xF1`
- **Data1**: NEC User Code High Byte
- **Data2**: NEC User Code Low Byte
- **Data3**: Command Code
*Note: The module automatically generates the inverse command byte.*

**Example**: Send Device 0x00, Command 0x12
`A1 F1 00 00 12`

#### 2. Modify Address (0xF2)

Changes the module's UART address.

- **Op**: `0xF2`
- **Data1**: New Address (0x01 - 0xFF)
- **Data2**: 0x00
- **Data3**: 0x00

#### 3. Modify Baud Rate (0xF3)

Changes the UART communication speed.

- **Op**: `0xF3`
- **Data1**: Baud Rate Index
  - `1`: 4800
  - `2`: 9600 (Default)
  - `3`: 19200
  - `4`: 57600
- **Data2**: 0x00
- **Data3**: 0x00

## Receiving IR Codes

When the module detects a valid NEC IR signal, it automatically sends 3 bytes to the serial port:
`[User Code High] [User Code Low] [Command Code]`

## Physical Dimensions

- Size: 2.7cm x 1.7cm x 1.3cm

## Integration Notes

- **NEC Only**: This module **only** supports the NEC protocol. It cannot send or receive RAW timing data for other protocols like Sony SIRC or RC5/6.
- **Buffers**: The ESP32 HardwareSerial buffer is sufficient for normal operation.
- **Power**: High peak current during transmission. Ensure 5V rail can supply >100mA bursts.

## Attack Vectors

1. **IR Cloning**: Capture NEC codes from remotes and replay them.
2. **Brute Force**: Iterate through command codes (0x00-0xFF) for a specific address.
3. **TV-B-Gone (NEC)**: Send known power codes for NEC-based TVs.
