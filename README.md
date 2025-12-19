# 🦎 Lele Origin 1.0 - Arsenal Completo de Pentesting

<p align="center">
  <strong>Firmware ESP32-S3 com arsenal completo de ferramentas de segurança ofensiva</strong>
</p>

---

## 📋 Sobre

O **Lele Origin** é um firmware ESP32-S3 com arsenal completo para operações de segurança ofensiva, incluindo ferramentas avançadas de pentesting. Badge Black Hat completo em um dispositivo ESP32-S3.

### 🔧 Hardware Principal: ESP32-S3-WROOM-1 N16R8

| Recurso | Especificação |
|---------|---------------|
| **MCU** | ESP32-S3 Dual-Core 240MHz Xtensa LX7 |
| **Flash** | 16MB QIO 80MHz |
| **PSRAM** | 8MB Octal SPI 120MHz |
| **Display** | ILI9341 2.8" 320x240 TFT + Touch XPT2046 |
| **Áudio** | PCM5102A DAC I2S + PAM8403 Amp + Speaker 4Ω 3W |
| **RTC** | DS3231 com bateria CR2032 |
| **Sensores** | PAJ7620U2 (gestos), DS18B20 (temperatura) |
| **RF** | CC1101 (433MHz), NRF24L01+PA+LNA (2.4GHz) |
| **NFC** | PN532 I2C (MIFARE, NTAG, FeliCa) |
| **IR** | Multi-zone: 5x TSOP38238 RX + TX via YS-IRTM |
| **GPS** | NEO-6M com antena externa |
| **Bateria** | 4x 18650 via Shield V3 (35h autonomia) |
| **Deep Sleep** | <5µA com wake por gesto ou timer |

---

### ✨ Arsenal Completo de Pentesting

| Módulo | Recursos | Ataques |
|--------|----------|---------|
| 📡 **WiFi Evil Twin + Deauth** | Scan redes, AP fake SSID igual, deauth floods bursts 10pkts/100ms | Evil Twin + Deauth |
| 📶 **BLE Keyboard Inject** | ESP32-S3 como HID via NimBLE, injeta payloads | Rubber Ducky BLE |
| 🔌 **UART Keylogger** | Buffer circular 1K IRAM, AES criptografado SD | Hardware Keylogger |
| 💳 **NFC Skimmer** | PN532 I2C, lê MIFARE 1K (nested attack), copia UID + setores | RFID Cloning |
| 🔀 **Persistence** | DNS poisoning, HTTPS POST logs, ICMP tunneling | Backdoor Implants |
| 🛡️ **Stealth Total** | Deep sleep 90%, MAC spoof random 5min, no serial debug | Zero Detection |
| 🚀 **Performance** | 240MHz dual-core, PSRAM 120MHz, DMA para I2S/SPI/UART | High-Speed Attacks |
| 🤖 **IA Local** | Q-Learning + TensorFlow Lite, aprende ataques automaticamente | Adaptive Attacks |

**Total: 41+ vetores de ataque em 9 tecnologias + IA adaptativa**

---

### 🔒 Segurança & Core

- **Aggressive SD Boot:** Wipe automático do flash interno a cada boot. Operação 100% via SD.
- **Secure Boot (Paranoid Mode):** Verificação CRC32, monitoramento térmico/voltagem.
- **Thermal Throttling:** 240MHz → 160MHz quando temperatura > 70°C (DS18B20).
- **Power Gating:** IRF520 MOSFET desliga módulos inativos (CC1101, NRF24, GPS, Audio).
- **Gesture Wake:** PAJ7620U2 acorda de deep sleep com gesto.
- **Battery Modes:** Economy (14d), Balanced (5d), Force (8h).

---

## 🔧 Instalação

### Gravando o Firmware (ESP32-S3-N16R8)

```bash
# Via esptool
esptool.py --port COM3 --chip esp32s3 write_flash 0x0 Lele-S3-N16R8.bin

# Ou compile via PlatformIO
pio run -e CYD-S3-N16R8 -t upload
```

### Monitorando

```bash
pio device monitor -e CYD-S3-N16R8
```

---

## 📚 Documentação

A documentação completa está na pasta [docs/](docs/):

- [Início Rápido](docs/INICIO_RAPIDO.md)
- [Hardware ESP32-S3](docs/HARDWARE.md)
- [Pinout Completo](docs/ESP32_S3_COMPLETE_PINOUT.md)
- [Funcionalidades](docs/FUNCIONALIDADES.md)
- [Compilação](docs/COMPILACAO.md)
- [Thermal/Power Management](docs/THERMAL_POWER_MANAGEMENT.md)
- [🤖 IA Local - Aprendizado Contínuo](docs/IA_LOCAL_APRENDIZADO.md)

---

## 🤖 IA Local - Aprendizado Adaptativo

### Sistema de IA Integrado

O **Lele Origin** inclui **IA local completa** baseada em Q-Learning + TensorFlow Lite Micro:

#### **Funcionalidades IA**

- **Q-Learning Table:** 64 estados x 8 ações (2KB em PSRAM)
- **Brute Force Brasil:** 🇧🇷 Lista inteligente de PINs brasileiros (CVE, Exploit-DB, Reclame Aqui)
- **Aprendizado Contextual:** GPS, temperatura, bateria, gestos
- **Atualização Automática:** CVEs do Exploit-DB, NIST NVD + fontes brasileiras
- **Geração Inteligente:** Cria variantes de ataques funcionais
- **Feedback Manual:** Botões GPIO34/35 para treinamento
- **Modo Stealth:** Ativação automática por condições

#### **Como Usar**

1. Menu > "IA Aprendiz" > "Atualizar CVEs"
2. Execute ataques e dê feedback "Certo/Errado"
3. Menu > "Gerar Novo Ataque" para variantes inteligentes
4. IA aprende e otimiza automaticamente

#### **Arquivos IA Criados**

```
src/core/ai_attack_learner.*     # Core Q-Learning + TFLite
src/core/menu_items/AIMenu.*    # Interface touchscreen
sd_files/ai_example/             # Templates Q-table e modelo
```

**🎯 IA evolui com você - roda 100% offline após primeira atualização!**

## 🔌 Hardware Suportado

| Placa | Status | Notas |
|-------|--------|-------|
| **ESP32-S3-WROOM-1 N16R8** | ✅ Principal | 16MB Flash + 8MB PSRAM |
| CYD-2USB | ⚠️ Legacy | 4MB Flash, sem PSRAM |

### Módulos Externos Integrados

| Módulo | Interface | Função |
|--------|-----------|--------|
| CC1101 | SPI3 (CS=5) | SubGHz RF 433MHz |
| NRF24L01+PA+LNA | SPI3 (CS=7) | 2.4GHz RF longo alcance |
| PN532 | I2C (0x24) | RFID/NFC Read/Write |
| DS3231 | I2C (0x68) | RTC com bateria |
| PAJ7620U2 | I2C (0x73) | Sensor de gestos |
| PCM5102A | I2S | DAC áudio hi-fi |
| DS18B20 | 1-Wire | Sensor temperatura |
| NEO-6M | UART1 | GPS |
| YS-IRTM | UART2 | IR transceiver |

---

## 🙏 Créditos

Baseado no projeto [Lele](https://github.com/pr3y/Lele) - agradecimentos especiais a toda a comunidade.

Veja [CONTRIBUIDORES](docs/CONTRIBUIDORES.md) para a lista completa.

---

## ⚠️ Aviso Legal

O Lele Origin é uma ferramenta para **testes de segurança autorizados**.

O uso para atividades maliciosas ou não autorizadas é **estritamente proibido**.

Distribuído sob licença **AGPL**. Use por sua conta e risco.
