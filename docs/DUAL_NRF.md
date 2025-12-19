# 📡 Dual NRF24 & Advanced Jamming

Este sistema permite o uso de **dois módulos NRF24L01** simultaneamente para aumentar a eficácia de ataques de jamming e permitir operações paralelas (TX + RX).

> **Atualizado:** 2025-12-19

---

## 🛠️ Configuração ESP32-S3 (Atual)

### Radio 1 (Primário) - SPI3 Compartilhado

| Função | GPIO | Notas |
|--------|:----:|-------|
| MISO | 13 | Compartilhado SD |
| MOSI | 11 | Compartilhado SD |
| SCK | 12 | Compartilhado SD |
| CS | 46 | Compartilhado CC1101 |
| CE | 47 | Compartilhado CC1101 GDO0 |

> [!WARNING]
> NRF24 e CC1101 compartilham o mesmo barramento SPI. Não podem operar simultaneamente.

### Radio 2 (Secundário) - Opcional

Se precisar de dual NRF24, conecte o segundo rádio em pinos disponíveis:

| Função | GPIO Sugerido |
|--------|:-------------:|
| CS | 15 |
| CE | 16 |
| MISO/MOSI/SCK | Compartilhado |

---

## 🛠️ Configuração CYD-2USB (Legacy)

> [!NOTE]
> Esta seção é para placas CYD legacy, não ESP32-S3.

### Radio 1 (Primário) - VSPI

- MISO: 19, MOSI: 23, SCK: 18, CS: 5, CE: 17

### Radio 2 (Secundário) - HSPI

- MISO: 12, MOSI: 13, SCK: 14, CS: 22, CE: 27

---

## 🚀 Ataques Disponíveis

### 1. Dual Carrier Jammer

Transmite ruído contínuo em **dois canais simultâneos**.

- Dobra probabilidade contra FHSS

### 2. Hyper BLE Spam

Foca nos canais de Advertising BLE (37, 38, 39).

- DoS quase total para descoberta BLE

### 3. Protocol Hunter (Em Breve)

Um rádio WiFi, outro Bluetooth.

### 4. MouseJack Dual (Em Breve)

Sniffing + injection simultâneos.

---

## 🔗 Documentação Relacionada

- [CC1101_MODULE.md](CC1101_MODULE.md) - SubGHz (compartilha SPI)
- [MASTER_SYSTEM.md](MASTER_SYSTEM.md) - Visão geral
