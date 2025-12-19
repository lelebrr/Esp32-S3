# ESP32-S3-WROOM-1 (N16R8) - Pinagem Definitiva

**Project**: Lele Origin S3 Monster
**Chip**: ESP32-S3-WROOM-1 (16MB Flash / 8MB Octal PSRAM)
**Status**: Validado para evitar conflitos OPI vs SPI.

## Resumo de Conflitos OPI Resolvidos

O módulo N16R8 usa PSRAM Octal. Isso consome os pinos GPIO 33-37 para comunicação interna de alta velocidade com a RAM.
**NÃO USE ESTES PINOS para periféricos externos:**

- GPIO 35 (SPI MOSI antigo) -> **MUDADO PARA 11**
- GPIO 36 (SPI SCK antigo) -> **MUDADO PARA 12**
- GPIO 37 (SPI MISO antigo) -> **MUDADO PARA 13**
- GPIO 33 (IO Generico) -> Reservado
- GPIO 34 (IO Generico) -> Reservado

## Mapa de Pinos (Final)

| Periférico | Função | GPIO S3 | Notas |
|------------|--------|---------|-------|
| **SPI BUS 1** (Display/Touch) | MOSI | 11 | Compartilhado |
| | MISO | 13 | Compartilhado |
| | SCK | 12 | Compartilhado |
| | CS (TFT) | 10 | Dedicado |
| | DC (TFT) | 9 | Dedicado |
| | CS (Touch) | 15 | Dedicado (ex-3) |
| **SPI BUS 2** (SD/RF) | MOSI | 11 | Compartilhado |
| | MISO | 13 | Compartilhado |
| | SCK | 12 | Compartilhado |
| | CS (SD) | 14 | Dedicado (Safe Boot) |
| | CS (CC1101) | 5 | Opcional |
| **I2S Audio** | BCLK | 40 | PCM5102A |
| | LRCK (WS) | 39 | PCM5102A |
| | DOUT | 41 | PCM5102A |
| **I2C Bus** | SDA | 1 | PN532, RTC, Gestos |
| | SCL | 2 | PN532, RTC, Gestos |
| **LEDs** | Red High | 4 | Attack Feedback |
| | Red Low | 0 | Idle/Boot |
| | Blue 1 | 6 | AI Thinking |
| | Blue 2 | 7 | Sleep Mode |
| **Controles** | Button | 33 | Fault Trigger (Checar conflito OPI - usar 0 se der erro) |
| | MOSFET | 42 | Fault Injection Power |
| **Switches** | SW1 | 15 | Stealth UART1 |
| | SW2 | 16 | Stealth UART2 |
| | SW3 | 14 | Atk RF (Checar conflito SD CS - se usar SD evitar SW3) |
| | SW4 | 17 | Atk NFC |
| | SW5 | 18 | AI Gesture |
| | SW6 | 19 | AI RTC |

> **Nota Crítica**: GPIO 33 pode estar ligado ao PSRAM OPI em alguns módulos N16R8. Se o botão de Fault falhar ou causar crash, mova para GPIO 3 ou 46.

## Power Domain

- **VCC 3.3V**: Alimentar ESP32 e Sensores.
- **VCC 5V**: Alimentar Audio Amp (PAM8403) e LED Strip (se houver).
- **MOSFET**: Controla VCC do PN532 (5V ou 3.3V gate dependent).

## Configuração PlatformIO

Flags obrigatórias para esta pinagem:

```ini
-DTFT_MOSI=11
-DTFT_MISO=13
-DTFT_SCLK=12
-DTFT_CS=10
-DTFT_DC=9
-DTOUCH_CS=15
-DSDCARD_CS=14
-DI2S_DOUT=41
-DI2S_BCLK=40
-DI2S_LRCK=39
```
