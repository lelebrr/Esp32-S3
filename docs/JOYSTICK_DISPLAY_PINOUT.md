# Joystick Analógico + Display TFT 3.5" ILI9488 - Pinagem e Montagem

**Última atualização:** 2025-12-19

## Resumo de Hardware

| Componente | Modelo | Interface | Tensão |
|------------|--------|-----------|--------|
| MCU | ESP32-S3-WROOM-1 N16R8 | - | 3.3V |
| Display | ILI9488 TFT 3.5" | SPI2 | 3.3V |
| Touch | XPT2046 | SPI2 (shared) | 3.3V |
| SD Card | MicroSD Slot | SPI3 | 3.3V |
| Joystick | Módulo Analógico 2-eixos | ADC | 3.3V |

---

## Diagrama de Conexão ASCII

```
                    ┌─────────────────────────────────────┐
                    │        ESP32-S3-WROOM-1 N16R8       │
                    │                                      │
     ┌──────────────┼────────────────┬────────────────────┤
     │              │                │                    │
  ┌──┴──┐       ┌───┴───┐       ┌────┴────┐          ┌────┴────┐
  │JOYST│       │DISPLAY│       │  TOUCH  │          │ SD CARD │
  │ICKM│       │ILI9488│       │ XPT2046 │          │  SLOT   │
  └──┬──┘       └───┬───┘       └────┬────┘          └────┬────┘
     │              │                │                    │
     │  GPIO4 ◄─VRx │                │                    │
     │  GPIO5 ◄─VRy │  GPIO11─►MOSI  │                    │
     │  GPIO0 ◄─SW  │  GPIO12─►SCK   │  GPIO3─►CS         │  GPIO38─►CS
     │  3.3V ──►VCC │  GPIO13◄─MISO  │  (shared SPI)      │  GPIO36─►SCK
     │  GND ───►GND │  GPIO10─►CS    │                    │  GPIO35─►MOSI
     │              │  GPIO46─►DC    │  GPIO8◄─IRQ        │  GPIO37◄─MISO
     │              │  GPIO9 ─►RST   │                    │
     │              │  GPIO48─►BL    │                    │
     └──────────────┴────────────────┴────────────────────┘
```

---

## Tabela de Pinagem Detalhada

### Joystick Analógico

| Pino Joystick | ESP32-S3 GPIO | Função | Notas |
|---------------|---------------|--------|-------|
| VCC | 3.3V | Alimentação | Cap 100µF recomendado |
| GND | GND | Terra | Comum com ESP32 |
| VRx | GPIO4 | ADC1_CH3 | Range 0-4095, centro ~2048 |
| VRy | GPIO5 | ADC1_CH4 | Range 0-4095, centro ~2048 |
| SW | GPIO0 | Digital Input | Pull-up 10KΩ, LOW ativo |

> [!NOTE]
> O ESP32-S3 **não possui** GPIO34/35. Usamos GPIO4/5 que são ADC1 válidos.

### Display ILI9488 3.5" (SPI2)

| Pino Display | ESP32-S3 GPIO | Função | Notas |
|--------------|---------------|--------|-------|
| VCC | 3.3V | Alimentação | Display 3.3V nativo |
| GND | GND | Terra | |
| SCK | GPIO12 | SPI Clock | 40MHz |
| MOSI | GPIO11 | SPI Data Out | |
| MISO | GPIO13 | SPI Data In | |
| CS | GPIO10 | Chip Select | LOW ativo |
| DC | GPIO46 | Data/Command | |
| RST | GPIO9 | Reset | |
| BL | GPIO48 | Backlight PWM | 0-255 |

### Touch XPT2046 (SPI2 Compartilhado)

| Pino Touch | ESP32-S3 GPIO | Função | Notas |
|------------|---------------|--------|-------|
| CS | GPIO3 | Chip Select | LOW ativo |
| IRQ | GPIO8 | Interrupt | Touch detected |
| CLK/MOSI/MISO | Compartilhado | SPI2 | Mesmo barramento do display |

### SD Card (SPI3)

| Pino SD | ESP32-S3 GPIO | Função | Notas |
|---------|---------------|--------|-------|
| CS | GPIO38 | Chip Select | |
| SCK | GPIO36 | SPI Clock | |
| MOSI | GPIO35 | Data Out | |
| MISO | GPIO37 | Data In | |

---

## Instruções de Montagem

### 1. Verificação Pré-montagem

**Com multímetro, verifique:**

- [ ] 3.3V no pino VCC (após ligar ESP32)
- [ ] Resistência do joystick VRx: ~5KΩ no centro
- [ ] Resistência do joystick VRy: ~5KΩ no centro
- [ ] Continuidade GND entre todos os componentes

### 2. Esquema de Soldagem

```
Ordem recomendada:
1. GND (todos os componentes primeiro)
2. VCC (3.3V para todos)
3. SPI2 Display (SCK, MOSI, MISO)
4. Display CS, DC, RST, BL
5. Touch CS, IRQ
6. SPI3 SD Card
7. Joystick ADC (VRx, VRy)
8. Joystick SW
```

### 3. Capacitores Recomendados

| Componente | Capacitor | Local |
|------------|-----------|-------|
| Joystick VCC | 100µF eletrolítico | Entre VCC e GND |
| Display VCC | 10µF cerâmico | Próximo ao conector |
| ESP32-S3 VCC | 100µF + 100nF | Próximo ao módulo |

### 4. Pull-up Resistor

- **SW Button**: 10KΩ entre GPIO0 e 3.3V
  - *Nota: GPIO0 já tem pull-up interno, resistor externo é opcional*

---

## Consumo de Energia Estimado

| Modo | Consumo | Notas |
|------|---------|-------|
| Ativo (240MHz, backlight 100%) | ~150mA | CPU + Display + Periféricos |
| Idle (160MHz, backlight 50%) | ~50mA | Target atingido |
| Light Sleep | ~1mA | CPU pausado |
| Deep Sleep | <5µA | Apenas RTC ativo |

### Breakdown por Componente

| Componente | Ativo | Idle |
|------------|-------|------|
| ESP32-S3 @ 240MHz | 100mA | 20mA |
| ILI9488 Display | 25mA | 25mA |
| Backlight 50% | 15mA | 0mA |
| Joystick Module | 2mA | 2mA |
| XPT2046 Touch | 1mA | 0.1mA |
| SD Card (idle) | 5mA | 0.5mA |

---

## Troubleshooting

| Problema | Causa Provável | Solução |
|----------|----------------|---------|
| Display branco | CS ou DC incorreto | Verificar GPIO10/46 |
| Touch não responde | CS touch conflito | Verificar GPIO3 está HIGH quando não usado |
| Joystick lê 0 | VCC não conectado | Verificar 3.3V no módulo |
| Joystick lê 4095 | VRx/VRy invertido | Trocar conexões |
| SD não monta | SPI3 conflito | Verificar CS=HIGH quando idle |
| Boot loop | GPIO0 em LOW | SW button não deve estar pressionado no boot |

---

## Código de Teste Rápido

```cpp
// Teste de leitura do joystick
void testJoystick() {
    int x = analogRead(4);  // VRx
    int y = analogRead(5);  // VRy
    bool btn = digitalRead(0) == LOW;
    
    Serial.printf("X=%4d Y=%4d BTN=%d\n", x, y, btn);
}
```

---

## Referências

- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ILI9488 Datasheet](https://www.newhavendisplay.com/resources_dataFiles/datasheets/LCDs/ILI9488.pdf)
- [XPT2046 Touch Controller](https://www.ti.com/lit/ds/symlink/xpt2046.pdf)
