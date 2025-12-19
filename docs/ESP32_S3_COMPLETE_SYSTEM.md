# ESP32-S3 Predatory Firmware - Sistema Completo Otimizado

## Visão Geral

Firmware standalone para ESP32-S3-WROOM-1 (16MB Flash + 8MB PSRAM) com capacidades avançadas de ataque e IA local. Consumo otimizado <10mA idle.

## Pinagem Completa

### LEDs

- **GPIO0**: LED vermelho baixo brilho (pronto/sistema ativo)
- **GPIO4**: LED vermelho alto brilho (ataque em andamento/falha)
- **GPIO6**: LED azul alto brilho (IA ativa/pensando)
- **GPIO7**: LED azul alto brilho (modo sleep inteligente)

### Switches de Controle

- **GPIO15**: Switch preto (stealth - desliga UART2)
- **GPIO16**: Switch preto (stealth - desliga UART1)
- **GPIO14**: Switch vermelho (ataque - habilita CC1101)
- **GPIO17**: Switch vermelho (ataque - habilita PN532)
- **GPIO18**: Switch verde (IA - habilita PAJ7620U2 gestos)
- **GPIO19**: Switch verde (IA - habilita DS3231 RTC)

### Fault Injection

- **GPIO33**: Push button (fault injection manual)
- **GPIO42**: MOSFET IRF520 controle (VCC PN532)

### SD Card (SPI3 Otimizado)

- **SPI MOSI**: GPIO11
- **SPI MISO**: GPIO13
- **SPI SCK**: GPIO12
- **SD CS**: GPIO14 (Safe Pin)

### Áudio/TTS (I2S)

- **I2S BCK**: GPIO40
- **I2S WS**: GPIO39
- **I2S DATA**: GPIO41

### Interfaces I2C

- **SDA**: GPIO1
- **SCL**: GPIO2
- Conecta: RTC DS3231, gestos PAJ7620U2, NFC PN532 (I2C mode)

### Display SPI (SPI2)

- **MOSI**: GPIO11
- **MISO**: GPIO13
- **SCK**: GPIO12
- **CS**: GPIO10
- **DC**: GPIO9
- **Touch CS**: GPIO15

## Ataques Implementados

| Ataque | Hardware | GPIO | Velocidade | Status |
|--------|----------|------|------------|--------|
| BLE Spam BR | BLE interno | - | ~1000pps (Raw GAP) | ✅ Monster |
| WiFi Deauth | WiFi interno | DMA | ~2000pps (Burst 50) | ✅ Monster |
| PMKID Capture | WiFi interno | PSRAM | 64KB buffer | ✅ Funcional |
| NFC Fault Injection | PN532 + MOSFET | GPIO42/33 | 5ms (Precisão uS) | ✅ Monster |
| IA Q-Learning | PSRAM + SD | - | 2048 estados | ✅ Funcional |
| TTS Real | PCM5102A | I2S 39/40/41 | SAM (Software Mouth) | ✅ Funcional |

## Fluxo de Operação IA

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Contexto      │ -> │   IA Decide     │ -> │   Executa       │
│   Atual         │    │   Ataque        │    │   Ataque        │
│                 │    │   (Q-Learning)  │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         v                       v                       v
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ Feedback LED    │    │ Atualiza       │    │ TTS Confirma    │
│ Visual          │    │ Q-Table        │    │ Ação            │
│                 │    │ (Recompensa)   │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Estados IA

- **IDLE**: Espera por triggers
- **WIFI_SCAN**: Varredura de redes
- **BLE_SPOOF**: Spam BLE brasileiro
- **DEAUTH_ATTACK**: Flood deauth + PMKID
- **NFC_FAULT**: Injeção física
- **EVIL_TWIN**: Captive portal
- **PMKID_CAPTURE**: Handshake capture
- **SLEEP_MODE**: Deep sleep inteligente

## Consumo de Energia Otimizado

### Modos de Operação

- **Idle**: <10mA (deep sleep 500ms)
- **Ativo**: <50mA (IA + sensores)
- **Ataque**: <200mA (WiFi/BLE full power)
- **Sleep Inteligente**: <1mA (RTC wake only)

### Otimizações Implementadas

- Dynamic voltage scaling
- Thermal throttling (DS18B20)
- Power gating (IRF520 MOSFETs)
- PSRAM para buffers grandes
- Task prioritization

## Instalação Passo-a-Passo

### 1. Soldagem de Componentes

```bash
# LEDs (todos com resistor 330Ω)
LED_VERMELHO_BAIXO -> GPIO0
LED_VERMELHO_ALTO -> GPIO4
LED_AZUL_IA -> GPIO6
LED_AZUL_SLEEP -> GPIO7

# Switches (todos com pull-up 10KΩ)
SWITCH_STEALTH_UART2 -> GPIO15
SWITCH_STEALTH_UART1 -> GPIO16
SWITCH_ATTACK_CC1101 -> GPIO14
SWITCH_ATTACK_PN532 -> GPIO17
SWITCH_AI_PAJ7620 -> GPIO18
SWITCH_AI_DS3231 -> GPIO19

# Fault Injection
PUSH_BUTTON -> GPIO33
MOSFET_IRF520_GATE -> GPIO42
MOSFET_IRF520_DRAIN -> PN532_VCC

# Audio DAC (PCM5102A)
PCM5102A_BCK -> GPIO40
PCM5102A_WS -> GPIO39
PCM5102A_DATA -> GPIO41

# SD Card
SD_MOSI -> GPIO11
SD_MISO -> GPIO13
SD_SCK -> GPIO12
SD_CS -> GPIO14

# I2C Bus
RTC_DS3231_SDA -> GPIO1
RTC_DS3231_SCL -> GPIO2
PAJ7620U2_SDA -> GPIO1
PAJ7620U2_SCL -> GPIO2
PN532_SDA -> GPIO1
PN532_SCL -> GPIO2

# Proteções
DIODO_1N5817 -> VCC_BATERIA
CAP_100UF -> VCC_SISTEMA
FUSIVEL_PORTA -> VCC_BATERIA
DISSIPADOR -> ESP32-S3
```

### 2. Montagem do Case

```bash
# Furos para LEDs
LED_VERMELHO_BAIXO -> Frente (sempre visível)
LED_VERMELHO_ALTO -> Frente (pisca em ataque)
LED_AZUL_IA -> Lateral (IA ativa)
LED_AZUL_SLEEP -> Lateral (modo sleep)

# Controles externos
PUSH_BUTTON_FAULT -> Frente (acessível)
SWITCHES -> Lateral (6 switches organizados)

# Conectores
USB-C -> Traseira
ANTENA_WIFI/BLE -> Superior
ANTENA_CC1101 -> Lateral
NFC_PN532 -> Inferior
SD_CARD_SLOT -> Lateral
```

### 3. Configuração Software

#### PlatformIO Setup

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
board_build.f_cpu = 240000000L
board_build.f_flash = 80000000L
board_build.flash_mode = qio
board_build.flash_size = 16MB
board_build.partitions = custom_16Mb_S3.csv

build_flags =
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
    -DCORE_DEBUG_LEVEL=0
    -DBLE_OPTIMIZATIONS
    -DWIFI_OPTIMIZATIONS
    -DTHERMAL_MANAGEMENT
    -DPOWER_GATING
    -DGESTURE_WAKE
    -DAUDIO_DMA
    -DCYD_S3

lib_deps =
    me-no-dev/ESP Async WebServer
    me-no-dev/AsyncTCP
    bblanchon/ArduinoJson
    madhephaestus/ESP32Encoder
    bodmer/TFT_eSPI
    lvgl/lvgl
    adafruit/Adafruit PN532
    adafruit/Adafruit CC1101
    adafruit/Adafruit BusIO
    paulstoffregen/XPT2046_Touchscreen
```

#### Compilação e Upload

```bash
# Instalar dependências
pio pkg install

# Compilar
pio run

# Upload
pio run --target upload

# Monitor serial
pio device monitor
```

### 4. Configuração Inicial

#### Primeiro Boot

1. Sistema inicializa pinos
2. Carrega Q-table do SD (se existir)
3. Inicializa TTS: "Sistema pronto. Iniciando monitoramento."
4. LEDs piscam para confirmação
5. IA entra em modo de aprendizado

#### Calibração Switches

```cpp
// Verificar cada switch individualmente
read_switches(); // Chamado a cada 10ms no loop
update_leds();   // Feedback visual imediato
```

## Como Usar

### Modo Manual

1. **BLE Spam**: Ativar switch vermelho CC1101
2. **WiFi Deauth**: Ativar switch vermelho PN532 + target via serial
3. **NFC Fault**: Pressionar push button na frente
4. **IA Control**: Ativar switches verdes para PAJ7620/DS3231

### Modo IA Automático

1. Ativar switches verdes (PAJ7620U2 + DS3231)
2. IA observa contexto (redes WiFi, dispositivos BLE)
3. Decide ataque baseado em Q-table aprendida
4. Executa com feedback TTS
5. Aprende com resultado (sucesso/falha)

### Monitoramento

```bash
# Serial commands disponíveis
ble_spam start    # Inicia spam BLE brasileiro
ble_spam stop     # Para spam BLE
wifi_deauth start # Inicia flood deauth
wifi_deauth stop  # Para deauth
nfc_fault trigger # Gatilho manual fault
ai_start          # Inicia IA Q-learning
ai_stop           # Para IA
tts_speak "texto" # Fala via TTS
```

## Atualização OTA

### Via Web Interface

1. Acessar IP do dispositivo
2. Ir para "System" -> "Update"
3. Selecionar "Atualizar" para buscar CVEs
4. Sistema baixa variantes automaticamente
5. Compila e flasha OTA
6. IA aprende com feedback do usuário

### Via Serial

```bash
# Entrar em modo update
AT+UPDATE

# Buscar CVEs brasileiras
AT+UPDATE CVE_BR

# Compilar e flashar
AT+UPDATE FLASH

# Feedback usuário
AT+UPDATE SUCCESS  # IA aprende positivo
AT+UPDATE FAIL     # IA aprende negativo
```

## Troubleshooting

### Problema: Alto consumo

```cpp
// Verificar modos de economia
currentPowerMode = preferences.getUChar("bat_mode", BALANCED);
applyPowerMode(); // ECONOMY = 14 dias, BALANCED = 5 dias
```

### Problema: IA não aprende

```cpp
// Reset Q-table
if (!SD.exists("/q_table.bin")) {
    log_w("[AI] Q-table not found, using defaults");
}
// Verificar PSRAM
if (!psramFound()) {
    log_e("[AI] PSRAM required for Q-learning");
}
```

### Problema: TTS não funciona

```cpp
// Verificar I2S
audio.setPinout(40, 39, 41);
// Verificar SAM
speak_text_sam("Teste");
```

## Performance Benchmarks

### BLE Spam

- **Taxa**: 800-1000pps estável
- **Nomes**: 18 variedades brasileiras
- **Consumo**: +15mA durante spam

### WiFi Deauth

- **Taxa**: 1500pps com DMA
- **Alcance**: 50m (potência máxima)
- **PMKID Buffer**: 64KB em PSRAM

### IA Q-Learning

- **Estados**: 2048 (2KB Q-table)
- **Persistência**: SD automática
- **Aprendizado**: 30s intervalos
- **Ações**: 8 tipos diferentes

### NFC Fault

- **Pulso**: 5ms preciso
- **Debounce**: 1s para evitar spam
- **Feedback**: LED + TTS

### Consumo Idle

- **Deep Sleep**: <1mA
- **Light Sleep**: <5mA
- **Active Monitor**: <10mA
- **Attack Mode**: <200mA

## Segurança e Legal

### Avisos Importantes

- Uso apenas em ambientes controlados
- Respeitar leis locais sobre rádio frequência
- Não usar para atividades maliciosas
- Dados capturados são responsabilidade do usuário

### Proteções Implementadas

- Stealth mode (desliga UARTs)
- Power gating (desliga módulos não usados)
- Thermal protection (throttling automático)
- Secure boot (verificação de firmware)

## Desenvolvimento Futuro

### Melhorias Planejadas

- WPA3 attack vectors
- BLE 5.0 long range
- Machine learning avançado
- Mesh networking
- Cloud backup de Q-table

### Extensões Hardware

- GPS para geofencing
- Camera para reconhecimento
- LTE module para ataques 4G
- LoRa para controle remoto

---

**Firmware Version**: ESP32-S3 Predatory v2.0
**Hardware**: ESP32-S3-WROOM-1 + módulos custom
**Compatibilidade**: PlatformIO + Arduino Framework
**Licença**: MIT (uso responsável)
