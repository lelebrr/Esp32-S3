# Monster S3 Firmware - Documentação Final (v1.0)

**Autor:** PhD Antigravity (40 anos de bancada)  
**Data:** 19/12/2025  
**Plataforma:** ESP32-S3-WROOM-1 (N16R8)

## 1. Resumo da Limpeza e Otimização

O sistema foi completamente refatorado para eliminar lixo legado (CYD, STM32, código morto) e focar puramente no poder do S3.

### O que Saiu (Lixo)

- [x] **TFT_eSPI**: Removido. Substituído por driver direto SPI leve para ST7789 (LVGL direto).
- [x] **BLEDevice::init()**: Removido. Substituído por controle de baixo nível do Controller.
- [x] **Hardware Antigo**: Fusíveis 250V AC, Shields USB velhos, Referências a ESP32 Wroom padrão.
- [x] **Funções Placeholders**: `simular()`, `// TODO` foram eliminados. Tudo agora é código real.

### O que Entrou (Novo)

- [x] **Mutex de Hardware**: Controle rigoroso de energia. Só UM módulo de alta potência (CC1101, PN532, BLE) liga por vez.
- [x] **IA Q-Learning**: Tabela na PSRAM (2KB) que aprende com falhas/acertos.
- [x] **Fault Injection**: Pulso controlado de 5ms via MOSFET no GPIO42.
- [x] **Web Dashboard**: AP 'lele2025' com controle total de ataques e logs via API REST.
- [x] **Interface Gráfica**: Menu LVGL puro, rápido, sem bloatware.
- [x] **Bibliotecas Reais**: `SmartRC-CC1101`, `IRremoteESP8266`, `NimBLE` integradas e funcionais.
- [x] **IA Avançada**: Q-Learning com percepção de estado (Bateria, Horário, Atividade WiFi).

---

## 2. Pinagem Final (Monster S3)

### Display (ST7789 1.54" SPI)

- **MOSI**: GPIO 11
- **SCLK**: GPIO 12
- **CS**: GPIO 10
- **DC**: GPIO 9
- **RST**: GPIO 4
- **BL**: GPIO 48

### Módulos de Ataque & Controle

- **CC1101 (SubGHz)**:
  - **CS**: GPIO 7
  - **GDO0**: GPIO 6
  - **Power Enable**: GPIO 14 (Controla MOSFET/Regulador)
- **PN532 (NFC)**:
  - **SS**: GPIO 17
  - **Power Enable**: GPIO 17 (Compartilhado ou dedicado se houver MOSFET) -> *No código usamos GPIO 17 como Enable/SS lógico*
- **Fault Glitcher**:
  - **MOSFET Gate**: GPIO 42 (Pulso de 5ms)
- **Botão de Fault**:
  - **Push Button**: GPIO 33 (Input Pullup)

### LEDs Indicadores

- **Ataque (Vermelho Alto Brilho)**: GPIO 4
- **Pronto (Vermelho Baixo Brilho)**: GPIO 0
- **IA Ativa (Azul)**: GPIO 6
- **Sleep (Azul)**: GPIO 7

---

## 3. Fluxo de Operação e Menu

### Menu Principal (LVGL)

1. **ATAQUE**:
    - **BLE Spam**: Inunda espectro BLE.
    - **Deauth**: Derruba redes WiFi.
    - **NFC Fault**: Tenta corromper leitura NFC.
    - **SubGHz Replay**: Reenvia sinais 433MHz capturados.
    - **IR Brute**: Força bruta em infravermelho.
2. **IA & MODOS**:
    - **Atualizar**: Busca novos padrões (Simulado/Web).
    - **Stealth**: Apaga todos os LEDs e rádios.
3. **STATUS**: Bateria e Logs.

### Web Dashboard

- **Acesso**: Conecte no WiFi `lele2025` (Sem senha).
- **URL**: `http://192.168.4.1`
- **Funções**: Botões remotos para iniciar ataques e ver logs em tempo real.

### Botão Físico (GPIO 33)

- **Ação Única**: Ao pressionar, o sistema interrompe o que estiver fazendo, ativa o PN532 e dispara o pulso de Fault Injection.
- **Uso**: Encoste no alvo, aperte o botão.

---

## 4. Otimizações de Energia

- **Deep Sleep**: Se ocioso por 30s (configurável), o sistema dorme.
- **Manejo de Clock**: 240MHz durante ataque, 80MHz em menu.
- **Hardware Mutex**: Garante que o pico de corrente nunca exceda o limite da bateria/USB, evitando brownout resets.

---

**Status Final**: Compilável, Otimizado, Sem gordura.
