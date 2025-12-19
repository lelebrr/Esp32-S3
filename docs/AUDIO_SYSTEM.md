# 🔊 Sistema de Áudio - ESP32-S3

> **PCM5102A DAC + PAM8403 Amplifier + Speaker**  
> **Atualizado:** 2025-12-19

---

## 📋 Visão Geral

O sistema de áudio do ESP32-S3 utiliza I2S com DMA para playback de alta qualidade sem consumir CPU:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Audio Signal Chain                            │
│                                                                 │
│   ESP32-S3          PCM5102A           PAM8403         Speaker  │
│   ┌────────┐       ┌─────────┐        ┌───────┐       ┌──────┐ │
│   │  I2S   │─BCLK─→│         │──L──→  │       │───L───│      │ │
│   │  DMA   │─LRCK─→│  32-bit │        │Class D│       │ 4Ω   │ │
│   │        │─DOUT─→│   DAC   │──R──→  │  3W   │───R───│ 3W   │ │
│   └────────┘       └─────────┘        └───────┘       └──────┘ │
│    GPIO 40,39,41      Analog            Amp            Output   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔌 Conexões

### PCM5102A DAC (I2S)

| Pino PCM5102A | GPIO ESP32-S3 | Função |
|---------------|---------------|--------|
| **BCK** | 40 | Bit Clock |
| **LCK/WS** | 39 | Word Select (L/R) |
| **DIN** | 41 | Data Input |
| **VIN** | 3.3V | Alimentação |
| **GND** | GND | Terra |
| **SCK** | GND | Sistema clock (interno) |
| **FLT** | GND | Filtro sharp |
| **DEMP** | GND | De-emphasis off |
| **XSMT** | 3.3V | Soft mute off |
| **FMT** | GND | I2S standard |

> [!TIP]
> Conecte SCK ao GND para usar o PLL interno do PCM5102A.

### PAM8403 Amplifier

| Pino PAM8403 | Conexão | Função |
|--------------|---------|--------|
| **VCC** | MOSFET Drain | 5V via power gating |
| **GND** | GND | Terra comum |
| **L_IN** | PCM5102A L_OUT | Entrada esquerda |
| **R_IN** | PCM5102A R_OUT | Entrada direita |
| **L_OUT+** | Speaker + | Saída esquerda |
| **L_OUT-** | Speaker - | Saída esquerda |

> [!WARNING]
> O PAM8403 é alimentado via MOSFET IRF520 (GPIO 42) para permitir desligar quando não usado.

### Speaker

| Especificação | Valor |
|---------------|-------|
| Impedância | 4Ω ±15% |
| Potência | 1.25W - 3W |
| Frequência | 0 ~ 20kHz |
| SPL | 89 ±3dB |
| Dimensões | 25mm x 35mm x 6.5mm |

---

## ⚙️ Configuração I2S

### platformio.ini

```ini
# Flags de configuração I2S
-DHAS_I2S_DAC=1
-DI2S_BCLK_PIN=40
-DI2S_LRCK_PIN=39
-DI2S_DOUT_PIN=41
-DHAS_NS4168_SPKR=1
-DBCLK=40
-DWCLK=39
-DDOUT=41
```

### sdkconfig.defaults

```ini
# I2S DMA
CONFIG_I2S_ISR_IRAM_SAFE=y
```

### Código de Inicialização

```cpp
#include <driver/i2s.h>

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
};

i2s_pin_config_t pin_config = {
    .bck_io_num = 40,
    .ws_io_num = 39,
    .data_out_num = 41,
    .data_in_num = I2S_PIN_NO_CHANGE
};

i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
i2s_set_pin(I2S_NUM_0, &pin_config);
```

---

## 🎵 Funcionalidades

### Funções Disponíveis

```cpp
// Arquivos: src/modules/others/audio.h, audio.cpp

// Reproduz arquivo WAV/MP3 do SD ou LittleFS
bool playAudioFile(FS *fs, String filepath);

// Reproduz ringtone RTTTL
bool playAudioRTTTLString(String song);

// Text-to-Speech (SAM)
bool tts(String text);

// Verifica se é arquivo de áudio
bool isAudioFile(String filePath);

// Tom simples (buzzer ou DAC)
void playTone(unsigned int frequency, unsigned long duration, short waveType);

// Tom via buzzer (fallback)
void _tone(unsigned int frequency, unsigned long duration);
```

### Exemplo de Uso

```cpp
// Inicializa áudio
power_module_enable(MODULE_PAM8403);
delay(100); // Estabilização

// Reproduz boot sound
if (SD.exists("/boot.wav")) {
    playAudioFile(&SD, "/boot.wav");
}

// Desliga amplificador quando terminar
power_module_disable(MODULE_PAM8403);
```

---

## 🔋 Consumo de Energia

| Estado | Corrente |
|--------|----------|
| PAM8403 Idle | ~10mA |
| PAM8403 Playback (baixo volume) | ~50mA |
| PAM8403 Playback (máximo) | ~150mA |
| Desligado (via MOSFET) | 0mA |

> [!IMPORTANT]
> O amplificador é desligado automaticamente pelo power gating quando não há áudio.

---

## 🛠️ Troubleshooting

### Sem Áudio

1. Verifique conexões I2S (BCLK=40, LRCK=39, DOUT=41)
2. Confirme que SCK do PCM5102A está em GND
3. Verifique que MOSFET está ligado (`power_module_enable(MODULE_PAM8403)`)

### Ruído/Estática

1. Adicione capacitor 100µF entre VCC e GND do PCM5102A
2. Mantenha trilhas de áudio curtas
3. Separe terra de áudio do terra digital

### Áudio Distorcido

1. Verifique impedância do speaker (deve ser 4Ω)
2. Reduza volume no software
3. Verifique alimentação 5V estável para PAM8403

---

## 📐 Diagrama de Ligação

```
                ESP32-S3                    PCM5102A
              ┌───────────┐               ┌───────────┐
              │           │               │           │
     GPIO 40 ─┤ I2S_BCLK ├───────────────┤ BCK       │
              │           │               │           │
     GPIO 39 ─┤ I2S_LRCK ├───────────────┤ LCK       │
              │           │               │           │
     GPIO 41 ─┤ I2S_DOUT ├───────────────┤ DIN       │
              │           │               │           │
        3.3V ─┤ VCC      ├───────────────┤ VIN       │
              │           │               │           │
         GND ─┤ GND      ├───┬───────────┤ GND       │
              └───────────┘   │           │ SCK       │
                              └───────────┤ FLT       │
                                          │ DEMP      │
                                          └───────────┘
                                               │
                              ┌────────────────┴────────┐
                              │         PAM8403         │
                              │                         │
                         5V ──┤ VCC (via MOSFET)        │
                        GND ──┤ GND                     │
                   L_OUT ─────┤ L_IN                    ├──── Speaker +
                   R_OUT ─────┤ R_IN                    ├──── Speaker -
                              └─────────────────────────┘
```
