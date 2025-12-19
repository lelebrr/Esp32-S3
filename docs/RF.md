# 📻 RF (Rádio Frequência)

> **SubGHz, 433MHz e NRF24.**
> Última atualização: 2025-12-19

Documentação das funcionalidades de RF, CC1101 e NRF24 do Lele Origin.

---

## 🔧 Módulos Suportados

| Módulo | Frequência | Uso |
|--------|-----------|-----|
| CC1101 | 300-928 MHz | SubGhz (controles, sensores) |
| RF433 | 433 MHz | Controles simples |
| NRF24L01 | 2.4 GHz | Dispositivos wireless |

---

## 📡 CC1101 (SubGhz)

### Scan/Copy

Captura sinais RF e permite replay.

**Frequências comuns:**

- 315 MHz (EUA)
- 433 MHz (Europa, Brasil)
- 868 MHz (Europa)
- 915 MHz (EUA)

**Como usar:**

1. Conecte o módulo CC1101
2. Acesse **RF → Scan/Copy**
3. Selecione a frequência
4. Pressione o botão do controle original
5. O sinal é capturado

### Custom SubGhz

Carrega e reproduz sinais salvos em formato compatível com Flipper Zero.

**Formatos suportados:**

- `.sub` (Flipper Zero)
- `.txt` (RAW)

**Como usar:**

1. Copie os arquivos para o SD Card (pasta `/subghz/`)
2. Acesse **RF → Custom SubGhz**
3. Selecione o arquivo
4. Reproduza o sinal

### Spectrum

Analisador de espectro para visualizar atividade RF.

**Faixas disponíveis:**

- 300-348 MHz
- 387-464 MHz
- 779-928 MHz

### Jammer Full

Envia onda quadrada contínua na frequência selecionada.

> [!CAUTION]
> Jammers são ilegais na maioria dos países. Use apenas em ambientes isolados para testes.

### Jammer Intermittent

Envia sinal PWM intermitente.

### Replay

Reproduz o último sinal capturado.

---

## 🛠️ Configuração CC1101

Acesse **RF → Config** para ajustar:

| Opção | Descrição |
|-------|-----------|
| RF TX Pin | GPIO de transmissão (GDO0) |
| RF RX Pin | GPIO de recepção |
| RF Module | Tipo de módulo (CC1101/RF433) |
| RF Frequency | Frequência de operação |

**Pinagem CC1101:**

| CC1101 | ESP32-S3 | CYD-2USB (Legacy) |
|--------|----------|-------------------|
| GDO0 | GPIO 4 | GPIO 22 |
| CSN | GPIO 5 | GPIO 27 |
| MOSI | GPIO 35 (SPI3) | GPIO 23 |
| SCK | GPIO 36 (SPI3) | GPIO 18 |
| MISO | GPIO 37 (SPI3) | GPIO 19 |
| VCC | 3.3V | 3.3V |
| GND | GND | GND |

---

## 📡 NRF24L01 (2.4GHz)

### NRF24 Jammer

Interfere em comunicações 2.4GHz.

**Alvos potenciais:**

- Mouses/teclados wireless
- Drones
- Dispositivos IoT

### 2.4G Spectrum

Analisador de espectro para banda 2.4GHz.

**Canais:**

- 0-125 (2400-2525 MHz)

### Mousejack

Ataque a mouses e teclados wireless vulneráveis.

> [!NOTE]
> Funcionalidade em desenvolvimento.

---

## 🛠️ Configuração NRF24
>
> **SubGHz, 433MHz e NRF24.**
> Última atualização: 2025-12-19

**Pinagem NRF24L01:**

| NRF24 | ESP32-S3 | CYD-2USB (Legacy) |
|-------|----------|-------------------|
| CE | GPIO 6 | GPIO 22 |
| CSN | GPIO 7 | GPIO 27 |
| MOSI | GPIO 35 (SPI3) | GPIO 23 |
| SCK | GPIO 36 (SPI3) | GPIO 18 |
| MISO | GPIO 37 (SPI3) | GPIO 19 |
| VCC | 3.3V | 3.3V |
| GND | GND | GND |

> [!WARNING]
> **Tensão**: O NRF24L01 opera em 3.3V. Nunca conecte 5V!

---

## 📁 Formato de Arquivos

### Flipper Zero (.sub)

```
Filetype: Flipper SubGhz Key File
Version: 1
Frequency: 433920000
Preset: FuriHalSubGhzPresetOok650Async
Protocol: Princeton
Bit: 24
Key: 00 00 00 00 00 12 34 56
```

### RAW (.txt)

```
RAW_Data: 500 -500 500 -500 ...
```

---

## 📚 Protocolos Suportados

| Protocolo | Uso |
|-----------|-----|
| Princeton | Controles genéricos |
| Nice FLO | Portões Nice |
| CAME | Portões CAME |
| Linear | Controles Linear |
| Holtek | Controles baratos |
| PT2262 | Sensores, alarmes |

---

## 🔪 RF Attacks (Novos Ataques)

Menu dedicado a ataques RF para pentest educacional.

### 433MHz Jammer

Flood 433MHz com ruído aleatório.

**Como funciona:**

- Usa CC1101 com ELECHOUSE library
- Envia 255 bytes noise por segundo
- Desabilita controles remotos
- GFSK modulation, 50kbaud
- Low power para discrição

**Alvos:**

- Portões garagem
- Controles carro
- Sensores IoT
- Controles remoto

### Ghost Replay

Captura e replay sinais aleatoriamente.

**Fases:**

1. **Capture**: Espera sinal da vítima
2. **Replay**: Repete 3-12x a cada 8-45s

**Como funciona:**

- CC1101 em receive mode
- Captura código em buffer 64 bytes
- Replay automático com random timing
- Portão parece "assombrado"

**Uso:** Social engineering psicológico.

---

## 🛠️ Implementação Técnica

### CC1101 Setup

```cpp
ELECHOUSE_cc1101.Init();
ELECHOUSE_cc1101.setMHZ(433.92);
ELECHOUSE_cc1101.setModulation(2); // GFSK
ELECHOUSE_cc1101.setDRate(4800);   // 4.8kbaud gates
```

### Noise Generation

```cpp
for(int i=0; i<100; i++) {
    uint8_t noise[255];
    for(int j=0; j<255; j++) {
        noise[j] = esp_random() & 0xFF;
    }
    ELECHOUSE_cc1101.SendDataBurst(noise, 255);
    delay(10);
}
```

### Signal Capture

```cpp
if (ELECHOUSE_cc1101.CheckRxFifo()) {
    codeLength = ELECHOUSE_cc1101.ReceiveData(capturedCode);
}
```

### Random Replay

```cpp
int repeats = random(3, 13);
for (int i = 0; i < repeats; i++) {
    ELECHOUSE_cc1101.SendData(capturedCode, codeLength);
    delay(10);
}
int wait = random(8000, 45000);
delay(wait);
```
