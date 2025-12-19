# PAJ7620U2 - Gesture Recognition Sensor (GY-PAJ7620)

## Overview

O **PAJ7620U2** (também conhecido como GY-PAJ7620 ou CJMCU-7620) é um sensor de reconhecimento de gestos de infravermelho fabricado pela **PixArt Imaging**. Ele usa tecnologia IR para detectar movimentos de mãos e é ideal para projetos interativos, como controle de luzes por gestos, jogos, robótica e interfaces touch-free.

---

## Especificações Técnicas

| Parâmetro | Valor |
|-----------|-------|
| **Chip** | PAJ7620U2 |
| **Tensão de Operação** | 2.8V a 3.3V |
| **Tensão I/O** | 1.8V a 3.3V |
| **Corrente de Operação** | 23mA (típico) |
| **Interface** | I2C (até 400 kHz) |
| **Endereço I2C** | 0x73 |
| **Distância de Detecção** | 5 a 15 cm |
| **Ângulo de Reconhecimento** | 60° (típico) |
| **Velocidade do Gesto (Normal)** | 60°/s a 600°/s |
| **Velocidade do Gesto (Game Mode)** | 60°/s a 1200°/s |
| **Imunidade à Luz Ambiente** | < 100k Lux |
| **Temperatura Ambiente** | -40°C a +85°C |
| **Resolução IR** | 30x30 pixels |
| **Frame Rate (Normal)** | 120fps |
| **Frame Rate (Game)** | 240fps |
| **Dimensões** | 20.0 x 10.5 x 3.8 mm (LxWxH) |

---

## Pinout do Módulo (GY-PAJ7620)

| Pino | Nome | Descrição |
|------|------|-----------|
| 1 | **VCC** | Alimentação (2.8V - 3.3V) |
| 2 | **GND** | Ground / Terra |
| 3 | **SDA** | I2C Data Line |
| 4 | **SCL** | I2C Clock Line |
| 5 | **INT** | Interrupt Output (Active Low) |

### Diagrama de Pinos

```text
┌─────────────────────────────────────┐
│         PAJ7620U2 Module            │
│                                     │
│  ┌─────────────────────────────┐    │
│  │      IR LED & Photodiode    │    │
│  │         ┌───────┐           │    │
│  │         │ ○   ○ │           │    │
│  │         │ TX  RX│           │    │
│  │         └───────┘           │    │
│  └─────────────────────────────┘    │
│                                     │
│   VCC  GND  SDA  SCL  INT           │
│    │    │    │    │    │            │
└────┼────┼────┼────┼────┼────────────┘
     │    │    │    │    │
    3.3V GND  IO8  IO9  IO[x]
```

---

## Conexão com ESP32-S3

### Pinagem para ESP32-S3-WROOM-1

| PAJ7620 Pin | Função | ESP32-S3 GPIO |
|-------------|--------|---------------|
| VCC | Alimentação | 3.3V |
| GND | Terra | GND |
| SDA | I2C Data | GPIO 8 |
| SCL | I2C Clock | GPIO 9 |
| INT | Interrupção | GPIO 15 (opcional) |

> [!TIP]
> O sensor compartilha o barramento I2C com outros dispositivos como PN532 NFC. Verifique se não há conflitos de endereço.

### Esquema de Ligação

```text
ESP32-S3-WROOM-1                    PAJ7620U2
    ┌───────────┐                  ┌─────────┐
    │           │                  │         │
    │     3.3V  ├──────────────────┤ VCC     │
    │           │                  │         │
    │      GND  ├──────────────────┤ GND     │
    │           │                  │         │
    │   GPIO 8  ├─────[4.7kΩ]──────┤ SDA     │
    │           │         │        │         │
    │   GPIO 9  ├─────[4.7kΩ]──────┤ SCL     │
    │           │         │        │         │
    │   GPIO 15 ├──────────────────┤ INT     │
    │           │                  │         │
    └───────────┘                  └─────────┘
                    │    │
                   3.3V 3.3V
                (Pull-up resistors)
```

> [!IMPORTANT]
> Utilize resistores pull-up de **4.7kΩ** nas linhas SDA e SCL se não estiverem presentes no módulo.

---

## Gestos Reconhecidos (9 Gestos)

| Gesto | Código | Descrição |
|-------|--------|-----------|
| ⬆️ **UP** | `GES_UP` | Movimento para cima |
| ⬇️ **DOWN** | `GES_DOWN` | Movimento para baixo |
| ⬅️ **LEFT** | `GES_LEFT` | Movimento para esquerda |
| ➡️ **RIGHT** | `GES_RIGHT` | Movimento para direita |
| ↪️ **FORWARD** | `GES_FORWARD` | Aproximar a mão do sensor |
| ↩️ **BACKWARD** | `GES_BACKWARD` | Afastar a mão do sensor |
| 🔄 **CLOCKWISE** | `GES_CLOCKWISE` | Rotação horária |
| 🔃 **ANTI-CLOCKWISE** | `GES_ANTICLOCKWISE` | Rotação anti-horária |
| 👋 **WAVE** | `GES_WAVE` | Acenar (conta até 15 acenos) |

---

## Modos de Operação

### Gesture Mode (Padrão)

Detecta os 9 gestos básicos e pode acionar interrupções.

### Cursor Mode

Rastreia a posição (X, Y) de um objeto na frente do sensor. Útil para aplicações de tracking de dedo.

```cpp
sensor.setCursorMode();
if (sensor.isCursorInView()) {
    int x = sensor.getCursorX();  // 0-3712
    int y = sensor.getCursorY();  // 0-3712
}
sensor.setGestureMode();  // Voltar ao modo gesto
```

### Game Mode

Aumenta a velocidade de resposta de 120fps para 240fps.

```cpp
sensor.setGameSpeed();    // 240fps, gestos até 1200°/s
sensor.setNormalSpeed();  // 120fps, gestos até 600°/s
```

---

## Biblioteca Recomendada

**RevEng_PAJ7620** - Arduino Library

### Instalação

Via Arduino IDE:

1. Tools → Manage Libraries...
2. Pesquisar: "RevEng PAJ7620"
3. Instalar

Via PlatformIO:

```ini
lib_deps =
    acrandal/RevEng_PAJ7620@^1.5.1
```

### Código de Exemplo ESP32

```cpp
#include <Wire.h>
#include "RevEng_PAJ7620.h"

RevEng_PAJ7620 sensor = RevEng_PAJ7620();

void setup() {
    Serial.begin(115200);
    
    // Inicializar I2C nos pinos corretos
    Wire.begin(8, 9);  // SDA = GPIO8, SCL = GPIO9
    
    if (!sensor.begin()) {
        Serial.println("PAJ7620 não encontrado!");
        while (1);
    }
    Serial.println("PAJ7620 inicializado!");
}

void loop() {
    Gesture gesture = sensor.readGesture();
    
    switch (gesture) {
        case GES_UP:
            Serial.println("Gesto: ⬆️ CIMA");
            break;
        case GES_DOWN:
            Serial.println("Gesto: ⬇️ BAIXO");
            break;
        case GES_LEFT:
            Serial.println("Gesto: ⬅️ ESQUERDA");
            break;
        case GES_RIGHT:
            Serial.println("Gesto: ➡️ DIREITA");
            break;
        case GES_FORWARD:
            Serial.println("Gesto: ↪️ APROXIMAR");
            break;
        case GES_BACKWARD:
            Serial.println("Gesto: ↩️ AFASTAR");
            break;
        case GES_CLOCKWISE:
            Serial.println("Gesto: 🔄 HORARIO");
            break;
        case GES_ANTICLOCKWISE:
            Serial.println("Gesto: 🔃 ANTI-HORARIO");
            break;
        case GES_WAVE:
            Serial.println("Gesto: 👋 ACENO (" + 
                String(sensor.getWaveCount()) + " vezes)");
            break;
        default:
            break;
    }
    
    delay(50);
}
```

### Código com Interrupção

```cpp
#include <Wire.h>
#include "RevEng_PAJ7620.h"

#define GESTURE_INT_PIN 15

RevEng_PAJ7620 sensor = RevEng_PAJ7620();
volatile bool gestureDetected = false;

void IRAM_ATTR onGestureInterrupt() {
    gestureDetected = true;
}

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);
    
    if (!sensor.begin()) {
        Serial.println("Erro ao inicializar PAJ7620!");
        while (1);
    }
    
    // Configurar pino de interrupção
    pinMode(GESTURE_INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(GESTURE_INT_PIN), 
                   onGestureInterrupt, FALLING);
    
    Serial.println("PAJ7620 pronto com interrupção!");
}

void loop() {
    if (gestureDetected) {
        gestureDetected = false;
        Gesture g = sensor.readGesture();
        if (g != GES_NONE) {
            processGesture(g);
        }
    }
}

void processGesture(Gesture g) {
    // Processar o gesto detectado
    Serial.printf("Gesto detectado: %d\n", g);
}
```

---

## Recursos Avançados

### Detecção de Tamanho e Brilho do Objeto

```cpp
int brightness = sensor.getObjectBrightness();  // 0-255
int size = sensor.getObjectSize();              // 0-900 pixels
```

### Tracking de Velocidade do Objeto

```cpp
int velocityX = sensor.getObjectVelocityX();  // -63 a +63
int velocityY = sensor.getObjectVelocityY();  // -63 a +63
```

### Detecção de Canto (Quadrante)

```cpp
Corner corner = sensor.getCorner();
// CORNER_NE, CORNER_NW, CORNER_SE, CORNER_SW, CORNER_MIDDLE, CORNER_NONE
```

### Configuração de Timing

```cpp
sensor.setGestureEntryTime(100);  // ms antes de iniciar gesto
sensor.setGestureExitTime(200);   // ms após finalizar gesto
```

---

## Aplicações no Projeto

### Wake from Deep Sleep

O PAJ7620 pode acordar o ESP32-S3 do deep sleep quando detecta um gesto específico, economizando energia.

```cpp
// Configurar wake-up por gesto
esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0);  // INT pin, LOW level
esp_deep_sleep_start();
```

### Controle de Interface

- **UP/DOWN**: Navegar menus
- **LEFT/RIGHT**: Trocar telas/abas
- **FORWARD**: Confirmar/Selecionar
- **BACKWARD**: Voltar/Cancelar
- **WAVE**: Ativar modo específico

---

## Troubleshooting

| Problema | Solução |
|----------|---------|
| Sensor não detectado | Verificar conexões I2C e tensão (3.3V) |
| Gestos imprecisos | Evitar luz solar direta, limpar superfície do sensor |
| Leituras intermitentes | Verificar pull-ups I2C (4.7kΩ) |
| Funciona com 5V? | **NÃO** - Use level shifter ou regulador 3.3V |
| Distância inadequada | Manter mão entre 5-15cm do sensor |

---

## Componentes do Módulo

1. **PAJ7620U2 IC** - Chip de reconhecimento de gestos
2. **IR LED** - Emissão infravermelha
3. **Infrared Photodiode** - Receptor IR
4. **Regulador de Tensão** - Conversão para 2.8V interno (em alguns módulos)
5. **Resistores Pull-up** - Para linhas I2C (em alguns módulos)
6. **Capacitores de Desacoplamento** - Filtragem de ruído

---

## Referências

- [PixArt PAJ7620U2 Datasheet](https://datasheetspdf.com/pdf-file/1309990/PixArt/PAJ7620U2/1)
- [RevEng_PAJ7620 Library (GitHub)](https://github.com/acrandal/RevEng_PAJ7620)
- [Seeed Studio Wiki](http://wiki.seeedstudio.com/Grove-Gesture_v1.0/)
- [Cirkit Designer Docs](https://docs.cirkitdesigner.com/component/ff8937ad-d5e4-4c3e-a5d0-264463dab298/gesture-recognition-sensor-paj7620)

---

**Última Atualização:** 2025-12-19

---

## Integração no Monster S3

### Arquivos de Implementação

| Arquivo | Localização | Função |
|---------|-------------|--------|
| `gesture_sensor.h` | `include/` | Header com classe GestureSensor |
| `gesture_sensor.cpp` | `src/` | Implementação do driver |
| `pin_config.h` | `include/` | Definições de pinos PAJ7620 |
| `main.cpp` | `src/` | Task FreeRTOS para gestos |
| `lvgl_menu.cpp` | `src/` | Integração com LVGL UI |
| `s3_driver.cpp` | `src/` | Inicialização no boot |

### Mapeamento Gesto → Ação no Sistema

| Gesto | Ação UI | Ação Attack |
|-------|---------|-------------|
| ⬆️ UP | Navegar para cima | - |
| ⬇️ DOWN | Navegar para baixo | - |
| ⬅️ LEFT | Tela anterior | - |
| ➡️ RIGHT | Próxima tela | - |
| ↪️ FORWARD | Confirmar/Selecionar | Iniciar ataque selecionado |
| ↩️ BACKWARD | Voltar/Cancelar | Parar ataque atual |
| 🔄 CLOCKWISE | Próximo item | Próximo tipo de ataque |
| 🔃 ANTI-CW | Item anterior | Tipo de ataque anterior |
| 👋 WAVE | Acordar tela | **PARAR TODOS ATAQUES** |

### Configuração no `pin_config.h`

```cpp
// ========================================
// GESTURE SENSOR - PAJ7620U2 (I2C)
// Uses shared I2C bus: SDA = GPIO 8, SCL = GPIO 9
// ========================================
#define PIN_PAJ7620_INT     -1     // Interrupt pin (-1 = polling mode)
#define PAJ7620_I2C_ADDR    0x73   // Default I2C address
#define PAJ7620_ENABLED     1      // Enable gesture sensor
```

### Task FreeRTOS

O sensor é lido por uma task dedicada rodando no Core 1:

```cpp
// main.cpp
xTaskCreatePinnedToCore(taskGestureSensor, "Gesture", 4096, NULL, 3, &hGestureTask, 1);
```

- **Frequência de polling**: 30ms (33Hz)
- **Prioridade**: 3 (média)
- **Stack**: 4KB

### Componentes de Proteção Recomendados

| Componente | Valor | Função |
|------------|-------|--------|
| Resistor Pull-up SDA | 4.7kΩ | I2C pull-up |
| Resistor Pull-up SCL | 4.7kΩ | I2C pull-up |
| Capacitor Desacoplamento | 100nF | Filtro VCC |
| Capacitor Bulk | 10µF | Estabilização |

### Diagrama de Conexão Completo

```text
┌─────────────────────────────────────────────────────────────────┐
│                     ESP32-S3-WROOM-1 (N8R8)                     │
│                                                                 │
│  ┌─────────┐     ┌──────────┐     ┌───────────────────────────┐ │
│  │  SD     │     │  PN532   │     │        PAJ7620U2          │ │
│  │  Card   │     │  NFC     │     │    Gesture Sensor         │ │
│  └────┬────┘     └────┬─────┘     └────────────┬──────────────┘ │
│       │               │                        │                │
│       │               │    ┌───────────────────┘                │
│       │               │    │                                    │
│   SPI Bus         I2C Bus (shared)                              │
│  (GPIO 39-42)    (GPIO 8-9)                                     │
│                       │                                         │
│                  ┌────┴────┐                                    │
│                  │         │                                    │
│              GPIO 8    GPIO 9                                   │
│               (SDA)     (SCL)                                   │
│                  │         │                                    │
│              4.7kΩ      4.7kΩ   (Pull-up to 3.3V)              │
│                  │         │                                    │
└──────────────────┼─────────┼────────────────────────────────────┘
                   │         │
           ┌───────┴─────────┴───────┐
           │      PAJ7620U2          │
           │  ┌─────────────────┐    │
           │  │    ┌───┐        │    │
           │  │    │IR │        │    │
           │  │    └───┘        │    │
           │  └─────────────────┘    │
           │                         │
           │  VCC GND SDA SCL INT    │
           │   │   │   │   │   │     │
           └───┼───┼───┼───┼───┼─────┘
               │   │   │   │   │
              3.3V GND IO8 IO9 NC
```

### Verificação de Funcionamento

Ao iniciar o dispositivo, verifique no Serial Monitor:

```text
[GESTURE] Initializing PAJ7620U2...
[GESTURE] PAJ7620U2 initialized successfully!
[GESTURE] I2C Address: 0x73
[GESTURE] Normal mode (120fps)
[GESTURE] Ready for gesture detection!
[SYSTEM] Gesture sensor ready
```

Se o sensor não for detectado:

```text
[GESTURE] ERROR: PAJ7620 not found!
[GESTURE] Check I2C connections (SDA=GPIO8, SCL=GPIO9)
[WARNING] Gesture sensor not available
```
