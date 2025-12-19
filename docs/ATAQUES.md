# ⚔️ Ataques e Funcionalidades Avançadas

Doc> **Explicação técnica de como cada ataque funciona.**
> Última atualização: 2025-12-19

---

## 🎯 Resumo de Ataques

| Categoria | Ataques | Descrição |
|-----------|---------|-----------|
| WiFi | 8 | Deauth, Beacon Spam, Evil Portal, Handshake |
| BLE | 7 | Spam iOS/Android/Windows/Samsung, iBeacon |
| RF | 5 | Jammer, Bruteforce, Replay, Spectrum |
| RFID | 6 | Clone, EMV Reader, Chameleon, Amiibo |
| Ethernet | 4 | ARP Spoof, DHCP Starvation, MAC Flood |
| IR | 4 | TV-B-Gone, Jammer, Custom, Receiver |
| NRF24 | 3 | Jammer 2.4GHz, Spectrum, Channel Hop |

---

## 📡 Ataques WiFi

### Deauthentication Attack

- **Arquivo:** `wifi_atks.cpp`
- **Função:** Envia pacotes de desautenticação 802.11
- **Alvo:** Clientes de uma rede específica ou broadcast
- **Uso:** Forçar reconexão, capturar handshakes

### Beacon Spam

- **Arquivo:** `wifi_atks.cpp`
- **Função:** Cria múltiplas redes WiFi falsas
- **Modos:** Random, Rickroll, Custom (arquivo SD)
- **Uso:** Confundir usuários, saturar lista de redes

### Evil Portal

- **Arquivo:** `evil_portal.cpp`
- **Função:** Portal cativo para captura de credenciais
- **Templates:** Facebook, Google, Microsoft, Custom
- **Recursos:**
  - Deauth simultâneo para forçar conexão
  - Verificação de senha real
  - Salva credenciais em CSV

### Captura de Handshake

- **Arquivo:** `wifi_atks.cpp` (função `capture_handshake`)
- **Função:** Captura 4-way handshake WPA/WPA2
- **Saída:** Arquivo compatível com hashcat/aircrack

### Karma Attack

- **Arquivo:** `karma_attack.cpp`
- **Função:** Responde a probe requests com AP falso
- **Uso:** Capturar dispositivos buscando redes conhecidas

### Sniffer RAW

- **Arquivo:** `sniffer.cpp`
- **Função:** Captura pacotes WiFi em modo promíscuo
- **Tipos:** Beacons, Probes, Data, EAPOL

### Responder (LLMNR/NBNS)

- **Arquivo:** `responder.cpp`
- **Função:** Responder a requisições de nome de rede
- **Uso:** Capturar hashes NTLM em redes Windows

### Pwnagotchi/Lelegotchi

- **Arquivo:** `pwnagotchi.cpp`, `pwngrid.cpp`
- **Funções:**
  - Detectar Pwnagotchis próximos
  - Spam de faces e nomes falsos
  - DoScreen (nome longo para travamento)

### 🆕 WPS Attacks

- **Arquivo:** `wps_attacks.cpp`
- **Funções:**
  - **Scan WPS:** Encontra redes com WPS habilitado
  - **Pixie Dust:** Ataque offline via vulnerabilidade E-Hash
  - **PIN Flood DoS:** Envia M4 malformados para travar WPS
- **Uso:** Recuperar senhas de redes vulneráveis

### 🆕 IoT Deauth Storm

- **Arquivo:** `iot_deauth.cpp`
- **Função:** Deauth direcionado por OUI (fabricante)
- **Alvos:**
  - **Chromecast:** Google Nest, Chromecast
  - **Alexa:** Amazon Echo, Fire TV
  - **Smart TVs:** Samsung, LG
  - **Todos IoT:** Combina todos os OUIs
- **Uso:** Desconectar dispositivos IoT específicos

### 🆕 One Tap Nuke

- **Arquivos:** Multiple (combo attack)
- **Função:** Inicia todos os ataques simultaneamente
- **Componentes:**
  - IoT Deauth (todos OUIs)
  - BLE Flood (todos modos)
- **Uso:** Causar caos máximo no espectro RF

---

## 📶 Ataques Bluetooth (BLE)

### Applejuice (iOS Spam)

- **Arquivo:** `ble_spam.cpp`
- **Função:** Envia pacotes BLE de pareamento Apple
- **Efeito:** Pop-ups constantes em iPhones próximos
- **Dispositivos:** AirPods, Apple TV, HomePod, Vision Pro

### SourApple

- **Função:** Versão mais agressiva do Applejuice
- **Efeito:** Pode travar dispositivos iOS

### Windows Swift Pair Spam

- **Função:** Notificações de pareamento no Windows
- **Efeito:** Pop-ups constantes de dispositivos BLE

### Samsung Spam

- **Função:** Spam em dispositivos Samsung
- **Dispositivos:** Galaxy Buds, SmartTag, Watch

### Android Fast Pair Spam

- **Função:** Spam de dispositivos Google Fast Pair
- **Efeito:** Notificações em todos dispositivos Android

### iBeacon

- **Arquivo:** `ble_spam.cpp` (função `ibeacon`)
- **Função:** Emular beacon BLE
- **Uso:** Tracking, automação

### Ninebot Control

- **Arquivo:** `ble_ninebot.cpp`
- **Função:** Controlar patinetes Ninebot via BLE

### 🆕 BLE Flood (Combo Attack)

- **Arquivo:** `ble_flood.cpp`
- **Função:** Envia múltiplos payloads BLE simultaneamente
- **Modos:**
  - **Apple:** AirPods falsos (50+ dispositivos)
  - **Samsung:** Galaxy Buds virtuais
  - **Windows:** Swift Pair spam
  - **Android:** Fast Pair flood
  - **TODOS:** Combina todos os modos
- **Uso:** Saturar banda BLE, confundir dispositivos

---

## 📻 Ataques RF (SubGhz)

### RF Jammer

- **Arquivo:** `rf_jammer.cpp`
- **Função:** Interfere em comunicações RF
- **Modos:** Full (onda quadrada), Intermittent (PWM)
- **Frequências:** 300-928 MHz

### RF Bruteforce

- **Arquivo:** `rf_bruteforce.cpp`
- **Função:** Testa todas combinações de códigos
- **Alvos:** Portões, alarmes (Princeton, PT2262)

### RF Replay

- **Arquivo:** `rf_send.cpp`
- **Função:** Captura e reproduz sinais RF
- **Compatível:** Arquivos .sub do Flipper Zero

### RF Spectrum Analyzer

- **Arquivo:** `rf_spectrum.cpp`
- **Função:** Visualiza atividade RF em tempo real

### RF Waterfall

- **Arquivo:** `rf_waterfall.cpp`
- **Função:** Visualização em cascata do espectro

---

## 💳 Ataques RFID/NFC

### Tag Clone

- **Arquivo:** `RFID2.cpp`, `PN532.cpp`
- **Função:** Clona cartões MIFARE para Magic Cards
- **Suporta:** MIFARE Classic 1K/4K, Ultralight

### EMV Reader

- **Arquivo:** `emv_reader.cpp`
- **Função:** Lê dados de cartões de crédito/débito
- **Dados:** PAN, nome, data expiração (sem CVV)

### Chameleon Mode

- **Arquivo:** `chameleon.cpp`
- **Função:** Emula múltiplos cartões
- **Compatível:** Chameleon Mini/Tiny

### Amiibo Emulation

- **Arquivo:** `amiibo.cpp`
- **Função:** Emula Amiibos para Nintendo
- **Formato:** Dumps .bin de 540 bytes

### RFID 125kHz

- **Arquivo:** `rfid125.cpp`
- **Função:** Leitura/clonagem de cartões LF
- **Tipos:** EM4100, HID Prox II

### PN532 Killer

- **Arquivo:** `PN532KillerTools.cpp`
- **Função:** Ferramentas avançadas de NFC
- **Recursos:** Nested attack, hardnested

---

## 🌐 Ataques Ethernet (W5500)

### ARP Scanner

- **Arquivo:** `ARPScanner.cpp`
- **Função:** Descobre hosts na rede local

### ARP Spoofer

- **Arquivo:** `ARPSpoofer.cpp`
- **Função:** Man-in-the-Middle via ARP
- **Uso:** Interceptar tráfego de rede

### ARP Poisoner

- **Arquivo:** `ARPoisoner.cpp`
- **Função:** Envenenar tabela ARP de hosts

### DHCP Starvation

- **Arquivo:** `DHCPStarvation.cpp`
- **Função:** Esgota pool DHCP do servidor
- **Efeito:** Novos dispositivos não conseguem IP

### MAC Flooding

- **Arquivo:** `MACFlooding.cpp`
- **Função:** Inunda switch com MACs falsos
- **Efeito:** Switch entra em modo hub

---

## 📺 Ataques Infravermelho

### TV-B-Gone

- **Arquivo:** `TV-B-Gone.cpp`
- **Função:** Desliga TVs de múltiplas marcas
- **Códigos:** 100+ modelos (Samsung, LG, Sony, etc.)

### IR Jammer

- **Arquivo:** `ir_jammer.cpp`
- **Função:** Interfere em controles IR
- **Uso:** Bloquear TVs, ar-condicionado

### Custom IR

- **Arquivo:** `custom_ir.cpp`
- **Função:** Reproduz sinais IR capturados
- **Formatos:** Flipper .ir, RAW

### IR Receiver

- **Arquivo:** `ir_read.cpp`
- **Função:** Captura/decodifica sinais IR
- **Protocolos:** NEC, Samsung, Sony, RC5, RC6

### 🆕 TV NUKE

- **Arquivo:** `tv_nuke.cpp`
- **Função:** Envia códigos de desligar para centenas de TVs
- **Marcas:** 40+ (Samsung, LG, Sony, Philips, Philco, AOC, TCL, Semp, Hisense, Sharp, Sanyo, JVC, RCA, Vizio, Roku, Fire TV, etc.)
- **Protocolos:** NEC, Samsung, Sony, RC5, RC6, LG
- **Uso:** Desliga TODAS as TVs em um ambiente

---

## 📡 Ataques NRF24 (2.4GHz)

### NRF24 Jammer

- **Arquivo:** `nrf_jammer.cpp`
- **Função:** Interfere em comunicações 2.4GHz
- **Alvos:** Mouses, teclados, drones

### Channel Jammer

- **Função:** Jammer em canal específico

### Channel Hopper

- **Função:** Jammer alternando entre canais

### 2.4G Spectrum

- **Arquivo:** `nrf_spectrum.cpp`
- **Função:** Analisador de espectro 2.4GHz

---

## ⌨️ Ataques BadUSB/HID

### Bad BLE (Ducky via Bluetooth)

- **Arquivo:** `ducky_typer.cpp`
- **Função:** Executa scripts Ducky via BLE
- **Compatível:** Arquivos .txt Rubber Ducky

---

## 🔧 Recursos Adicionais

### iButton

- **Arquivo:** `ibutton.cpp`
- **Função:** Leitura/emulação de iButtons

### QR Code Generator

- **Arquivo:** `qrcode_menu.cpp`
- **Função:** Gera QR Codes (URL, PIX, etc.)

### JavaScript Interpreter

- **Diretório:** `bjs_interpreter/`
- **Função:** Executa scripts JavaScript no dispositivo

### FM Radio (Si4713)

- **Diretório:** `fm/`
- **Função:** Transmissor FM

---

## 🌐 Ataques WiFi Avançados CVE 2024

### DoS/Flood Attacks

| Ataque | CVE/Base | Descrição |
|--------|----------|-----------|
| MediaTek SoftAP DoS | CVE-2024-20017 | Zero-click DoS em Netgear/Xiaomi/Ubiquiti |
| SSID Confusion Downgrade | CVE-2023-52424 | Força downgrade de segurança |
| FragAttacks 2024 | FragAttacks | Management frame injection |
| Auth Flood DoS | 2024 variant | 100% em TP-Link/Asus antigos |
| Disassoc Flood | WPA3 effective | Mais efetivo em WPA3 |
| Beacon Fuzzing | MediaTek/Realtek | Tag length overflow DoS |
| Channel Hop Deauth | Storm | Deauth em todos canais 1-14 |
| Mgmt Frame Crash | Fuzzing 2025 | Crash em firmwares vulneráveis |
| Loud Beacon | Max power | Interferência por potência |
| Probe Response Flood | Overload | Sobrecarrega clientes/APs |

### Captura PMKID/Handshake

| Ataque | Técnica | Descrição |
|--------|---------|-----------|
| PMKID Clientless | 2018-2025 | Captura sem clientes conectados |
| Multi-Target PMKID | Batch | Coleta PMKIDs em massa |
| Fast Roaming PMKID | 802.11r | Explora roaming ativado |
| Handshake Passivo | Sniffing | 100% em redes ativas |
| Deauth + Capture | Clássico | Força reconexão para captura |
| Deauth Storm Multi | Multi-band | Múltiplos handshakes simultâneos |
| Channel Hop Sniff | Dual-band | Varre canais capturando |
| Evil Twin Downgrade | WPA3→WPA2 | Força clientes WPA3 a usar WPA2 |
| SAE to PSK | 2025 variant | Downgrade em transition mode |
| Evil Twin Captive | Phishing | Combina captura com portal |

> ⚠️ **Uso educacional apenas!** Teste somente em redes autorizadas.
