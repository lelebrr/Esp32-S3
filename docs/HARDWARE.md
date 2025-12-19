# 🔧 Hardware - ESP32-S3-WROOM-1 N16R8

> **Especificações técnicas, pinouts e guia de montagem para ESP32-S3**  
> **Última atualização:** 2025-12-19

---

## 📋 Hardware Suportado

| Placa | Status | Flash | PSRAM |
|-------|--------|-------|-------|
| **ESP32-S3-WROOM-1 N16R8** | ✅ Principal | 16MB | 8MB Octal |
| CYD-2USB | ⚠️ Legacy | 4MB | Não |

> [!TIP]
> Para pinout completo do ESP32-S3, veja [ESP32_S3_COMPLETE_PINOUT.md](ESP32_S3_COMPLETE_PINOUT.md)

---

## 🎯 ESP32-S3-WROOM-1 N16R8 (Principal)

### Especificações

| Característica | Valor |
|---------------|-------|
| **MCU** | ESP32-S3 Dual-Core Xtensa LX7 @ 240MHz |
| **Flash** | 16MB QIO 80MHz |
| **PSRAM** | 8MB Octal SPI @ 120MHz |
| **Display** | ILI9341 2.8" 320x240 TFT |
| **Touch** | Resistivo (XPT2046) |
| **USB** | Nativo CDC (sem chip FTDI) |
| **Deep Sleep** | <5µA |
| **Alimentação** | 4x 18650 via Shield V3 |

### Pinagem Principal (ESP32-S3)

#### Display TFT (SPI2)

| Função | GPIO |
|--------|------|
| MOSI | 11 |
| MISO | 13 |
| SCK | 12 |
| CS | 10 |
| DC | 46 |
| RST | 9 |
| Backlight | 48 |

#### Touch Screen XPT2046 (SPI2)

| Função | GPIO |
|--------|------|
| CS | 3 |
| IRQ | 8 |

#### SD Card (SPI3)

| Função | GPIO |
|--------|------|
| MOSI | 35 |
| MISO | 37 |
| SCK | 36 |
| CS | 38 |

#### I2C Bus (400kHz)

| Função | GPIO | Dispositivos |
|--------|------|--------------|
| SDA | 1 | PN532 (0x24), DS3231 (0x68), PAJ7620U2 (0x73) |
| SCL | 2 | Compartilhado |

#### I2S DAC (PCM5102A)

| Função | GPIO |
|--------|------|
| BCLK | 40 |
| LRCK | 39 |
| DOUT | 41 |

#### UARTs

| UART | TX | RX | Dispositivo |
|------|----|----|-------------|
| UART0 | 43 | 44 | USB CDC |
| UART1 | 16 | 15 | GPS NEO-6M |
| UART2 | 17 | 18 | YS-IRTM IR |

#### RF Modules (SPI3)

| Módulo | CS | IRQ/CE |
|--------|-------|--------|
| CC1101 | 5 | GDO0=4 |
| NRF24 | 7 | CE=6 |

#### Sensores e Controle

| Função | GPIO |
|--------|------|
| DS18B20 (1-Wire) | 47 |
| WS2812B RGB | 48 |
| Buzzer | 45 |
| MOSFET IRF520 | 42 |
| Battery ADC | 14 |
| Boot Button | 0 |

---

## ⚠️ CYD-2USB (Legacy)

> [!WARNING]
> O CYD-2USB é mantido apenas para compatibilidade. Novos projetos devem usar ESP32-S3.

### Especificações CYD-2USB

| Característica | Valor |
|---------------|-------|
| **Microcontrolador** | ESP32-WROOM-32 |
| **Flash** | 4MB |
| **PSRAM** | Não disponível |
| **Display** | ILI9341 2.8" 320x240 TFT |
| **Touch** | Resistivo (XPT2046) |
| **USB** | 2x Micro USB (UART bridge) |
| **Alimentação** | 5V via USB |

### Pinagem CYD-2USB (Legacy)

#### Display TFT (ILI9341)

| Função | GPIO |
|--------|------|
| MISO | 12 |
| MOSI | 13 |
| SCLK | 14 |
| CS | 15 |
| DC | 2 |
| Backlight | 21 |

#### Touch (XPT2046)

| Função | GPIO |
|--------|------|
| CS | 33 |

#### SD Card

| Função | GPIO |
|--------|------|
| CS | 5 |
| SCK | 18 |
| MISO | 19 |
| MOSI | 23 |

#### I2C (Grove)

| Função | GPIO |
|--------|------|
| SDA | 27 |
| SCL | 22 |

---

## 🔌 Módulos Externos Conectados

> [!NOTE]
> Alguns módulos compartilham os mesmos pinos (conexão em paralelo). Isso é normal e funciona corretamente desde que apenas um módulo seja ativo por vez.

---

### 📡 NEO-6M GPS Module (com Antena Externa)

**Descrição:** Módulo GPS de alta sensibilidade com suporte a antena externa para melhor recepção de sinal.

**Conector:** P1 (pinos de expansão) + Alimentação via Shield V3

```
┌─────────────────────────────────────────────┐
│           NEO-6M GPS Module                  │
│  ┌──────────────────────────────────────┐   │
│  │   [Antena Externa]                   │   │
│  │        ◯                             │   │
│  │   ╔══════════════════════════════╗   │   │
│  │   ║     NEO-6M GPS              ║   │   │
│  │   ║     ~~~~~~~~~~~~~~~~~~~     ║   │   │
│  │   ╚══════════════════════════════╝   │   │
│  │                                      │   │
│  │  [VCC] [RX] [TX] [GND]              │   │
│  └──────────────────────────────────────┘   │
└─────────────────────────────────────────────┘
```

**Conexões (✅ INSTALADO):**

| GPS NEO-6M | Conexão Física | Destino | Notas |
|------------|----------------|---------|-------|
| **VCC** | Shield V3 (3ª Saída 5V) | Fio (+) | **Com Interruptor** Liga/Desliga |
| **GND** | P1 GND (CYD) | Fio (-) | - |
| **RX** | P1 RX (CYD) | Fio Direto | *Verificar se requer cruzamento (TX)* |
| **TX** | P1 TX (CYD) | Fio Direto | *Verificar se requer cruzamento (RX)* |

> [!TIP]
> **Alimentação:** Conectado na 3ª porta de 5V do Shield V3 para garantir corrente estável sem sobrecarregar o regulador da CYD.

> [!WARNING]
> **Nota sobre RX/TX:** A comunicação Serial padrão é cruzada (TX de um vai no RX do outro).
> Se o GPS não for detectado, tente inverter: GPS TX → CYD RX e GPS RX → CYD TX.

---

### 💳 PN532 NFC/RFID Module V3 (Read/Write)

**Descrição:** Módulo NFC/RFID para leitura e escrita de tags MIFARE, NTAG, FeliCa, etc.

**Modo:** I2C (DIP switches configurados na placa NFC)
**Conector:** Híbrido (CN1 + P3)

```
┌─────────────────────────────────────────────┐
│          PN532 NFC Module V3                 │
│  ┌──────────────────────────────────────┐   │
│  │   DIP Switches: [ON][OFF]            │   │
│  │      (Para modo I2C)                 │   │
│  │                                      │   │
│  │   ╔══════════════════════════════╗   │   │
│  │   ║      PN532 NFC/RFID         ║   │   │
│  │   ║   ┌──────────────────┐      ║   │   │
│  │   ║   │   Antena NFC    │      ║   │   │
│  │   ║   └──────────────────┘      ║   │   │
│  │   ╚══════════════════════════════╝   │   │
│  │                                      │   │
│  │  [VCC] [GND] [SDA] [SCL]            │   │
│  └──────────────────────────────────────┘   │
└─────────────────────────────────────────────┘
```

**Conexões (✅ INSTALADO):**

⚠️ **Configuração Personalizada:** Usa CN1 e P3 simultaneamente.

| PN532 NFC | Conexão Física | Destino na CYD | Interruptor? | Notas |
|-----------|----------------|----------------|--------------|-------|
| **VCC** | CN1 3.3V | CN1 Pino 3.3V | ✅ SIM | Liga/Desliga Geral |
| **GND** | CN1 GND | CN1 Pino GND | - | Comum |
| **SDA** | CN1 IO27 | CN1 Pino 27 | ✅ SIM | Isolamento de Barramento |
| **SCL** | P3 IO22 | P3 Pino 22 | - | Conexão Direta |

> [!IMPORTANT]
> **Interruptores de Ativação:**
>
> - Fio **VCC** possui interruptor (Alimentação)
> - Fio **SDA** possui interruptor (Dados)
> - **Ambos** devem estar ligados para o módulo funcionar.
> - Isso evita conflitos no barramento I2C quando o módulo não é necessário.

> [!TIP]
> **Configuração do PN532 para I2C:**
>
> 1. Localize os DIP switches no módulo
> 2. Configure: **SW1 = ON**, **SW2 = OFF**
> 3. Isso força o chip a operar em modo I2C.

---

### CC1101 (SubGhz RF)

```
CYD-2USB    →    CC1101
GPIO 22     →    GDO0
GPIO 27     →    CSN
GPIO 23     →    MOSI
GPIO 18     →    SCK
GPIO 19     →    MISO
3.3V        →    VCC
GND         →    GND
```

### NRF24L01 (2.4GHz)

```
CYD-2USB    →    NRF24
GPIO 22     →    CE
GPIO 27     →    CSN
GPIO 23     →    MOSI
GPIO 18     →    SCK
GPIO 19     →    MISO
3.3V        →    VCC
GND         →    GND
```

### LED Infravermelho

```
CYD-2USB    →    IR LED
GPIO 22     →    Anodo (via resistor 100Ω)
GND         →    Catodo
```

### Receptor Infravermelho

```
CYD-2USB    →    IR Receiver
GPIO 35     →    OUT
3.3V        →    VCC
GND         →    GND
```

---

## 🔀 Conexões em Paralelo

> [!NOTE]
> Os seguintes módulos compartilham os mesmos pinos e podem ser conectados em paralelo:

| Grupo | Módulos | Pinos Compartilhados |
|-------|---------|---------------------|
| **I2C** | PN532, Sensores I2C | GPIO 27 (SDA), GPIO 22 (SCL) |
| **SPI** | CC1101, NRF24, SD Card | GPIO 18, 19, 23 |

**Regra:** Apenas um módulo SPI deve estar ativo (CS baixo) por vez. Módulos I2C podem coexistir com endereços diferentes.

---

## ⚡ Notas Importantes

> [!WARNING]
> **Tensão**: O ESP32 opera em 3.3V. Não conecte sinais de 5V diretamente nos GPIOs.

> [!TIP]
> **Módulos SPI**: Os módulos CC1101 e NRF24 compartilham os mesmos pinos SPI. Use apenas um de cada vez ou adicione um multiplexador.

> [!NOTE]
> **SD Card**: O slot SD usa barramento SPI separado do display, permitindo uso simultâneo.

---

## 🔋 Alimentação

A placa possui 2 conectores Micro USB:

- **USB Superior**: Programação e alimentação
- **USB Inferior**: Apenas alimentação

Consumo típico: ~150-200mA em operação normal.

| Modo | Consumo Estimado |
|------|------------------|
| Idle (tela ligada) | ~150mA |
| WiFi ativo | ~200-250mA |
| GPS + WiFi + NFC | ~300-350mA |

> [!CAUTION]
> **Com múltiplos módulos conectados**, use uma fonte de alimentação adequada (mínimo 500mA, recomendado 1A).

---

## 📊 Diagrama de Blocos

```
                    ┌─────────────────────────────────────┐
                    │          CYD-2USB (ESP32)           │
                    │                                     │
    ┌───────────┐   │   ┌─────────────────────────────┐   │   ┌───────────┐
    │  NEO-6M   │◄──┼───┤ UART (TX/RX)               │   │   │ PN532 NFC │
    │   GPS     │   │   │                             │   │   │ (I2C)     │
    └───────────┘   │   │ ┌─────────┐  ┌───────────┐  │   │   └─────┬─────┘
                    │   │ │ Display │  │   Touch   │  │   │         │
                    │   │ │ ILI9341 │  │  XPT2046  │  │◄──┼─────────┘
                    │   │ └─────────┘  └───────────┘  │   │
    ┌───────────┐   │   │                             │   │   ┌───────────┐
    │  CC1101   │◄──┼───┤ SPI                         │───┼──►│ NRF24L01  │
    │  SubGHz   │   │   │                             │   │   │  2.4GHz   │
    └───────────┘   │   │ ┌─────────┐  ┌───────────┐  │   │   └───────────┘
                    │   │ │ SD Card │  │    IR     │  │   │
                    │   │ │  Slot   │  │  TX/RX    │  │   │
                    │   │ └─────────┘  └───────────┘  │   │
                    │   └─────────────────────────────┘   │
                    └─────────────────────────────────────┘
```

---

## 💾 Cartao SD

**Modelo Utilizado:** SanDisk Ultra 128GB Classe 10

| Caracteristica | Valor |
|---------------|-------|
| Capacidade | 128GB |
| Classe | Classe 10 (U1) |
| Velocidade Leitura | Ate 100MB/s |
| Formato | exFAT ou FAT32 |

> [!TIP]
> Recomenda-se formatar o cartao em FAT32 para melhor compatibilidade com o ESP32.

---

## 🔋 Modulo de Bateria

**Modelo:** Shield V3 Carregador de Bateria 18650 4X
**Fonte:** [Usinainfo](https://www.usinainfo.com.br/carregador-de-bateria/shield-v3-carregador-de-bateria-18650-4x-com-usb-saida-5v-3v-e-protecao-de-sobrecarga-6162.html)

```
┌────────────────────────────────────────────────────────────┐
│           Shield V3 Carregador 18650 4X                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                                                      │  │
│  │  [BAT1] [BAT2] [BAT3] [BAT4]                        │  │
│  │   18650  18650  18650  18650                        │  │
│  │                                                      │  │
│  │  ╔═══════════════════════════════════════════════╗  │  │
│  │  ║   Circuito de Carga + Protecao Integrada      ║  │  │
│  │  ╚═══════════════════════════════════════════════╝  │  │
│  │                                                      │  │
│  │  LADO ESQUERDO (3V):    LADO DIREITO (5V):          │  │
│  │  [+][-] [+][-] ...      [+][-] [+][-] ...           │  │
│  │   3V     3V              5V     5V                   │  │
│  │                                                      │  │
│  │  [Micro USB IN] [USB-C IN] [USB OUT 5V] [BOTAO]     │  │
│  │                                                      │  │
│  │  [HOLD SWITCH]  ← Para dispositivos baixo consumo   │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────┘
```

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Modelo | 18650 Shield V3 4X |
| Baterias Suportadas | 4x 18650 (Flat-top e Button-top) |
| Entrada Carga | Micro USB ou USB-C (5V 600-800mA) |
| Saida 5V | 5V DC 3A (5 pares de pinos laterais) |
| Saida 3V | 3V DC 1A (5 pares de pinos laterais) |
| Saida USB | 5V 1A (porta USB) |
| Protecao | Sobrecarga e descarga excessiva |
| Indicadores | LED Vermelho (carregando), LED Verde (completo) |
| Dimensoes | 100x90x20mm |
| Peso | 71g |

### Conexao com CYD-2USB (✅ INSTALADO)

```
     Shield V3 (5V)              Interruptor           CYD-2USB (P1)
    ┌─────────────┐             ┌─────────┐          ┌─────────────┐
    │             │             │         │          │             │
    │  5V (+) ────┼─────────────┤ ON/OFF  ├──────────┼── VCC (P1)  │
    │  1ª saída   │             │         │          │             │
    │  direita    │             └─────────┘          │             │
    │             │                                  │             │
    │  GND (-) ───┼──────────────────────────────────┼── GND (P1)  │
    │  1ª saída   │                                  │             │
    │  direita    │                                  │             │
    │             │                                  │             │
    └─────────────┘                                  └─────────────┘
```

| Shield V3 18650 | CYD-2USB | Status | Notas |
|-----------------|----------|--------|-------|
| 5V (+) - 1ª saída direita | VCC (P1) | ✅ Conectado | Via interruptor liga/desliga |
| GND (-) - 1ª saída direita | GND (P1) | ✅ Conectado | Fio direto |

### Funcoes do Shield V3

**Botao Principal:**

- 1 clique = Ligar
- Segurar 3 segundos = Desligar

**Switch HOLD:**

- Para dispositivos de baixo consumo que desligam automaticamente
- Ativa carga resistiva para manter o shield ligado

### Modo de Protecao (Importante!)
>
> [!IMPORTANT]
> **Se o Shield entrar em modo de protecao apos trocar bateria:**
>
> 1. Insira a bateria no modulo
> 2. Conecte cabo Micro USB para alimentacao externa
> 3. Aguarde alguns segundos
> 4. Remova o cabo USB
> 5. Saidas estarao habilitadas normalmente

> [!WARNING]
> **Baterias 18650:**
>
> - Use baterias de qualidade (Samsung, LG, Sony, Panasonic)
> - Todas devem ter mesma capacidade e marca
> - Nao misture baterias velhas com novas
> - **NAO INVERTA A POLARIDADE** (queimara o equipamento!)

> [!CAUTION]
> **Autonomia Estimada:**
>
> - 4x 18650 (2600mAh cada) = ~10400mAh
> - Consumo CYD + modulos = ~300mA
> - Autonomia: ~30-35 horas de uso contínuo

---

## 🔊 Alto-Falante

**Tipo:** Mini Speaker 4 Ohms 1.25W

```
┌─────────────────────────────┐
│     Mini Speaker            │
│  ┌───────────────────────┐  │
│  │    ┌───────────┐      │  │
│  │    │   ◉◉◉◉◉   │      │  │
│  │    │   ◉◉◉◉◉   │      │  │
│  │    │   ◉◉◉◉◉   │      │  │
│  │    └───────────┘      │  │
│  │                       │  │
│  │  [+] ────── [-]       │  │
│  └───────────────────────┘  │
│     25mm x 35mm x 6.5mm     │
└─────────────────────────────┘
```

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Impedancia | 4 ohms ±15% |
| Potencia | 1.25W - 3W |
| Frequencia | 0 ~ 20KHz |
| Pressao Sonora | 89 ±3dB |
| Dimensoes | 25mm x 35mm x 6.5mm |

### Conexao

| Speaker | CYD-2USB |
|---------|----------|
| (+) Positivo | SPEAKER + (conector dedicado) |
| (-) Negativo | SPEAKER - (conector dedicado) |

> [!NOTE]
> A placa CYD-2USB possui conector dedicado para alto-falante.
> Basta conectar os terminais respeitando a polaridade.

> [!TIP]
> **Funcionalidades de Audio:**
>
> - Sons de feedback do sistema
> - Alertas sonoros
> - Reproducao de tons simples

---

## 🌡️ Sensor de Temperatura DS18B20

**Tipo:** Sensor Digital de Temperatura 1-Wire (TO-92)

```
┌──────────────────────┐
│     DS18B20 TO-92    │
│                      │
│    ┌──────────────┐  │
│    │   ┌──────┐   │  │
│    │   │ CHIP │   │  │
│    │   └──────┘   │  │
│    │              │  │
│    │  [1] [2] [3] │  │
│    │  GND DQ  VDD │  │
│    └──────────────┘  │
│                      │
│  Vista Frontal (lado │
│  plano com texto)    │
└──────────────────────┘
```

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Protocolo | 1-Wire (Dallas) |
| Faixa Temperatura | -55C a +125C |
| Precisao | ±0.5C (-10C a +85C) |
| Resolucao | 9-12 bits (configuravel) |
| Alimentacao | 3.0V a 5.5V |
| Encapsulamento | TO-92 |

### Pinout DS18B20 (Vista Frontal)

| Pino | Nome | Funcao |
|------|------|--------|
| 1 (Esquerda) | GND | Terra |
| 2 (Centro) | DQ | Linha de Dados (1-Wire) |
| 3 (Direita) | VDD | Alimentacao (3.3V) |

### Conexao com CYD-2USB

| DS18B20 | CYD-2USB | Notas |
|---------|----------|-------|
| GND (Pino 1) | GND | Terra comum |
| DQ (Pino 2) | GPIO 4 | Linha de dados |
| VDD (Pino 3) | 3.3V | Alimentacao |

> [!IMPORTANT]
> **Resistor Pull-up Obrigatorio:**
> Conectar resistor de **4.7k ohms** entre DQ (dados) e 3.3V (VDD)

```
         3.3V
          │
         [4.7k]  ← Resistor Pull-up
          │
ESP32 ────┼──── DS18B20 (DQ)
GPIO 4    │
          │
         GND
```

### Uso Previsto

- Monitorar temperatura das baterias 18650
- Protecao contra superaquecimento
- Alertas de temperatura critica

> [!WARNING]
> Se a temperatura das baterias ultrapassar 45C, considere pausar operacoes pesadas.

---

### 📻 Modulo CC1101 (SubGhz 433MHz)

**Tipo:** Transceiver RF SubGHz
**Conexão:** Paralela ao NRF24 (Mesmos Pinos)

```
┌────────────────────────────────────┐
│         CC1101 Module              │
│  ┌──────────────────────────────┐  │
│  │                              │  │
│  │   ┌────────────────────┐     │  │
│  │   │      CC1101       │     │  │
│  │   │    TI Chip        │     │  │
│  │   └────────────────────┘     │  │
│  │                              │  │
│  │  [Antena de Mola ou SMA]    │  │
│  │                              │  │
│  └──────────────────────────────┘  │
│                                    │
│  VCC GND MOSI MISO SCK CSN GDO0   │
│  [1] [2] [3]  [4]  [5] [6] [7]    │
└────────────────────────────────────┘
```

**Conexões (✅ INSTALADO):**

⚠️ **Hack de Hardware:** Compartilha **EXATAMENTE** os mesmos pinos do NRF24.

| Pino CC1101 | Função | Conexão Física | Destino na CYD | Interruptor? | Notas |
|:---:|:---:|:---:|:---|:---:|:---|
| **1** | VCC | 3.3V | **CN1 3.3V** | ✅ SIM | **EXCLUSIVO** (Não ligar junto com NRF) |
| **2** | GND | GND | **CN1 GND** | - | - |
| **3** | MOSI | MOSI | **SD CMD** (IO23) | - | Soldado no pino do SD |
| **4** | MISO | MISO | **SD DAT0** (IO19)| - | Soldado no pino do SD |
| **5** | SCK | CLK | **SD CLK** (IO18) | - | Soldado no pino do SD |
| **6** | CSN | IO27 | **CN1 IO27** | ✅ SIM | Compartilhado NRF/NFC |
| **7** | GDO0 | IO22 | **P3 IO22** | - | Compartilhado NRF/NFC |

> [!DANGER]
> **CONEXÃO EM PARALELO (CRÍTICO):**
> Você ligou o CC1101 e o NRF24 nos **MESMOS** pinos lógico.
> **REGRA DE OURO:** Você **JAMAIS** pode ligar os interruptores de alimentação (VCC) do NRF24 e do CC1101 ao mesmo tempo.
>
> - Se ligar os dois: O barramento SPI travará (conflito de dados no MISO).
> - Use apenas UM módulo RF de cada vez.

> [!TIP]
> A CyD reconhecerá automaticamente qual módulo está ativo se apenas um estiver energizado.

### Funcionalidades Suportadas

- Leitura/Clonagem de controles remotos (433MHz)
- Jammer SubGHz
- Analisador de Espectro

---

### 📡 Modulo NRF24L01 (2.4GHz)

**Tipo:** Transceiver RF 2.4GHz
**Conexão:** Híbrida (CN1 + P3 + Slot SD Card)

```
┌────────────────────────────────────┐
│         NRF24L01 Module            │
│  ┌──────────────────────────────┐  │
│  │   [Antena PCB ou Externa]   │  │
│  │                              │  │
│  │   ┌────────────────────┐     │  │
│  │   │     NRF24L01      │     │  │
│  │   │     Nordic        │     │  │
│  │   └────────────────────┘     │  │
│  │                              │  │
│  └──────────────────────────────┘  │
│                                    │
│  GND VCC CE  CSN SCK MOSI MISO IRQ│
│  [1] [2] [3] [4] [5]  [6]  [7] [8]│
└────────────────────────────────────┘
```

**Conexões (✅ INSTALADO):**

⚠️ **Hack de Hardware:** Utiliza pinos soldados diretamente no slot do cartão SD para o barramento SPI.

| Pino NRF24 | Função | Conexão Física | Destino na CYD | Interruptor? | Notas |
|:---:|:---:|:---:|:---|:---:|:---|
| **1** | GND | GND | **CN1 GND** | - | - |
| **2** | VCC | 3.3V | **CN1 3.3V** | ✅ SIM | Liga/Desliga Geral |
| **3** | CE | IO22 | **P3 IO22** | - | Compartilhado com NFC (SCL) |
| **4** | CSN | IO27 | **CN1 IO27** | ✅ SIM | Isolamento (Comp. com NFC SDA) |
| **5** | SCK | CLK | **SD CLK** (IO18) | - | Soldado no pino do SD |
| **6** | MOSI | MOSI | **SD CMD** (IO23) | - | Soldado no pino do SD |
| **7** | MISO | MISO | **SD DAT0** (IO19)| - | Soldado no pino do SD |
| **8** | IRQ | - | (Não conectado)| - | - |

> [!IMPORTANT]
> **Gestão de Conflitos (NFC vs NRF24):**
> Como os pinos **IO27** e **IO22** são compartilhados com o módulo NFC:
>
> 1. **NUNCA** ligue os interruptores de alimentação (VCC) de ambos ao mesmo tempo.
> 2. O interruptor no pino **CSN (IO27)** e no **SDA (IO27)** do NFC garante que eles não interfiram um no outro quando desligados.

> [!TIP]
> **Pinos do SD Card:**
>
> - **CLK** = IO18
> - **CMD** = MOSI (IO23)
> - **DAT0** = MISO (IO19)
> Essa conexão libera os pinos laterais da placa para outros usos.

---

## 🔴 Modulos IR (Infravermelho)

### IR Receptor

```
┌───────────────────────┐
│   IR Receiver Module  │
│  ┌─────────────────┐  │
│  │   ┌─────────┐   │  │
│  │   │  ◉ IR   │   │  │
│  │   └─────────┘   │  │
│  │                 │  │
│  │  [OUT] [GND] [VCC] │
│  └─────────────────┘  │
└───────────────────────┘
```

| IR Receptor | CYD-2USB | GPIO |
|-------------|----------|------|
| VCC | 3.3V | - |
| GND | GND | - |
| OUT | - | GPIO 35 |

### IR Transmissor (LED)

```
┌───────────────────────┐
│   IR LED Transmitter  │
│  ┌─────────────────┐  │
│  │                 │  │
│  │     ◉ LED IR    │  │
│  │                 │  │
│  │  [Anodo] [Catodo] │
│  └─────────────────┘  │
└───────────────────────┘
```

| IR LED | CYD-2USB | Notas |
|--------|----------|-------|
| Anodo (+) | GPIO 22 | Via resistor 100-150 ohms |
| Catodo (-) | GND | - |

### Funcionalidades Suportadas

- TV-B-Gone (desligar TVs)
- Captura de comandos IR
- Replay de comandos
- Controle de dispositivos IR

---

## 📡 Modulo NRF24L01+PA+LNA (2.4GHz Longo Alcance)

**Modelo Especifico:** NRF24L01+PA+LNA com Antena SMA Externa

```
┌────────────────────────────────────────────┐
│       NRF24L01+PA+LNA Module               │
│  ┌──────────────────────────────────────┐  │
│  │                                      │  │
│  │   [Conector SMA para Antena]        │  │
│  │          ◯                          │  │
│  │                                      │  │
│  │   ┌────────────────────────────┐    │  │
│  │   │     NRF24L01               │    │  │
│  │   │   + PA (Power Amp)         │    │  │
│  │   │   + LNA (Low Noise Amp)    │    │  │
│  │   └────────────────────────────┘    │  │
│  │                                      │  │
│  └──────────────────────────────────────┘  │
│                                            │
│  GND VCC CE  CSN SCK MOSI MISO IRQ        │
│  [1] [2] [3] [4] [5]  [6]  [7] [8]        │
└────────────────────────────────────────────┘
```

### Diferencas do PA+LNA vs Standard

| Caracteristica | Standard | PA+LNA |
|---------------|----------|--------|
| Potencia TX | 0 dBm | **+20 dBm** |
| Alcance | ~100m | **~1100m** |
| Antena | PCB integrada | **SMA externa** |
| Consumo TX | ~12mA | **~115mA** |

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Frequencia | 2.4GHz - 2.525GHz (126 canais) |
| Taxa de Dados | 250kbps, 1Mbps, 2Mbps |
| Potencia TX | Ate +20 dBm (ajustavel) |
| Sensibilidade | -92 a -104 dBm |
| Alcance | ~520m (2Mbps), ~750m (1Mbps), ~1100m (250kbps) |
| Interface | SPI |
| Alimentacao | 3.0V - 3.6V (usar 3.3V) |
| Consumo TX | ~115mA |

### Conexao com CYD-2USB (SPI)

Ver tabela acima (idêntica ao NRF24 padrão).

> [!CAUTION]
> **Alimentacao Critica:**
>
> - Usar APENAS 3.3V no VCC
> - Consumo alto (115mA) - considerar fonte externa
> - Capacitor 10-100uF entre VCC e GND recomendado

> [!TIP]
> Para alcance maximo de 1100m, use taxa de 250kbps em campo aberto. (Configurável no software)

---

## 🔴 Modulo YS-IRTM (IR Transceiver UART)

**Modelo Especifico:** YS-IRTM - Modulo IR com Interface UART

```
┌────────────────────────────────────────────┐
│           YS-IRTM Module                    │
│  ┌──────────────────────────────────────┐  │
│  │                                      │  │
│  │   ┌──────────┐    ┌──────────┐      │  │
│  │   │  IR TX   │    │  IR RX   │      │  │
│  │   │    ◉     │    │    ◉     │      │  │
│  │   └──────────┘    └──────────┘      │  │
│  │                                      │  │
│  │   ┌────────────────────────────┐    │  │
│  │   │      Microcontrolador     │    │  │
│  │   │       (Processa IR)       │    │  │
│  │   └────────────────────────────┘    │  │
│  │                                      │  │
│  └──────────────────────────────────────┘  │
│                                            │
│        [GND] [5V] [TX] [RX]               │
└────────────────────────────────────────────┘
```

### Diferencas do YS-IRTM vs LED/Receptor Simples

| Caracteristica | LED/Receptor Simples | YS-IRTM |
|---------------|---------------------|---------|
| Interface | GPIO direto | **UART Serial** |
| Processamento | No ESP32 | **No modulo** |
| Protocolo | Varios (biblioteca) | **NEC integrado** |
| TX e RX | Separados | **Integrados** |
| Complexidade | Maior | **Menor** |

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Interface | UART (TTL Serial) |
| Protocolo IR | NEC |
| Baud Rate | 9600 (padrao), 4800, 19200, 57600 |
| Alimentacao | 5V |
| Logica | 5V (requer level shifter) |
| Funcao | Transmissor + Receptor IR |

### Conexao com CYD-2USB (UART)

| YS-IRTM | Level Shifter | CYD-2USB | GPIO |
|---------|---------------|----------|------|
| 5V | - | VIN (5V) | - |
| GND | - | GND | - |
| TX | 5V→3.3V | RX | GPIO 16 |
| RX | 3.3V→5V | TX | GPIO 17 |

> [!WARNING]
> **Level Shifter Obrigatorio!**
> O YS-IRTM opera em 5V. Conectar diretamente ao ESP32 (3.3V) pode danifica-lo.

### Esquema de Conexao com Level Shifter

```
     YS-IRTM (5V)          Level Shifter         CYD-2USB (3.3V)
    ┌───────────┐         ┌─────────────┐        ┌───────────┐
    │           │         │   HV    LV  │        │           │
    │   5V ─────┼────────▶│   5V    3.3V│◀───────┼── 3.3V    │
    │   GND ────┼────────▶│   GND   GND │◀───────┼── GND     │
    │   TX ─────┼────────▶│   HV1   LV1 │◀───────┼── GPIO 16 │
    │   RX ◀────┼─────────│   HV2   LV2 │────────┼── GPIO 17 │
    │           │         │             │        │           │
    └───────────┘         └─────────────┘        └───────────┘
```

### Comandos UART do YS-IRTM

| Comando | Funcao |
|---------|--------|
| A1 | Transmitir codigo IR gravado |
| A2 | Gravar codigo IR recebido |
| F1 | Ler endereco do modulo |
| F2 | Gravar endereco do modulo |
| F3 | Configurar baud rate |

> [!TIP]
> O YS-IRTM simplifica muito o uso de IR pois toda decodificacao eh feita no modulo.

---

## 🔧 Lista de Componentes Passivos e Auxiliares

Esta secao lista todos os componentes adicionais necessarios para protecao, estabilidade e correto funcionamento do sistema.

---

### 📋 RESUMO - LISTA DE COMPRAS

| Qtd | Componente | Valor | Para que Modulo |
|-----|------------|-------|-----------------|
| 1 | Resistor | 4.7k ohms | DS18B20 (pull-up 1-Wire) |
| 1 | Resistor | 100-150 ohms | IR LED (limitador corrente) |
| 2 | Capacitor Eletrolitico | 10-100uF 16V | NRF24L01+PA+LNA (estabilidade) |
| 2 | Capacitor Ceramico | 100nF (0.1uF) | CC1101, NRF24 (desacoplamento) |
| 1 | Level Shifter Bidirecional | 4 canais 3.3V-5V | YS-IRTM (UART) |
| 3-5 | Chave/Switch Mini | SPST ON/OFF | Liga/desliga modulos |
| 1 | Diodo Schottky | 1N5817 ou similar | Protecao polaridade bateria |
| 1 | Fusivel Resetavel (PTC) | 500mA-1A | Protecao sobrecorrente |

---

### 🔴 RESISTORES

#### 1. Resistor Pull-up para DS18B20

| Especificacao | Valor |
|---------------|-------|
| Resistencia | **4.7k ohms** (4700 ohms) |
| Potencia | 1/4W (0.25W) |
| Tolerancia | 5% ou 1% |
| Quantidade | 1 unidade |
| Funcao | Pull-up para linha 1-Wire |

```
         3.3V
          │
         [4.7k]  ← Este resistor
          │
GPIO 4 ───┼──── DS18B20 (DQ)
          │
```

#### 2. Resistor Limitador para IR LED

| Especificacao | Valor |
|---------------|-------|
| Resistencia | **100-150 ohms** |
| Potencia | 1/4W (0.25W) |
| Tolerancia | 5% |
| Quantidade | 1 unidade |
| Funcao | Limitar corrente do LED IR |

> [!NOTE]
> Se usar o modulo YS-IRTM, este resistor NAO eh necessario (ja integrado).

---

### 🔵 CAPACITORES

#### 1. Capacitor de Estabilidade para NRF24L01+PA+LNA

| Especificacao | Valor |
|---------------|-------|
| Capacitancia | **10uF a 100uF** |
| Tensao | 16V ou superior |
| Tipo | Eletrolitico |
| Quantidade | 1-2 unidades |
| Funcao | Estabilizar alimentacao durante TX (115mA) |

```
    NRF24L01+PA+LNA
         │
   VCC ──┼──┬── 3.3V
         │ [C]  ← Capacitor 10-100uF
   GND ──┼──┴── GND
```

#### 2. Capacitores de Desacoplamento

| Especificacao | Valor |
|---------------|-------|
| Capacitancia | **100nF (0.1uF)** |
| Tensao | 25V ou superior |
| Tipo | Ceramico |
| Quantidade | 2-3 unidades |
| Funcao | Filtrar ruido de alta frequencia |
| Para | CC1101, NRF24, PN532 |

> [!TIP]
> Posicionar o mais proximo possivel do pino VCC de cada modulo.

---

### 🟡 LEVEL SHIFTER (CONVERSOR DE NIVEL)

#### Para YS-IRTM (5V ↔ 3.3V)

| Especificacao | Valor |
|---------------|-------|
| Tipo | Bidirecional 4 canais |
| Tensao HV | 5V |
| Tensao LV | 3.3V |
| Quantidade | 1 unidade |
| Canais usados | 2 (TX e RX) |

**Modelos Recomendados:**

- TXS0108E (8 canais)
- BSS138 (4 canais - mais comum)
- Modulo generico "Logic Level Converter"

```
┌─────────────────────────────────┐
│     Level Shifter BSS138        │
│                                 │
│   HV ────────── LV              │
│   5V            3.3V            │
│                                 │
│   HV1 ←────────→ LV1            │
│   HV2 ←────────→ LV2            │
│   HV3 ←────────→ LV3            │
│   HV4 ←────────→ LV4            │
│                                 │
│   GND ────────── GND            │
└─────────────────────────────────┘
```

---

### 🟢 CHAVES E SWITCHES

#### Chaves Liga/Desliga para Modulos

| Especificacao | Valor |
|---------------|-------|
| Tipo | Mini Switch SPST |
| Corrente | 1A ou mais |
| Quantidade | 3-5 unidades |
| Funcao | Desligar modulos nao usados |

**Modulos que podem ter switch:**

- PN532 NFC (VCC e/ou SDA)
- GPS NEO-6M (VCC)
- NRF24L01 (VCC)
- CC1101 (VCC)
- YS-IRTM (VCC)

> [!TIP]
> Switches economizam bateria desligando modulos nao utilizados.

---

### 🔶 PROTECAO

#### 1. Diodo de Protecao de Polaridade

| Especificacao | Valor |
|---------------|-------|
| Tipo | Schottky |
| Modelo | 1N5817, 1N5819, SS14 |
| Corrente | 1A |
| Queda de Tensao | ~0.3V |
| Quantidade | 1 unidade |
| Funcao | Proteger contra inversao de polaridade |

```
    Bateria (+) ──►│── VIN (CYD)
                  Diodo Schottky
```

#### 2. Fusivel Resetavel (PTC)

| Especificacao | Valor |
|---------------|-------|
| Tipo | PTC Resetavel (Polyfuse) |
| Corrente | 500mA a 1A |
| Tensao | 6V ou superior |
| Quantidade | 1 unidade |
| Funcao | Proteger contra curto-circuito |

---

### 🟣 CONECTORES E CABOS

| Qtd | Item | Para que |
|-----|------|----------|
| 1 | Cabo Micro USB | Programacao e carga |
| 1 | Jumpers Macho-Femea | Conexoes temporarias |
| 1 | Jumpers Macho-Macho | Protoboard |
| 1 | Protoboard (opcional) | Testes |
| 1 | Fita Termica | Sensor DS18B20 nas baterias |
| 1 | Cola Quente | Fixacao de modulos |
| 1 | Espaguete Termoretrátil | Isolamento de conexoes |

---

### 📊 TABELA CONSOLIDADA

| Componente | Valor | Qtd | Preco Estimado |
|------------|-------|-----|----------------|
| Resistor 4.7k 1/4W | 4700 ohms | 1 | R$ 0,10 |
| Resistor 100-150 ohms 1/4W | 100-150 ohms | 1 | R$ 0,10 |
| Capacitor Eletrolitico 10-100uF | 16V | 2 | R$ 0,50 |
| Capacitor Ceramico 100nF | 25V | 3 | R$ 0,30 |
| Level Shifter 4ch (BSS138) | 3.3V-5V | 1 | R$ 5,00 |
| Mini Switch SPST | 1A | 5 | R$ 2,50 |
| Diodo Schottky 1N5817 | 1A | 1 | R$ 0,50 |
| Fusivel PTC Resetavel | 500mA | 1 | R$ 1,00 |
| **TOTAL ESTIMADO** | | | **~R$ 10,00** |

---

### ⚡ DIAGRAMA DE PROTECAO COMPLETO

```
                         Fusivel PTC
    Bateria 18650 ──────[  500mA  ]──────►│──── VIN
     Shield V9                         Diodo     │
                                       1N5817    │
                                                 │
                              ┌──────────────────┼──────────┐
                              │                  │          │
                             [C]               [C]         [C]
                            100uF             100nF       100nF
                              │                  │          │
                              ▼                  ▼          ▼
                           NRF24L01           CC1101      PN532
                           +PA+LNA
```

> [!IMPORTANT]
> **Ordem de Instalacao Recomendada:**
>
> 1. Primeiro: Diodo Schottky e Fusivel PTC (protecao geral)
> 2. Segundo: Capacitores de desacoplamento em cada modulo
> 3. Terceiro: Level Shifter para YS-IRTM
> 4. Quarto: Resistor pull-up para DS18B20
> 5. Por ultimo: Switches de liga/desliga

---

## 💡 LEDs WS2812B (NeoPixel RGB)

**Modelo:** WS2812B 5050 SMD RGB LED (4 unidades)

```
┌──────────────────────────────────────────────┐
│         WS2812B 5050 SMD LED                  │
│  ┌────────────────────────────────────────┐  │
│  │                                        │  │
│  │    ┌────────────────────────────┐      │  │
│  │    │         ◉ ◉ ◉             │      │  │
│  │    │       R  G  B             │      │  │
│  │    │    [Chip Controlador]     │      │  │
│  │    └────────────────────────────┘      │  │
│  │                                        │  │
│  │    [VCC] [DOUT] [GND] [DIN]           │  │
│  │     5V    Saida  Terra  Entrada       │  │
│  └────────────────────────────────────────┘  │
│                                              │
│         Tamanho: 5mm x 5mm                   │
└──────────────────────────────────────────────┘
```

### Especificacoes

| Caracteristica | Valor |
|---------------|-------|
| Tipo | LED RGB Enderecavel |
| Chip | WS2812B integrado |
| Encapsulamento | 5050 SMD (5mm x 5mm) |
| Alimentacao | 5V DC |
| Consumo | ~60mA max por LED (RGB branco) |
| Consumo Total (4 LEDs) | ~240mA max |
| Protocolo | Sinal digital 800kHz |
| Cores | 16.7 milhoes (24-bit RGB) |

### Pinout WS2812B (4 pinos)

| Pino | Nome | Funcao |
|------|------|--------|
| 1 | VCC | Alimentacao 5V |
| 2 | DOUT | Saida de dados (para proximo LED) |
| 3 | GND | Terra |
| 4 | DIN | Entrada de dados |

### Conexao em Cadeia (4 LEDs)

```
ESP32          LED1           LED2           LED3           LED4
GPIO ──[R]──► DIN→DOUT ────► DIN→DOUT ────► DIN→DOUT ────► DIN→DOUT
              │    │         │    │         │    │         │    │
         5V ──┴────┴─────────┴────┴─────────┴────┴─────────┴────┴── 5V
        GND ──┴────┴─────────┴────┴─────────┴────┴─────────┴────┴── GND

[R] = Resistor 220-470 ohms (protecao linha de dados)
```

### Conexao com CYD-2USB

| WS2812B | CYD-2USB | Notas |
|---------|----------|-------|
| VCC | 5V (VIN ou bateria) | Fonte externa recomendada |
| GND | GND | Terra comum |
| DIN (primeiro LED) | GPIO 16 | Via resistor 220-470 ohms |
| DOUT (ultimo LED) | (nao conectar) | - |

### Componentes Adicionais Necessarios

| Componente | Valor | Funcao |
|------------|-------|--------|
| Resistor | **220-470 ohms** | Protecao linha de dados |
| Capacitor | **100-1000uF 10V** | Suavizar picos de corrente |

> [!IMPORTANT]
> **Alimentacao:**
>
> - 4 LEDs x 60mA = 240mA maximo
> - Usar fonte externa 5V se possivel
> - Capacitor 100uF+ recomendado entre VCC e GND

> [!TIP]
> **Usos no Lele Origin:**
>
> - Indicador de status do sistema
> - Feedback visual de operacoes
> - Efeitos de iluminacao personalizados
> - Notificacoes coloridas (vermelho=erro, verde=ok, etc.)

### Esquema Eletrico Completo

```
                    Fonte 5V Externa
                         │
                    ┌────┴────┐
                   [C]        │
                  100uF       │
                    │         │
    ┌───────────────┴─────────┴───────────────────┐
    │                                             │
    │    ┌─────┐    ┌─────┐   ┌─────┐   ┌─────┐  │
    │    │LED1 │    │LED2 │   │LED3 │   │LED4 │  │
    │    │     │    │     │   │     │   │     │  │
    5V───┤VCC  │────┤VCC  │───┤VCC  │───┤VCC  │──┤
         │     │    │     │   │     │   │     │
ESP32    │DIN──┼───►│DIN──┼──►│DIN──┼──►│DIN  │
GPIO16──[R]    │    │     │   │     │   │     │
         │     │    │     │   │     │   │     │
GND──────┤GND  │────┤GND  │───┤GND  │───┤GND  │──┤
    │    └─────┘    └─────┘   └─────┘   └─────┘  │
    │                                             │
    └─────────────────────────────────────────────┘

[R] = Resistor 330 ohms
[C] = Capacitor 100uF+ 10V
```

---

## 📚 Referência Rápida de Módulos

| Módulo | Interface | Pinos Principais | Tensão |
|--------|-----------|------------------|--------|
| **NEO-6M GPS** | UART | TX→22, RX→27 | 3.3-5V |
| **PN532 NFC** | I2C/SPI | SDA→27, SCL→22 | 3.3-5V |
| **NRF24L01+ PA/LNA** | SPI | CE→27, CSN→5 | 3.3V |
| **CC1101 RF** | SPI | CS→27, GDO0→22 | 3.3V |
| **TSOP38238 IR RX** | Digital | OUT→35 | 2.5-5.5V |
| **WS2812B LEDs** | 1-Wire | DIN→16 | 5V |
| **DS3231 RTC** | I2C | SDA→27, SCL→22 | 3.3-5V |

> [!TIP]
> Documentação completa de cada módulo disponível em `add/hardware_modules/`

---

## ✅ Checklist de Montagem

Acompanhamento do progresso da montagem física do dispositivo.

| Componente | Status | Data | Notas |
|------------|:------:|:----:|:------|
| **🔊 Speaker 4Ω** | ✅ Instalado | 14/12/2024 | Conectado ao plugue dedicado (JST 1.25mm) |
| **🔋 Shield V3 Power** | ✅ Instalado | 14/12/2024 | Saída 5V → P1 VIN via Interruptor |
| **📡 GPS NEO-6M** | ✅ Instalado | 14/12/2024 | VCC no Shield V3 (3ª porta) + Switch |
| **💳 PN532 NFC** | ✅ Instalado | 14/12/2024 | Conexão Híbrida (CN1 + P3) + 2 Switches |
| **📡 NRF24L01+** | ✅ Instalado | 14/12/2024 | Pinos soldados Slot SD + CN1/P3 + 2 Switches |
| **📻 CC1101 (SubGhz)** | ✅ Instalado | 14/12/2024 | Paralelo ao NRF (Slot SD) (USO EXCLUSIVO) |
| **🔴 IR Transceiver** | ⏳ Pendente | - | - |
| **🌡️ Sensor DS18B20** | ⏳ Pendente | - | - |
| **💾 SD Card** | ⏳ Pendente | - | - |
| **🔌 Interruptores** | ⏳ Pendente | - | Apenas o principal instalado |
