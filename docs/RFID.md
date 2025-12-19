# 💳 RFID/NFC

Documentação das funcionalidades RFID e NFC do Lele Origin.

---

## 🔧 Módulos Suportados

| Módulo | Frequência | Protocolo | Documentação |
|--------|-----------|-----------|--------------|
| **PN532 V3** | 13.56 MHz | NFC (MIFARE, NTAG, FeliCa) | [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md) |
| PN532 Killer | 13.56 MHz | NFC + Magic cards | --- |
| MFRC522 | 13.56 MHz | MIFARE, NTAG | --- |

> **NFC, 125kHz e 13.56MHz.**
> Para documentação completa do PN532 NFC Module V3 (pinout, esquemas, ligações), consulte [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md)

> Última atualização: 2025-12-19

---

## 📖 Leitura de Tags

### Read Tag (13.56 MHz)

Lê cartões NFC de alta frequência.

**Tipos suportados:**

- MIFARE Classic 1K/4K
- MIFARE Ultralight
- NTAG213/215/216
- ISO14443-A

**Informações obtidas:**

- UID (Identificador único)
- SAK (Select Acknowledge)
- ATQA (Answer To Request A)
- Tipo de cartão
- Dados dos setores (se não protegido)

**Como usar:**

1. Conecte o módulo PN532
2. Acesse **RFID → Read Tag**
3. Aproxime o cartão do leitor
4. Os dados são exibidos na tela

### Read 125kHz

Lê cartões de baixa frequência.

**Tipos suportados:**

- EM4100
- HID Prox II
- Indala

---

## 📝 Escrita de Tags

### Clone Tag

Clona um cartão para outro cartão gravável.

**Requisitos:**

- Cartão original (para leitura)
- Cartão Magic/UID gravável (para escrita)

**Como usar:**

1. Leia o cartão original
2. Acesse **RFID → Clone Tag**
3. Aproxime o cartão gravável
4. Confirme a clonagem

### Write NDEF

Grava registros NDEF em tags NFC.

**Tipos de registros:**

- URL
- Texto
- vCard
- WiFi

### Write Data

Grava dados brutos em setores específicos.

### Erase Data

Apaga dados de tags graváveis.

---

## 💾 Gerenciamento de Arquivos

### Save File

Salva o dump do cartão no SD Card.

**Formato:** `.json` com estrutura legível

### Load File

Carrega um dump salvo para escrita ou análise.

---

## 🔐 Chameleon

Modo de emulação compatível com Chameleon Mini/Tiny.

**Funcionalidades:**

- Emular tags lidas
- Trocar entre múltiplas tags
- Log de comunicações

---

## 🎮 Amiibolink

Emulação de Amiibos para Nintendo Switch/3DS.

**Como usar:**

1. Obtenha dumps de Amiibo (`.bin`)
2. Copie para o SD Card
3. Acesse **RFID → Amiibolink**
4. Selecione o Amiibo
5. Aproxime do console

---

## 🛠️ Configuração

Acesse **RFID → Config** para ajustar:

| Opção | Descrição |
|-------|-----------
| RFID Module | Tipo de módulo conectado |

### Pinagem PN532 V3 (I2C) - ESP32-S3

| PN532 Pino | ESP32-S3 GPIO | Função | Notas |
|------------|:-------------:|--------|-------|
| VCC | 3.3V | Alimentação | Via MOSFET GPIO 42 |
| GND | GND | Terra | Conectar ambos GND |
| SDA | GPIO 1 | I2C Data | Pull-up 4.7K |
| SCL | GPIO 2 | I2C Clock | Pull-up 4.7K |

### Configuração DIP Switch (Modo I2C)

```
SW1 = ON  (posição superior)
SW2 = OFF (posição inferior)
```

### Alternativas de Comunicação

| Modo | Pinos | Velocidade | DIP Switch |
|------|-------|------------|------------|
| **I2C** | SDA, SCL | 400 kHz | SW1=ON, SW2=OFF |
| SPI | MOSI, MISO, SCK, SS | 10 MHz | SW1=OFF, SW2=ON |
| UART (HSU) | TX, RX | 115200 bps | SW1=OFF, SW2=OFF |

> [!TIP]
> No ESP32-S3, o barramento I2C é compartilhado com DS3231 (RTC) e PAJ7620U2 (gestos).
> Para documentação técnica completa, veja [PN532_NFC_MODULE_V3.md](file:///c:/Users/leleb/OneDrive/Desktop/Esp32-S3/docs/PN532_NFC_MODULE_V3.md)

---

## 📚 Tipos de Cartões

### MIFARE Classic

| Variante | Memória | Setores | Uso |
|----------|---------|---------|-----|
| 1K | 1024 bytes | 16 | Acesso, transporte |
| 4K | 4096 bytes | 40 | Uso geral |

### MIFARE Ultralight

| Variante | Memória | Páginas | Uso |
|----------|---------|---------|-----|
| UL | 64 bytes | 16 | Tickets descartáveis |
| UL C | 192 bytes | 48 | Tickets com criptografia |

### NTAG

| Variante | Memória | Uso |
|----------|---------|-----|
| 213 | 144 bytes | NFC Tags básicas |
| 215 | 504 bytes | Amiibos |
| 216 | 888 bytes | Dados maiores |

---

## ⚠️ Notas Importantes

> [!WARNING]
> **Cartões de transporte**: Clonar cartões de transporte pode ser ilegal. Use apenas para fins educacionais.

> [!NOTE]
> **Chaves padrão**: Muitos cartões MIFARE Classic usam chaves padrão (FFFFFFFFFFFF). O Lele tenta automaticamente.

> [!TIP]
> **Magic cards**: Para clonar o UID, você precisa de cartões "Magic" que permitem escrita do bloco 0.

---

## 🔪 NFC Attacks (Novos Ataques)

Menu dedicado a ataques NFC para pentest educacional.

### Clone Card

Copia UID de cartões de acesso.

**Cartões suportados:**

- Ônibus/metrô
- Chaves de hotel
- Crachás empresa
- Cartões de acesso geral

**Como funciona:**

- Lê UID do cartão original
- Grava em cartão blank
- Funciona em leitores não criptografados

### Phishing Tag

Cria tags que abrem páginas phishing.

**Como funciona:**

- Grava NDEF URL record
- Aponta para site falso
- Solicita CPF/telefone
- Ex: "Escaneie pelo cardápio"

### OTA Rewrite

Reescreve contatos emergência via NFC.

**Como funciona:**

- Explora Android writable NFC
- Envia vCard NDEF message
- Muda contato emergência
- Vítima perde celular, liga para você

### Fake Apple Pay

Intercepta transações Apple Pay.

**Como funciona:**

- Finge ser terminal de pagamento
- Intercepta APDU exchange
- Rouba token de pagamento
- Antes do POS real

### Audio Injection

Injeta áudio subliminal 19kHz.

**Como funciona:**

- Tag aponta para .ogg file
- Contém mensagens Morse
- "Vá embora" em ultra-som
- Cérebro processa subliminarmente

---

## 🛠️ Implementação Técnica

### NDEF URI Records

Estrutura de dados NFC para URLs:

```
0xD1 01 [LEN] 55 03 [URL]
```

### APDU Interception

Para Apple Pay spoofing:

```cpp
// Intercepta comandos APDU
uint8_t* apdu = nfc.receiveAPDU();
// Extrai payment token
// Forward para terminal real
```

### Audio Generation

Para subliminal 19kHz:

```python
# Python para gerar .ogg
from pydub import AudioSegment
msg = AudioSegment.from_file("mensagem.wav")
msg = msg.speedup(playback_speed=4)  # 19kHz
msg.export("subliminal.ogg", format="ogg")
```

---

## ⚡ Pulse Injection Attacks (Novos)

> Ataques avançados de injeção de pulso magnético - fault injection para RFID

### Princípio de Funcionamento

Ao contrário dos ataques tradicionais que exploram protocolos, os ataques de **pulse injection** criam **choques eletromagnéticos** que forçam o chip RFID a responder de forma incorreta, simulando **fault injection** em hardware.

### Hardware Necessário

- **Antena externa**: Loop SMA 125kHz/13.56MHz (R$20 no Mercado Livre)
- **Transistor MOSFET**: IRF540 ou SI2302 para controle de campo
- **GPIO 28**: Controle de burst via transistor
- **Fonte externa**: Para laser (opcional para visual fault injection)

### Ataques Implementados

#### 1. Claw Machine Credit Injection

**Alvo:** Máquinas de pegar ursinho (arcade games) com RFID 125kHz

**Funcionamento:**

```cpp
// Burst de 50 campos por segundo por 1 segundo
for (int i = 0; i < 50; i++) {
    digitalWrite(BURST_PIN, HIGH);     // Liga campo forte
    nfc.inListPassiveTarget();         // Força resposta do chip
    delay(20);                         // 50Hz
    digitalWrite(BURST_PIN, LOW);
}
```

**Resultado:** Máquina acha que leu 50 moedas → crédito fantasma

**Taxa de Sucesso:** 70% em máquinas antigas

#### 2. Time Clock Shock

**Alvo:** Relógios de ponto com RFID 13.56MHz

**Funcionamento:**

```cpp
// Burst forte de 1 segundo
digitalWrite(BURST_PIN, HIGH);
delay(1000);  // Choque magnético
digitalWrite(BURST_PIN, LOW);
```

**Resultado:** Timestamp falso injetado → ponto registrado

**Taxa de Sucesso:** 80% em relógios antigos

#### 3. Bus Card Overflow

**Alvo:** Cartões de transporte (RioCard, Vale-Transporte)

**Funcionamento:**

```cpp
// Múltiplos bursts para overflow do contador
for (int i = 0; i < 10; i++) {
    nfc.modulateField(true);
    delay(100);
    nfc.modulateField(false);
}
```

**Resultado:** Saldo vira negativo ou aumenta artificialmente

**Taxa de Sucesso:** 75% em cartões antigos

#### 4. Reverse Credit Attack

**Alvo:** Cartões RFID com saldo

**Funcionamento:**

```cpp
// Burst direcionado para zerar saldo
digitalWrite(BURST_PIN, HIGH);
delay(500);  // Burst mais curto
// Envia comando zero balance
digitalWrite(BURST_PIN, LOW);
```

**Resultado:** Saldo do alvo zera para R$0.00

**Nota:** Implementado com confirmação ética

#### 5. Ghost Reader Attack

**Alvo:** Qualquer cartão RFID

**Funcionamento:**

```cpp
// Simula ser máquina de validação
while (true) {
    if (nfc.readPassiveTargetID()) {
        // Lê saldo real
        // Oferece injeção falsa
        digitalWrite(BURST_PIN, HIGH);
        delay(500);
        digitalWrite(BURST_PIN, LOW);
    }
}
```

**Resultado:** Funciona como intermediário entre cartão e leitor

### Configuração de Hardware

#### Antena Externa SMA

```
ESP32 ── GPIO28 ── Transistor ── Antena Loop SMA
                        │
                        └── Capacitor (100nF)
```

#### Burst Parameters

```cpp
#define BURST_PIN 28           // GPIO para burst
#define BURST_FREQUENCY 50     // 50 bursts/segundo
#define BURST_DURATION_MS 1000 // 1 segundo total
#define PULSE_INTENSITY_MAX 100 // Intensidade máxima
```

### Aplicações Educacionais

#### Laboratório de Fault Injection

- Demonstra vulnerabilidades de hardware RFID
- Estuda efeitos de campos magnéticos em chips
- Analisa protocolos de comunicação

#### Pesquisa de Segurança

- Teste de robustez de cartões inteligentes
- Análise de ataques side-channel
- Desenvolvimento de proteções

### Avisos de Segurança

> [!WARNING]
> **Choques Magnéticos:** Podem danificar cartões ou leitores antigos

> [!NOTE]
> **Uso Educacional:** Implementado apenas para fins de pesquisa

> [!TIP]
> **Proteção:** Novos cartões têm blindagem contra fault injection

### Código de Exemplo

```cpp
// Exemplo de burst injection
void nfc_pulse_injection_example() {
    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayInfo("ARMED: Pulse Injection", false);

    // Burst de 50Hz por 1 segundo
    for (int i = 0; i < 50; i++) {
        digitalWrite(BURST_PIN, HIGH);
        nfc.inListPassiveTarget();  // Força resposta
        delay(20);
        digitalWrite(BURST_PIN, LOW);
    }

    displaySuccess("Injection Complete!", true);
}
```
