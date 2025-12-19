# 🔧 Módulos de Ataque - Lele Origin 1.0

Documentação dos módulos de ataque funcionais.

---

## 📊 Resumo

| Categoria | Ataques |
|-----------|---------|
| WiFi | 40 |
| BLE Spam | 7 |
| USB | 10 |
| IR | 1 |
| Network Simulation | 2 |
| **Total** | **60** |

---

## 🌐 WiFi

### Ataques CVE 2024 (20 ataques DoS/CVE)

| Ataque | CVE/Base | Alvo |
|--------|----------|------|
| MediaTek SoftAP DoS | CVE-2024-20017 | Netgear, Xiaomi, Ubiquiti |
| SSID Confusion | CVE-2023-52424 | Redes mistas WPA2/WPA3 |
| FragAttacks 2024 | FragAttacks | APs não patchados |
| Auth Flood DoS | 2024 variant | TP-Link, Asus antigos |
| Disassoc Flood | WPA3 effective | APs WPA3 |
| Beacon Fuzzing | Tag overflow | MediaTek, Realtek |
| Channel Hop Deauth | Storm | Dual-band |
| Mgmt Frame Crash | Fuzzing 2025 | Firmwares vulneráveis |

### Captura Avançada (20 ataques)

| Ataque | Técnica | Sucesso |
|--------|---------|---------|
| PMKID Clientless | 2018-2025 | Alto em roaming |
| Multi-Target PMKID | Batch | Em massa |
| Handshake Passivo | Sniffing | 100% em ativas |
| Deauth + Capture | Clássico | Muito alto |
| Evil Twin Downgrade | WPA3→WPA2 | Em transition mode |

---

## 📶 BLE Spam (7 ataques - OTIMIZADO)

> **OTIMIZADO**: 3.5x mais rápido que versão padrão!

| Ataque | Descrição | Status |
|--------|-----------|--------|
| Applejuice | iOS popup spam | ✅ Otimizado |
| SourApple | iOS crash | ✅ Otimizado |
| Spam Windows | Swift Pair popup | ✅ Otimizado |
| Spam Samsung | SmartThings popup | ✅ Otimizado |
| Spam Android | Fast Pair popup | ✅ Otimizado |
| Spam Todos | Todos os modos | ✅ Otimizado |
| Spam Personalizado | Payload custom | ✅ Otimizado |

---

## 🔌 USB (10 payloads)

| Payload | Função | Status |
|---------|--------|--------|
| BadUSB Script | Ducky scripts | ✅ Funcional |
| Reverse Shell | Shell reverso PS | ✅ Funcional |
| WiFi Stealer | Exporta senhas WiFi | ✅ Funcional |
| Browser Creds | Extrai senhas | ✅ Funcional |
| Disable Defender | Desativa AV | ✅ Funcional |

---

## 📺 IR

### TV Nuke

40+ marcas suportadas: Samsung, LG, Sony, Philips, Panasonic, etc.

---

## 🚀 Network Simulation (Performance Absoluta)

> **Módulo de simulação de rede crítica de ultra-alta performance**
> ESP32 rodando em modo berserk com otimizações zero-compromisso

### Funcionalidades Principais

| Ataque | Técnica | Performance | Status |
|--------|---------|-------------|--------|
| **BLE Burst Capture** | Core 1 pinned task | 1000 pkt/s | ✅ Funcional |
| **WiFi Raw Injection** | Core 0 DMA task | 1200 frames/s | ✅ Funcional |

### Especificações Técnicas

#### Arquitetura Dual-Core

- **Core 1 (Priority 15)**: BLE packet capture com lock-free queue
- **Core 0 (Priority 10)**: WiFi injection + UART DMA real-time
- **Comunicação**: Queue 64 slots sem mutex/blocking

#### Otimizações de Performance

- **Clock Scaling**: 160-240MHz ADC-monitored (GPIO34)
- **DMA Total**: UART + BLE zero-CPU overhead
- **IRAM Hot Path**: 12ns access time para MAC tables
- **Memory Zero Heap**: Static allocation + SD chunks

#### Burst Engine

- **WiFi Frames**: 1200/s com `ets_delay_us(800)` timing preciso
- **BLE Packets**: 1000/s simulated capture com timestamps
- **Metrics Logging**: `burst: %.3fms/pkt` to SD em tempo real
- **Threshold Check**: >0.9ms/pkt → task restart automático

#### Berserk Mode

- **Comando**: `berserk on` ou `berserk total`
- **Performance**: 240MHz ambos cores simultaneamente
- **Sleep**: Desabilitado completamente
- **Warning**: 15 minutos autonomia de bateria

### Interface de Controle

#### Menu Touchscreen

```
Outros → Network Sim
- Ativa simulação com status em tempo real
- Pressione qualquer tecla para parar
```

#### Comandos Serial

```bash
netsim start     # Inicia simulação
netsim stop      # Para simulação
netsim status    # Status atual
berserk on|off   # Modo berserk
```

### Arquivos de Log

- **burst_metrics.log**: Performance WiFi em tempo real
- **adc_monitor.log**: Tensão bateria (200ms interval)
- **system_status.log**: Estado geral do sistema

### Hardware Requirements

- **ESP32 Dual-Core**: Para task pinning
- **GPIO34 ADC**: Battery voltage monitoring
- **UART1**: DMA buffer para real-time data
- **BLE Controller**: Memory release optimization
- **SD Card**: Metrics logging e large payloads

---

## ⚠️ Avisos

> [!WARNING]
> **Uso educacional apenas!**
> Todos os ataques devem ser usados somente em dispositivos autorizados.
