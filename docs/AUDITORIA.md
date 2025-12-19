# 🔍 Auditoria de Código - Lele Origin 1.0

**Data**: 2025-12-13
**Build**: Flash 86.5% | RAM 33.2%

---

## 📊 Resumo

| Categoria | Itens | Prioridade |
|-----------|-------|------------|
| Memory Leaks | 10 | 🔴 Alta |
| Delays Longos | 3 | 🟡 Média |
| Strings sem F() | 10+ | 🟡 Média |
| TODOs Pendentes | 17 | 🟢 Baixa |
| Arquivos Grandes | 23 | 🟢 Baixa |

---

## 🔴 ALTA PRIORIDADE

### Memory Leaks Potenciais

**Problema**: 19 alocações (malloc/new) vs apenas 9 liberações (free/delete)

**Arquivos a revisar**:
- Verificar todos `new` e garantir `delete` correspondente
- Verificar todos `malloc` e garantir `free` correspondente
- Usar smart pointers onde possível

---

## 🟡 MÉDIA PRIORIDADE

### Delays Longos (Bloqueantes)

| Arquivo | Linha | Delay |
|---------|-------|-------|
| display.cpp | 1209 | 2500ms |
| massStorage.cpp | 27 | 1000ms |
| settings.cpp | 686 | 1000ms |

**Recomendação**: Substituir por `vTaskDelay()` ou timer não-bloqueante

### Strings sem F() Macro

Strings longas em `Serial.println()` consomem RAM:

```
config.cpp: Serial.println("Failed to read config file...")
display.cpp: Serial.println("Image too wide...")
sd_functions.cpp: Serial.println("SDCard in the same Bus...")
```

**Recomendação**: Usar `Serial.println(F("texto"))` para mover strings para Flash

---

## 🟢 BAIXA PRIORIDADE

### TODOs Pendentes (17)

| Arquivo | Descrição |
|---------|-----------|
| main.cpp:91 | TODO put in a namespace |
| display.cpp:266,278,289 | todo: add newlines to txt |
| sd_functions.cpp:366 | TODO: use PSRAM |
| sd_functions.cpp:776 | TODO: reinit serial port |
| passwords.cpp:95 | TODO: parse unsupported params |
| passwords.cpp:167 | TODO: add AES |
| settings.cpp:678 | TODO: check valid freq |

### Arquivos Grandes (>20KB)

| KB | Arquivo |
|----|---------|
| 132 | WORLD_IR_CODES.h |
| 61 | display.cpp |
| 55 | settings.cpp |
| 51 | pn532ble.cpp |
| 43 | sniffer.cpp |

**Recomendação**: WORLD_IR_CODES.h poderia ser movido para PROGMEM ou SD

---

## ✅ Otimizações Implementadas

### WiFi Attacks (CONCLUÍDO)
1. ✅ IRAM_ATTR no sniffer callback (10x mais rápido)
2. ✅ Delays removidos de send_raw_frame (2-3x mais frames/s)
3. ✅ wifi_opt_enable_attack_mode() (logs OFF, power saving OFF, CPU 240MHz)
4. ✅ wifi_opt_set_max_power() (potência TX máxima)

### Pendentes (Fáceis)
5. Adicionar F() macro em strings Serial
6. Verificar e corrigir memory leaks

### Pendentes (Complexas)
7. Mover WORLD_IR_CODES para Flash/SD
8. Refatorar display.cpp e settings.cpp
9. Implementar TODOs pendentes
10. Usar PSRAM para buffers grandes

---

## 📁 Strings Duplicadas

```
"%02X:%02X:%02X:%02X:%02X:%02X" (4 ocorrências)
"Failed to open config file" (3 ocorrências)
"Failed to write config file" (3 ocorrências)
```

**Recomendação**: Criar constantes globais para evitar duplicação
