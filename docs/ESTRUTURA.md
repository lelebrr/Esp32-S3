# 📁 Estrutura do Projeto

**Mapa de arquivos e diretórios do projeto.**
Última atualização: 2025-12-19

---

## 🗂️ Visão Geral

```
CYD 28/
├── src/                    # Código fonte principal
├── boards/                 # Configurações de placas
├── docs/                   # Documentação
├── embedded_resources/     # Interface web embarcada
├── sd_files/               # Arquivos para cartão SD
├── lib/                    # Bibliotecas customizadas
├── include/                # Headers globais
├── media/                  # Imagens e ícones
├── pcbs/                   # Designs de PCB
├── .pio/                   # Build PlatformIO (gerado)
├── .vscode/                # Configurações VS Code
└── .github/                # Workflows GitHub
```

---

## 📦 src/ - Código Fonte Principal

### src/main.cpp

Ponto de entrada do firmware. Inicializa display, WiFi, e loop principal.

### src/core/ - Núcleo do Sistema

| Arquivo | Descrição |
|---------|-----------|
| `config.cpp/h` | Configurações persistentes (NVS) |
| `display.cpp/h` | Funções de desenho na tela TFT |
| `mykeyboard.cpp/h` | Tratamento de entrada (touch, botões) |
| `sd_functions.cpp/h` | Operações com cartão SD |
| `settings.cpp/h` | Menus de configuração |
| `led_control.cpp/h` | Controle de LEDs RGB |
| `main_menu.cpp/h` | Menu principal |
| `passwords.cpp/h` | Gerenciador de senhas salvas |
| `utils.cpp/h` | Funções utilitárias |
| `theme.cpp/h` | Sistema de temas visuais |
| `powerSave.cpp/h` | Gerenciamento de energia |
| `massStorage.cpp/h` | Modo USB Mass Storage |

#### src/core/menu_items/ - Menus por Categoria

| Arquivo | Menu |
|---------|------|
| `WifiMenu.cpp/h` | Menu WiFi |
| `BleMenu.cpp/h` | Menu Bluetooth |
| `RFMenu.cpp/h` | Menu RF/SubGhz |
| `RFIDMenu.cpp/h` | Menu RFID/NFC |
| `IRMenu.cpp/h` | Menu Infravermelho |
| `NRF24.cpp/h` | Menu NRF24L01 |
| `FMMenu.cpp/h` | Menu Rádio FM |
| `FileMenu.cpp/h` | Gerenciador de arquivos |
| `ConfigMenu.cpp/h` | Configurações |
| `OthersMenu.cpp/h` | Outros recursos |
| `ScriptsMenu.cpp/h` | Scripts JavaScript |
| `ClockMenu.cpp/h` | Relógio |
| `ConnectMenu.cpp/h` | ESP-NOW |
| `GpsMenu.cpp/h` | GPS |
| `EthernetMenu.cpp/h` | Ethernet W5500 |

#### src/core/wifi/ - Módulo WiFi Core

| Arquivo | Função |
|---------|--------|
| `webInterface.cpp/h` | Servidor web e WebUI |
| `wifi_common.cpp/h` | Funções WiFi comuns |
| `wifi_mac.cpp/h` | Manipulação de MAC |
| `wg.cpp/h` | Wireguard VPN |

#### src/core/serial_commands/ - Comandos Serial

| Arquivo | Tipo de Comandos |
|---------|------------------|
| `badusb_commands.cpp` | BadUSB via serial |
| `crypto_commands.cpp` | TOTP, criptografia |
| `rfid_commands.cpp` | RFID via serial |
| `util_commands.cpp` | Utilitários gerais |
| `wifi_commands.cpp` | WiFi via serial |

#### src/core/connect/ - ESP-NOW

Comunicação P2P entre dispositivos Lele.

---

### src/modules/ - Módulos de Funcionalidades

#### src/modules/wifi/ - Ataques WiFi

| Arquivo | Funcionalidade |
|---------|----------------|
| `wifi_atks.cpp/h` | Deauth, Beacon Spam, Handshake |
| `evil_portal.cpp/h` | Portal cativo malicioso |
| `deauther.cpp/h` | Deautenticação 802.11 |
| `sniffer.cpp/h` | Captura de pacotes |
| `karma_attack.cpp/h` | Karma Attack (Probe) |
| `responder.cpp/h` | LLMNR/NBNS Responder |
| `clients.cpp/h` | Scan de clientes WiFi |
| `scan_hosts.h` | Scan de hosts na rede |
| `tcp_utils.cpp/h` | Cliente/Servidor TCP |
| `ap_info.cpp/h` | Informações de AP |

#### src/modules/ble/ - Bluetooth Low Energy

| Arquivo | Funcionalidade |
|---------|----------------|
| `ble_common.cpp/h` | Funções BLE comuns |
| `ble_spam.cpp/h` | Spam iOS/Android/Windows |
| `ble_ninebot.cpp/h` | Controle Ninebot |

#### src/modules/rf/ - SubGhz/CC1101

| Arquivo | Funcionalidade |
|---------|----------------|
| `rf_scan.cpp/h` | Scan e cópia de sinais |
| `rf_send.cpp/h` | Transmissão RF |
| `rf_jammer.cpp/h` | Jammer RF |
| `rf_bruteforce.cpp/h` | Bruteforce de códigos |
| `rf_spectrum.cpp/h` | Analisador de espectro |
| `rf_waterfall.cpp/h` | Visualização cascata |
| `rf_listen.cpp/h` | Recepção passiva |
| `rf_utils.cpp/h` | Utilitários RF |
| `emit.cpp/h` | Emissão de sinais |
| `record.cpp/h` | Gravação de sinais |
| `save.cpp/h` | Salvar em arquivo |

#### src/modules/rfid/ - RFID/NFC

| Arquivo | Funcionalidade |
|---------|----------------|
| `PN532.cpp/h` | Driver PN532 |
| `pn532ble.cpp/h` | PN532 via BLE |
| `RFID2.cpp/h` | Driver MFRC522 |
| `rfid125.cpp/h` | Leitor 125kHz |
| `tag_o_matic.cpp/h` | Interface unificada |
| `chameleon.cpp/h` | Modo Chameleon |
| `amiibo.cpp/h` | Emulação Amiibo |
| `emv_reader.cpp/h` | Leitor de cartões EMV |
| `apdu.cpp/h` | Comandos APDU |
| `PN532KillerTools.cpp/h` | Ferramentas avançadas |

#### src/modules/ir/ - Infravermelho

| Arquivo | Funcionalidade |
|---------|----------------|
| `TV-B-Gone.cpp/h` | Desliga TVs |
| `ir_read.cpp/h` | Captura de sinais IR |
| `custom_ir.cpp/h` | Replay de sinais |
| `ir_jammer.cpp/h` | Jammer IR |
| `WORLD_IR_CODES.h` | Base de códigos TV |

#### src/modules/NRF24/ - 2.4GHz

| Arquivo | Funcionalidade |
|---------|----------------|
| `nrf_jammer.cpp/h` | Jammer 2.4GHz |
| `nrf_spectrum.cpp/h` | Spectrum analyzer |
| `nrf_common.cpp/h` | Funções comuns |

#### src/modules/badusb_ble/ - BadUSB/HID

| Arquivo | Funcionalidade |
|---------|----------------|
| `ducky_typer.cpp/h` | Executor Ducky Script |

#### src/modules/ethernet/ - Ataques Ethernet

| Arquivo | Funcionalidade |
|---------|----------------|
| `ARPScanner.cpp/h` | Scan de rede via ARP |
| `ARPSpoofer.cpp/h` | Man-in-the-Middle |
| `ARPoisoner.cpp/h` | Envenenamento ARP |
| `DHCPStarvation.cpp/h` | Esgota pool DHCP |
| `MACFlooding.cpp/h` | Inunda switch |
| `EthernetHelper.cpp/h` | Utilitários |

#### src/modules/pwnagotchi/ - Modo Pwnagotchi

| Arquivo | Funcionalidade |
|---------|----------------|
| `pwnagotchi.cpp/h` | Modo Lelegotchi |
| `pwngrid.cpp/h` | Comunicação Pwngrid |
| `spam.cpp/h` | Spam de faces |
| `mood.cpp/h` | Sistema de humor |
| `ui.cpp/h` | Interface visual |

#### src/modules/others/ - Outros

| Arquivo | Funcionalidade |
|---------|----------------|
| `clicker.cpp/h` | Auto-clicker USB |
| `ibutton.cpp/h` | Leitor iButton |
| `qrcode_menu.cpp/h` | Gerador QR Code |
| `timer.cpp/h` | Timer/Cronômetro |
| `audio.cpp/h` | Reprodução de áudio |
| `mic.cpp/h` | Microfone/Gravação |
| `tururururu.cpp/h` | Efeitos sonoros |

#### src/modules/bjs_interpreter/ - JavaScript

Interpretador JavaScript embarcado para scripts customizados.

#### src/modules/fm/ - Rádio FM

Transmissor FM usando módulo Si4713.

#### src/modules/gps/ - GPS

Suporte a módulos GPS para wardriving.

---

## 📋 boards/ - Configurações de Placas

```
boards/
├── CYD-2432S028/          # Placa CYD (2.8" Touch)
│   └── CYD-2432S028.ini   # Config PlatformIO
├── _boards_json/          # Definições JSON
└── pinouts/               # Diagramas de pinagem
```

**Ambientes disponíveis:**

- `CYD-2USB` - Cheap Yellow Display 2 USB (padrão)

---

## 🌐 embedded_resources/ - Interface Web

```
embedded_resources/
└── web_interface/
    ├── index.html         # Página principal
    ├── index.js           # JavaScript
    ├── index.css          # Estilos
    ├── login.html         # Página de login
    └── theme.css          # Temas
```

**Funcionalidades:**

- Gerenciador de arquivos
- Navegador do dispositivo (tela remota)
- Upload de arquivos
- Configurações

---

## 💾 sd_files/ - Arquivos para SD Card

```
sd_files/
├── portals/               # Templates Evil Portal
│   ├── pt-br/            # Português (14 templates)
│   │   ├── nubank.html
│   │   ├── mercadolivre.html
│   │   ├── ifood.html
│   │   ├── whatsapp.html
│   │   ├── bancodobrasil.html
│   │   ├── caixa.html
│   │   ├── netflix.html
│   │   ├── pix.html
│   │   ├── wifi_gratis.html
│   │   └── ... (facebook, google, etc)
│   └── en/               # Inglês
├── infrared/             # Códigos IR
│   ├── TVs/              # TVs por marca
│   │   ├── Samsung/      # 48 arquivos
│   │   ├── LG/
│   │   ├── Philco/
│   │   ├── Sony/
│   │   └── ...
│   ├── ACs/              # Ar condicionado
│   │   ├── Gree/
│   │   ├── Consul/
│   │   ├── Springer/
│   │   ├── LG/
│   │   └── ...
│   ├── Consoles/         # Videogames
│   └── Universal_TV_Remotes/
├── BadUSB and BlueDucky/ # Scripts Ducky
├── interpreter/          # Scripts JavaScript
├── nfc/                  # Dumps NFC
├── pwnagotchi/           # Faces customizadas
├── themes/               # Temas visuais
├── wifi/                 # Configs WiFi
└── reverseshell/         # Payloads
```

---

## 📚 docs/ - Documentação

```
docs/
├── README.md              # Índice
├── INICIO_RAPIDO.md       # Guia rápido
├── HARDWARE.md            # Especificações CYD
├── FUNCIONALIDADES.md     # Lista de recursos
├── ATAQUES.md             # Documentação de ataques
├── USB_ATTACKS.md         # BadUSB e Fault Injection
├── WIFI.md                # Funcionalidades WiFi
├── BLE.md                 # Bluetooth
├── RF.md                  # SubGhz/CC1101
├── RFID.md                # RFID/NFC
├── IR.md                  # Infravermelho
├── CONFIGURACAO.md        # Ajustes
├── COMPILACAO.md          # Como compilar
├── CONTRIBUIDORES.md      # Créditos
└── ESTRUTURA.md           # Este arquivo
```

---

## 📁 Outros Diretórios

### lib/ - Bibliotecas

Bibliotecas C++ customizadas usadas pelo projeto.

### include/ - Headers Globais

Headers compartilhados e `globals.h`.

### media/ - Recursos de Mídia

Imagens, ícones e logos do projeto.

### pcbs/ - Designs PCB

Designs de placas de circuito customizadas.

---

## 📄 Arquivos Raiz

| Arquivo | Descrição |
|---------|-----------|
| `platformio.ini` | Configuração PlatformIO |
| `README.md` | README principal |
| `LICENSE` | Licença AGPL |
| `build.py` | Script de build |
| `patch.py` | Patches de bibliotecas |
| `custom_*.csv` | Esquemas de partição |
| `.gitignore` | Arquivos ignorados Git |
| `.clang-format` | Formatação de código |

---

## 📊 Estatísticas

| Item | Quantidade |
|------|------------|
| Arquivos fonte (.cpp) | ~150 |
| Headers (.h) | ~100 |
| Módulos de ataque | 15 |
| Evil Portals PT-BR | 14 |
| Arquivos IR | 135+ |
| Linhas de código | ~50.000 |
