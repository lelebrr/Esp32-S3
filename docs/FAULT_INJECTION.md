# ⚡ Fault Injection (Glitcher)

> **Glitching e injeção de falhas.**
> Última atualização: 2025-12-19
---Este módulo implementa ataques de injeção de falhas via voltage glitching para bypass de proteções em microcontroladores e dispositivos IoT.

---

## 🛠️ Hardware Necessário

### Componentes Básicos (~R$ 50)

| Componente | Modelo | Preço Aprox. |
|------------|--------|--------------|
| MOSFET N-Channel | **IRLZ44N** | R$ 5-10 |
| Resistor Gate | 10-100Ω | R$ 0.50 |
| Protoboard | Mini 400 pontos | R$ 10 |
| Fios Jumper | M-M, M-F | R$ 10 |
| (Opcional) Diodo Flyback | 1N4007 | R$ 1 |
| (Opcional) Optocoupler | PC817 | R$ 3 |

### Esquema de Conexão IRLZ44N

```
ESP32 GPIO (27) ──[100Ω]──> IRLZ44N Gate
                           IRLZ44N Drain ──> GND do ALVO
                           IRLZ44N Source ──> VDD do ALVO (3.3V/5V)
```

> ⚠️ **CUIDADO**: Pode danificar hardware! Sempre teste em alvos descartáveis primeiro.

---

## 📋 Funcionalidades Planejadas

### Fase 1 (Básico)

- [ ] Voltage Glitch via GPIO único
- [ ] Configuração de parâmetros (delay, pulse width, repeats)
- [ ] Trigger manual via touchscreen
- [ ] Log de resultados no SD Card

### Fase 2 (Avançado)

- [ ] Múltiplos GPIOs (arm, glitch, reset)
- [ ] Trigger externo via UART
- [ ] Scan automático de parâmetros
- [ ] Scripts prontos para alvos comuns

### Fase 3 (Profissional)

- [ ] EMFI (Electromagnetic Fault Injection) com bobina
- [ ] Análise pós-glitch via SWD
- [ ] Extração de chaves criptográficas
- [ ] Integração com dump de flash

---

## 🎯 Alvos Suportados (Futuro)

| Dispositivo | Ataque | Dificuldade |
|-------------|--------|-------------|
| ESP32 V1/V2 | Flash key extraction | Médio |
| Arduino Uno/Mega | Bypass fuses | Fácil |
| Smart Locks | Bypass senha | Médio |
| IoT Toys | Dump firmware | Fácil |
| NRF52 | Bypass readback | Difícil |

---

## 📚 Referências

- [fault-injector](https://github.com/PythonHacker24/fault-injector)
- [ESP32-C3/C6 Fault Injection](https://courk.cc/esp32-c3-c6-fault-injection)
- [PWN ESP32 Secure Boot](https://limitedresults.com/2019/09/pwn-the-esp32-secure-boot/)
- [nrf52-glitcher](https://github.com/shutingrz/nrf52-glitcher)
- [inode fault_injection](https://github.com/inode-/fault_injection)

---

## 🔧 Código de Exemplo (Futuro)

```cpp
// Glitch básico - pulso de ~1µs
#define GLITCH_PIN 27
#define GLITCH_DELAY_US 500    // Delay antes do glitch
#define GLITCH_WIDTH_US 1      // Largura do pulso

void voltage_glitch() {
    delayMicroseconds(GLITCH_DELAY_US);
    digitalWrite(GLITCH_PIN, HIGH);
    delayMicroseconds(GLITCH_WIDTH_US);
    digitalWrite(GLITCH_PIN, LOW);
}
```

---

## ⚠️ Avisos Legais

- Use apenas em dispositivos de sua propriedade
- Pode causar danos permanentes ao hardware
- Para fins educacionais e pesquisa de segurança
