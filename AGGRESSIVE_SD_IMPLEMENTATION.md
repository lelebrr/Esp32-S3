# 💾 IMPLEMENTAÇÃO BOOT AGRESSIVO SD E SECURE BOOT

**Data:** 19/12/2025
**Status:** ✅ **Concluído e Integrado**
**Autor:** Lele Origin Team

---

## 📋 Visão Geral

Este documento detalha a implementação do "Aggressive SD Boot" e do "Secure Boot (Paranoid Mode)". O objetivo principal foi remover a dependência do sistema de arquivos interno (SPIFFS/LittleFS) para armazenamento persistente, garantindo que o dispositivo opere primariamente a partir do cartão SD e realize uma limpeza (wipe) do flash interno a cada inicialização para segurança máxima.

---

## 🏗️ Arquitetura Implementada

### 1. 🚀 Aggressive SD Boot (`src/core/aggressive_sd.cpp`)

Lógica de inicialização que precede o carregamento do sistema principal.

* **Montagem Antecipada:** O sistema monta o cartão SD via driver SPI (ESP-IDF VFS) imediatamente no boot.
* **Backup Automático:** Antes de qualquer ação destrutiva, copia recursivamente todos os arquivos do LittleFS interno para `/sd/update/` no cartão SD.
* **Wipe do Flash:** Formata a partição LittleFS interna (`LittleFS.format()`).
* **Resource Cleanup:** Desmonta o SD e libera o barramento SPI para permitir que o firmware principal (Arduino `SD` library) assuma o controle sem conflitos.

### 2. 🛡️ Secure Boot - Paranoid Mode (`src/core/secure_boot.cpp`)

Camada de segurança adicional que roda após o boot agressivo.

* **Integridade do Firmware:** Verifica o CRC32 da partição de app rodando contra um hash salvo no SD (`/sd/hash.txt`).
* **Monitoramento Térmico:** Verifica a temperatura do chip; desliga se crítico.
* **Overclock Seguro:** Tenta subir o clock para 240MHz com estabilidade verificada.
* **Watchdog:** Inicializa um Watchdog Timer (WDT) de 3 segundos para reiniciar em caso de travamento.

### 3. ⚙️ Config Migration (`src/core/config.cpp`)

Migração completa do armazenamento de configurações.

* **SD Primário:** A função `LeleConfig::saveFile()` foi reescrita para salvar `config.json` diretamente no SD.
* **Remoção de Sync:** A lógica antiga que copiava configs do SD para o LittleFS foi removida.
* **Fallback:** O sistema ainda pode ler do LittleFS se o SD falhar (embora o LittleFS esteja vazio após o boot agressivo, servindo como "factory reset" state).

### 4. 🖼️ Compatibilidade de Módulos

Vários módulos foram ajustados para respeitar a nova hierarquia de armazenamento.

* **Image Compression:** (`src/ui/image_compression.cpp`) Migrado de `SD_MMC` para `SD` (SPI) padrão.
* **PN532 Tools:** Prioriza arquivos de áudio (`device_detected.wav`, `uid_found.wav`) do SD.
* **Sniffer/WiFi Attacks:** Prioriza armazenamento de PCAPs e Handshakes no SD.

---

## 🔧 Detalhes Técnicos

### Fluxo de Boot (`main.cpp`)

```cpp
void setup() {
    // 1. Aggressive Boot: Monta SD, Copia Flash->SD, Formata Flash
    aggressive_boot_logic(); 

    // 2. Secure Boot: Checa integridade, temperatura, voltagem
    SecureBoot::run();

    // 3. Inicialização Padrão
    Serial.begin(115200);
    // ... setup GPIO, Display, etc.
}

void loop() {
    // Feed do Watchdog Paranóico
    SecureBoot::loopCheck();
    
    // Main loop
    // ...
}
```

### Estrutura de Arquivos Modificada

* `src/core/aggressive_sd.h` (Novo)
* `src/core/aggressive_sd.cpp` (Novo)
* `src/core/secure_boot.h` (Novo/Atualizado)
* `src/core/secure_boot.cpp` (Novo/Atualizado)
* `src/core/config.cpp` (Modificado)
* `src/ui/image_compression.cpp` (Modificado)
* `src/main.cpp` (Modificado)

---

## ✅ Verificação e Testes

### Procedimento de Teste

1. **Preparação:**
    * Formatar SD Card (FAT32).
    * Inserir no Slot SD do CYD-2USB.
2. **Flash:**
    * Compilar e gravar firmware via PlatformIO.
3. **Primeiro Boot:**
    * Observar Serial Monitor.
    * Mensagens esperadas: `Aggressive SD Boot Init...`, `SD up, flash limpo`.
4. **Verificação Pós-Boot:**
    * Verificar LED de status (se configurado).
    * Verificar funcionalidade de configurações (criar config, reiniciar, verificar persistência no SD).
    * Verificar ausência de arquivos no LittleFS (menus que listam arquivos internos devem estar vazios).

### Resultados Esperados

* **Performance:** Boot ligeiramente mais longo na primeira vez (devido à formatação), rápido nas subsequentes.
* **Segurança:** Nenhum dado sensível (senhas wifi, configs, logs) reside no flash interno após o boot.
* **Estabilidade:** O sistema opera normalmente lendo assets e configs do SD.

---

## ⚠️ Notas Importantes

* **Sem SD Card:** Se o dispositivo for ligado sem SD, o "Aggressive Boot" falha graciosamente (não formata flash se não conseguir copiar), mas o "Secure Boot" pode falhar na verificação de hash (se configurado para estrito).
* **Updates:** Atualizações OTA devem considerar que o hash de integridade precisa ser atualizado no SD.

---

**Lele Origin Team - Security Research Division**
