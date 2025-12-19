# 💡 Guia de Iluminação (WS2812B)

O **Lele Origin 1.0** suporta nativamente uma fita ou painel de LEDs endereçáveis **WS2812B** (RGB) ou SK6812 (RGBW). Este recurso adiciona estilo visual e feedback útil através de notificações.

> **Atualização 2024-12-13:** O driver foi reescrito para usar o **RMT (Remote Control) nativo do ESP32**, eliminando a dependência do FastLED e melhorando a estabilidade.

---

## 🛠️ Instalação do Hardware

### Componentes Necessários
- 4x LEDs WS2812B (pode usar pedaço de fita LED ou módulos individuais)
- 1x Resistor 220Ω - 470Ω (para linha de dados)
- 1x Capacitor 100µF - 1000µF (recomendado para estabilidade)

### Esquema de Ligação
O firmware é pré-configurado para comunicar através do **GPIO 16**.

| Pino LED | Conexão ESP32 | Notas |
|----------|---------------|-------|
| 5V / VCC | 5V (VUSB) | Use alimentação externa se tiver > 8 LEDs |
| GND | GND | Terra comum é essencial |
| DIN | GPIO 16 | Use resistor em série |
| DOUT | N/C | Para conectar mais LEDs em cascata |

> **NOTA:** Se usar o suporte de baterias 18650, conecte o VCC dos LEDs na saída de 5V do shield ou na saída 3V3 se forem poucos LEDs (mas brilho será menor).

---

## 🎮 Menu Iluminação

Acesse em: `Menu Principal` → `Iluminação`

### 1. Controle Básico
- **Ligar/Desligar:** Interruptor mestre para todos os efeitos.
- **Cor:** Selecione uma das 9 cores sólidas predefinidas.
- **Selecionar LED:** Escolha controlar TODOS os LEDs juntos ou UM LED específico (LED 1 a 4).
- **Brilho:** Ajuste a intensidade (10%, 25%, 50%, 75%, 100%).

### 2. Notificações Visuais
O sistema usa os LEDs para informar status sem precisar olhar para a tela:
- 🔴 **Erro (Pisca Vermelho):** Falha em operação, WiFi desconectado, erro SD.
- 🟢 **Sucesso (Pisca Verde):** Conexão estabelecida, arquivo salvo, ataque concluído.
- 🟡 **Aviso (Pisca Amarelo):** Bateria fraca, temperatura alta.
- 🔵 **Captura (Pulsa Azul):** Handshake capturado, PMKID encontrado.
- ⚪ **WiFi (Ciano):** Indica status da conexão.

---

## ✨ Efeitos Disponíveis

O sistema possui **16 efeitos** divididos em categorias:

### Básicos
| Efeito | Descrição |
|--------|-----------|
| **Estático** | Cor sólida sem movimento. |
| **Piscar** | Liga e desliga na cor selecionada. |
| **Fade** | Transição suave de brilho (pulsação). |
| **Arco-Íris** | Ciclo contínuo de todas as cores do espectro. |
| **Respirar** | Variação suave de intensidade, similar ao Fade mas mais orgânico. |
| **Scanner** | Um ponto de luz vai e volta (efeito Knight Rider/KITT). |
| **Aleatório** | Cores randômicas em LEDs randômicos. |
| **Perseguir** | Uma cor "persegue" a outra em loop. |

### Avançados (Novos!)
| Efeito | Descrição |
|--------|-----------|
| **Pulso Indep.** | Cada LED pulsa em sua própria cor (definida individualmente) e velocidade. |
| **Onda de Cores** | Uma onda suave de matizes percorre a fita. |
| **Ping Pong** | Um ponto de luz brilhante rebate nas extremidades com rastro suave. |
| **Fogo** | Simulação de chamas usando tons de vermelho, laranja e amarelo. |
| **Gelo** | Cintilação suave em tons de azul e branco frio. |
| **Polícia** | Alternância rápida estilo estroboscópico entre Vermelho e Azul. |
| **Arco-Íris Gira** | Cores do arco-íris girando como uma roda. |
| **Batimento** | Pulso duplo rítmico simulando um coração (vermelho). |

---

## 🎨 Controle Individual de Cores

Uma das funcionalidades mais poderosas é o controle individual. Você pode configurar, por exemplo:
- LED 1: Vermelho
- LED 2: Verde
- LED 3: Azul
- LED 4: Branco

**Como configurar:**
1. Vá em `Selecionar LED` e escolha `LED 1`.
2. Vá em `Cor` e escolha `Vermelho`.
3. Volte e selecione `LED 2`.
4. Escolha `Verde`.
5. E assim por diante.
6. Ative o efeito **Estático** ou **Pulso Indep.** para ver o resultado.

---

## ⚡ Velocidade dos Efeitos

Você pode ajustar a velocidade das animações em `Efeitos` → `Velocidade`:
- **Lento:** Para iluminação ambiente relaxante.
- **Normal:** Padrão balanceado.
- **Rápido:** Para maior dinamismo.
- **Turbo:** Para efeitos estroboscópicos ou alertas urgentes.

---

## 🔧 Solução de Problemas

**LEDs não acendem:**
- Verifique se a opção `Ligar/Desligar` está ativa.
- Confirme se o pino DIN está no GPIO 16.
- Verifique a alimentação (VCC e GND).
- No monitor serial, procure por `[WS2812B] RMT inicializado` para confirmar inicialização.

**Cores erradas (ex: Vermelho aparece Verde):**
- O chip WS2812B usa ordem GRB. O código já trata isso automaticamente.
- Alguns clones usam ordem RGB. Se acontecer, edite `ws2812b_control.cpp` e inverta `ledColors[i].g` e `ledColors[i].r`.

**LEDs piscando aleatoriamente (glitch):**
- Adicione um capacitor de 1000µF entre VCC e GND próximo aos LEDs.
- Certifique-se de que o GND do ESP32 e dos LEDs (se usar fonte externa) estão conectados.
- O driver RMT nativo é mais estável que bit-banging, mas ruído elétrico ainda pode afetar.

---

## 🔬 Detalhes Técnicos

O driver WS2812B usa o periférico **RMT (Remote Control Transceiver)** do ESP32 para gerar os pulsos precisos necessários:

| Parâmetro | Valor |
|-----------|-------|
| T0H (bit 0 high) | 350ns |
| T0L (bit 0 low) | 900ns |
| T1H (bit 1 high) | 900ns |
| T1L (bit 1 low) | 350ns |
| Reset | 280µs |
| Clock RMT | 40MHz (25ns/tick) |

Esta implementação **não requer FastLED**, reduzindo o tamanho do firmware e evitando conflitos de interrupção.
