# 📡 PN532 NFC Module V3 - Documentação Completa

> **Guia técnico completo do módulo PN532 NFC V3 para integração com ESP32-S3.**
> Última atualização: 2025-12-19

---

## 📋 Especificações Técnicas

### Informações do Chip

| Característica | Especificação |
|----------------|---------------|
| **Chip Principal** | NXP PN532 |
| **Frequência** | 13.56 MHz |
| **Tensão Operacional** | 3.3V ~ 5V |
| **Tensão Lógica I2C/UART** | 3.3V ~ 24V TTL |
| **Tensão Lógica SPI** | 3.3V TTL (com resistores de 100Ω) |
| **Corrente Idle** | ~2 mA |
| **Corrente Ativa** | ~80 mA |
| **Corrente Pico** | ~150 mA |
| **Temperatura Operacional** | -25°C ~ +85°C |
| **Dimensões** | 42.7mm x 40.4mm x 4mm |
| **Distância de Leitura** | 4-7 cm (com antena PCB) |

---

## 🔧 Componentes do Módulo

### Layout da PCB

```
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│   ┌────────────────────────────────────────┐                 │
│   │           ANTENA PCB INTEGRADA         │                 │
│   │              (13.56 MHz)               │                 │
│   │           ┌──────────────┐             │                 │
│   │           │   Bobina     │             │                 │
│   │           │   Espiral    │             │                 │
│   │           │   (Copper)   │             │                 │
│   │           └──────────────┘             │                 │
│   └────────────────────────────────────────┘                 │
│                                                              │
│     ┌─────────┐      ┌─────────────────┐    ┌────────────┐   │
│     │PN532    │      │ Level Shifter   │    │ DIP Switch │   │
│     │NXP Chip │      │ (3.3V ↔ 5V TTL) │    │  SW1  SW2  │   │
│     └─────────┘      └─────────────────┘    └────────────┘   │
│                                                              │
│        ┌──────────────────────────────────────────┐          │
│        │  VCC  GND  SDA  SCL  MOSI MISO SCK  SS   │          │
│        │   │    │    │    │    │    │    │    │   │          │
│        └───┴────┴────┴────┴────┴────┴────┴────┴───┘          │
│                     PINOS DE CONEXÃO                          │
└──────────────────────────────────────────────────────────────┘
```

### Componentes Principais

| Componente | Função |
|------------|--------|
| **NXP PN532** | Chip controlador NFC principal |
| **Antena PCB** | Bobina espiral de cobre para comunicação RF |
| **Level Shifter** | Conversor de nível lógico 3.3V ↔ 5V |
| **DIP Switch** | Seleção de modo de comunicação (I2C/SPI/UART) |
| **Capacitores SMD** | Desacoplamento e filtragem de ruídos |
| **LED Indicador** | Status de operação (alguns módulos) |
| **Cristal 27.12MHz** | Clock de referência para o PN532 |

---

## 📌 Pinout Completo

### Descrição dos Pinos

| # | Pino | Função | Descrição |
|---|------|--------|-----------|
| 1 | **VCC** | Alimentação | 3.3V ou 5V (conectar ambos VCC para estabilidade) |
| 2 | **GND** | Terra | Conectar ambos GND para estabilidade |
| 3 | **SDA** | I2C Data / UART TX | Dados I2C ou transmissão UART |
| 4 | **SCL** | I2C Clock / UART RX | Clock I2C ou recepção UART |
| 5 | **MOSI** | SPI Master Out | Dados do mestre para o escravo |
| 6 | **MISO** | SPI Master In | Dados do escravo para o mestre |
| 7 | **SCK** | SPI Clock | Clock serial SPI |
| 8 | **SS (NSS)** | SPI Chip Select | Seleção do chip (LOW = ativo) |
| 9 | **IRQ** | Interrupt | Saída de interrupção (opcional) |
| 10 | **RSTO** | Reset | Reset do módulo (opcional) |

### Diagrama Visual dos Pinos

```
        ┌───────────────────────────────────────┐
        │           PN532 NFC Module V3         │
        │                                       │
        │   ┌───────────────────────────────┐   │
        │   │      ANTENA PCB (TOPO)        │   │
        │   └───────────────────────────────┘   │
        │                                       │
        │     [SW1]  [SW2]   ← DIP Switches     │
        │                                       │
        │   ┌─────────────────────────────────┐ │
        │   │ VCC VCC GND GND SDA SCL RST IRQ │ │
        │   └─┬───┬───┬───┬───┬───┬───┬───┬───┘ │
        │     │   │   │   │   │   │   │   │     │
        │     1   2   3   4   5   6   7   8     │
        │                                       │
        │   ┌─────────────────────────────────┐ │
        │   │ SS  SCK MISO MOSI               │ │
        │   └─┬───┬───┬───┬───────────────────┘ │
        │     9   10  11  12                    │
        └───────────────────────────────────────┘
```

---

## 🔀 Seleção de Modo (DIP Switches)

O módulo PN532 V3 possui dois DIP switches para selecionar o modo de comunicação:

### Configuração dos Switches

| Modo | SW1 | SW2 | Descrição |
|------|:---:|:---:|-----------|
| **I2C** | ON | OFF | Comunicação I2C (recomendado) |
| **SPI** | OFF | ON | Comunicação SPI alta velocidade |
| **UART (HSU)** | OFF | OFF | Comunicação Serial (ambos desligados) |

### Diagrama Visual

```
     ╔═══════════════════════════════════════════════════════╗
     ║                 CONFIGURAÇÃO DIP SWITCH                ║
     ╠═══════════════════════════════════════════════════════╣
     ║                                                        ║
     ║   ┌─────────┐          ┌─────────┐          ┌─────────┐║
     ║   │         │          │         │          │         │║
     ║   │ ▓▓  □□  │          │ □□  ▓▓  │          │ □□  □□  │║
     ║   │ SW1 SW2 │          │ SW1 SW2 │          │ SW1 SW2 │║
     ║   │ ON  OFF │          │ OFF ON  │          │ OFF OFF │║
     ║   └─────────┘          └─────────┘          └─────────┘║
     ║       I2C                  SPI                 UART    ║
     ║    (2 pinos)            (4 pinos)           (2 pinos)  ║
     ╚═══════════════════════════════════════════════════════╝
```

> [!TIP]
> O modo **I2C** é recomendado por usar menos pinos (apenas SDA e SCL) e permitir compartilhamento do barramento com outros dispositivos I2C.

---

## 🔌 Conexão com ESP32-S3

### Modo I2C (Recomendado)

| Pino PN532 | GPIO ESP32-S3 | Notas |
|------------|---------------|-------|
| VCC | 3.3V | Via MOSFET (GPIO 42) para power gating |
| GND | GND | Conectar ambos GND |
| SDA | GPIO 1 | Compartilhado com DS3231, PAJ7620U2 |
| SCL | GPIO 2 | Compartilhado com DS3231, PAJ7620U2 |

**Endereço I2C:** `0x24` (padrão)

```
                    ESP32-S3-WROOM-1
                   ┌────────────────────┐
                   │                    │
    PN532 VCC ◄────┤ 3.3V (via MOSFET)  │
    PN532 GND ◄────┤ GND                │
    PN532 SDA ◄───►│ GPIO 1 (I2C SDA)   │─── Pull-up 4.7K
    PN532 SCL ◄───►│ GPIO 2 (I2C SCL)   │─── Pull-up 4.7K
                   │                    │
                   │ GPIO 42 (MOSFET)───┼──► IRF520 Gate
                   └────────────────────┘
```

### Modo SPI (Alta Velocidade)

| Pino PN532 | GPIO ESP32-S3 | Notas |
|------------|---------------|-------|
| VCC | 3.3V | Via MOSFET |
| GND | GND | --- |
| SCK | GPIO 36 | SPI3 (SD Card bus) |
| MISO | GPIO 37 | SPI3 (SD Card bus) |
| MOSI | GPIO 35 | SPI3 (SD Card bus) |
| SS (CS) | GPIO 38 | Chip Select dedicado |

> [!WARNING]
> Modo SPI compartilha barramento com SD Card. Use apenas um de cada vez!

### Modo UART (HSU)

| Pino PN532 | GPIO ESP32-S3 | Notas |
|------------|---------------|-------|
| VCC | 3.3V | Via MOSFET |
| GND | GND | --- |
| TX (SDA) | GPIO 16 | ESP RX ← PN532 TX |
| RX (SCL) | GPIO 17 | ESP TX → PN532 RX |

**Baud Rate:** 115200 bps

---

## ⚡ Esquema de Ligação Completo

### Circuito I2C com Power Gating

```
                    ┌─────────────────────────────────────────────────────┐
                    │                    PN532 NFC V3                     │
                    │                                                     │
    ┌───────────┐   │   ┌───────────┐                                     │
    │ ESP32-S3  │   │   │  IRF520   │                                     │
    │           │   │   │  MOSFET   │                                     │
    │ GPIO 42 ──┼───┼──►│ Gate      │                                     │
    │           │   │   │    Drain ─┼─────────────► VCC (PN532)           │
    │           │   │   │    Source │                    │                │
    │ 3.3V ─────┼───┼──►│           │                   [C]               │
    │           │   │   └───────────┘                  100nF              │
    │           │   │                                    │                │
    │ GND ──────┼───┼────────────────────────────────────┴──► GND (PN532) │
    │           │   │                                                     │
    │           │   │   ┌───────────────────┐                             │
    │ GPIO 1 ───┼───┼──►│◄────► SDA (PN532) │◄───┤ 4.7K ├──► 3.3V         │
    │  (SDA)    │   │   │  I2C Data         │                             │
    │           │   │   └───────────────────┘                             │
    │           │   │                                                     │
    │           │   │   ┌───────────────────┐                             │
    │ GPIO 2 ───┼───┼──►│◄────► SCL (PN532) │◄───┤ 4.7K ├──► 3.3V         │
    │  (SCL)    │   │   │  I2C Clock        │                             │
    │           │   │   └───────────────────┘                             │
    └───────────┘   │                                                     │
                    └─────────────────────────────────────────────────────┘
```

### Lista de Componentes Adicionais

| Componente | Valor | Quantidade | Função |
|------------|-------|:----------:|--------|
| Resistor Pull-up | 4.7kΩ | 2 | I2C SDA e SCL |
| Capacitor Desacoplamento | 100nF (0.1µF) | 1 | Filtragem ruído VCC |
| Capacitor Reserva | 10µF | 1 | Estabilização VCC |
| MOSFET IRF520 | --- | 1 | Power gating (opcional) |

---

## 📶 Protocolos Suportados

### Cartões/Tags RFID

| Protocolo | Tipos Suportados | Frequência |
|-----------|------------------|------------|
| **MIFARE** | Classic 1K, 4K, Ultralight, DESFire | 13.56 MHz |
| **ISO/IEC 14443-A** | Type A cards | 13.56 MHz |
| **ISO/IEC 14443-B** | Type B cards | 13.56 MHz |
| **FeliCa** | Sony RCS-860, RCS-854 | 13.56 MHz |
| **Jewel/Topaz** | Innovision IRT5001 | 13.56 MHz |
| **NTAG** | 213, 215, 216 | 13.56 MHz |

### Modos de Operação

| Modo | Descrição |
|------|-----------|
| **Reader/Writer** | Lê e escreve tags NFC/RFID |
| **Card Emulation** | Emula cartões NFC |
| **Peer-to-Peer** | Comunicação entre dispositivos NFC |

---

## 📊 Compatibilidade de Cartões

### MIFARE Classic

| Variante | Memória | Setores | Uso Típico |
|----------|---------|:-------:|------------|
| EV1 1K | 1024 bytes | 16 | Acesso, transporte |
| EV1 4K | 4096 bytes | 40 | Uso geral, armazenamento |
| Mini | 320 bytes | 5 | Tickets |

### MIFARE Ultralight

| Variante | Memória | Páginas | Segurança |
|----------|---------|:-------:|-----------|
| UL | 64 bytes | 16 | Nenhuma |
| UL C | 192 bytes | 48 | 3DES |
| UL EV1 | 128/320 bytes | 32/80 | Senha 32-bit |

### NTAG

| Variante | Memória | Uso Típico |
|----------|---------|------------|
| 213 | 144 bytes | NFC Tags básicas, URLs |
| 215 | 504 bytes | **Amiibo**, dados médios |
| 216 | 888 bytes | Dados maiores |

---

## 🔋 Consumo de Energia

| Estado | Corrente | Notas |
|--------|----------|-------|
| **Idle** | 2 mA | Esperando comandos |
| **Scan** | 80 mA | Procurando tags |
| **Transmit** | 150 mA | Comunicação ativa |
| **Deep Sleep** | <1 mA | Via power gating |

### Power Gating com MOSFET

Para economizar energia, o PN532 pode ser desligado quando não estiver em uso:

```cpp
// Ligar PN532
void pn532_power_on() {
    digitalWrite(42, HIGH);  // IRF520 MOSFET
    delay(50);               // Tempo de inicialização
}

// Desligar PN532
void pn532_power_off() {
    digitalWrite(42, LOW);   // Corta alimentação
}
```

---

## 🛠️ Código de Inicialização

### Bibliotecas Necessárias

```cpp
#include <Wire.h>
#include <Adafruit_PN532.h>

// Pinos I2C ESP32-S3
#define PN532_SDA 1
#define PN532_SCL 2
#define PN532_POWER 42  // MOSFET gate

// Criar instância I2C
Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);
```

### Setup Básico

```cpp
void setup_pn532() {
    // Ligar alimentação
    pinMode(PN532_POWER, OUTPUT);
    digitalWrite(PN532_POWER, HIGH);
    delay(50);
    
    // Inicializar I2C
    Wire.begin(PN532_SDA, PN532_SCL);
    Wire.setClock(400000);  // 400kHz
    
    // Inicializar PN532
    nfc.begin();
    
    // Verificar conexão
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("PN532 não encontrado!");
        return;
    }
    
    // Mostrar versão
    Serial.print("Chip: PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware: "); 
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);
    
    // Configurar SAM (Secure Access Module)
    nfc.SAMConfig();
    
    Serial.println("PN532 pronto!");
}
```

### Leitura de Tag

```cpp
void read_nfc_tag() {
    uint8_t uid[7];
    uint8_t uidLength;
    
    // Aguardar cartão
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.print("UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(" 0x"); Serial.print(uid[i], HEX);
        }
        Serial.println();
        
        // Identificar tipo
        if (uidLength == 4) {
            Serial.println("Tipo: MIFARE Classic");
        } else if (uidLength == 7) {
            Serial.println("Tipo: NTAG ou MIFARE Ultralight");
        }
    }
}
```

---

## ⚠️ Troubleshooting

### Problemas Comuns

| Problema | Causa Provável | Solução |
|----------|----------------|---------|
| Não detecta tags | DIP switch errado | Verificar SW1=ON, SW2=OFF para I2C |
| I2C não responde | Pull-ups ausentes | Adicionar resistores 4.7kΩ |
| Leitura intermitente | Ruído na alimentação | Adicionar capacitores 100nF + 10µF |
| Distância curta | Interferência metálica | Afastar de objetos metálicos |
| "Chip not found" | Endereço I2C errado | Scanner I2C para verificar (0x24) |

### Scanner I2C

```cpp
void i2c_scanner() {
    Serial.println("Scanning I2C...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("Encontrado: 0x");
            Serial.println(addr, HEX);
        }
    }
}
```

---

## 📚 Referências

| Recurso | Link |
|---------|------|
| Datasheet NXP PN532 | [NXP PN532 User Manual](https://www.nxp.com/docs/en/user-guide/141520.pdf) |
| Elechouse Manual V3 | elechouse.com (Manual do módulo) |
| Adafruit PN532 Library | [GitHub Adafruit_PN532](https://github.com/adafruit/Adafruit-PN532) |
| How2Electronics Tutorial | how2electronics.com |

---

## 📌 Resumo de Conexão Rápida (ESP32-S3)

```
╔════════════════════════════════════════════════════════════════╗
║               PN532 NFC V3 → ESP32-S3 (I2C)                    ║
╠══════════════╦═════════════╦═══════════════════════════════════╣
║ PN532 Pino   ║ ESP32-S3    ║ Notas                             ║
╠══════════════╬═════════════╬═══════════════════════════════════╣
║ VCC          ║ 3.3V        ║ Via MOSFET GPIO 42 (power gate)   ║
║ GND          ║ GND         ║ Conectar ambos GND do módulo      ║
║ SDA          ║ GPIO 1      ║ + Resistor 4.7K pull-up           ║
║ SCL          ║ GPIO 2      ║ + Resistor 4.7K pull-up           ║
╠══════════════╬═════════════╬═══════════════════════════════════╣
║              ║ DIP Switch: ║ SW1 = ON, SW2 = OFF               ║
╚══════════════╩═════════════╩═══════════════════════════════════╝

Endereço I2C: 0x24
Barramento compartilhado com: DS3231 (RTC), PAJ7620U2 (Gestos)
```

---

> [!NOTE]
> Este módulo faz parte do sistema Lele Origin com ESP32-S3-WROOM-1 N16R8.
> Para funcionalidades completas de RFID/NFC, consulte [RFID.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/RFID.md).
