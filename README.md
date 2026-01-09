# MorphNode Firmware

## ESP32-S3 Pentest Device 🦎

Firmware completo para dispositivo de segurança ofensiva portátil, suportando 27+ vetores de ataque.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![RAM Usage](https://img.shields.io/badge/RAM-22%25-blue)
![Flash Usage](https://img.shields.io/badge/Flash-24%25-blue)

---

## 🚀 Quick Start

```bash
# Compilar e fazer upload
pio run -e MorphNode -t upload

# Monitorar saída serial
pio device monitor
```

---

## 🔧 Hardware

### Microcontrolador

| Especificação | Valor |
|---------------|-------|
| **MCU** | ESP32-S3-WROOM-1 N16R8 |
| **CPU** | Xtensa 32-Bit LX7 Dual Core @ 240MHz |
| **Flash** | 16MB QIO |
| **PSRAM** | 8MB Octal |
| **WiFi** | 802.11 b/g/n 2.4GHz |
| **Bluetooth** | 5.0 BLE + Mesh |

### Periféricos

| Componente | Modelo | Interface |
|------------|--------|-----------|
| Display | MSP2402 ILI9341 2.4" 240x320 | SPI (FSPI) |
| Touch | XPT2046 Resistivo | SPI (compartilhado) |
| NFC | PN532 | I2C |
| SubGHz RF | CC1101 | SPI (HSPI) |
| IR | YS-IRTM | UART |
| GPS | GY-NEO6MV2 | UART2 |
| RTC | DS3231 | I2C |
| Audio | PCM5102A DAC | I2S |
| Gestos | PAJ7620U2 | I2C |
| Fault Injection | MOSFET IRF520 | GPIO |

---

## 📌 Pinout

### Display + Touch (SPI - FSPI)
| Pino | GPIO | Função |
|------|------|--------|
| TFT_CS | 10 | Display Chip Select |
| TFT_RST | 11 | Display Reset |
| TFT_DC | 12 | Data/Command |
| TFT_MOSI | 13 | SPI MOSI |
| TFT_SCLK | 14 | SPI Clock |
| TFT_BL | 21 | Backlight (PWM) |
| TOUCH_CS | 15 | Touch Chip Select |
| TOUCH_IRQ | 16 | Touch Interrupt |

### SD Card + CC1101 (SPI - HSPI)
| Pino | GPIO | Função |
|------|------|--------|
| SD_CS | 39 | SD Chip Select |
| SD_SCK | 40 | SD Clock |
| SD_MOSI | 41 | SD MOSI |
| SD_MISO | 42 | SD MISO |
| CC1101_CS | 38 | RF Chip Select |
| CC1101_GDO0 | 47 | RF Interrupt |

### I2C Bus (PN532, DS3231, PAJ7620)
| Pino | GPIO | Função |
|------|------|--------|
| I2C_SDA | 8 | I2C Data |
| I2C_SCL | 9 | I2C Clock |
| PN532_EN | 7 | NFC Power Enable (MOSFET) |

### I2S Audio (PCM5102A)
| Pino | GPIO | Função |
|------|------|--------|
| I2S_BCK | 43 | Bit Clock |
| I2S_WS | 44 | Word Select |
| I2S_DOUT | 5 | Data Out |

### UART
| Pino | GPIO | Função |
|------|------|--------|
| IR_TX | 17 | YS-IRTM TX |
| IR_RX | 18 | YS-IRTM RX |
| GPS_TX | 1 | GPS TX |
| GPS_RX | 2 | GPS RX |

### Outros
| Pino | GPIO | Função |
|------|------|--------|
| RGB_LED | 48 | LED Onboard (WS2812) |
| FAULT_GATE | 45 | MOSFET IRF520 |
| JOY_X | 4 | Joystick X (ADC) |
| JOY_Y | 6 | Joystick Y (ADC) |

---

## ⚔️ Ataques Implementados

### WiFi (5 ataques)
| Ataque | Status |
|--------|--------|
| Evil Twin | ✅ Funcional |
| Deauth Flood | ✅ Funcional |
| Beacon Spam | ✅ Funcional |
| PMKID Capture | ✅ Funcional |
| WPS Pixie Dust | ✅ Funcional |

### BLE (4 ataques)
| Ataque | Status |
|--------|--------|
| iOS Spam | ✅ Funcional |
| Windows Spam | ✅ Funcional |
| Samsung Spam | ✅ Funcional |
| Android Spam | ✅ Funcional |

### RF SubGHz (10 ataques)
| Ataque | Status |
|--------|--------|
| Scan/Capture | ✅ Funcional |
| Replay | ✅ Funcional |
| Ghost Replay | ✅ Funcional |
| Jammer 433MHz | ✅ Funcional |
| Jammer 315MHz | ✅ Funcional |
| Jammer 868MHz | ✅ Funcional |
| Brute Force | ✅ Funcional |
| De Bruijn | ✅ Funcional |
| Spectrum Analyzer | ✅ Funcional |
| Protocol Detection | ✅ Princeton/CAME/NiceFLO |

### NFC (3 ataques)
| Ataque | Status |
|--------|--------|
| Read/Clone | ✅ PN532 |
| Fault Injection | ✅ MOSFET |
| Relay | ✅ Funcional |

### IR (3 ataques)
| Ataque | Status |
|--------|--------|
| TV-B-Gone | ✅ YS-IRTM |
| Capture | ✅ Funcional |
| Replay | ✅ Funcional |

### USB (2 ataques)
| Ataque | Status |
|--------|--------|
| BadUSB HID | ✅ Funcional |
| Ducky Script | ✅ Funcional |

---

## 📡 Web Dashboard

- **SSID**: `MorphNode` (oculto)
- **Senha**: `lele2025`
- **URL**: http://192.168.4.1

### API Endpoints

| Endpoint | Descrição |
|----------|-----------|
| `GET /api/status` | Status do sistema |
| `GET /api/attack/{type}` | Iniciar ataque |
| `GET /api/stop` | Parar todos os ataques |
| `GET /api/nfc/read` | Ler cartão NFC |
| `POST /api/usb/payload` | Executar script USB |

---

## 💾 Estrutura do SD Card (128GB FAT32)

```
/sd/
├── attacks/           # Payloads de ataques
│   ├── wifi/
│   ├── ble/
│   ├── rf/
│   ├── nfc/
│   └── ir/
├── ai/                # Modelos Q-Learning
│   └── q_table.json
├── tts/               # Text-to-Speech
│   └── voices/
├── cve/               # Banco de vulnerabilidades
│   └── brute_br.txt
├── logs/              # Logs do sistema
├── config/            # Configurações
└── backup/            # Backups automáticos
```

---

## 🛠️ Montagem do Hardware

### Componentes Necessários

| Componente | Quantidade | Notas |
|------------|------------|-------|
| ESP32-S3-DevKitC-1 N16R8 | 1 | 16MB Flash + 8MB PSRAM |
| Display ILI9341 2.4" | 1 | Com touch XPT2046 |
| PN532 NFC | 1 | Modo I2C |
| CC1101 | 1 | 433MHz SubGHz |
| YS-IRTM | 1 | IR encoder/decoder |
| GY-NEO6MV2 GPS | 1 | 9600 baud |
| PCM5102A DAC | 1 | I2S audio |
| PAJ7620U2 | 1 | Sensor de gestos |
| DS3231 RTC | 1 | Relógio de tempo real |
| MOSFET IRF520 | 1 | Fault injection |
| Capacitor 180µF 450V | 1 | Descarga fault injection |
| Capacitor 100µF | 5 | Estabilização por módulo |
| Diodo 1N5817 | 3 | Proteção reversa |
| Bobina 2.5mm² | 1 | 15 voltas PVC |
| SD Card 128GB | 1 | FAT32 |

### Level Shifters

Todos os módulos de 5V precisam de level shifter 3.3V:
- YS-IRTM
- GPS (se 5V)
- PN532 (se 5V)

---

## 📖 Documentação Completa

Documentação detalhada disponível em [`/docs`](docs/):

- [HARDWARE.md](docs/HARDWARE.md) - Especificações do hardware
- [FUNCIONALIDADES.md](docs/FUNCIONALIDADES.md) - Lista completa de funções
- [ATAQUES.md](docs/ATAQUES.md) - Detalhes dos ataques
- [GUIA_MONTAGEM_HARDWARE.md](docs/GUIA_MONTAGEM_HARDWARE.md) - Guia de montagem

---

## 📱 Interface LVGL (Cyber-Glass UI)

O sistema conta com uma nova interface visual "Neon Cyber-Glass" rodando em LVGL 8.4:

### ✨ Destaques Visuais
-   **Estilo Glassmorphism**: Painéis semi-transparentes com bordas neon (Cyan/Magenta).
-   **Grid Layout**: Navegação moderna baseada em ícones grandes e scroll suave.
-   **Startup Cinematográfico**: Animação de boot com "Morphy" (logo vivo).
-   **Menu Status Bar**: Barra superior persistente com status de WiFi, BLE e Bateria.

### 🦎 Morphy (O Camaleão Processual)
Um assistente animado processualmente que reage ao sistema:
-   **Idle**: Observa o ambiente, pisca os olhos.
-   **Ataque**: Fica vermelho e agressivo.
-   **Sucesso**: Fica verde e celebra.
-   **Eating**: Estende a língua para "pegar" pacotes (Hardware Check).
-   **Party Mode**: Efeito Rainbow RGB (ativável via Web).

### Menu Principal (8 Categorias)
| Categoria | Descrição |
|-----------|----------|
| **Ataque Rápido** | Grid de ícones para combos (Porteiro, Casa, BLE Storm, IR Clone, Full Auto) |
| **WiFi** | Deauth, Beacon, Evil Twin (templates BR), PMKID, WPS |
| **BLE** | Spam (iOS, Windows, Android, Samsung), BLE BR |
| **NFC** | Clone, Fault Injection, Relay |
| **SubGHz** | Capture, Replay, Jammers, Brute Force, Spectrum |
| **IR** | TV-B-Gone, Brute, Clone |
| **Hardware** | Módulos On/Off, LEDs WS2812B, Energia, Reset |
| **IA & Voz** | Modo Treino/Combate, CVE, TTS, Galeria de Animações |

### 🛠️ MorphNode Studio (Web Customizer)
Personalize o Morphy em tempo real via navegador:
-   **Acesso**: `http://192.168.4.1/chameleon`
-   **Funções**: Troque cores de todas as skins, ajuste velocidade e ative o modo Rainbow.

### Submenus Deep
- **WiFi → Evil Twin**: Templates BR (Vivo, Claro, Oi, TIM, GVT)
- **Hardware → LEDs**: Neon Pulse, Matrix Rain, Rainbow, Ataque Blink
- **Hardware → Energia**: Deep Sleep 500ms/2s, Overclock 240/160MHz
- **IA & Voz → Galeria**: Previsão de todas as animações do Morphy.

### Recursos de Entrada
- Touch calibrado (XPT2046)
- Navegação por gestos (PAJ7620U2 - Wave, Clockwise)

---

## 🤖 IA Q-Learning

### Recursos
- Q-Table 32 estados × 8 ações em PSRAM
- Modo Treino/Combate
- `ai_get_best_action()` para decisão autônoma
- Export JSON para análise
- Epsilon-greedy com decay (30% → 5%)

### Combo Full Auto
A IA escolhe o melhor ataque baseado em:
- Nível de bateria
- Hora do dia
- Atividade WiFi no ambiente
- Histórico de sucesso

---

## 💡 LEDs WS2812B

| Efeito | Descrição |
|--------|----------|
| Neon Pulse | Pulsa entre cyan e magenta |
| Matrix Rain | Cascata verde estilo Matrix |
| Rainbow Cycle | Espectro completo em loop |
| Ataque Blink | Azul pulsante durante ataques |
| Status Verde | Pronto para uso |

---

## 🔋 Gerenciamento de Energia

### Modos de Energia

| Modo | CPU | Uso |
|------|-----|-----|
| Deep Sleep 500ms | Wake rápido | Monitoramento |
| Deep Sleep 2s | Wake lento | Standby |
| Normal | 160MHz | Uso diário |
| Overclock | 240MHz | Ataques intensos |

---

## ⚠️ Aviso Legal

Este projeto é **apenas para fins educacionais**. Use com responsabilidade e apenas em redes/dispositivos que você possui ou tem autorização explícita para testar.

---

## 📄 Licença

Uso educacional apenas. Veja [LICENSE](LICENSE) para detalhes.
