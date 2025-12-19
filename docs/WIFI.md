# 📡 WiFi

Documentação das funcionalidades WiFi do Lele Origin.

---

## 🔌 Conectar a uma Rede

### Connect to WiFi

Conecta o dispositivo a uma rede WiFi existente.

1. Acesse **WiFi → Connect to WiFi**
2. Aguarde a varredura de redes
3. Selecione a rede desejada
4. Digite a senha (se necessário)
5. Aguarde a conexão

> [!TIP]
> A conexão é salva automaticamente para reconexão futura.

---

## 📶 Ponto de Acesso

### WiFi AP

Cria um ponto de acesso WiFi para outros dispositivos se conectarem.

- **SSID padrão**: Lele_AP
- **Senha padrão**: 12345678
- **IP do dispositivo**: 192.168.4.1

---

## ⚔️ Ataques WiFi

### Beacon Spam

Cria múltiplas redes WiFi falsas para confundir usuários.

**Modos disponíveis:**

- **Random**: Nomes aleatórios
- **Rickroll**: Letras da música
- **Custom**: Lista personalizada (arquivo no SD)

**Como usar:**

1. Acesse **WiFi → WiFi Atks → Beacon Spam**
2. Selecione o modo
3. O ataque inicia automaticamente
4. Toque na tela para parar

---

### Target Attack

Ataque direcionado a uma rede específica.

**Opções:**

- **Information**: Exibe informações da rede
- **Target Deauth**: Desautentica clientes
- **EvilPortal + Deauth**: Portal cativo com deauth

**Como usar:**

1. Acesse **WiFi → WiFi Atks → Target Atk**
2. Selecione a rede alvo
3. Escolha o tipo de ataque
4. Execute

---

### Deauth Flood

Envia pacotes de desautenticação para múltiplas redes.

> [!WARNING]
> Este ataque pode desconectar dispositivos de suas redes. Use apenas em ambientes controlados e com autorização.

---

# > **Ferramentas de análise e ataque WiFi.**
> Última atualização: 2025-12-19

---

## 🗺️ Wardriving

Mapeamento de redes WiFi com localização GPS.

**Requisitos:**

- Módulo GPS conectado (TX: GPIO 1, RX: GPIO 3)
- SD Card para salvar dados

**Formato de saída**: CSV compatível com Wigle

**Como usar:**

1. Conecte o módulo GPS
2. Acesse **WiFi → Wardriving**
3. Inicie a captura
4. Os dados são salvos automaticamente no SD

---

## 🌐 Evil Portal

Portal cativo falso para captura de credenciais.

**Templates disponíveis:**

- Facebook
- Google
- Microsoft
- Personalizado (via SD Card)

**Como usar:**

1. Acesse **WiFi → Evil Portal**
2. Selecione o template
3. O ponto de acesso é criado
4. Credenciais capturadas são exibidas na tela

---

## 🔍 Scan Hosts

Varredura de hosts na rede local.

**Informações coletadas:**

- IP
- MAC Address
- Fabricante (OUI)
- Portas abertas

---

## 💻 Ferramentas de Rede

### TelNet

Cliente Telnet para conexão remota.

### SSH

Cliente SSH para conexão segura.

### RAW Sniffer

Captura de pacotes WiFi em modo promíscuo.

**Tipos de pacotes:**

- Beacons
- Probe Requests
- Probe Responses
- Data frames

### TCP Client

Cliente TCP para conexão a servidores.

### TCP Listener

Servidor TCP para receber conexões.

---

## 🔒 Wireguard

Túnel VPN Wireguard para conexão segura.

**Configuração:**

1. Crie um arquivo de configuração no SD Card
2. Acesse **WiFi → Wireguard**
3. Selecione o arquivo de configuração
4. Conecte ao túnel

---

## 👾 Lelegotchi

Modo de interação com Pwnagotchis.

**Funcionalidades:**

- Detectar Pwnagotchis próximos
- Exibir faces e nomes
- Spam de identidades
- DoScreen (nome longo para travamento)

---

## 🔪 MITM Attacks (Novos Ataques)

Menu dedicado a ataques Man-in-the-Middle WiFi.

### DNS Spoof

Redirecionamento de consultas DNS.

**Como funciona:**

- Intercepta requests DNS
- Responde com IPs falsos
- Redireciona para servidores maliciosos
- Requer ettercap/dnsspoof externo

### HTTPS Downgrade

Strip SSL/TLS para forçar HTTP.

**Como funciona:**

- Usa sslstrip2/BetterCAP5
- Remove proteção HTTPS
- Captura credenciais em plain text
- Funciona com certificados auto-assinados

### Captive Survey

Portal phishing com survey.

**Como funciona:**

- AP "McFreeWiFi" falso
- Redireciona para survey
- Pede CPF/telefone
- "Ganhe café grátis" como isca

### Beacon Spam

Flood com APs falsas para drenar bateria.

**Como funciona:**

- Cria 50+ redes fake
- Nomes: FreeNetflix, GratisDisney+
- Dispositivo tenta conectar constantemente
- Bateria cai 30%, força troca de aparelho
- Captura dados do novo device

### Spotify OAuth

Portal falso para roubar tokens Spotify.

**Como funciona:**

- AP "SpotifyFree" falso
- Página login falsa OAuth
- Captura tokens de autenticação
- Acesso completo à conta

---

## 🛠️ Implementação Técnica

### Evil Portal Setup

```cpp
WiFi.softAP(apName.c_str(), "", 1);
dnsServer.start(53, "*", WiFi.softAPIP());
webServer.begin();
```

### Beacon Spam Generation

```cpp
// Flood with fake SSIDs
WiFi.softAP("FreeNetflix", "", random(1,14));
WiFi.softAP("GratisDisney+", "", random(1,14));
// 50+ APs simultâneos
```

### HTTPS Downgrade

```bash
# BetterCAP5 command
bettercap -iface wlan0 -T TARGET_IP --proxy-https
```

### OAuth Phishing

```html
<!-- Fake Spotify login -->
<form action="/oauth" method="POST">
    <input name="username" placeholder="Username">
    <input name="password" type="password">
    <input name="oauth_token" hidden>
</form>
```

### Survey Portal

```cpp
// Captive portal with survey
webServer.on("/", [](){
    request->send(200, "text/html",
        "<h1>Win Free Coffee!</h1>"
        "<form action='/steal'>"
        "<input name='cpf' placeholder='CPF'>"
        "<input name='phone' placeholder='Phone'>"
        "</form>"
    );
});
```
