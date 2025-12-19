# ⚙️ Configuração

> **Guia de personalização e ajustes do sistema.**
> Última atualização: 2025-12-19

---

## 🖥️ Menu de Configurações

Acesse **Config** no menu principal para ajustar as preferências.

---

## 💡 Brightness (Brilho)

Ajusta o brilho da tela.

| Nível | Percentual | Uso |
|-------|-----------|-----|
| Baixo | 25% | Uso noturno, economia de bateria |
| Médio | 50% | Uso normal |
| Alto | 75% | Ambientes claros |
| Máximo | 100% | Luz solar direta |

> [!TIP]
> Brilho mais baixo economiza energia e prolonga a vida útil do display.

---

## ⏰ Dim Time

Tempo para escurecer a tela automaticamente quando inativo.

| Opção | Tempo |
|-------|-------|
| 10s | 10 segundos |
| 30s | 30 segundos |
| 1min | 1 minuto |
| 5min | 5 minutos |
| Never | Nunca escurecer |

---

## 📐 Orientation (Orientação)

Define a orientação do display.

| Opção | Descrição |
|-------|-----------|
| 0 | Paisagem (USB em baixo) |
| 1 | Retrato (USB à direita) |
| 2 | Paisagem invertido (USB em cima) |
| 3 | Retrato invertido (USB à esquerda) |

---

## 🎨 UI Color (Cor da Interface)

Personaliza a cor principal da interface.

**Cores disponíveis:**

- Branco
- Vermelho
- Verde
- Azul
- Amarelo
- Ciano
- Magenta
- Laranja

> [!NOTE]
> A cor afeta os menus, bordas e elementos de destaque.

---

## 🔊 Boot Sound (Som de Inicialização)

Ativa ou desativa o som ao ligar o dispositivo.

| Opção | Descrição |
|-------|-----------|
| On | Reproduz som ao iniciar |
| Off | Inicia silenciosamente |

---

## 🕐 Clock (Relógio)

Configuração do relógio interno.

### NTP Adjust

Sincroniza com servidor de hora via internet.

**Requisitos:**

- Conexão WiFi ativa

### Manual Adjust

Ajusta data e hora manualmente.

### RTC Support

Se você tem um módulo RTC (DS3231, etc.) conectado, o horário é mantido mesmo sem energia.

---

## 😴 Sleep (Modo de Espera)

Coloca o dispositivo em modo de baixo consumo.

**Para acordar:**

- Toque na tela
- Pressione botão (se disponível)

---

## 🔄 Restart

Reinicia o dispositivo.

---

## 📡 Configurações de Módulos

### RF Config

- **RF TX Pin**: Pino de transmissão RF
- **RF RX Pin**: Pino de recepção RF
- **RF Module**: Tipo de módulo (CC1101/RF433)
- **RF Frequency**: Frequência de operação

### IR Config

- **IR TX Pin**: Pino do LED transmissor
- **IR RX Pin**: Pino do receptor

### RFID Config

- **RFID Module**: Tipo de módulo (PN532, etc.)

---

## 💾 Armazenamento

### SD Card Manager

Gerencia arquivos no cartão SD.

**Operações:**

- Navegar pastas
- Visualizar arquivos
- Deletar arquivos
- Ver informações

### LittleFS Manager

Gerencia a memória flash interna.

**Pastas importantes:**

- `/wifi/` - Credenciais WiFi salvas
- `/ir/` - Sinais IR salvos
- `/rf/` - Sinais RF salvos
- `/rfid/` - Dumps de cartões

---

## 🌐 WebUI

Interface web para controle remoto.

**Como acessar:**

1. Conecte o Lele a uma rede WiFi
2. Anote o IP exibido
3. Acesse `http://[IP]` no navegador

**Ou via Access Point:**

1. Ative o WiFi AP
2. Conecte ao Lele
3. Acesse `http://192.168.4.1`

---

## ⚙️ Configurações Avançadas

### Serial Commands

O Lele aceita comandos via porta serial (115200 baud).

**Comandos úteis:**

- `version` - Exibe versão
- `info` - Informações do sistema
- `wifi scan` - Varredura WiFi
- `reboot` - Reinicia

### GPIO Personalizados

Para usar todos os GPIOs disponíveis para IR/RF, defina no código:

```cpp
-DALLOW_ALL_GPIO_FOR_IR_RF=1
```

---

## 📱 Primeiro Uso

**Checklist de configuração inicial:**

- [ ] Ajustar brilho confortável
- [ ] Definir orientação preferida
- [ ] Configurar cor da interface
- [ ] Conectar ao WiFi (para sincronizar hora)
- [ ] Formatar SD Card (se necessário)
