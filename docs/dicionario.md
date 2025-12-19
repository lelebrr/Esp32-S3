# 📚 Dicionário Técnico - Lele Origin

> Explicações detalhadas de TODOS os termos técnicos usados no firmware.
> Organizado por categoria e ordem alfabética.

---

# 🔤 ÍNDICE

- [Comunicação Sem Fio](#comunicação-sem-fio)
- [Protocolos e Padrões](#protocolos-e-padrões)
- [Hardware e Componentes](#hardware-e-componentes)
- [Ataques e Técnicas](#ataques-e-técnicas)
- [Redes e Internet](#redes-e-internet)
- [Criptografia e Segurança](#criptografia-e-segurança)
- [Termos de Programação](#termos-de-programação)
- [Unidades e Medidas](#unidades-e-medidas)

---

# 📡 COMUNICAÇÃO SEM FIO

## BLE (Bluetooth Low Energy)

**O que é:**
Versão de baixo consumo do Bluetooth, projetada para dispositivos pequenos e IoT.

**Diferença do Bluetooth clássico:**

- Consome menos energia (bateria dura meses/anos)
- Menor taxa de dados (~1 Mbps vs ~3 Mbps)
- Alcance similar (~100m)

**Onde é usado:**

- Smartwatches, pulseiras fitness
- AirPods, fones wireless
- Sensores IoT, termômetros
- Teclados e mouses Bluetooth

**No Lele:**

- Spam de advertising (Applejuice)
- Emulação de teclado BLE
- Scan de dispositivos

---

## WiFi (Wireless Fidelity)

**O que é:**
Tecnologia de rede sem fio baseada no padrão IEEE 802.11.

**Frequências:**

| Banda | Canais | Velocidade | Alcance |
|-------|--------|------------|---------|
| 2.4 GHz | 1-13 | até 600 Mbps | ~50m indoor |
| 5 GHz | 36-165 | até 6.9 Gbps | ~30m indoor |

**No Lele:**

- Scan de redes
- Ataques Deauth
- Captura de handshakes
- Evil Portal

---

## NRF24 / NRF24L01

**O que é:**
Chip transceptor de rádio 2.4GHz fabricado pela Nordic Semiconductor.

**Especificações:**

- Frequência: 2.4-2.525 GHz (126 canais)
- Taxa: 250 Kbps, 1 Mbps ou 2 Mbps
- Alcance: 100m (NRF24L01+PA+LNA pode chegar a 1km)
- Potência TX: até 0 dBm

**Usos comuns:**

- Drones e controles RC
- Teclados/mouses wireless
- Sensores sem fio
- Comunicação entre Arduinos

**No Lele:**

- Spectrum analyzer
- Jammer 2.4 GHz
- MouseJack (ataque a mouses wireless)
- BLE Spam via NRF

---

## RF (Radio Frequency)

**O que é:**
Ondas eletromagnéticas usadas para transmitir dados sem fio.

**Faixas comuns:**

| Frequência | Nome | Uso típico |
|------------|------|------------|
| 315 MHz | ISM | Controles remotos (EUA/Ásia) |
| 433 MHz | ISM | Controles, portões, alarmes (Brasil/Europa) |
| 868 MHz | ISM | IoT, LoRa (Europa) |
| 915 MHz | ISM | IoT, LoRa (EUA/Brasil) |

**No Lele:**

- Captura de sinais RF
- Replay de códigos
- Bruteforce de controles
- Jammer SubGHz

---

## SubGHz

**O que é:**
Frequências de rádio abaixo de 1 GHz (sub-gigahertz).

**Vantagens:**

- Maior alcance que 2.4 GHz
- Penetra melhor em paredes
- Menos interferência

**No Lele:**
Sinônimo do menu RF (315/433/868/915 MHz).

---

## IR (Infrared / Infravermelho)

**O que é:**
Luz invisível com comprimento de onda maior que a luz vermelha (700nm-1mm).

**Como funciona:**
LED IR pisca rapidamente em padrões específicos. Receptor interpreta os padrões como comandos.

**Protocolos comuns:**

| Protocolo | Fabricante | Bits |
|-----------|------------|------|
| NEC | LG, Samsung, genéricos | 32 |
| Sony SIRC | Sony | 12, 15 ou 20 |
| RC5 | Philips | 14 |
| RC6 | Microsoft MCE | 16+ |

**No Lele:**

- TV-B-Gone (desliga TVs)
- Captura e replay de comandos
- IR Jammer

---

## ZigBee

**O que é:**
Protocolo de rede mesh para IoT, opera em 2.4 GHz.

**Características:**

- Baixo consumo de energia
- Suporta milhares de dispositivos em rede mesh
- Taxa: 250 Kbps

**Onde é usado:**

- Lâmpadas inteligentes (Philips Hue, IKEA)
- Sensores de casa inteligente
- Automação industrial

---

# 📋 PROTOCOLOS E PADRÕES

## NDEF (NFC Data Exchange Format)

**O que é:**
Formato padrão para armazenar dados em tags NFC.

**Estrutura:**

```
NDEF Message
├── Record 1 (ex: URL)
├── Record 2 (ex: Texto)
└── Record N
```

**Tipos de registro:**

| Tipo | Descrição | Exemplo |
|------|-----------|---------|
| URI | Link | <https://google.com> |
| Text | Texto simples | "Olá mundo" |
| Smart Poster | URL + título | "Site - https://..." |
| vCard | Contato | Nome, telefone, email |

**No Lele:**

- Ler dados NDEF de cartões
- Escrever NDEF em tags

---

## WPA / WPA2 / WPA3

**O que é:**
Protocolos de segurança para redes WiFi.

**Evolução:**

| Versão | Ano | Criptografia | Segurança |
|--------|-----|--------------|-----------|
| WEP | 1997 | RC4 | ❌ Quebrável em minutos |
| WPA | 2003 | TKIP | ⚠️ Vulnerável |
| WPA2 | 2004 | AES-CCMP | ✅ Seguro (com senha forte) |
| WPA3 | 2018 | SAE | ✅✅ Mais seguro |

**No Lele:**

- Exibe tipo de criptografia no scan
- Captura handshakes WPA/WPA2

---

## WPS (Wi-Fi Protected Setup)

**O que é:**
Sistema para conectar dispositivos WiFi sem digitar senha.

**Métodos:**

1. **PIN**: Digita 8 dígitos
2. **Push Button**: Aperta botão no roteador
3. **NFC**: Aproxima dispositivo

**Vulnerabilidades:**

- PIN pode ser descoberto em ~4 horas (Reaver)
- Pixie Dust extrai PIN em segundos (alguns modelos)

**No Lele:**

- Scan de redes com WPS
- Tentativa de Pixie Dust
- PIN Flood DoS

---

## EAPOL (Extensible Authentication Protocol over LAN)

**O que é:**
Protocolo usado na autenticação WPA/WPA2.

**4-Way Handshake:**

```
1. AP → Cliente: ANonce
2. Cliente → AP: SNonce + MIC
3. AP → Cliente: GTK + MIC
4. Cliente → AP: ACK
```

**Por que é importante:**
Capturando o handshake, é possível tentar quebrar a senha offline.

**No Lele:**

- Sniffer filtra pacotes EAPOL
- Conta handshakes capturados

---

## PMKID

**O que é:**
Pairwise Master Key Identifier - identificador único gerado no início da conexão WiFi.

**Vantagem sobre handshake:**
Pode ser capturado SEM cliente conectado (basta enviar association request).

**Onde fica:**
No primeiro pacote EAPOL (message 1).

**No Lele:**

- Captura PMKID Attack
- Salva para crack offline

---

## Beacon Frame

**O que é:**
Pacote WiFi enviado pelo roteador anunciando a rede.

**Contém:**

- SSID (nome da rede)
- BSSID (MAC do AP)
- Canal
- Tipo de criptografia
- Taxa suportada

**Frequência:**
~10 beacons/segundo por rede.

**No Lele:**

- Beacon Spam (cria redes falsas)
- Conta beacons no sniffer

---

## Deauth Frame

**O que é:**
Pacote WiFi que força desconexão de um cliente.

**Como funciona:**

```
[Pacote Deauth]
├── Tipo: Management Frame (0x00)
├── Subtipo: Deauthentication (0x0C)
├── Destino: Cliente MAC
├── Origem: AP MAC (spoofado)
└── Motivo: "Classe 3 frame received"
```

**Usos:**

- Forçar reconexão (para capturar handshake)
- Denial of Service
- Expulsar dispositivos da rede

**No Lele:**

- Deauth direcionado
- Deauth Flood (broadcast)

---

## Probe Request / Response

**O que é:**
Pacotes WiFi para descoberta de redes.

**Probe Request:**
Cliente pergunta: "Rede X existe?"

**Probe Response:**
AP responde: "Sim, sou a rede X!"

**Problema de privacidade:**
Dispositivos enviam probes com SSIDs conhecidos, revelando redes salvas.

**No Lele:**

- Karma Attack (responde a qualquer probe)
- Sniffer de probes

---

# 🛠️ HARDWARE E COMPONENTES

## PN532

**O que é:**
Chip fabricado pela NXP para comunicação NFC/RFID.

**Especificações:**

- Frequência: 13.56 MHz
- Protocolos: ISO14443A/B, FeliCa, Mifare
- Interfaces: I2C, SPI, UART

**Modos de operação:**

| Modo | Função |
|------|--------|
| Reader | Lê cartões |
| Writer | Escreve em cartões |
| Card Emulation | Finge ser um cartão |
| P2P | Comunicação entre dispositivos |

**No Lele:**

- PN532 I2C (pinos SDA/SCL)
- PN532 SPI (pinos MOSI/MISO/CLK/CS)
- PN532 UART (pinos TX/RX)

---

## RC522

**O que é:**
Chip RFID de baixo custo fabricado pela NXP.

**Diferença do PN532:**

| | RC522 | PN532 |
|-|-------|-------|
| Preço | ~R$10 | ~R$40 |
| Protocolos | Só Mifare | Multi-protocolo |
| Interface | Só SPI | I2C, SPI, UART |
| Card Emulation | ❌ Não | ✅ Sim |

**No Lele:**
Menu RFID detecta automaticamente qual módulo está conectado.

---

## CC1101

**O que é:**
Chip transceptor SubGHz fabricado pela Texas Instruments.

**Especificações:**

- Frequência: 300-928 MHz
- Modulação: ASK, OOK, FSK, GFSK, MSK
- Potência TX: -30 a +10 dBm
- Sensibilidade: -116 dBm

**Vantagem sobre TX/RX genérico:**

- Maior alcance
- Melhor filtro de ruído
- Mais configurável

**No Lele:**
Indicador "(CC1101)" aparece no menu RF se detectado.

---

## ESP32

**O que é:**
Microcontrolador fabricado pela Espressif com WiFi e Bluetooth integrados.

**Especificações do modelo no Lele (CYD-2USB):**

| Spec | Valor |
|------|-------|
| CPU | Dual-core Xtensa 240 MHz |
| RAM | 520 KB + 4 MB PSRAM |
| Flash | 16 MB |
| WiFi | 802.11 b/g/n 2.4 GHz |
| Bluetooth | 4.2 BR/EDR + BLE |
| GPIOs | 34 disponíveis |

---

## GPIO (General Purpose Input/Output)

**O que é:**
Pinos do microcontrolador que podem ser configurados como entrada ou saída.

**Modos:**

| Modo | O que faz |
|------|-----------|
| INPUT | Lê estado (HIGH/LOW) |
| OUTPUT | Define estado |
| INPUT_PULLUP | Entrada com resistor interno |
| PWM | Saída modulada (0-255) |

**No Lele:**
Você pode configurar qual GPIO é usado para RF TX, RF RX, IR TX, IR RX.

---

## UART (Universal Asynchronous Receiver-Transmitter)

**O que é:**
Interface serial para comunicação entre dispositivos.

**Pinos:**

| Pino | Função |
|------|--------|
| TX | Transmit (envia dados) |
| RX | Receive (recebe dados) |
| GND | Ground (terra comum) |

**Configuração típica:**

- Baud rate: 9600, 115200 (comum)
- Data bits: 8
- Stop bits: 1
- Parity: None (8N1)

**No Lele:**

- PN532 UART
- GPS UART
- Debug Serial

---

## SPI (Serial Peripheral Interface)

**O que é:**
Interface serial síncrona de alta velocidade.

**Pinos:**

| Pino | Nome | Função |
|------|------|--------|
| MOSI | Master Out Slave In | Dados master → slave |
| MISO | Master In Slave Out | Dados slave → master |
| SCK/CLK | Serial Clock | Clock de sincronização |
| CS/SS | Chip Select | Seleciona dispositivo |

**Vantagem sobre I2C:**

- Mais rápido (MHz vs kHz)
- Full-duplex (envia e recebe simultâneo)

**No Lele:**

- Display TFT (SPI)
- SD Card (SPI)
- NRF24 (SPI)
- CC1101 (SPI)

---

## I2C (Inter-Integrated Circuit)

**O que é:**
Interface serial com apenas 2 fios.

**Pinos:**

| Pino | Função |
|------|--------|
| SDA | Serial Data (dados) |
| SCL | Serial Clock (clock) |

**Endereços:**
Cada dispositivo tem endereço único (0x00 a 0x7F).

**Vantagem:**
Múltiplos dispositivos no mesmo barramento.

**No Lele:**

- PN532 (I2C)
- Sensores (DS18B20 via OneWire, similar)

---

## DS18B20

**O que é:**
Sensor digital de temperatura com interface 1-Wire.

**Especificações:**

- Faixa: -55°C a +125°C
- Precisão: ±0.5°C (entre -10°C e +85°C)
- Resolução: 9-12 bits configurável

**No Lele:**

- Monitor de temperatura
- Alerta de superaquecimento

---

## WS2812B

**O que é:**
LEDs RGB endereçáveis individualmente (NeoPixel).

**Características:**

- Cada LED tem chip controlador interno
- Alimentação: 5V
- 3 bytes por LED (R, G, B)

**No Lele:**

- Efeitos de iluminação
- Feedback visual de ataques

---

# ⚔️ ATAQUES E TÉCNICAS

## Jammer

**O que é:**
Dispositivo que emite ruído para bloquear comunicações.

**Tipos no Lele:**

| Alvo | Frequência | Efeito |
|------|------------|--------|
| WiFi Jammer | 2.4 GHz | Bloqueia WiFi e BLE |
| RF Jammer | 315/433 MHz | Bloqueia controles/alarmes |
| IR Jammer | 38 kHz IR | Bloqueia controles de TV |
| NRF Jammer | 2.4 GHz | Bloqueia 2.4 GHz específico |

**⚠️ AVISO LEGAL:**
Uso de jammers é ILEGAL em muitos países. Apenas para fins educacionais.

---

## Spam

**O que é:**
Envio massivo de pacotes/mensagens para sobrecarregar ou irritar.

**Tipos no Lele:**

| Ataque | Alvo | Efeito |
|--------|------|--------|
| BLE Spam | iOS/Android/Windows | Popups infinitos |
| Beacon Spam | WiFi | Dezenas de redes falsas |
| PIN Flood | WPS | Trava WPS do roteador |

---

## Evil Twin

**O que é:**
Rede WiFi falsa com mesmo nome de uma legítima.

**Como funciona:**

1. Cria AP com SSID igual ao alvo
2. Desautentica clientes do original
3. Clientes conectam no falso
4. Captura tráfego/credenciais

**No Lele:**
Clone AP → cria o Evil Twin

---

## Evil Portal (Captive Portal Malicioso)

**O que é:**
Página de login falsa para capturar credenciais.

**Fluxo:**

1. Vítima conecta no WiFi
2. Ao abrir navegador, vê página de login
3. Pensa que é o WiFi do hotel/aeroporto
4. Digita email/senha
5. Credenciais enviadas para atacante

**Templates no Lele:**

- Google
- Facebook
- Hotel WiFi
- Aeroporto
- Personalizado

---

## Applejuice

**O que é:**
Ataque que explora o protocolo de pareamento da Apple.

**Como funciona:**
Envia pacotes BLE fingindo ser AirPods. iPhone próximos mostram popup de pareamento infinitamente.

**Variantes:**

| Nome | Efeito |
|------|--------|
| Applejuice | Popup AirPods |
| SourApple | Crash no iOS antigo |

---

## MouseJack

**O que é:**
Vulnerabilidade em mouses/teclados wireless que usam protocolo proprietário (não BLE).

**Como funciona:**
Injeta keystroke no receptor USB do alvo.

**Marcas vulneráveis:**
Logitech, Microsoft, Dell, HP (modelos antigos)

---

## Pixie Dust

**O que é:**
Ataque offline contra WPS que explora geração fraca de números aleatórios.

**Por que funciona:**
Alguns roteadores usam valores previsíveis para gerar nonces WPS. Com análise matemática, é possível calcular o PIN.

**Requer:**
Captura de troca WPS + processamento no PC (Reaver, Bully).

---

## Karma Attack

**O que é:**
Responde a QUALQUER probe request com "Sim, sou essa rede!"

**Resultado:**
Dispositivos conectam automaticamente pensando que é rede conhecida.

---

## KRACK (Key Reinstallation Attack)

**O que é:**
Vulnerabilidade no handshake WPA2 (CVE-2017-13077).

**Como funciona:**
Força reinstalação de chave criptográfica, permitindo replay de pacotes.

---

## Replay Attack

**O que é:**
Gravar um sinal e retransmiti-lo depois.

**Uso típico:**
Capturar sinal de controle de portão e transmitir para abrir.

**Proteção (rolling code):**
Alguns controles usam código que muda a cada uso.

---

# 🌐 REDES E INTERNET

## IP Address

**O que é:**
Endereço numérico que identifica dispositivo na rede.

**IPv4:**
Formato: `192.168.1.100`
Ranges privados:

- 10.0.0.0/8
- 172.16.0.0/12
- 192.168.0.0/16

---

## MAC Address

**O que é:**
Endereço físico único de cada interface de rede.

**Formato:**
`AA:BB:CC:DD:EE:FF` (6 bytes em hexadecimal)

**Estrutura:**

- Primeiros 3 bytes: OUI (fabricante)
- Últimos 3 bytes: Identificador único

---

## BSSID

**O que é:**
Basic Service Set Identifier - MAC address do ponto de acesso WiFi.

---

## SSID

**O que é:**
Service Set Identifier - nome da rede WiFi (até 32 caracteres).

---

## ARP (Address Resolution Protocol)

**O que é:**
Protocolo que mapeia IP para MAC address.

**No Lele:**
ARP Scan descobre dispositivos na rede local.

---

## TCP/UDP

**TCP:**

- Conexão estabelecida
- Garantia de entrega
- Ordenação de pacotes
- Mais lento

**UDP:**

- Sem conexão
- Sem garantia de entrega
- Mais rápido
- Usado em streaming/jogos

---

## SSH

**O que é:**
Secure Shell - protocolo para acesso remoto seguro.

**Porta padrão:** 22

**No Lele:**
Cliente SSH para conectar em servidores Linux.

---

## Telnet

**O que é:**
Protocolo de acesso remoto **NÃO seguro** (dados em texto plano).

**Porta padrão:** 23

**⚠️ AVISO:**
Nunca use Telnet para dados sensíveis.

---

# 🔐 CRIPTOGRAFIA E SEGURANÇA

## AES (Advanced Encryption Standard)

**O que é:**
Algoritmo de criptografia simétrica usado no WPA2.

**Tamanhos de chave:**
128, 192 ou 256 bits.

---

## Hash

**O que é:**
Função que transforma dados em string de tamanho fixo.

**Propriedades:**

- Mesmo input = mesmo output (sempre)
- Diferente input = diferente output (idealmente)
- Impossível reverter

**Algoritmos comuns:**

| Algoritmo | Bits | Exemplo |
|-----------|------|---------|
| MD5 | 128 | `d41d8cd98f00b204e9800998ecf8427e` |
| SHA-1 | 160 | `da39a3ee5e6b4b0d3255bfef95601890afd80709` |
| SHA-256 | 256 | `e3b0c44298fc1c149afbf4c89...` |

---

## Handshake

**O que é:**
Troca de mensagens para estabelecer conexão segura.

**4-Way Handshake WPA:**
Troca de chaves que gera a PTK (Pairwise Transient Key).

---

# 💻 TERMOS DE PROGRAMAÇÃO

## Firmware

**O que é:**
Software permanente gravado no chip. É o "sistema operacional" do dispositivo.

---

## Byte / Bit

**Bit:** 0 ou 1 (menor unidade)
**Byte:** 8 bits

---

## Hexadecimal (Hex)

**O que é:**
Sistema numérico base 16 (0-9, A-F).

**Conversão:**

```
Decimal  Hex    Binário
0        0x00   00000000
15       0x0F   00001111
255      0xFF   11111111
```

---

## Buffer

**O que é:**
Área de memória para armazenar dados temporariamente.

---

## Callback

**O que é:**
Função que é chamada automaticamente quando algo acontece.

**Exemplo:**
Quando pacote WiFi chega, callback do sniffer é chamada.

---

# 📏 UNIDADES E MEDIDAS

## dBm (Decibéis-miliwatt)

**O que é:**
Unidade de potência usada em telecomunicações.

**Escala (WiFi):**

| dBm | Qualidade |
|-----|-----------|
| -30 a -50 | Excelente |
| -50 a -60 | Muito bom |
| -60 a -70 | Bom |
| -70 a -80 | Fraco |
| < -80 | Muito fraco |

---

## MHz / GHz

**MHz:** Megahertz (milhões de ciclos/segundo)
**GHz:** Gigahertz (bilhões de ciclos/segundo)

**Conversão:**
1 GHz = 1000 MHz

---

## Kbps / Mbps

**O que é:**
Taxa de transferência de dados.

**Kbps:** Kilobits por segundo (mil bits)
**Mbps:** Megabits por segundo (milhão bits)

**Cuidado:**
KBps (maiúsculo B) = KiloBytes (8x maior que Kbps)

---

## µs / ms / ns

**µs:** Microsegundo (1/1.000.000 segundo)
**ms:** Milisegundo (1/1.000 segundo)
**ns:** Nanosegundo (1/1.000.000.000 segundo)

**Uso no Lele:**
Pulsos IR e RF são medidos em µs.

---

# 🔧 TERMOS ESPECÍFICOS DO LELE

## Amiibo

**O que é:**
Figuras/cartões NFC da Nintendo para jogos.

**Amiibolink:**
Emulação de Amiibos no Lele.

---

## Chameleon

**O que é:**
Dispositivo USB de emulação RFID.

**Modelos:**

- Chameleon Mini
- Chameleon Tiny

**No Lele:**
Interface para controlar Chameleon conectado.

---

## TV-B-Gone

**O que é:**
Dispositivo que desliga TVs enviando códigos IR de power.

**No Lele:**
Versão integrada com 200+ códigos.

---

## Ducky Script

**O que é:**
Linguagem de script para BadUSB (USB Rubber Ducky).

**Comandos básicos:**

```
STRING Hello World    → Digita texto
DELAY 1000           → Espera 1 segundo
ENTER                → Pressiona Enter
GUI r                → Win+R
```

---

## PCAP

**O que é:**
Packet Capture - formato de arquivo para armazenar pacotes de rede.

**Programa para abrir:**
Wireshark

---

## OUI (Organizationally Unique Identifier)

**O que é:**
Primeiros 3 bytes do MAC address que identificam o fabricante.

**Exemplos:**

| OUI | Fabricante |
|-----|------------|
| 00:50:56 | VMware |
| DC:A6:32 | Raspberry Pi |
| AC:37:43 | Apple |
| 00:1A:7D | Espressif (ESP32) |

---

*Documentação Lele Origin - v1.0*
