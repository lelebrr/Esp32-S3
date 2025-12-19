# 🔨 Compilação

> **Guia passo-a-passo para compilar e fazer upload.**  
> **Última atualização:** 2025-12-19

---

## 📋 Requisitos

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO Extension](https://platformio.org/install/ide?install=vscode)
- Git (para clonar o repositório)

---

## 🎯 Environments Disponíveis

| Environment | Hardware | Flash | PSRAM |
|-------------|----------|-------|-------|
| **CYD-S3-N16R8** | ESP32-S3-WROOM-1 N16R8 | 16MB | 8MB Octal |
| CYD-2USB | ESP32 WROOM-32 | 4MB | Não |

---

## 🔧 Configuração

1. Clone o repositório
2. Abra a pasta do projeto no VS Code
3. Aguarde o PlatformIO baixar as dependências (~5min)

---

## 🏗️ Compilando

### ESP32-S3 (Principal)

#### Via Interface

1. Clique no ícone PlatformIO (barra lateral)
2. **CYD-S3-N16R8 → Build**

#### Via Terminal

```bash
pio run -e CYD-S3-N16R8
```

### CYD-2USB (Legacy)

```bash
pio run -e CYD-2USB
```

---

## 📤 Upload

### ESP32-S3 (USB Nativo)

```bash
# Upload via PlatformIO
pio run -e CYD-S3-N16R8 -t upload

# Upload via esptool (firmware binário)
esptool.py --port COM3 --chip esp32s3 \
  write_flash 0x0 .pio/build/CYD-S3-N16R8/firmware.bin
```

### CYD-2USB (Legacy)

```bash
pio run -e CYD-2USB -t upload
```

---

## 🔍 Monitor Serial

```bash
# ESP32-S3
pio device monitor -e CYD-S3-N16R8

# CYD-2USB
pio device monitor -e CYD-2USB
```

**Baud rate:** 115200

---

## ⚙️ Configurações Importantes (ESP32-S3)

### sdkconfig.defaults

O arquivo `sdkconfig.defaults` contém configurações otimizadas:

- `CONFIG_SPIRAM_MODE_OCT=y` - PSRAM Octal 120MHz
- `CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y` - CPU 240MHz
- `CONFIG_PM_ENABLE=y` - Power Management

### platformio.ini

O environment `CYD_S3_base` define:

- 16MB Flash partitions (`custom_16Mb_S3.csv`)
- PSRAM habilitada com fetch de instruções
- DMA para I2S, SPI e UART
- Todas as definições de pinos

---

## 🐛 Problemas Comuns

| Problema | Solução |
|----------|---------|
| Porta não encontrada | Pressione BOOT+RESET para modo download |
| PSRAM não detectada | Verifique se é N16**R8** (não N8R2) |
| Build muito lento | Use `build_cache_dir` no platformio.ini |
| Memória insuficiente | Use environment LITE |
| Permissão negada (Linux) | `sudo usermod -a -G dialout $USER` |

---

## 📊 Tamanho do Firmware

| Environment | Flash Usado | RAM Livre |
|-------------|-------------|-----------|
| CYD-S3-N16R8 | ~3.2MB / 16MB | ~280KB + 8MB PSRAM |
| CYD-2USB | ~3.0MB / 4MB | ~40KB |
