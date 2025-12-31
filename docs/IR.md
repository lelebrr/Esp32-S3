# 📺 Infravermelho (IR)

Documentação das funcionalidades de infravermelho do Monster S3.

> **Controle Universal e TV-B-Gone com 50+ marcas.**
> Última atualização: 2025-12-28

---

## 🔧 Hardware

### ESP32-S3 (Configuração Atual)

> [!IMPORTANT]
> Pinout sincronizado com `pin_config.h`

| Função | GPIO | Notas |
|--------|:----:|-------|
| **IR TX** | 3 | LED transmissor |
| **IR RX** | -1 | Não usado nesta config |

### YS-IRTM (Opcional - UART)

Módulo IR integrado com microcontrolador (opcional):

| YS-IRTM | ESP32-S3 | Notas |
|---------|:--------:|-------|
| TX | GPIO 17 | Requer level shifter 3.3V↔5V |
| RX | GPIO 18 | Requer level shifter 3.3V↔5V |
| VCC | 5V | Via MOSFET power gating |
| GND | GND | |

### CYD-2USB (Legacy)

| Função | GPIO |
|--------|:----:|
| TX | GPIO 22 ou 27 |
| RX | GPIO 35 |

---

## 📺 TV-B-Gone

Função clássica para desligar TVs automaticamente.

**Como funciona:**

1. O Monster S3 emite sequencialmente códigos de desligamento
2. Cobre mais de **50 marcas de TVs** brasileiras e internacionais
3. **Marcas BR:** Samsung, LG, Sony, Philips, Philco, CCE, Semp, AOC, Gradiente, Britania, Mondial, Lenoxx, H-Buster
4. **Marcas Int:** Panasonic, Toshiba, TCL, Hisense, Sharp, Sanyo, Daewoo, JVC, Aiwa, Dell, Asus, Acer
5. **Streaming:** Roku TV, Fire TV
6. **Projetores:** Epson, BenQ, Optoma

**Como usar:**

1. Aponte o LED IR para a TV
2. Acesse **IR → TV-B-Gone** ou **TV Nuke**
3. Aguarde (envia 50+ códigos x 3 passes = ~300 transmissões)
4. A TV será desligada quando o código correto for enviado

> [!TIP]
> Quanto mais perto e alinhado com o receptor da TV, melhor a eficácia.

---

## 📡 IR Receiver

Captura sinais IR de controles remotos.

**Informações capturadas:**

- Protocolo (NEC, Samsung, Sony, etc.)
- Endereço
- Comando
- Dados brutos

**Como usar:**

1. Acesse **IR → IR Receiver**
2. Aponte o controle para o receptor
3. Pressione o botão do controle
4. Os dados são exibidos na tela
5. Opção de salvar para replay

---

## 🎮 Custom IR

Reproduz sinais IR salvos.

**Formatos suportados:**

- `.ir` (formato Flipper Zero)
- `.txt` (formato próprio)

**Como usar:**

1. Copie os arquivos para o SD Card (pasta `/ir/`)
2. Acesse **IR → Custom IR**
3. Selecione o arquivo
4. Transmita o sinal

---

## 📚 Protocolos Suportados

| Protocolo | Descrição | Exemplos |
|-----------|-----------|----------|
| NEC | Protocolo mais comum | LG, Samsung antigos |
| NECext | NEC estendido | LG, Yamaha |
| Samsung32 | Protocolo Samsung | TVs Samsung |
| SIRC | Sony IR Code | TVs Sony |
| SIRC15 | Sony 15 bits | Equipamentos Sony |
| SIRC20 | Sony 20 bits | Equipamentos Sony |
| RC5 | Philips RC5 | TVs Philips |
| RC5X | RC5 estendido | Equipamentos Philips |
| RC6 | Philips RC6 | Xbox, MCE remotes |

---

## 🛠️ Configuração

Acesse **IR → Config** para ajustar:

| Opção | Descrição |
|-------|-----------|
| IR TX Pin | GPIO do LED transmissor |
| IR RX Pin | GPIO do receptor |

**Pinos ESP32-S3:**

| Função | GPIO | Notas |
|--------|------|-------|
| TX | 47 | Principal |
| RX Main | 14 | ADC compartilhado |
| RX Zone 1 | 21 | Auxiliar |

**Pinos CYD-2USB (Legacy):**

| Função | GPIO |
|--------|------|
| TX | 22 ou 27 |
| RX | 35 |

---

## 📁 Formato de Arquivos

### Flipper Zero (.ir)

```
Filetype: IR signals file
Version: 1
#
name: Power
type: parsed
protocol: NEC
address: 04 00 00 00
command: 08 00 00 00
```

### RAW (.ir)

```
name: Power
type: raw
frequency: 38000
duty_cycle: 0.330000
data: 9024 4512 564 564 564 1692 ...
```

---

## 🔌 Esquema de Conexão

### LED Transmissor

```
GPIO 22 ──[100Ω]──► LED IR ──► GND
                    (Anodo)   (Catodo)
```

### Receptor IR

```
       ┌─────────┐
VCC ───┤ VS1838B ├─── GND
       │   OUT   │
       └────┬────┘
            │
         GPIO 35
```

---

## 📚 Dicas de Uso

> [!TIP]
> **Alcance**: LEDs IR genéricos têm alcance de ~3-5 metros. Para maior alcance, use LEDs de alta potência ou transistor driver.

> [!NOTE]
> **Frequência**: A maioria dos controles usa 38kHz. Alguns equipamentos antigos usam 36kHz ou 40kHz.

> [!WARNING]
> **Polaridade**: LEDs IR são polarizados. Não inverta anodo/catodo ou o LED pode queimar.

---

## 🎯 Casos de Uso

1. **Universal Remote**: Grave todos os botões de um controle e use o Lele como controle universal
2. **Automação**: Controle equipamentos IR via scripts
3. **Testes de segurança**: Verifique se sistemas usam IR para controle
4. **TV-B-Gone**: A função clássica para trollar TVs públicas
