# 📡 Dual NRF24 & Advanced Jamming

Este sistema permite o uso de **dois módulos NRF24L01** simultaneamente na placa CYD (ESP32-2432S028R) para aumentar a eficácia de ataques de jamming e permitir operações paralelas (TX + RX).

---

## 🛠️ Configuração de Hardware

### Radio 1 (Primário)
Usa o barramento padrão **VSPI** (compartilhado com Touch e SD Card em alguns modelos).
- **MISO**: 19
- **MOSI**: 23
- **SCK**: 18
- **CS**: 5
- **CE**: 17

### Radio 2 (Secundário)
Usa o barramento **HSPI** (geralmente livre ou compartilhado com conector P3).
- **MISO**: 12
- **MOSI**: 13
- **SCK**: 14
- **CS**: 22 (Configurável via P3 Header)
- **CE**: 27 (Configurável via P3 Header)

> **Nota**: Se você estiver usando um adaptador ou placa customizada, verifique os pinos em `Configurar Pinos -> RF Config`.

---

## 🚀 Novos Ataques (Dual Mode)

### 1. Dual Carrier Jammer
Transmite ruído contínuo (Carrier Wave) em **dois canais simultâneos**.
- **Vantagem**: Dobra a probabilidade de interferir em dispositivos que usam Frequency Hopping (FHSS) e cobre mais espectro.
- **Uso**: Selecione Canal 1 e Canal 2 independentemente.

### 2. Hyper BLE Spam
Foca exclusivamente nos canais de Advertising BLE (37, 38, 39).
- **Funcionamento**: Um rádio ataca 2402MHz (Ch 37) e o outro 2480MHz (Ch 39), alternando para 2426MHz (Ch 38).
- **Efeito**: Causa negação de serviço (DoS) quase total para descoberta de dispositivos BLE na área.

### 3. Protocol Hunter (Em Breve)
Um rádio foca em WiFi (2.4GHz) enquanto o outro foca em Bluetooth.

### 4. MouseJack Dual (Em Breve)
Um rádio faz sniffing (escuta) para detectar mouses vulneráveis, enquanto o outro injeta os comandos (keystrokes) instantaneamente.

---

## ⚙️ Configuração de Código
O suporte Dual NRF é ativado automaticamente se os pinos do Radio 2 estiverem definidos em `LelePins.conf` ou no código fonte (`configPins.h`).

```cpp
// Exemplo de inicialização interna
RF24 NRFradio2(14, 12, 13, 22, 27); // SCK, MISO, MOSI, CS, CE
```
