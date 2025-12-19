# ⚡ WPS Attacks - Guia Completo

> **Ataques específicos ao protocolo WPS.**
> Última atualização: 2025-12-19

---

## 📋 Índice

1. [O que é WPS](#o-que-é-wps)
2. [Ataques Disponíveis](#ataques-disponíveis)
3. [Menu de Uso](#como-usar)
4. [Pixie Dust Attack](#pixie-dust-attack)
5. [PIN Brute-force](#pin-brute-force)
6. [WPS DoS Flood](#wps-dos-flood)
7. [Limitações](#limitações)
8. [Referências Técnicas](#referências)

---

## 🔐 O que é WPS

**WPS (Wi-Fi Protected Setup)** é um padrão que facilita conexão WiFi sem digitar senhas longas.

### Métodos WPS

| Método | Como funciona | Segurança |
|--------|---------------|-----------|
| **PIN** | Digita 8 dígitos no roteador | ⚠️ Vulnerável |
| **PBC** | Aperta botão no roteador | ✅ Mais seguro |
| **NFC** | Aproxima dispositivo | ✅ Seguro |

### Por que é vulnerável

O PIN de 8 dígitos (10^8 combinações) na verdade são **duas metades verificadas separadamente**:

- Primeira metade: 4 dígitos (10^4 = 10.000 tentativas)
- Segunda metade: 3 dígitos + checksum (10^3 = 1.000 tentativas)

**Total máximo: 11.000 tentativas** (não 100 milhões!)

---

## ⚔️ Ataques Disponíveis

| Ataque | Tipo | Tempo | Sucesso |
|--------|------|-------|---------|
| **Pixie Dust** | Offline | Segundos | Se vulnerável |
| **PIN Brute-force** | Online | Horas | Qualquer WPS |
| **Null PIN** | Exploit | Imediato | Raro |
| **WPS DoS** | DoS | Contínuo | Trava WPS |

---

## 📱 Como Usar

### Localização no Menu

```
WiFi → WPS Attacks
```

### Fluxo completo

1. **Scan WPS APs**
   - Escaneia por 10 segundos
   - Lista apenas redes com WPS ativado
   - Mostra: SSID, RSSI, canal, se é WPA3

2. **Selecionar Alvo**
   - Lista aparece com todas redes WPS
   - Selecione a rede desejada

3. **Escolher Ataque**
   - Pixie Dust (tente primeiro!)
   - PIN Brute-force
   - Null PIN
   - WPS DoS Flood

4. **Aguardar Resultado**
   - Pixie Dust: segundos
   - Brute-force: pode levar horas

5. **Salvar Resultados**
   - Salva em `/lele/logs/wps_scan.txt`

---

## 🔮 Pixie Dust Attack

### O que é

Ataque **offline** que explora geração fraca de números aleatórios em alguns chipsets.

### Como funciona

```
1. Captura M1-M3 do handshake WPS
2. Extrai E-Hash1, E-Hash2, PKE, PKR
3. Bruteforce de seeds (0-32768)
4. Se seed baixo → PIN descoberto em segundos
```

### Chipsets vulneráveis

| Fabricante | Modelos | Vulnerável |
|------------|---------|------------|
| Ralink | RT2860, RT3070 | ✅ Sim |
| Broadcom | BCM4329, BCM4330 | ✅ Sim |
| Realtek | RTL8196C | ✅ Sim |
| Atheros | AR9285 | ⚠️ Alguns |
| Mediatek | MT7620 | ❌ Não |

### Display durante ataque

```
PIXIE DUST
━━━━━━━━━━━━━━━━
Alvo: MinhaRede
CH:6 RSSI:-45

Capturando M1-M3...
[████████░░] 80%

Seed: 15847/32768
━━━━━━━━━━━━━━━━
```

### Resultado

| Status | Significado |
|--------|-------------|
| `PIN: 12345670` | ✅ Sucesso! Use este PIN |
| `Nao vulneravel` | ❌ Chipset moderno/patcheado |
| `Timeout M3` | ⚠️ Não capturou - tente de novo |

---

## 🔢 PIN Brute-force

### O que é

Ataque **online** que testa PINs sequencialmente (estilo Reaver/Bully).

### Fluxo

```
1. Envia M1 com PIN tentativa
2. Recebe M2/M2D do AP
3. Se M2: primeira metade OK → continua
4. Se M2D: PIN errado → próximo
5. Se M8: SUCESSO! → extrai PSK
```

### Tempo estimado

| Cenário | Tempo |
|---------|-------|
| Sem lockout | 2-6 horas |
| Com lockout (60s) | 12-24 horas |
| Primeira metade rápida | Menos que metade do tempo |

### Display

```
PIN BRUTE-FORCE
━━━━━━━━━━━━━━━━
Alvo: MinhaRede
PIN: 12340000 (247)

Primeira metade: OK!
Testando segunda...

[ESC para parar]
```

### Problemas comuns

| Problema | Causa | Solução |
|----------|-------|---------|
| **WPS Locked** | Muitas tentativas | Espera 60s automático |
| **Timeout** | AP instável | Tente de novo |
| **Nunca acha** | 11000 tentativas | AP tem WPS bugado |

---

## 💥 WPS DoS Flood

### O que faz

Inunda o AP com requisições WPS inválidas, travando o serviço WPS.

### Uso

- Testar se AP trava com flood
- Forçar lockout para testar timing
- Denial of Service em WPS

### Display

```
WPS DoS FLOOD
━━━━━━━━━━━━━━━━
Alvo: MinhaRede
Pacotes: 15847

[ESC para parar]
```

---

## ⚠️ Limitações

### No ESP32

| Limitação | Impacto |
|-----------|---------|
| RAM limitada | Máx 30 APs no scan |
| CPU lenta | Brute-force ~1000 PIN/hora |
| Sem wpa_supplicant | Associação limitada |
| WiFi stack fechado | Alguns frames não funcionam |

### Comparação com PC

| | ESP32 | PC + Alfa |
|-|-------|-----------|
| Pixie Dust | ✅ Funciona | ✅ Funciona |
| Brute-force | ⚠️ Lento | ✅ Rápido |
| Full Reaver | ❌ Parcial | ✅ Completo |

### Recomendação

1. **Sempre tente Pixie Dust primeiro** (segundos se vulnerável)
2. Se não vulnerável, use PC com Reaver completo
3. DoS é útil para teste de resistência

---

## 📚 Referências

### Ferramentas originais

- [Reaver](https://github.com/t6x/reaver-wps-fork-t6x) - Brute-force WPS
- [Pixiewps](https://github.com/wiire-a/pixiewps) - Offline Pixie Dust
- [Bully](https://github.com/aanarchyy/bully) - Alternativa ao Reaver
- [OneShot](https://github.com/fulvius31/OneShot) - Python WPS

### Documentação técnica

- [Wi-Fi Alliance WPS Specification](https://www.wi-fi.org/discover-wi-fi/wi-fi-protected-setup)
- [Reaver/pixiewps Paper](https://arxiv.org/abs/1305.5549)

---

## 🔧 Código Fonte

Implementação em:

- `src/modules/wifi/wps_full_attacks.h`
- `src/modules/wifi/wps_full_attacks.cpp`

### Funções principais

```cpp
wps_attacks_menu()      // Menu principal
wps_start_scan()        // Inicia scan WPS
wps_pixie_dust_attack() // Ataque Pixie Dust
wps_pin_bruteforce()    // Brute-force online
wps_dos_flood()         // DoS flood
```

---

## ⚖️ Aviso Legal

- Use **apenas em redes próprias** ou com autorização
- Ataques WPS são **ilegais** sem permissão
- Para fins **educacionais e pesquisa de segurança**

---

*Documentação Lele Origin Team - v1.0*
