# 🔧 ESP32-S3-WROOM-1 N16R8 Pinout Completo

> **Tabela de pinagem atualizada com novos componentes de hardware.**
> Última atualização: 2025-12-19

---

## 📋 Especificações do Microcontrolador

| Característica | Valor |
|----------------|-------|
| **Modelo** | ESP32-S3-WROOM-1 (N16R8) |
| **Flash** | 16MB QIO 80MHz |
| **PSRAM** | 8MB Octal 120MHz (OPI) |
| **CPU** | Dual-core Xtensa LX7 @ 240MHz |
| **WiFi** | 2.4GHz 802.11 b/g/n |
| **Bluetooth** | BLE 5.0 |
| **USB** | Native USB-OTG (CDC) |
| **Deep Sleep** | <5µA |

---

## 📌 Tabela de Pinagem Completa (Atualizada)

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                        ESP32-S3-WROOM-1 N16R8 PINOUT                          ║
╠═══════╦══════════════════════╦════════════════════════════════════════════════╣
║ GPIO  ║ Função               ║ Módulo / Notas                                 ║
╠═══════╬══════════════════════╬════════════════════════════════════════════════╣
║   0   ║ LED Ready Low        ║ LED vermelho baixo brilho (pronto p/ uso)     ║
║   1   ║ I2C SDA              ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)        ║
║   2   ║ I2C SCL              ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)        ║
║   3   ║ Touch CS             ║ XPT2046                                        ║
║   4   ║ LED Attack High      ║ LED vermelho alto brilho (ataque/falha)       ║
║   5   ║ CC1101 SS / Joy VRY  ║ SubGHz SPI + ADC                               ║
║   6   ║ LED AI Blue          ║ LED azul alto brilho (IA ativa)               ║
║   7   ║ LED Sleep Blue       ║ LED azul alto brilho (dormindo)               ║
║   8   ║ Touch IRQ            ║ XPT2046                                        ║
║   9   ║ TFT RST              ║ ILI9488                                        ║
║  10   ║ TFT CS               ║ ILI9488                                        ║
║  11   ║ SPI2 MOSI            ║ TFT + Touch (compartilhado)                    ║
║  12   ║ SPI2 SCK             ║ TFT + Touch (compartilhado)                    ║
║  13   ║ SPI2 MISO            ║ TFT + Touch (compartilhado)                    ║
║  14   ║ IR RX Main / VBAT    ║ TSOP38238 #1 + Battery ADC                     ║
║  15   ║ GPS RX / Stealth SW  ║ NEO-6M TX + Switch preto UART2 stealth         ║
║  16   ║ GPS TX / Stealth SW  ║ NEO-6M RX + Switch preto UART1 stealth         ║
║  17   ║ YS-IRTM TX / Attack  ║ IR Module UART2 + Switch vermelho PN532        ║
║  18   ║ YS-IRTM RX / AI SW   ║ IR Module UART2 + Switch verde PAJ7620U2       ║
║  19   ║ 1-Wire / AI SW       ║ DS18B20 x2 + Switch verde DS3231               ║
║  20   ║ PAM8403 EN / IR Z4   ║ Amp Enable + TSOP38238 #5                      ║
║  21   ║ IR RX Zone1          ║ TSOP38238 #2                                   ║
║  33   ║ Fault Inject Button  ║ Push button curto (fault injection manual)     ║
║  35   ║ SD MOSI              ║ SD Card (SPI3)                                 ║
║  36   ║ SD SCK               ║ SD Card (SPI3)                                 ║
║  37   ║ SD MISO              ║ SD Card (SPI3)                                 ║
║  38   ║ SD CS                ║ SD Card (SPI3)                                 ║
║  39   ║ I2S LRCK             ║ PCM5102A Word Select                           ║
║  40   ║ I2S BCLK             ║ PCM5102A Bit Clock                             ║
║  41   ║ I2S DOUT             ║ PCM5102A Data                                  ║
║  42   ║ IR RX Zone2 / MOSFET ║ TSOP38238 #3 + IRF520 Gate (PN532 power)       ║
║  43   ║ USB TX               ║ CDC Serial                                     ║
║  44   ║ USB RX               ║ CDC Serial                                     ║
║  45   ║ IR RX Zone3 / Buzzer ║ TSOP38238 #4 + PWM Beep                        ║
║  46   ║ TFT DC               ║ ILI9488 Data/Command                           ║
║  47   ║ IR TX                ║ LED IR Transmitter                             ║
║  48   ║ TFT BL / WS2812B     ║ Backlight PWM + RGB LEDs (10)                  ║
╚═══════╩══════════════════════╩════════════════════════════════════════════════╝
```

---

## 🔄 Switches por GPIO

| GPIO | Switch | Cor | Função |
|------|--------|-----|--------|
| 15 | Preto | Stealth UART2 | Desliga UART2 (GPS YS-IRTM) |
| 16 | Preto | Stealth UART1 | Desliga UART1 (GPS NEO-6M) |
| 14 | Vermelho | Attack CC1101 | Liga CC1101 SubGHz |
| 17 | Vermelho | Attack PN532 | Liga PN532 NFC/RFID |
| 18 | Verde | AI PAJ7620U2 | Liga sensor gesto |
| 19 | Verde | AI DS3231 | Liga RTC |

---

## 💡 LEDs por GPIO

| GPIO | LED | Cor | Estado | Função |
|------|-----|-----|--------|--------|
| 0 | Baixo brilho | Vermelho | HIGH | Pronto para uso |
| 4 | Alto brilho | Vermelho | HIGH | Ataque ativo / Falha |
| 6 | Alto brilho | Azul | HIGH | IA ativa |
| 7 | Alto brilho | Azul | HIGH | Deep sleep ativo |

---

## 🔌 Conectores Especiais

| Pino | Função | Notas |
|------|--------|-------|
| 33 | Fault Injection | Push button curto, pulso 5ms MOSFET |
| 42 | MOSFET IRF520 | Gate para power gating PN532 |

---

> [!NOTE]
> Switches funcionam com pull-up interno. LOW = ligado, HIGH = desligado.
> LEDs alto brilho piscam durante operação ativa.
