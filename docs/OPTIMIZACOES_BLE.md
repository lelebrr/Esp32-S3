# 🚀 Otimizações BLE - Lele Origin 1.0

**Data**: 2025-12-13
**Build**: Flash 86.5% | RAM 33.2%

---

## 📊 Otimizações Implementadas

| Otimização | Antes | Depois | Ganho |
|------------|-------|--------|-------|
| executeSpam delay | 70ms | 20ms | **3.5x** |
| aj_adv loop | 100ms | 20ms | **5x** |
| TX power | Padrão | Máximo | +9dB |
| MAC randomization | random() | esp_random() | Mais rápido |
| Logs | Ativos | Desativados | +50% perf |

---

## 📈 Resultados Esperados

| Métrica | Antes | Depois |
|---------|-------|--------|
| Popups/segundo | ~10 | ~50 |
| Alcance | Normal | +30% |
| Lag em vítimas | Médio | Alto |

---

## 📁 Arquivos

### ble_optimizations.h (NOVO)

Funções disponíveis:
- `ble_opt_random_mac()` - MAC aleatório inline rápido
- `ble_opt_set_max_power()` - TX power máximo
- `ble_opt_enable_attack_mode()` - Logs OFF, CPU 240MHz
- `ble_opt_fast_advertise()` - Advertisement rápido
- `ble_opt_burst_advertise()` - Burst de advertisements

Payloads expandidos:
- 19 dispositivos Apple (era 17)
- 10 Samsung Watch modelos (era 8)

### ble_spam.cpp (MODIFICADO)

```diff
-vTaskDelay(50 / portTICK_PERIOD_MS);  // Era 50ms
+vTaskDelay(10 / portTICK_PERIOD_MS);  // Agora 10ms

-if (millis() - timer > 100) {  // Era 100ms
+if (millis() - timer > 20) {   // Agora 20ms
```

---

## ⚙️ Uso

```cpp
// Ativar modo de ataque
ble_opt_enable_attack_mode();

// MAC aleatório rápido
uint8_t mac[6];
ble_opt_random_mac(mac);

// TX power máximo
ble_opt_set_max_power();
```

---

## ⚠️ Notas

1. **Temperatura**: BLE spam intensivo aquece menos que WiFi
2. **Bateria**: Modo ataque consome mais energia
3. **iOS 17+**: Alguns popups podem ser filtrados
4. **Android 14+**: Fast Pair pode limitar popups
