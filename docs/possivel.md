# 🚀 Recursos Potenciais do WavePwn para Lele Origin

Baseado na análise minuciosa do código fonte em `C:\Projetos\CYD 28\add\src`, aqui estão as funcionalidades avançadas que podem ser portadas para o Lele Origin.

---

## 📡 WiFi & Ataques Avançados
*Encontrado em: `wifi/wifi_attacks.h`, `wifi/wps_attacks.h`*

### 1. WPS Attacks (Prioridade Alta)
O WavePwn possui um módulo WPS muito completo que falta no Lele atual:
- **Pixie Dust:** Ataque offline que explora a entropia do E-Hash (muito eficiente).
- **WPS Bruteforce:** Tenta PINs sequenciais ou aleatórios.
- **PIN Flood:** DoS específico para travar o serviço WPS do roteador.

### 2. Novos Ataques WiFi
- **One Tap Nuke:** Provavelmente uma combinação massiva de Deauth + Beacon Flood + Probe Flood iniciada com um toque.
- **Downgrade Attack (TKIP):** Força dispositivos a usarem criptografia mais fraca (WPA1) enviando pacotes forjados.
- **Hidden SSID Reveal:** Tenta revelar redes ocultas ativamente.
- **HCXDump:** Captura raw específica para formatar hashes PMKID/Handshake para o hashcat (moderno).
- **Smart Deauth:** Foca apenas em clientes conectados em vez de floodar o broadcast (mais furtivo).
- **Turbo Deauth:** Usa injeção raw a ~300 pacotes/segundo (agressivo).

### 3. Otimizações de Captura
- **Queue Handshake:** Salva handshakes em uma fila FreeRTOS para não bloquear o loop principal (evita perder pacotes).

---

## 🧠 AI & Automação On-Device
*Encontrado em: `ai/ai_manager.h`, `detectors/tiny_classifiers.h`*

### 1. TinyClassifiers (Excelente para ESP32)
Modelos de ML leves para rodar no microcontrolador:
- **Handshake Quality Check:** Analisa se o handshake capturado é válido/quebrável na hora (evita salvar lixo).
- **PMKID Weakness:** Detecta se um PMKID é vulnerável a ataques conhecidos baseados no OUI/padrão.

### 2. Neura9 Defense
- Sistema de inferência que parece analisar tráfego para detectar se **você** está sendo atacado.

---

## 🦷 Bluetooth & Grid
*Encontrado em: `ble_grid/pwn_grid.h`*

### 1. PwnGrid (Cooperação)
- **Threat Sharing:** Dispositivos Lele/WavePwn próximos podem compartilhar "níveis de ameaça" via BLE advertisement customizado. Se um detecta perigo, avisa os outros.

### 2. Spam & Flood Aprimorados
- O diretório `iot_pwn` sugere implementações mais agressivas de BLE Flood e Beacon Spam.

---

## 🎙️ Voz e Interação
*Encontrado em: `voice/voice_assistant.h`*

### 1. Comandos de Voz
> *Nota: Requer microfone I2S (ex: INMP441) soldado no CYD.*
- Comandos como "Hey Lele, Scan WiFi", "Start Attack", "TV Nuke".
- Detecção de wake-word local.

### 2. TV Nuke
- Comando IR que envia códigos de desligar para centenas de modelos de TV sequencialmente (super TV-B-Gone).

---

## 📺 Interface & Sistema
- **Boot Animation:** Animação estilo "circuito" desenhando na tela.
- **GPU Acceleration:** Uso de DMA para desenho assíncrono (deixa a UI mais fluida durante ataques).

---

## 📋 Recomendação de Implementação

1.  **Fase 1 (Imediato):** Portar **WPS Attacks** (Pixie Dust é matador) e **AI Classifiers** (melhora muito a qualidade do wardriving).
2.  **Fase 2 (Intermediário):** Implementar **One Tap Nuke** e **Downgrade Attack**.
3.  **Fase 3 (Avançado):** Implementar **PwnGrid** para diversão multiplayer.
4.  **Hardware Extra:** Documentar mod de microfone para habilitar recursos de Voz.
