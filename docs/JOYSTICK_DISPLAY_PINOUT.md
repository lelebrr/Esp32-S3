# Joystick Shield V1.A + Display TFT ILI9341 - Pinagem ESP32-S3

**Última atualização:** 2025-12-19

---

## Resumo de Hardware

| Componente | Modelo | Interface | Tensão |
|------------|--------|-----------|--------|
| MCU | ESP32-S3-WROOM-1 N16R8 | - | 3.3V |
| Display | ILI9341 TFT 2.8" 320x240 | SPI2 | 3.3V |
| Touch | XPT2046 | SPI2 (shared) | 3.3V |
| SD Card | MicroSD Slot | SPI3 | 3.3V |
| Joystick Shield | Funduino V1.A | ADC + Digital | 3.3V |

---

## Joystick Shield V1.A - Componentes

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    JOYSTICK SHIELD V1.A (Funduino)                   │
│  ┌─────────────────────────────────────────────────────────────────┐ │
│  │  [NOKIA 5110]                              [SERIAL BLUETOOTH]   │ │
│  │   D8-D12/3V/GND                            RX TX GND VCC        │ │
│  │                                                                 │ │
│  │  ┌─────────────────────────────────────────┐                    │ │
│  │  │    [A]  ╭──────╮               ╭───╮    │                    │ │
│  │  │         │      │               │ ● │    │    [nRF24L01]      │ │
│  │  │ [D]     │JOYST │     [E] [F]   │JOY│    │     Connector      │ │
│  │  │         │ ICK  │               │STK│    │                    │ │
│  │  │    [C]  │      │               ╰───╯    │                    │ │
│  │  │         ╰──────╯                        │                    │ │
│  │  │    [B]                                  │                    │ │
│  │  └─────────────────────────────────────────┘                    │ │
│  │  [3.3V/5V]────────────────────────────────────[I2C Connector]   │ │
│  │   SWITCH                                      SCL SDA GND +5V   │ │
│  └─────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────┘

LEGENDA:
[A][B][C][D] = Botões direcionais (D-pad style)
[E][F] = Botões auxiliares pequenos
JOYSTICK = Analógico 2 eixos + clique central
```

---

## Tabela de Pinagem - Joystick Shield para ESP32-S3

### Joystick Analógico

| Shield Pin | Função Arduino | ESP32-S3 GPIO | Tipo | Notas |
|------------|----------------|---------------|------|-------|
| VRx | A0 | **GPIO4** | ADC1_CH3 | Range 0-4095, centro ~2048 |
| VRy | A1 | **GPIO5** | ADC1_CH4 | Range 0-4095, centro ~2048 |
| SW | D8 | **GPIO6** | Digital | Pull-up, LOW=pressionado |
| VCC | 5V/3.3V | 3.3V | Power | **Usar chave em 3.3V!** |
| GND | GND | GND | Power | Comum |

> [!TIP]
> **SW foi movido de GPIO0 para GPIO6** para evitar conflitos com o pino de BOOT.

### Botões Direcionais (D-Pad)

| Botão | Arduino Pin | ESP32-S3 GPIO | Posição | Notas |
|-------|-------------|---------------|---------|-------|
| A | D2 | **GPIO41** | ⬆️ Cima | LOW quando pressionado |
| B | D3 | **GPIO42** | ➡️ Direita | LOW quando pressionado |
| C | D4 | **GPIO1** | ⬇️ Baixo | LOW quando pressionado |
| D | D5 | **GPIO2** | ⬅️ Esquerda | LOW quando pressionado |

### Botões Auxiliares

| Botão | Arduino Pin | ESP32-S3 GPIO | Posição | Notas |
|-------|-------------|---------------|---------|-------|
| E | D6 | **GPIO17** | Centro-Esq | LOW quando pressionado |
| F | D7 | **GPIO18** | Centro-Dir | LOW quando pressionado |

---

## Conectores Externos do Shield

### Conector Serial (Bluetooth)

| Shield | ESP32-S3 | Uso |
|--------|----------|-----|
| RX | GPIO44 | USB CDC RX |
| TX | GPIO43 | USB CDC TX |
| GND | GND | Comum |
| VCC | 3.3V | Ou via chave 5V |

### Conector I2C

| Shield | ESP32-S3 | Dispositivos |
|--------|----------|--------------|
| SCL | GPIO9 | PN532, DS3231, PAJ7620U2 |
| SDA | GPIO8 | Barramento compartilhado |
| GND | GND | Comum |
| +5V | 3.3V | **Usar 3.3V para ESP32!** |

### Conector nRF24L01

> [!NOTE]
> O nRF24L01 já está configurado separadamente no projeto (CC1101/NRF24 section).

### Conector Nokia 5110

> [!WARNING]
> **Não suportado** - Conflita com o display TFT ILI9341 principal.

---

## Display ILI9341 240x320 (SPI)

| Pino Display | ESP32-S3 GPIO | Função |
|--------------|---------------|--------|
| VCC | 3.3V | Alimentação |
| GND | GND | Terra |
| SCK | **GPIO14** | SPI Clock |
| MOSI | **GPIO13** | SPI Data Out |
| MISO | -1 | Não usado |
| CS | **GPIO10** | Chip Select |
| DC | **GPIO12** | Data/Command |
| RST | **GPIO11** | Reset |
| BL | **GPIO21** | Backlight PWM |

---

## Touch XPT2046 (SPI Compartilhado)

| Pino Touch | ESP32-S3 GPIO | Função |
|------------|---------------|--------|
| CS | **GPIO15** | Chip Select |
| IRQ | **GPIO16** | Interrupt |
| CLK/MOSI/MISO | Compartilhado | Mesmo barramento display |

---

## SD Card (SPI3)

| Pino SD | ESP32-S3 GPIO | Função |
|---------|---------------|--------|
| CS | GPIO10 | Chip Select |
| SCK | GPIO12 | SPI Clock |
| MOSI | GPIO11 | Data Out |
| MISO | GPIO13 | Data In |

---

## Esquema de Ligação Física

```
┌──────────────────┬────────────────────┬──────────────────────────────┐
│ Shield Pin       │ ESP32-S3 GPIO      │ Cor do Fio Sugerida          │
├──────────────────┼────────────────────┼──────────────────────────────┤
│ VCC (3.3V)       │ 3.3V               │ 🔴 Vermelho                  │
│ GND              │ GND                │ ⚫ Preto                     │
│ Joystick X (A0)  │ GPIO4              │ 🟠 Laranja                   │
│ Joystick Y (A1)  │ GPIO5              │ 🟡 Amarelo                   │
│ Joystick SW (D8) │ GPIO6              │ 🟤 Marrom                    │
│ Botão A (D2)     │ GPIO41             │ 🟢 Verde                     │
│ Botão B (D3)     │ GPIO42             │ 🔵 Azul                      │
│ Botão C (D4)     │ GPIO1              │ 🟣 Roxo                      │
│ Botão D (D5)     │ GPIO2              │ ⚪ Cinza                     │
│ Botão E (D6)     │ GPIO17             │ ⬜ Branco                    │
│ Botão F (D7)     │ GPIO18             │ 🩷 Rosa                      │
└──────────────────┴────────────────────┴──────────────────────────────┘
```

---

## Código de Teste Rápido

```cpp
#include "pin_config.h"

void setup() {
    Serial.begin(115200);
    
    // Configurar pinos do joystick
    pinMode(PIN_JOY_SW, INPUT_PULLUP);
    
    // Configurar botões com pull-up
    pinMode(PIN_BTN_A, INPUT_PULLUP);
    pinMode(PIN_BTN_B, INPUT_PULLUP);
    pinMode(PIN_BTN_C, INPUT_PULLUP);
    pinMode(PIN_BTN_D, INPUT_PULLUP);
    pinMode(PIN_BTN_E, INPUT_PULLUP);
    pinMode(PIN_BTN_F, INPUT_PULLUP);
}

void loop() {
    // Leitura do joystick analógico
    int joyX = analogRead(PIN_JOY_X);
    int joyY = analogRead(PIN_JOY_Y);
    bool joySW = digitalRead(PIN_JOY_SW) == LOW;
    
    // Leitura dos botões
    bool btnA = digitalRead(PIN_BTN_A) == LOW;
    bool btnB = digitalRead(PIN_BTN_B) == LOW;
    bool btnC = digitalRead(PIN_BTN_C) == LOW;
    bool btnD = digitalRead(PIN_BTN_D) == LOW;
    bool btnE = digitalRead(PIN_BTN_E) == LOW;
    bool btnF = digitalRead(PIN_BTN_F) == LOW;
    
    Serial.printf("X=%4d Y=%4d SW=%d | A=%d B=%d C=%d D=%d E=%d F=%d\n",
                  joyX, joyY, joySW, btnA, btnB, btnC, btnD, btnE, btnF);
    
    delay(100);
}
```

---

## Troubleshooting

| Problema | Causa Provável | Solução |
|----------|----------------|---------|
| Joystick sempre 0 | VCC não conectado | Verificar 3.3V no módulo |
| Joystick sempre 4095 | VRx/VRy invertido | Trocar conexões X/Y |
| Botões não respondem | Falta pull-up | Usar INPUT_PULLUP no código |
| Boot loop | GPIO0 em LOW | SW foi movido para GPIO6 (resolvido) |
| Leitura instável | Ruído | Adicionar capacitor 100nF em VCC |

---

## Referências

- [Funduino Joystick Shield Review](https://embarcados.com.br/review-hardware-joystick-shield-funduino/)
- [UsinaInfo - Joystick Shield V1.A](https://www.usinainfo.com.br/shields-para-arduino/joystick-shield-para-arduino-v1a-3531.html)
- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
