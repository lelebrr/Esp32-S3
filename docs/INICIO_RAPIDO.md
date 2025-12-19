# ⚡ Início Rápido

> **Primeiros passos com seu Lele Origin.**  
> **Última atualização:** 2025-12-19

---

## 📦 O Que Você Precisa

### Hardware Principal (Recomendado)

- **ESP32-S3-WROOM-1 N16R8** (16MB Flash + 8MB PSRAM)
- Display ILI9341 2.8" TFT com touch XPT2046
- Cartão SD (recomendado 128GB)
- Bateria 4x 18650 + Shield V3

### Hardware Legacy

- Placa CYD-2USB (ESP32 com display 2.8")

### Software

- Cabo USB-C para programação
- Computador com Windows, Linux ou macOS
- VS Code + PlatformIO

---

## 🔧 Instalação

### Método 1: Compilação PlatformIO (Recomendado)

```bash
# ESP32-S3 (Principal)
pio run -e CYD-S3-N16R8 -t upload

# CYD-2USB (Legacy)
pio run -e CYD-2USB -t upload
```

### Método 2: esptool.py

```bash
# ESP32-S3
esptool.py --port COM3 --chip esp32s3 write_flash 0x0 Lele-S3-N16R8.bin

# CYD-2USB (Legacy)
esptool.py --port COM3 write_flash 0x0 Lele-CYD-2USB.bin
```

---

## 🎮 Primeiro Uso

### Seleção de Modo de Energia (ESP32-S3)

Ao ligar, pressione **GPIO0** (boot button) nos primeiros 3 segundos:

| Modo | Duração Bateria | Uso |
|------|-----------------|-----|
| **Economy** | ~14 dias | Standby/monitoring |
| **Balanced** | ~5 dias | Uso normal |
| **Force** | ~8 horas | Ataques intensivos |

### Navegação

- **Toque no display**: Selecionar opções
- **Deslize**: Navegar entre menus
- **Pressione e segure**: Opções adicionais / Deep sleep

### Menu Principal

| Ícone | Função |
|-------|--------|
| 📡 | WiFi - Ferramentas de rede |
| 📶 | BLE - Bluetooth Low Energy |
| 📻 | RF - SubGhz (CC1101) e 2.4GHz (NRF24) |
| 💳 | RFID - Leitura/escrita NFC (PN532) |
| 📺 | IR - Multi-zone infravermelho |
| 🔊 | Audio - Reprodução WAV/MP3 |
| ⚙️ | Config - Configurações |

---

## 🔌 Conexões de Hardware (ESP32-S3)

### Módulos I2C (SDA=1, SCL=2)

| Módulo | Endereço | Função |
|--------|----------|--------|
| PN532 | 0x24 | RFID/NFC |
| DS3231 | 0x68 | RTC |
| PAJ7620U2 | 0x73 | Gestos |

### Módulos SPI3 (MOSI=35, MISO=37, SCK=36)

| Módulo | CS | Função |
|--------|-------|--------|
| SD Card | 38 | Armazenamento |
| CC1101 | 5 | SubGHz 433MHz |
| NRF24 | 7 | 2.4GHz |

### Sensores

| Sensor | GPIO | Função |
|--------|------|--------|
| DS18B20 | 47 | Temperatura (thermal throttling) |
| Battery ADC | 14 | Nível da bateria |

> [!TIP]
> Veja [ESP32_S3_COMPLETE_PINOUT.md](ESP32_S3_COMPLETE_PINOUT.md) para pinout completo.

---

## ⚡ Dicas Rápidas

1. **Conectar WiFi**: Menu WiFi → Connect → Selecione rede
2. **Ajustar Brilho**: Config → Brightness
3. **Deep Sleep Manual**: Segure botão SEL por 5 segundos
4. **Wake por Gesto**: PAJ7620U2 desperta de deep sleep

---

## 📚 Próximos Passos

- [Funcionalidades](FUNCIONALIDADES.md) - Lista completa de recursos
- [Hardware ESP32-S3](HARDWARE.md) - Conexões detalhadas
- [Thermal/Power](THERMAL_POWER_MANAGEMENT.md) - Gerenciamento de energia
- [Audio](AUDIO_SYSTEM.md) - Sistema de áudio I2S
