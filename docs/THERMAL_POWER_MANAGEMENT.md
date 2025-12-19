# ⚡ Thermal & Power Management - ESP32-S3

> **Gerenciamento térmico, economia de energia e deep sleep**  
> **Atualizado:** 2025-12-19

---

## 📋 Visão Geral

O ESP32-S3-WROOM-1 N16R8 implementa um sistema avançado de gerenciamento de energia e temperatura:

| Recurso | Implementação |
|---------|---------------|
| **Thermal Throttling** | DS18B20 → reduz CPU 240→160MHz quando >70°C |
| **Power Gating** | IRF520 MOSFET desliga módulos inativos |
| **Gesture Wake** | PAJ7620U2 acorda de deep sleep |
| **Deep Sleep** | <5µA com timer ou gesto |
| **Battery Modes** | Economy (14d), Balanced (5d), Force (8h) |

---

## 🌡️ Thermal Throttling

### Como Funciona

O sensor DS18B20 monitora a temperatura do sistema a cada 10 segundos. Quando a temperatura ultrapassa limites seguros, o firmware reduz automaticamente a frequência do CPU.

```
┌─────────────────────────────────────────────────────────────┐
│                  Thermal Throttling Flow                     │
│                                                             │
│   DS18B20                ESP32-S3 CPU                       │
│   ┌──────┐              ┌──────────────┐                    │
│   │ Temp │──→ >70°C ──→ │  160 MHz    │  Throttled         │
│   │Sensor│              │  (Reduced)  │                    │
│   │      │──→ <60°C ──→ │  240 MHz    │  Normal            │
│   └──────┘              └──────────────┘                    │
│    GPIO 47                                                  │
└─────────────────────────────────────────────────────────────┘
```

### Limiares

| Estado | Temperatura | CPU | Ação |
|--------|-------------|-----|------|
| Normal | <60°C | 240MHz | Performance máxima |
| Hysteresis | 60-70°C | Mantém atual | Evita oscilação |
| Throttled | >70°C | 160MHz | Proteção térmica |

### Código de Referência

```cpp
// Arquivos: src/core/thermal_manager.h, thermal_manager.cpp

#define THERMAL_HIGH_THRESHOLD 70.0f  // Throttle acima
#define THERMAL_LOW_THRESHOLD  60.0f  // Restaura abaixo
#define THERMAL_CHECK_INTERVAL 10000  // ms entre checks

// APIs disponíveis:
void thermal_manager_init();    // Inicializa task de monitoramento
float thermal_get_temperature(); // Retorna temperatura atual
bool thermal_is_throttled();    // True se CPU está reduzido
```

---

## 🔌 Power Gating (IRF520 MOSFET)

### Como Funciona

O MOSFET IRF520 N-channel permite desligar completamente módulos de alta potência quando não estão em uso, reduzindo o consumo para zero.

```
┌─────────────────────────────────────────────────────────────┐
│                    Power Gating Circuit                      │
│                                                             │
│       VCC 3.3V                                              │
│          │                                                  │
│          ├──────────────────────────────┐                   │
│          │                              │                   │
│    ┌─────▼─────┐  ┌─────────┐    ┌─────▼─────┐             │
│    │  CC1101   │  │ NRF24   │    │   GPS     │             │
│    │  SubGHz   │  │ 2.4GHz  │    │  NEO-6M   │             │
│    └─────┬─────┘  └────┬────┘    └─────┬─────┘             │
│          │             │               │                    │
│          └──────┬──────┴───────────────┘                    │
│                 │                                           │
│           ┌─────▼─────┐                                     │
│           │  IRF520   │◄──── GPIO 42 (MOSFET_PIN)          │
│           │  MOSFET   │                                     │
│           └─────┬─────┘                                     │
│                 │                                           │
│                GND                                          │
└─────────────────────────────────────────────────────────────┘
```

### Módulos Controlados

| Módulo | Consumo Ativo | Consumo Desligado |
|--------|---------------|-------------------|
| PAM8403 Audio | ~50mA | 0mA |
| CC1101 SubGHz | ~30mA | 0mA |
| NRF24L01+PA+LNA | ~15mA (RX), ~115mA (TX) | 0mA |
| GPS NEO-6M | ~35mA | 0mA |

### Código de Referência

```cpp
// Arquivos: src/core/power_gating.h, power_gating.cpp

// Módulos disponíveis:
enum PoweredModule {
    MODULE_PAM8403,  // Audio amplifier
    MODULE_CC1101,   // SubGHz RF
    MODULE_NRF24,    // 2.4GHz RF
    MODULE_GPS       // NEO-6M GPS
};

// APIs:
void power_gating_init();                    // Inicializa (tudo off)
void power_module_enable(PoweredModule m);   // Liga módulo
void power_module_disable(PoweredModule m);  // Desliga módulo
bool power_module_is_enabled(PoweredModule m);
void power_all_modules_off();                // Desliga tudo
uint16_t power_get_estimated_draw();         // mA estimado
```

---

## 😴 Deep Sleep

### Wake Sources

O ESP32-S3 suporta múltiplas fontes de wake:

| Fonte | GPIO | Descrição |
|-------|------|-----------|
| **Gesture** | 8 (via PAJ7620U2) | Interrupção de gesto |
| **Timer** | N/A | Wake após X microsegundos |
| **Touch** | 8 | Interrupção de touch screen |
| **Boot Button** | 0 | Botão físico |

### Consumo por Estado

| Estado | Corrente | CPU | Periféricos |
|--------|----------|-----|-------------|
| Deep Sleep | <5µA | Off | Off (RTC mantém hora) |
| Light Sleep | ~800µA | Paused | Parcial |
| Idle 80MHz | ~25mA | Running | Mínimo |
| Active 240MHz | ~80mA | Running | Ativo |

### Código de Referência

```cpp
// Arquivos: src/core/gesture_wake.h, gesture_wake.cpp

// APIs:
void gesture_wake_init();                           // Configura wake pin
void enter_deep_sleep_with_gesture();               // Sleep até gesto
void enter_deep_sleep_with_timer(uint64_t sleep_us);// Sleep com timer
void enter_deep_sleep_multi(uint64_t us, bool gesture); // Combinado
esp_sleep_wakeup_cause_t get_wake_cause();          // Causa do wake
void prepare_for_deep_sleep();                      // Prepara (desliga módulos)
```

---

## 🔋 Modos de Bateria

### Seleção no Boot

Ao ligar o dispositivo, pressione GPIO0 (boot button) nos primeiros 3 segundos para entrar no menu de seleção de modo:

| Modo | CPU | WiFi Power | BLE Power | Display | Autonomia |
|------|-----|------------|-----------|---------|-----------|
| **Economy** | 80MHz | 8dBm | P3 | Dimmed | ~14 dias |
| **Balanced** | 160MHz | 15dBm | P7 | Normal | ~5 dias |
| **Force** | 240MHz | 20.5dBm | P9 | Bright | ~8 horas |

### Troca Dinâmica

O `OptimizationManager` ajusta automaticamente baseado na atividade:

```cpp
// Durante ataque ativo: sobe para Performance
if (attackActive) {
    optimizationManager.setMode(MODE_PERFORMANCE);
}

// Idle por 5 minutos: entra em PowerSave
if (idleTime > 300000) {
    optimizationManager.setMode(MODE_POWERSAVE);
}
```

---

## 📊 Estimativa de Autonomia

### Com 4x 18650 (10400mAh)

| Perfil de Uso | Consumo Médio | Autonomia |
|---------------|---------------|-----------|
| Standby (deep sleep) | 5µA | 14+ dias |
| Idle com tela | 25mA | ~16 horas |
| Scan WiFi contínuo | 100mA | ~8 horas |
| Ataque RF ativo | 200mA | ~4 horas |
| Tudo ligado (max) | 300mA | ~3 horas |

---

## ⚙️ Configuração

### sdkconfig.defaults

```ini
# Deep Sleep <5µA
CONFIG_ESP32S3_RTC_CLK_SRC_INT_RC=y
CONFIG_ESP32S3_DEEP_SLEEP_WAKEUP_DELAY=2000
CONFIG_PM_ENABLE=y
CONFIG_PM_DFS_INIT_AUTO=y
CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP=y
CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP=y
```

### platformio.ini

```ini
# Flags relevantes na seção [CYD_S3_base]
-DCONFIG_PM_ENABLE=1
-DCONFIG_PM_DFS_INIT_AUTO=1
-DHAS_TEMP_SENSOR=1
-DONEWIRE_PIN=47
-DHAS_MOSFET=1
-DMOSFET_PIN=42
-DHAS_GESTURE=1
```

---

## 🔧 Hardware Necessário

| Componente | Função | Conexão |
|------------|--------|---------|
| DS18B20 | Sensor temperatura | GPIO 47 + 4.7kΩ pull-up |
| IRF520 | MOSFET power gate | Gate = GPIO 42 |
| PAJ7620U2 | Sensor gestos | I2C 0x73 (SDA=1, SCL=2) |
| Capacitor 100µF | Estabilidade VCC | Paralelo em cada módulo |
| Diodo 1N5817 | Proteção polaridade | Série na bateria |
| PTC Fuse 500mA | Proteção sobrecorrente | Série na bateria |

---

## 📈 Benchmarks

| Métrica | Antes | Depois | Melhoria |
|---------|-------|--------|----------|
| Deep Sleep | 15µA | <5µA | -67% |
| Idle (tela on) | 45mA | 25mA | -44% |
| Ataque RF | 250mA | 200mA | -20% |
| Boot time | 7s | 5s | -28% |
| Autonomia standby | 5 dias | 14 dias | +180% |
