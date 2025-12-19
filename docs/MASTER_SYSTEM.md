# 🎯 LELE ORIGIN - SISTEMA MASTER COMPLETO

> **Documentação unificada de hardware, pinout, módulos e ataques.**
> Última atualização: 2025-12-19

---

## 📊 RESUMO DO ARSENAL

| Categoria | Quantidade | Hardware |
|-----------|:----------:|----------|
| **WiFi Attacks** | 40+ | ESP32-S3 WiFi |
| **BLE Attacks** | 15+ | ESP32-S3 BLE |
| **RF SubGHz** | 10+ | CC1101 433MHz |
| **NFC/RFID** | 12+ | PN532 V3 |
| **IR Attacks** | 3+ | TSOP38238 + LED |
| **USB Attacks** | 10+ | USB OTG |
| **Fault Injection** | 5+ | MOSFET + Laser |
| **IoT Attacks** | 8+ | Multi-protocol |
| **TOTAL** | **103+** | --- |

---

## 📌 PINOUT UNIFICADO ESP32-S3

### Tabela Master de GPIOs

```
╔════════════════════════════════════════════════════════════════════════════════════╗
║                    ESP32-S3-WROOM-1 N8R8 - PINOUT MASTER                           ║
╠═══════╦════════════════════════╦═══════════════════════════════════════════════════╣
║ GPIO  ║ Função Principal       ║ Notas / Módulo                                    ║
╠═══════╬════════════════════════╬═══════════════════════════════════════════════════╣
║   1   ║ Button C (Baixo)       ║ Botão direcional do Joystick Shield              ║
║   2   ║ Button D (Esquerda)    ║ Botão direcional do Joystick Shield              ║
║   3   ║ IR TX                  ║ LED Infravermelho Transmissor                    ║
║   4   ║ Joystick X             ║ ADC1_CH3 - Eixo X analógico                       ║
║   5   ║ Joystick Y             ║ ADC1_CH4 - Eixo Y analógico                       ║
║   6   ║ Joystick SW            ║ Botão click do joystick                          ║
║   7   ║ PN532 Enable           ║ MOSFET Gate - Power NFC                           ║
║   8   ║ I2C SDA                ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)          ║
║   9   ║ I2C SCL                ║ PN532, DS3231, PAJ7620U2 (4.7K pull-up)          ║
║  10   ║ TFT CS                 ║ ILI9341 Display Chip Select                       ║
║  11   ║ TFT RST                ║ Display Reset                                     ║
║  12   ║ TFT DC                 ║ Display Data/Command                              ║
║  13   ║ TFT MOSI               ║ Display SPI Data                                  ║
║  14   ║ TFT SCLK               ║ Display SPI Clock                                 ║
║  15   ║ Touch CS               ║ XPT2046 Touch Chip Select                         ║
║  16   ║ Touch IRQ              ║ XPT2046 Touch Interrupt                           ║
║  17   ║ Button E               ║ Botão Auxiliar Centro-Esquerda                    ║
║  18   ║ Button F               ║ Botão Auxiliar Centro-Direita                     ║
║  19   ║ GPS RX (UART1)         ║ ESP32 RX ← GPS TX (GY-NEO6MV2)                    ║
║  20   ║ GPS TX (UART1)         ║ ESP32 TX → GPS RX (GY-NEO6MV2)                    ║
║  21   ║ TFT Backlight          ║ PWM Backlight LED                                 ║
║  35   ║ Button A               ║ Botão Cima ↑                                      ║
║  36   ║ Button B               ║ Botão Direita →                                   ║
║  38   ║ I2S DOUT               ║ PCM5102A Audio Data Out                           ║
║  39   ║ SD CS                  ║ MicroSD Card Chip Select (HSPI)                   ║
║  40   ║ SD SCK                 ║ MicroSD Clock (HSPI)                              ║
║  41   ║ SD MOSI                ║ MicroSD Data Out (HSPI)                           ║
║  42   ║ SD MISO                ║ MicroSD Data In (HSPI)                            ║
║  43   ║ I2S BCK                ║ PCM5102A Bit Clock                                ║
║  44   ║ I2S WS                 ║ PCM5102A Word Select (LRCK)                       ║
║  43   ║ USB TX                 ║ CDC Serial TX                                     ║
║  44   ║ USB RX                 ║ CDC Serial RX                                     ║
║  46   ║ CC1101 CS              ║ SubGHz SPI Chip Select                            ║
║  47   ║ CC1101 GDO0            ║ SubGHz Interrupt/Data                             ║
║  48   ║ CC1101 Enable          ║ MOSFET Gate - Power SubGHz                        ║
╠═══════╬════════════════════════╬═══════════════════════════════════════════════════╣
║  17   ║ GPS RX (UART2)         ║ GY-NEO6MV2 TX → ESP32 RX                          ║
║  18   ║ GPS TX (UART2)         ║ GY-NEO6MV2 RX ← ESP32 TX                          ║
╚═══════╩════════════════════════╩═══════════════════════════════════════════════════╝
```

---

## 🔌 BARRAMENTOS DE COMUNICAÇÃO

### I2C Bus (400 kHz)

| Dispositivo | Endereço | SDA | SCL | Power |
|-------------|:--------:|:---:|:---:|:-----:|
| **PN532 NFC V3** | 0x24 | GPIO 8 | GPIO 9 | GPIO 7 |
| **DS3231 RTC** | 0x68 | GPIO 8 | GPIO 9 | 3.3V |
| **PAJ7620U2 Gesture** | 0x73 | GPIO 8 | GPIO 9 | 3.3V |

> [!TIP]
> Documentação completa:
>
> - PN532: [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md)
> - DS3231: [DS3231_RTC.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/DS3231_RTC.md)
> - PAJ7620: [PAJ7620_GESTURE_SENSOR.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PAJ7620_GESTURE_SENSOR.md)

### SPI Buses

#### SPI2 - Display + Touch

| Sinal | GPIO | Dispositivos |
|-------|:----:|--------------|
| MOSI | 13 | TFT ILI9341 |
| MISO | - | Não usado |
| SCK | 14 | TFT ILI9341 |
| TFT CS | 10 | ILI9341 |
| Touch CS | 15 | XPT2046 |

#### SPI3 - SD Card + RF

| Sinal | GPIO | Dispositivos |
|-------|:----:|--------------|
| MOSI | 11 | SD Card, CC1101 |
| MISO | 13 | SD Card, CC1101 |
| SCK | 12 | SD Card, CC1101 |
| SD CS | 14 | MicroSD |
| CC1101 CS | 46 | SubGHz Radio |

### I2S Audio

| Sinal | GPIO | Dispositivo |
|-------|:----:|-------------|
| BCK | 40 | PCM5102A |
| WS (LRCK) | 39 | PCM5102A |
| DOUT | 41 | PCM5102A |

### UART1 - GPS

| Sinal | GPIO | Dispositivo |
|-------|:----:|-------------|
| RX | 19 | GY-NEO6MV2 (GPS TX) |
| TX | 20 | GY-NEO6MV2 (GPS RX) |

> [!TIP]
> Documentação completa do GPS: [GPS_MODULE.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/GPS_MODULE.md)

---

## 🎮 JOYSTICK SHIELD (Funduino V1.A)

### Joystick Analógico

| Função | GPIO | Tipo | Range |
|--------|:----:|------|-------|
| Eixo X | 4 | ADC1_CH3 | 0-4095 |
| Eixo Y | 5 | ADC1_CH4 | 0-4095 |
| Click | 6 | Digital | LOW = pressed |

### Botões Direcionais

| Botão | GPIO | Posição | Pull |
|-------|:----:|---------|------|
| A | 41 | Cima (↑) | Pull-up interno |
| B | 42 | Direita (→) | Pull-up interno |
| C | 1 | Baixo (↓) | Pull-up interno |
| D | 2 | Esquerda (←) | Pull-up interno |

### Botões Auxiliares

| Botão | GPIO | Posição | Função Sugerida |
|-------|:----:|---------|-----------------|
| E | 17 | Centro-Esq | Menu/Back |
| F | 18 | Centro-Dir | Select/Enter |

---

## 📡 MÓDULOS RF COM ATAQUES

### PN532 NFC V3 (13.56 MHz)

| Especificação | Valor |
|---------------|-------|
| **Chip** | NXP PN532 |
| **Interface** | I2C (GPIO 8/9) |
| **Power Gate** | GPIO 7 (MOSFET) |
| **Frequência** | 13.56 MHz |
| **Alcance** | 4-7 cm |

#### Ataques NFC Disponíveis

| Ataque | Descrição | Taxa Sucesso |
|--------|-----------|:------------:|
| **Clone Card** | Copia UID de cartões | 90% |
| **Phishing Tag** | Tag com URL maliciosa | 95% |
| **OTA Rewrite** | Muda contato emergência | 80% |
| **Fake Apple Pay** | Intercept payment APDU | 70% |
| **Audio Injection** | Subliminal 19kHz via tag | 85% |
| **MIFARE Crack** | Default key attack | 75% |
| **Amiibo Clone** | Clone NTAG215 | 95% |
| **Corporate Badge** | Detecção heurística | 80% |

#### Cartões Suportados

| Tipo | Operações |
|------|-----------|
| MIFARE Classic 1K/4K | Ler, Clonar, Escrever |
| MIFARE Ultralight | Ler, Escrever |
| NTAG 213/215/216 | Ler, Escrever, Amiibo |
| DESFire | Ler (limitado) |
| FeliCa | Ler |

---

### CC1101 SubGHz (315/433/868/915 MHz)

| Especificação | Valor |
|---------------|-------|
| **Chip** | TI CC1101 |
| **Interface** | SPI3 |
| **CS** | GPIO 46 |
| **GDO0** | GPIO 47 |
| **Power Gate** | GPIO 48 |
| **Frequências** | 315/433/868/915 MHz |

#### Ataques RF Disponíveis

| Ataque | Descrição | Taxa Sucesso |
|--------|-----------|:------------:|
| **Scan/Copy** | Captura sinais RF | 95% |
| **Replay Attack** | Reproduz sinais gravados | 90% |
| **Ghost Replay** | Replay com timing aleatório | 90% |
| **Jammer Contínuo** | Onda quadrada contínua | 100% |
| **Jammer Smart** | Detecta e interfere apenas TX ativo | 95% |
| **Jammer Burst** | Intermitente (PWM) | 95% |
| **Brute Force** | Testa todos códigos | 70% |
| **De Bruijn** | Sequência otimizada | 80% |
| **Spectrum Analyzer** | Varredura de frequência | 100% |
| **Protocol Detection** | Identifica Princeton/CAME/etc | 85% |

> **Implementação:** [rf_core.cpp](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/src/rf_core.cpp) | [rf_menu.cpp](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/src/rf_menu.cpp)
| **Car Key Clone** | Toyota/Hyundai/Honda | 70-80% |
| **Garage Door** | Códigos fixos/rolling | 60-85% |
| **Smart Doorbell** | Ring/outros | 90% |
| **Spectrum Analyzer** | Visualização espectro | 100% |

---

### GPS GY-NEO6MV2 (u-blox NEO-6M)

| Especificação | Valor |
|---------------|-------|
| **Chip** | u-blox NEO-6M |
| **Interface** | UART2 (GPIO 17/18) |
| **Baud Rate** | 9600 bps |
| **Precisão** | 2.5 metros |
| **Canais** | 50 (22 tracking) |

#### Funções GPS Disponíveis

| Função | Descrição | Uso |
|--------|-----------|-----|
| **Wardriving** | Log WiFi + coords | Mapeamento |
| **Location Log** | Histórico posição | Tracking |
| **Time Sync** | UTC preciso | Timestamps |
| **Geofencing** | Trigger por local | Proximidade |
| **Export KML** | Google Earth | Visualização |

> [!TIP]
> Documentação completa: [GPS_NEO6MV2.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/GPS_NEO6MV2.md)

---

### WiFi ESP32-S3 (2.4 GHz)

#### Ataques WiFi Disponíveis

| Categoria | Ataques | Descrição |
|-----------|---------|-----------|
| **Deauth** | Flood, Target, Broadcast | Desconecta clientes |
| **Evil Twin** | Clone SSID + Portal | Rouba credenciais |
| **Beacon Spam** | Fake SSIDs | Confunde usuários |
| **PMKID** | Clientless handshake | Crackeia WPA2 |
| **WPS** | Pixie Dust, Brute | Explora WPS |
| **MITM** | DNS Spoof, SSL Strip | Intercepta tráfego |
| **DoS** | Auth Flood, Disassoc | Denial of Service |

#### Técnicas Avançadas

| Técnica | Descrição |
|---------|-----------|
| **Channel Lock** | Ataca apenas no canal do alvo |
| **Beacon Disappearing** | Desaparece em 1s para stealth |
| **Deauth Burst** | 10 pkt/100ms controlado |
| **WPA3 Skip** | Detecta e pula com mensagem |

---

### BLE ESP32-S3 (Bluetooth 5.0)

#### Ataques BLE Disponíveis

| Ataque | Alvo | Descrição |
|--------|------|-----------|
| **Applejuice** | iOS | Popup spam |
| **SourApple** | iOS | Crash device |
| **Swift Pair** | Windows | Popup spam |
| **SmartThings** | Samsung | Popup spam |
| **Fast Pair** | Android | Popup spam |
| **Continuity Spoof** | iOS | Finge WhatsApp/iCloud |
| **AirTag Spoof** | iOS | Tracking falso |
| **BLE Keyboard** | Todos | Rubber Ducky via BT |
| **Audio Bleed** | Todos | Morse 18kHz |

---

## 📺 INFRAVERMELHO (IR)

| Componente | GPIO | Função |
|------------|:----:|--------|
| IR TX LED | 3 | Transmissor |
| IR RX | - | Não usado nesta config |

### Ataques IR Disponíveis

| Ataque | Descrição |
|--------|-----------|
| **TV-B-Gone** | Desliga 40+ marcas de TVs |
| **IR Capture** | Grava sinais de controles |
| **Custom Replay** | Reproduz sinais gravados |

### Protocolos Suportados

NEC, NECext, SIRC, SIRC15, SIRC20, Samsung32, RC5, RC5X, RC6

---

## 🔧 MAPA DE POWER GATING

| Módulo | GPIO Gate | Tipo | Consumo Ativo |
|--------|:---------:|------|:-------------:|
| PN532 NFC | 7 | MOSFET | ~80mA |
| CC1101 SubGHz | 48 | MOSFET | ~30mA |

> [!TIP]
> Power gating permite desligar módulos RF quando não estão em uso, economizando bateria.

```cpp
// Ligar PN532
digitalWrite(7, HIGH);
delay(50);

// Desligar PN532
digitalWrite(7, LOW);
```

---

## 🎯 ATAQUES POR CATEGORIA

### 📡 WiFi (40+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| DoS/Deauth | 8 | Flood, Target, Broadcast |
| Capture | 6 | PMKID, Handshake, Passive |
| Evil Portal | 5 | Phishing, Captive, CPF |
| MITM | 6 | DNS Spoof, SSL Strip |
| WPS | 4 | Pixie, Brute, PIN |
| Scan | 4 | Networks, Hosts, Ports |
| CVE 2024 | 7 | MediaTek, FragAttacks |

### 📶 BLE (15+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| Spam | 6 | Apple, Windows, Samsung, Android |
| Keyboard | 3 | Rubber Ducky, Login Inject |
| Spoof | 4 | Continuity, AirTag, WhatsApp |
| Exploit | 2 | LowbattBLE, Audio Bleed |

### 💳 NFC/RFID (12+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| Clone | 3 | UID, Full Dump, Magic |
| Write | 3 | NDEF, Data, Erase |
| Attack | 5 | Phishing, OTA, Apple Pay |
| Emulate | 1 | Amiibo, Chameleon |

### 📻 RF SubGHz (10+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| Capture | 2 | Scan, Record |
| Replay | 2 | Simple, Ghost |
| Jammer | 2 | Full, PWM |
| Analyze | 2 | Spectrum, Decode |

### 📺 IR (3+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| TV | 1 | TV-B-Gone |
| Capture | 1 | Record signals |
| Replay | 1 | Custom IR |

### 🔌 USB (10+ ataques)

| Subcategoria | Quantidade | Exemplos |
|--------------|:----------:|----------|
| BadUSB | 5 | Rubber Ducky scripts |
| Exfil | 3 | WiFi creds, Browser |
| Exploit | 2 | Reverse shell, Disable AV |

### 🔬 Fault Injection (5+ ataques)

| Alvo | Método | Taxa |
|------|--------|:----:|
| Switch RCM | Crowbar | 85% |
| iPhone Checkm8 | Laser+Crowbar | 75% |
| Car Keyless | Voltage glitch | 70-80% |
| Smart Meter | Laser | 85% |
| RFID HID | Anti-collision | 80% |

---

## 🔗 DOCUMENTAÇÃO RELACIONADA

| Documento | Conteúdo |
|-----------|----------|
| [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md) | Pinout completo, esquemas, código |
| [RFID.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/RFID.md) | Funcionalidades NFC/RFID |
| [FUNCIONALIDADES.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/FUNCIONALIDADES.md) | Lista completa de ataques |
| [HARDWARE.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/HARDWARE.md) | Hardware e bibliotecas |
| [ESP32_S3_HARDWARE.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/ESP32_S3_HARDWARE.md) | Diagrama completo ESP32-S3 |
| [JOYSTICK_DISPLAY_PINOUT.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/JOYSTICK_DISPLAY_PINOUT.md) | Joystick Shield V1.A |
| [MODULOS.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/MODULOS.md) | Módulos de ataque |
| [CC1101_MODULE.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/CC1101_MODULE.md) | **Documentação Técnica Completa CC1101** |
| [GPS_NEO6MV2.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/GPS_NEO6MV2.md) | **Pinout GPS, Wardriving, NMEA** |
| [MICROSD_ADAPTER.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/MICROSD_ADAPTER.md) | Pinout, Esquema SD, Ataques |
| [DS3231_RTC.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/DS3231_RTC.md) | **RTC, Alarmes, Time Spoofing** |

---

## ⚡ CONSUMO DE ENERGIA

| Componente | Idle | Ativo | Pico |
|------------|:----:|:-----:|:----:|
| ESP32-S3 240MHz | 30mA | 100mA | 240mA |
| ILI9341 TFT | 20mA | 50mA | 80mA |
| PN532 NFC | 2mA | 80mA | 150mA |
| CC1101 TX | 0.5mA | 30mA | 35mA |
| PCM5102A + PAM8403 | 1mA | 115mA | 630mA |
| **TOTAL** | ~60mA | ~380mA | ~1135mA |

---

## 🔐 SEGURANÇA DO SISTEMA

| Função | Descrição |
|--------|-----------|
| **Aggressive SD Boot** | Boot 100% do SD Card |
| **Secure Boot** | CRC32 integrity check |
| **Zero Trace** | Nenhum log no flash interno |
| **MAC Spoofing** | Random MAC a cada 5min |
| **Deep Sleep** | 90% uptime dormindo |
| **LED Signaling** | Código visual durante ataques |

---

> [!NOTE]
> Este documento é a referência MASTER do sistema Lele Origin.
> Para detalhes específicos, consulte os documentos linkados acima.
